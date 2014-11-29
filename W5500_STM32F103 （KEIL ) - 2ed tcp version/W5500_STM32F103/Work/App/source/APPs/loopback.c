#include "stm32f10x.h"
#include "config.h"
#include "W5500\w5500.h"
#include "W5500\socket.h"
#include "util.h"
#include "alarm.h"
#include "flash.h"
#include "adc.h"
#include "gsm.h"
#include "ds1302.h"
#include "APPs\loopback.h"
#include <stdio.h>
#include <string.h>
#include "stm32f10x_it.h"
#include "i2c_ee.h"


uint8 ch_status[MAX_SOCK_NUM] = { 0, };	/** 0:close, 1:ready, 2:connected 3:init*/
uint8 TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications
uint8 TX_BUF1[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications

uint32_t presentTime;
uint32_t presentTime1;
uint32_t try_again_time;

extern int test_data;

u8 try_again[MAX_NEED_RECV];
u32 try_data[MAX_NEED_RECV];

extern sys_para sys;



unsigned char calcfcs(unsigned char *pmsg, unsigned char len)
{
	unsigned char result = 0;
	while (len--)
	{
		result ^= *pmsg++;
	}
	return result;
}

int do_common_rsp(SOCKET s, u8 mode, u8 cmd)
{
	int ret=ERROR, a;
	u8 recv_flag;
	
	recv_flag=10;
	while(((a=recv_loop(s, mode, cmd))!= OK && recv_flag))
	{	
		recv_flag--;
		//printf("wait for live rsp in while\n");
		Delay_ms(100);
	}
	if(a != OK)
	{
		printf("recv RSP timeout\n");
	}
	else
		ret=OK;

	return ret;
}

u8 do_tcp_get_time(SOCKET s, u8 mode)
{
	u8 cmd[12];
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 7;
	cmd[2] = NET_TCP_GET_TIME_RSP;
	ds1302_get_time(cmd+3);
	cmd[10] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	//do_common_rsp(s);
    return OK;
}

u8 do_tcp_stat_get(SOCKET s, u8 mode)
{	
	u8 i;
	u8 cmd[26];
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 16;
	cmd[2] = NET_TCP_STAT_GET_RSP;

	for(i=0; i<16; i++)
		*(u32*)(cmd+3) |= (get_alarm_in(i)<<i);
	
	for(i=0; i<8; i++)
		*(u16*)(cmd+7) |= (get_alarm_out(i)<<i);

	get_ip_addr_from_eeprom((cmd+9));
	get_id_from_eeprom(((u32*)(cmd+13)));
	get_mac_from_eeprom(cmd+17);

	*(u16*)(cmd+23) |= sys.is_net_loss;
	*(u16*)(cmd+23) |= is_poweroff() << 1;
	*(u16*)(cmd+23) |= is_power_low() << 2;
	
	cmd[25] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	//do_common_rsp(s);
	return OK;
}


u8 do_tcp_link_set(SOCKET s, u8* rcv, u8 mode)
{
	u8 a, i;

	a = (rcv[0]-3)/7;
	sys.link.link_group = a;
	for(i=0; i<sys.link.link_group; i++)
	{
		sys.link.in[i] = *(u32*)(rcv+2+(2+1)*i);
		sys.link.out[i] = *(u16*)(rcv+2+(4+1)*i);
		sys.link.xor[i] = *(rcv+2+(4+2)*i);
		save_link_to_eeprom(i, sys.link.in[i], sys.link.out[i], sys.link.xor[i]);
	}
	do_send_ok(s, mode, NET_TCP_LINK_SET_RSP);
	return OK;
}

u8 do_tcp_get_arm_stat(SOCKET s, u8 mode)
{
	u8 cmd[12];
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 4;
	cmd[2] = NET_TCP_GET_ARM_STAT_RSP;

	*(u32*)(cmd+3) = sys.arming;
	cmd[10] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	return OK;
}

u8 do_tcp_get_link_stat(SOCKET s, u8 mode)
{
	u8 cmd[12];
	u8 i;
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 3+sys.link.link_group*7;
	cmd[2] = NET_TCP_GET_LINK_STAT_RSP;

	for(i=0; i<sys.link.link_group; i++)
	{
		*(u32*)(cmd+3+(2+1)*i) = sys.link.in[i];
		*(u16*)(cmd+3+(4+1)*i) = sys.link.out[i];
		*(cmd+3+(4+2)*i) = sys.link.xor[i];
	}
	cmd[cmd[1]+3] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	return OK;
}

u8 do_tcp_set_cell_num(SOCKET s, u8* rcv, u8 mode)
{
	u8 len,i,j;

	len = rcv[0];
	//for(i=0; i<len+3; i++)
	//	printf("recv: %d\n", rcv[i]);
	sys.cell.cell_group = (len-1)/11;

	sys.cell.cell_xor = rcv[2];
	for(i=0; i<sys.cell.cell_group; i++)
	{
		for(j=0; j<11; j++)
		{
			sys.cell.cell[i][j] = *(rcv+3+i*j+j);
			//printf("cell[%d][%d]: %d\n", i, j, cell[i][j]);
		}
		save_cell_num_to_eeprom(i, sys.cell.cell[i]);
	}
	do_send_ok(s, mode, NET_TCP_SET_CELL_NUM_RSP);
	return OK;
}

u8 do_tcp_set_cell_text(SOCKET s, u8* rcv, u8 mode, u8 index)
{
	u8 len[5],i,j,old=0;

	sys.text.text_group++;
	save_text_group_to_eeprom(sys.text.text_group);

	for(i=index; i<index+1; i++)
	{	
		len[i] = *(rcv+14+12*i+old);
		sys.text.text_length[i] = len[i];
		for(j=0; j<11; j++)
			sys.text.text[i][j] = *(rcv+3+12*i+old+j);
		save_text_num_to_eeprom(i, sys.text.text[i]);
		sys.text.text_enable |= 1<<i;
		for(j=0; j<len[i]; j++)
			sys.text.text_content[i][j] = *(rcv+15+12*i+old+j);
		printf("len[%d]: %d\n", i, len[i]);
		save_text_content_to_eeprom(i, len[i], sys.text.text_content[i]);
		printf("text_content[%d]: %s\n", i, sys.text.text_content[i]);
		old += len[i];
	}
	do_send_ok(s, mode, NET_TCP_SET_TEXT_NUM_RSP);
	return OK;
}

u8 do_tcp_cancel_text_alarm(SOCKET s, u8 mode, u8 data)
{
	u8 i;

	if(data & (1<<i))
	{
		sys.text.text_enable |=  1<<i;
		sys.text.text_group--;
		
	}
	save_text_enable_to_eeprom(sys.text.text_enable);
	save_text_group_to_eeprom(sys.text.text_group);
	return OK;
}

u8 do_tcp_cancel_cell_alarm(SOCKET s, u8 mode, u8 data)
{
	u8 i;

	if(data & (1<<i))
	{
		sys.cell.cell_enable |=  1<<i;
		sys.cell.cell_group--;
	}
	save_cell_enable_to_eeprom(sys.cell.cell_enable);
	save_cell_group_to_eeprom(sys.cell.cell_group);
	return OK;
}

u8 do_tcp_get_balance_rsp(SOCKET s, u8 mode, u8 data)
{
	u8 cmd[205];
	u8 l_text[512]={0};
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 200;
	cmd[2] = NET_TCP_GET_BALANCE_RSP;

	if(gsm_get_balance(l_text) == OK)
		memcpy(cmd+3, l_text, 200);
	cmd[203] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	return OK;
}



u8 do_send_ok(SOCKET s, u8 mode, u8 cmd)
{
	u8 snd[5];

	if(mode)
		snd[0] = FRAME_HEAD;
	else
		snd[0] = GPRS_FRAME_HEAD;
	snd[1] = 0x01;
	snd[2] = cmd;
	snd[3] = NET_TCP_RSP_OK;
	snd[4] = calcfcs(snd+1, snd[1]+2);

	if(mode)
		send(s, snd, snd[1]+4, (bool)0);
	else
		gsm_gprs_send(snd, snd[1]+4);
	
	return OK;
}


int recv_one_command(SOCKET s, u8 mode, u8* rcv, u8* one_command)
{
	uint16 real_len, recv_len, len=0;
	u8 ch=0;
	u8 ret=ERROR;
	u8 recv_flag=10;
	if(mode)
	{	
		if((len = getSn_RX_RSR(s))>0)
		{
			while(ch != FRAME_HEAD && (len>0))
			{
				if(recv_n_char(s, &ch, 1) == OK)
					len--;
				else
					goto error;
			}
			if((ch == FRAME_HEAD) && (len > 0))
			{
				if(recv_n_char(s, rcv, 1) == OK)
					len--;
				else
					goto error;
			}
			else 
				goto error;

			while((len = getSn_RX_RSR(s)) < (rcv[0]+2) && recv_flag )
			{
				Delay_ms(100);
				recv_flag--;
			}

			if(len >= rcv[0]+2)
			{
				if(recv_n_char(s, rcv+1, rcv[0]+2) == OK)
				{
					*one_command = 1;
					ret = OK;
				}
			}
				
		}

	#if 0	
		if ((getSn_RX_RSR(s)) > 0) 
		{	
			//printf("recv len: 0x%x\n", len);
			while(ch != FRAME_HEAD)
				recv(s, &ch, 1);
			//printf("ch: 0x%x\n", ch);
			real_len = recv(s, rcv, 1);
			//printf("frame len: 0x%x, real_len: 0x%x\n", rcv[0], real_len);
			real_len = recv(s, rcv+1, rcv[0]+2);
			//printf("real_len: 0x%x\n", real_len);
			*one_command = 1;
		}
	#endif
	}
	else
		recv_one_cmd_from_gprs(rcv, one_command);

error:
	
	return ret;
}
int recv_loop(SOCKET s, u8 mode, u8 cmd)
{	
	int ret=ERROR;
	uint16 len;
	u8 rcv[512];
	u8 cell_num[12];
	u8 text_content[17];
	int i;
	
	u8 one_command=0;
	
	recv_one_command(s, mode, rcv, &one_command);
	if(one_command)
	{	
		if (rcv[1] == cmd)
			ret = OK;
			
		len = rcv[0];
		if(rcv[1] == NET_TCP_SET_DETECTOR_SENS)
			;
		else if(rcv[1] == NET_TCP_ALIVE_RSP)
			//printf("recv tcp live in recv_loop\n");
			;
		else if(rcv[1] == NET_TCP_ARMING)
		{
			printf("recv arming order in recv_loop\n");
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{
				sys.arming = *(u32*)(rcv+2);
				printf("recv arming: 0x%x\n", sys.arming);
				//save_arming_to_flash(arming);
				save_arming_to_eeprom(sys.arming);
				if(!sys.arming)
					do_disarm_link();
				do_send_ok(s, mode, NET_TCP_ARMING_RSP);
			}
		}
		else if(rcv[1] == NET_TCP_DISARMING)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{
				sys.arming = *(u32*)(rcv+2);
				printf("recv disarming: 0x%x\n", sys.arming);
				//save_arming_to_flash(arming);
				save_arming_to_eeprom(sys.arming);
				do_send_ok(s, mode, NET_TCP_DISARMING_RSP);
			}
		}
		else if(rcv[1] == NET_TCP_SET_TIME)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{
				ds1302_set_time(rcv);
				do_send_ok(s, mode, NET_TCP_SET_TIME_RSP);
			}
		}
		else if(rcv[1] == NET_TCP_GET_TIME)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_get_time(s, mode);
				;
		}
		else if(rcv[1] == NET_TCP_STAT_GET)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_stat_get(s, mode);
		}	
		else if(rcv[1] == NET_TCP_LINK_SET)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_link_set(s, rcv, mode);
		}	

		else if(rcv[1] == NET_TCP_SET_CELL_NUM)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{
				if(rcv[2] == 1)
				{
					sys.cell_alarm = 1;
					printf("recv set sell num\n");
					do_tcp_set_cell_num(s, rcv, mode);
					
				
					Delay_ms(20);
					get_cell_num_from_eeprom(0, cell_num);
					//for(i=0; i<11; i++)
						//printf("cell_num: %d\n", cell_num[i]);
				}
				else 
					sys.cell_alarm = 0;
			}
		}
		else if(rcv[1] == NET_TCP_SET_TEXT_NUM)
		{
			printf("recv set text num 1\n");
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{	
				printf("recv set text num 2\n");
				do_tcp_set_cell_text(s, rcv, mode, rcv[2]);;
		
				
				Delay_ms(20);
				get_text_num_from_eeprom(rcv[2], cell_num);
				for(i=0; i<11; i++)
					printf("text_num: %d\n", cell_num[i]);
				get_text_content_from_eeprom(rcv[2], sys.text.text_length[0], text_content);
				printf("text_content: %s\n", text_content);
				
			}
		}
		else if(rcv[1] == NET_TCP_SET_DETECTOR_SENS)
			;
		else if(rcv[1] == NET_TCP_GET_ARM_STAT)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_get_arm_stat(s, mode);
		}
		else if(rcv[1] == NET_TCP_GET_LINK_STAT)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_get_link_stat(s, mode);
		}
		else if(rcv[1] == NET_TCP_GET_DETECTOR_SENS)
			;
		else if(rcv[1] == NET_TCP_GET_BALANCE)
			;
		else if(rcv[1] == NET_TCP_CANCEL_TEXT_ALARM)
			do_tcp_cancel_text_alarm(s, mode, rcv[2]);
		else if(rcv[1] == NET_TCP_CANCEL_CELL_ALARM)
			do_tcp_cancel_cell_alarm(s, mode, rcv[2]);
		else if(rcv[1] == NET_TCP_GET_BALANCE)
			do_tcp_get_balance_rsp(s, mode, rcv[2]);
		else if(rcv[1] == NET_TCP_INPUT_ALARM_STAT_RSP)
			try_again[NET_TCP_INPUT_ALARM_STAT] = 0;
		else if(rcv[1] == NET_TCP_DETECTOR_LOSS_RSP)
			try_again[NET_TCP_DETECTOR_LOSS] = 0;
		else if(rcv[1] == NET_TCP_ALARM_BATTERY_LOW_POWER_RSP)
			try_again[NET_TCP_ALARM_BATTERY_LOW_POWER] = 0;
		else if(rcv[1] == NET_TCP_ALARM_NET_LOSS_RSP)
			try_again[NET_TCP_ALARM_NET_LOSS] = 0;
		else if(rcv[1] == NET_TCP_ALARM_POWEROFF_RSP)
			try_again[NET_TCP_ALARM_POWEROFF] = 0;
		else if(rcv[1] == NET_TCP_UPLOAD_ARM_STAT_RSP)
			try_again[NET_TCP_UPLOAD_ARM_STAT] = 0;
		else 
			;
	}
	
	return ret;
}

int do_tcp_alarm_input_stat(SOCKET s, u32 data, u8 mode)
{	
	u8 cmd[9];


	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 4;
	cmd[2] = NET_TCP_INPUT_ALARM_STAT;
	cmd[3] = data & 0xff;
	cmd[4] = (data & 0xff00)>>8;
	cmd[5] = (data & 0xff0000)>>16;
	cmd[6] = (data & 0xff000000)>>24;
	cmd[7] = calcfcs(cmd+1,cmd[1]+3);

	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	if(!do_common_rsp(s, mode, NET_TCP_INPUT_ALARM_STAT_RSP))
		try_again[NET_TCP_INPUT_ALARM_STAT] = 0;
	else
	{	
		try_again[NET_TCP_INPUT_ALARM_STAT] = 1;
		try_data[NET_TCP_INPUT_ALARM_STAT] = data;
	}
	
	return OK;
	
}

int do_tcp_detector_loss(SOCKET s, u32 data, u8 mode)
{	
	u8 cmd[9];

	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 4;
	cmd[2] = NET_TCP_DETECTOR_LOSS;
	cmd[3] = data & 0xff;
	cmd[4] = (data & 0xff00)>>8;
	cmd[5] = (data & 0xff0000)>>16;
	cmd[6] = (data & 0xff000000)>>24;
	cmd[7] = calcfcs(cmd+1,3);

	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	if(!do_common_rsp(s, mode, NET_TCP_DETECTOR_LOSS_RSP))
		try_again[NET_TCP_DETECTOR_LOSS] = 0;
	else
	{	
		try_again[NET_TCP_DETECTOR_LOSS] = 1;
		try_data[NET_TCP_DETECTOR_LOSS] = data;
	}
	
	return OK;
	
}

int do_tcp_net_loss()
{	
	u8 cmd[4];

	
	cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 0;
	cmd[2] = NET_TCP_ALARM_NET_LOSS;
	cmd[3] = calcfcs(cmd+1,2);

	gsm_gprs_send(cmd, cmd[1]+4);
	if(!do_common_rsp(0, 0, NET_TCP_ALARM_NET_LOSS_RSP))
		try_again[NET_TCP_ALARM_NET_LOSS]=0;
	else
		try_again[NET_TCP_ALARM_NET_LOSS]=1;
	
	return OK;
}

int do_tcp_poweroff(SOCKET s, u8 mode)
{
	u8 cmd[4];

	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 0;
	cmd[2] = NET_TCP_ALARM_POWEROFF;
	cmd[3] = calcfcs(cmd+1,2);

	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	if(!do_common_rsp(s, mode, NET_TCP_ALARM_POWEROFF_RSP))
		try_again[NET_TCP_ALARM_POWEROFF] = 0;
	else
		try_again[NET_TCP_ALARM_POWEROFF] = 1;
	
	return OK;
}

int do_tcp_lowbatterypow(SOCKET s, u8 mode)
{
	u8 cmd[4];


	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 0;
	cmd[2] = NET_TCP_ALARM_BATTERY_LOW_POWER;
	cmd[3] = calcfcs(cmd+1,2);

	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	if(!do_common_rsp(s, mode, NET_TCP_ALARM_BATTERY_LOW_POWER_RSP))
		try_again[NET_TCP_ALARM_BATTERY_LOW_POWER] = 0;
	else
		try_again[NET_TCP_ALARM_BATTERY_LOW_POWER] = 1;
	
	return OK;
}


int do_tcp_upload_arm_stat(SOCKET s, u8 val, u8 mode)
{	
	u8 cmd[9];

	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 4;
	cmd[2] = NET_TCP_UPLOAD_ARM_STAT;
	if(val)
	{
		cmd[3] = 0xff;
		cmd[4] = 0xff;
		cmd[5] = 0xff;
		cmd[6] = 0xff;
	}
	else
	{
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x00;
		cmd[6] = 0x00;
	}
	cmd[7] = calcfcs(cmd+1,6);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	if(!do_common_rsp(s, mode, NET_TCP_UPLOAD_ARM_STAT_RSP))
		try_again[NET_TCP_UPLOAD_ARM_STAT]= 0;
	else
	{
		try_data[NET_TCP_UPLOAD_ARM_STAT] = val;
		try_again[NET_TCP_UPLOAD_ARM_STAT]= 1;
	}
	
	return OK;
}


u8 check_live_rsp(u8* cmd, int len)
{
	u8 ret=ERROR;

	if((len == 4) && cmd[0] == 0xfe && cmd[2] == NET_TCP_ALIVE_RSP)
	{
		ret = OK;
	}


	return ret;
}

int do_tcp_alive(SOCKET s, u8 mode)
{	
	u8 cmd[4];
	int ret, time_flag, gprs_time_flag, a=ERROR;
	static u8 i=0;
	
	static uint32_t keep_alive_time;
	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = calcfcs(cmd+1,2);
	


	if (!mode && (sys.gprs_flag > 5))
	{	
		//text_alarm();
		//do_cell_alarm();
		printf("cannot recv gprs live, and text and cell alarm\n");
	}


	if (mode && (sys.tcp_flag > 5))
	{
		printf("cannot recv tcp live\n");
	}
	
	
	if (time_return()- keep_alive_time > (tick_second*3))
	{	
		if(!mode)
		{
			gsm_gprs_send(cmd, cmd[1]+4);
			gprs_time_flag=10;
			while(((a=recv_loop(s, mode, NET_TCP_ALIVE_RSP))!= OK && gprs_time_flag))
			{	
				gprs_time_flag--;
				//printf("wait for live rsp in while\n");
				Delay_ms(500);
			}
			if(a==OK)
			{
				printf("recv gprs live\n");
				sys.gprs_flag=0;
			
			}
			else
			{
				printf("recv gprs live timeout\n");
				sys.gprs_flag++;
			}

			#if 0
			//check if connected server in tcp mode every ten second when in gprs mode
			if(i==5)
			{	
				i = 0;
				//if(ch_status[s] == 2)
				{
					cmd[0] = FRAME_HEAD;
					ret = send(s, cmd, 4, (bool)0);
					if (ret != 0)
					{
						time_flag=10;
						while(((a=recv_loop(s, TCP_MODE, NET_TCP_ALIVE_RSP))!= OK && time_flag))
						{	
							time_flag--;
							//printf("wait for live rsp in while\n");
							Delay_ms(100);
						}
						if(a==OK)
						{
							//sys.need_in_net = 1;
							printf("recv live rsp from net in gprs mode\n");
						}
						else
						{
							sys.tcp_flag=0;
							printf("recv live rsp from net in gprs mode\n");
						}
					}
				}

			}
			else
				i++;
			#endif
		}
		else
		{
			ret = send(s, cmd, 4, (bool)0);
			time_flag=10;
			while(((a=recv_loop(s, mode, NET_TCP_ALIVE_RSP))!= OK && time_flag))
			{	
				time_flag--;
				//printf("wait for live rsp in while\n");
				Delay_ms(100);
			}
			if(a==OK)
			{
				printf("recv tcp live in net mode\n");
				sys.tcp_flag=0;
			}
			else
			{	
				printf("recv tcp live in net mode timeout\n");
				sys.tcp_flag++;
			}
			
		}
		
		keep_alive_time = time_return();
	}
	
	return OK;
}

u8 get_alarm_stat(SOCKET s, u32* status)
{
	u8 i;
	u32 stat=0xffff0000;

	for(i=0; i<16; i++)
		stat |= (get_alarm_in(i)<<i);
	//printf("send alarm, stat: 0x%x\n", stat);
	//if ((stat & 0xffff) != 0xffff)

	*status = ~stat;
	
	return OK;
}

u8 get_poweroff_stat(SOCKET s, u8 mode)
{
	u8 ret;
	static u8 stat=0;
	ret = is_poweroff();
	if(stat != ret)
	{
		if (ret)
			do_tcp_poweroff(s, mode);
	}
	stat = ret;
	return OK;
}

u8 get_lowbatterypow_stat(SOCKET s, u8 mode)
{
	u8 ret;
	static u8 stat=0;
	ret = is_battery_lowpow();
	
	if(stat != ret)
	{
		if (ret)
			do_tcp_lowbatterypow(s, mode);
	}
	stat = ret;
	return OK;
}
u8 get_arm_stat(SOCKET s, u8 mode)
{
	u8 ret;
	static u8 stat=1;
	static u8 disarm=1;

	
	ret = get_key(0);
	if(stat != ret)
	{
		if (ret == 0)
			do_tcp_upload_arm_stat(s, 1, mode);
	}
	stat = ret;
	
	ret = get_key(1);
	if(disarm != ret)
	{
		if (ret == 0)
			do_tcp_upload_arm_stat(s, 0, mode);
	}
	disarm = ret;
	
	return OK;
}

u8 cruise_io(SOCKET s, u8 mode)
{
	static uint32_t cruise_interval;
	u32 status=0;
	u8 i, press_value;
	
	if(sys.arming != 0)
	{	
		//printf("in arimng cruise\n");
		if (time_return()- cruise_interval > (tick_second*1))
		{	
			get_alarm_stat(s, &status);
			if(status)

			{
				do_tcp_alarm_input_stat(s, status, mode);
				do_alarm_link(status);
			}
			cruise_interval = time_return();
		}
	}

	get_poweroff_stat(s, mode);
	get_lowbatterypow_stat(s, mode);
	get_arm_stat(s, mode);

	return OK;
}

//0:GPRS mode 1:TCP mode

u8 check_net_mode(SOCKET s, u8* mode)
{
	u8 ret=ERROR;
	static u8 check=0;
	static uint32_t net_check_time;
	
	if (check > 5)
	{
		
		if(sys.gprs_flag > 5)
		{
			*mode = 1;
			printf("going in net mode cause of didn't recv live rsp timeout\n");
		}
		else
		{
			*mode = 0;
			printf("going in gprs mode cause of cann't recv live in net mode\n");
		}
			
	}

	if (time_return()- net_check_time > (tick_second*3))
	{
		//if not connected
		if (ch_status[s] != 2)
			check++;
		/*if connected without receiving tcp live*/
		else if((ch_status[s] == 2)&& (sys.tcp_flag>5))
			check++;
		else
		{	
			*mode = 1;
			check=0;
			printf("going in tcp mode because connected established, ch_status[s]: %d\n", ch_status[s]);
		}

		net_check_time = time_return();
	}
	ret = OK;
	return ret;
}


u8 set_tcp_utils()
{
	set_current_time(&presentTime);
	set_current_time(&presentTime1);
	set_current_time(&try_again_time);
	return OK;
}


u8 check_try_again(SOCKET s, u8 mode)
{
	
	if(time_return() - try_again_time >= (tick_second * 5)) 
	{
		try_again_time = time_return();
		//printf("SOCK_ESTABLISHED\n");
			
		if(try_again[NET_TCP_INPUT_ALARM_STAT])
			do_tcp_alarm_input_stat(s,try_data[NET_TCP_INPUT_ALARM_STAT],mode);

		if(try_again[NET_TCP_DETECTOR_LOSS])
			do_tcp_detector_loss(s,try_data[NET_TCP_DETECTOR_LOSS], mode);

		if(try_again[NET_TCP_ALARM_BATTERY_LOW_POWER])
			do_tcp_lowbatterypow(s,mode);

		if(try_again[NET_TCP_ALARM_NET_LOSS])
			do_tcp_net_loss();
		
		if(try_again[NET_TCP_ALARM_POWEROFF])
			do_tcp_poweroff(s,mode);

		if(try_again[NET_TCP_UPLOAD_ARM_STAT])
			do_tcp_upload_arm_stat(s,try_data[NET_TCP_UPLOAD_ARM_STAT], mode);
	}
}

void loopback_tcpc(SOCKET s, uint16 port, u8 mode)
{
	uint16 RSR_len;
	uint8 * data_buf = TX_BUF;

	
	if(!mode && sys.gsm_init_flag && sys.gprs_init_flag)
	{
		do_tcp_alive(s, mode);
		cruise_io(s, mode);
		recv_loop(s, mode, NET_TCP_VALID_CMD);
		check_try_again(s, mode);
	}
	
	switch (getSn_SR(s))
	{
	case SOCK_ESTABLISHED:                 /* if connection is established */
		//printf("SOCK_ESTABLISHED\n");
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			presentTime = time_return();
			printf("SOCK_ESTABLISHED\n");
		}
		if(ch_status[s]==1 || ch_status[s] == 4)
		{
			 printf("\r\n%d : Connected",s);
			 ch_status[s] = 2;
		}

		do_tcp_alive(s, mode);
		cruise_io(s, mode);
		recv_loop(s, mode, NET_TCP_VALID_CMD);
		check_try_again(s, mode);

		break;
	case SOCK_CLOSE_WAIT:  
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			presentTime = time_return();
			printf("SOCK_CLOSE_WAIT\n");
		}
		/* If the client request to close */
		//printf("\r\n%d : CLOSE_WAIT", s);
		if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
		{
			 if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
			                                                                            /* the data size to read is MAX_BUF_SIZE. */
			 recv(s, data_buf, RSR_len);         /* read the received data */
		}
		disconnect(s);
		ch_status[s] = 0;
		break;
	case SOCK_CLOSED: 
		//printf("SOCK_CLOSED\n");
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			presentTime = time_return();
			printf("SOCK_CLOSED\n");
		}
		/* if a socket is closed */
		if(!ch_status[s])
		{
			 printf("\r\n%d : Loop-Back TCP Client Started. port: %d", s, port);
			 ch_status[s] = 1;
		}
		if(socket(s, Sn_MR_TCP, sys.net_para.any_port++, 0x00) == 0)    /* reinitialize the socket */
		{
			 printf("\a%d : Fail to create socket.",s);
			 ch_status[s] = 0;
		}
		break;
	case SOCK_INIT:     /* if a socket is initiated */
		//printf("SOCK_INIT\n");
		if(ch_status[s] == 2)
			ch_status[s] = 4;
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			connect(s, sys.dest_net_para.destip, sys.dest_net_para.port);
			presentTime = time_return();
			printf("SOCK_INIT destip: %d.%d.%d.%d. port: %d\n", sys.dest_net_para.destip[0], sys.dest_net_para.destip[1], 
				sys.dest_net_para.destip[2], sys.dest_net_para.destip[3], sys.dest_net_para.port);
		}
		break;
	default:
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			presentTime = time_return();
			printf("default\n");
		}
		break;
		
	}
}
u8 do_tcp_test_data(SOCKET s, u8 mode)
{
	u8 cmd[12];
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 4;
	cmd[2] = NET_TCP_TEST;

	*(s32*)(cmd+3) = test_data;
	cmd[10] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_gprs_send(cmd, cmd[1]+4);
	return OK;
}

void loopback_tcp_test(SOCKET s, uint16 port, u8 mode)
{
	uint16 RSR_len;
	uint8 * data_buf = TX_BUF1;
	u16 l_port = sys.net_para.any_port+100;
	

	
	switch (getSn_SR(s))
	{
	case SOCK_ESTABLISHED:                 /* if connection is established */
		//printf("SOCK_ESTABLISHED\n");
		if(time_return() - presentTime1 >= (tick_second * 2)) 
		{  
			presentTime1 = time_return();
			do_tcp_test_data(s, mode);
			printf("SOCK_ESTABLISHED\n");
		}
		if(ch_status[s]==1 || ch_status[s] == 4)
		{
			 printf("\r\n%d : Connected",s);
			 ch_status[s] = 2;
		}

		

		break;
	case SOCK_CLOSE_WAIT:  
		if(time_return() - presentTime1 >= (tick_second * 2)) 
		{  
			presentTime1 = time_return();
			printf("SOCK_CLOSE_WAIT\n");
		}
		/* If the client request to close */
		//printf("\r\n%d : CLOSE_WAIT", s);
		if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
		{
			 if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
			                                                                            /* the data size to read is MAX_BUF_SIZE. */
			 recv(s, data_buf, RSR_len);         /* read the received data */
		}
		disconnect(s);
		ch_status[s] = 0;
		break;
	case SOCK_CLOSED: 
		//printf("SOCK_CLOSED\n");
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			presentTime = time_return();
			printf("SOCK_CLOSED\n");
		}
		/* if a socket is closed */
		if(!ch_status[s])
		{
			 printf("\r\n%d : Loop-Back TCP Client Started. port: %d", s, port);
			 ch_status[s] = 1;
		}
		if(socket(s, Sn_MR_TCP, l_port++, 0x00) == 0)    /* reinitialize the socket */
		{
			 printf("\a%d : Fail to create socket.",s);
			 ch_status[s] = 0;
		}
		break;
	case SOCK_INIT:     /* if a socket is initiated */
		//printf("SOCK_INIT\n");
		if(ch_status[s] == 2)
			ch_status[s] = 4;
		if(time_return() - presentTime1 >= (tick_second * 2)) 
		{  
			connect(s, sys.dest_net_para.destip, sys.dest_net_para.port+1);
			presentTime1 = time_return();
			printf("SOCK_INIT destip: %d.%d.%d.%d. port: %d\n", sys.dest_net_para.destip[0], sys.dest_net_para.destip[1], 
				sys.dest_net_para.destip[2], sys.dest_net_para.destip[3], sys.dest_net_para.port+1);
		}
		break;
	default:
		if(time_return() - presentTime1 >= (tick_second * 2)) 
		{  
			presentTime1 = time_return();
			printf("default\n");
		}
		break;
	}
}



