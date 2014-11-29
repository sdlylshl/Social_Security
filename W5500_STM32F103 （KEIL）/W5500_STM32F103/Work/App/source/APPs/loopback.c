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


#define tick_second 1
#define MAX_TIMES_CHECK_FRAME 50
//指令接收表
struct msgStu netRecvDataCMDS[RECV_CMDS_NUM];


extern uint8 ch_status[MAX_SOCK_NUM];
extern CHCONFIG_TYPE_DEF Chconfig_Type_Def;
extern uint32_t presentTime;
extern uint32_t keep_alive_time;
extern uint32_t cruise_interval;
extern uint32_t net_check_time;


uint16 any_port = 1000;


//u8 cmd1[32];
u8 tcp_flag=0;
u8 gprs_flag=0;
u32 arming;
u8 is_net_loss;

u32 in[16];
u16 out[16];
u8 xor[16];
u8 group;

u8 cell[5][12];
u8 cell_group;
u8 cell_xor;

u8 text[5][12];
u8 text_group;
u8 text_content[5][140];




typedef struct
{
	u8 head;
	u8 len;
	u8 cmd;
	u8 checkout;
}FRAME;








unsigned char calcfcs(unsigned char *pmsg, unsigned char len)
{
	unsigned char result = 0;
	while (len--)
	{
		result ^= *pmsg++;
	}
	return result;
}

int do_common_rsp(SOCKET s)
{
	int ret=1;
	u8 rcv[4];
	
	if ((getSn_RX_RSR(s)) > 0) 
	{	
		ret = recv(s, rcv, 4);
		if (ret == 4)
		{
			if(rcv[3] == calcfcs(rcv+1,2))
			{
				if(rcv[3] == NET_TCP_RSP_OK)
					return OK;
			}
		}
	}

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
		gsm_usart2_send(cmd, cmd[1]+4);
	//do_common_rsp(s);
    return OK;
}

u8 do_tcp_stat_get(SOCKET s, u8 mode)
{	
	u8 i;
	u8 cmd[20];
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 16;
	cmd[2] = NET_TCP_STAT_RSP;

	for(i=0; i<16; i++)
		*(u32*)(cmd+3) |= (get_alarm_in(i)<<i);
	
	for(i=0; i<8; i++)
		*(u16*)(cmd+7) |= (get_alarm_out(i)<<i);

	get_ip_addr((u32*)(cmd+9));
	get_id((u32*)(cmd+13));

	*(cmd+17) |= is_net_loss;
	*(cmd+17) |= is_poweroff() << 1;
	*(cmd+17) |= is_power_low() << 2;
	
	cmd[19] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_usart2_send(cmd, cmd[1]+4);
	//do_common_rsp(s);
	return OK;
}


u8 do_tcp_link_set(SOCKET s, u8* rcv)
{
	u8 a, i;

	a = (rcv[0]-3)/7;
	group = a;
	for(i=0; i<group; i++)
	{
		in[i] = *(u32*)(rcv+2+(2+1)*i);
		out[i] = *(u16*)(rcv+2+(4+1)*i);
		xor[i] = *(rcv+2+(4+2)*i);
		save_link_to_flash(i, in[i], out[i], xor[i]);
	}
	do_common_rsp(s);
	return OK;
}

u8 do_tcp_get_arm_stat(SOCKET s, u8 mode)
{
	u8 cmd[12];
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 7;
	cmd[2] = NET_TCP_GET_ARM_STAT_RSP;

	*(u32*)(cmd+3) = arming;
	cmd[10] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_usart2_send(cmd, cmd[1]+4);
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
	cmd[1] = 3+group*7;
	cmd[2] = NET_TCP_GET_LINK_STAT_RSP;

	for(i=0; i<group; i++)
	{
		*(u32*)(cmd+3+(2+1)*i) = in[i];
		*(u16*)(cmd+3+(4+1)*i) = out[i];
		*(cmd+3+(4+2)*i) = xor[i];
	}
	cmd[cmd[1]+3] = calcfcs(cmd+1, cmd[1]+2);
	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_usart2_send(cmd, cmd[1]+4);
	return OK;
}

u8 do_tcp_set_cell_num(SOCKET s, u8* rcv)
{
	u8 len,i,j;

	len = rcv[0];
	cell_group = (len-1)/11;

	cell_xor = rcv[2];
	for(i=0; i<cell_group; i++)
	{
		for(j=0; j<11; j++)
			cell[i][j] = *(rcv+3+i*j+j);
		save_cell_num(i, cell[i]);
	}
	return OK;
}

u8 do_tcp_set_cell_text(SOCKET s, u8* rcv)
{
	u8 len[5],i,j,old=0;

	text_group = rcv[2];

	for(i=0; i<text_group; i++)
	{	
		len[i] = *(rcv+13+11*i+old);
		for(j=0; j<11; j++)
			text[i][j] = *(rcv+3+11*i+old+j);
		save_text_num(i, text[i]);
		for(j=0; j<len[i]; j++)
			text_content[i][j] = *(rcv+3+11*i+1+old+j);
		save_text_content(i, len[i], text_content[i]);
		old += len[i];
	}
	return OK;
}

u8 do_send_ok(SOCKET s, u8 mode)
{
	u8 snd[4];

	if(mode)
		snd[0] = FRAME_HEAD;
	else
		snd[0] = GPRS_FRAME_HEAD;
	snd[1] = 0x00;
	snd[2] = 0x02;
	snd[3] = 0x02;

	if(mode)
		send(s, snd, snd[1]+4, (bool)0);
	else
		gsm_usart2_send(snd, snd[1]+4);
	
	return OK;
}

/**************************************
*
* 已经检测到帧头的指令的数据内容的长度
*
***************************************/
uint16 cmdLen = 0;

/**************************************
*
*	上一次的接收中已经检测到帧头及len
*
***************************************/
u8 flagGetFrame = 0;
int recv_one_command(SOCKET s, u8 mode, u8* rcv, u8* one_command)
{
	uint16 len = 0, real_len = 0;
	
	u8 ret=ERROR;
	if(mode)
	{ 
		if ((len = getSn_RX_RSR(s)) > 0) 
		{	
			//上一次的接收中已经检测到帧头及len
			u16 dat_Used = 0;
			
			while (dat_Used < len)
			{
				if (flagGetFrame == 0)
				{
					//printf("recv len: 0x%x\n", len);
					u8 ch = 0;

					while (ch != FRAME_HEAD && dat_Used < len && dat_Used < MAX_TIMES_CHECK_FRAME)
					{
						recv(s, &ch, 1);
						dat_Used++;
					}
					if (ch == FRAME_HEAD)
					{
						//printf("ch: 0x%x\n", ch);
						real_len = recv(s, rcv, 1);
						dat_Used++;
						//printf("frame len: 0x%x, real_len: 0x%x\n", rcv[0], real_len);
						cmdLen = rcv[0];
						flagGetFrame = 1;
						//real_len = recv(s, rcv+1, rcv[0]+2);
						//printf("real_len: 0x%x\n", real_len);
					}
				}
				if (flagGetFrame == 1)
				{
					if (len - dat_Used >= cmdLen + 1)
					{
						int i = 0;
						struct msgStu *pNmsgR;
						uint8_t *psndat;
						uint32_t crc,crc_stm32;
						
            pNmsgR = get_NetRecvDataCMDS();
						if (pNmsgR == NULL)
						{
								//return;
						}
						pNmsgR->usable = 1;
						pNmsgR->head = FRAME_HEAD;
						pNmsgR->len = cmdLen;
						recv(s, pNmsgR->sn, 2);
						dat_Used += 2;	
						//pNmsgR->sn[0] = 0;
						//pNmsgR->sn[1] = 0;
						
						real_len = recv(s, pNmsgR->data, cmdLen );
						dat_Used += real_len;
						recv(s,pNmsgR->crc,1);
						recv(s,pNmsgR->crc + 1,1);
						recv(s,pNmsgR->crc + 2,1);
						recv(s,pNmsgR->crc + 3,1);
						recv(s,&pNmsgR->endl,1);	
						dat_Used += 5;						
						
						psndat = &pNmsgR->len;
						CRC_ResetDR();
						for (i = 0; i < cmdLen + 3; i++)
						{
								CRC_CalcCRC((uint32_t)(*psndat++));
						}
						crc_stm32 = CRC_GetCRC();
						crc = (pNmsgR->crc[0] << 24) | (pNmsgR->crc[1] << 16) | (pNmsgR->crc[2] << 8) | (pNmsgR->crc[3]);
						if (crc == crc_stm32 && pNmsgR->endl == FRAME_END)
						{
								pNmsgR->usable = 2;
								NET_parseInstruction(pNmsgR);
						}
						else
						{
								//校验失败
								pNmsgR->usable = 0;
						}
										
						printf("send : ");
						for (i = 0; i < pNmsgR->len; i++)
							printf("%2X ", pNmsgR->data[i]);
						printf("\n");
						send(s,&pNmsgR->head,1,(bool)0);
						send(s,&pNmsgR->len,1,(bool)0);
						send(s,pNmsgR->sn,2,(bool)0);
						send(s,pNmsgR->data,pNmsgR->len,(bool)0);
						send(s,pNmsgR->crc,4,(bool)0);
						send(s,&pNmsgR->endl,1,(bool)0);
						flagGetFrame = 0;
						cmdLen = 0;
					}
					else
					{
						break;
					}
				}
			}
			*one_command = 1;
		}
	}
	else
		recv_one_cmd_from_gprs(rcv, one_command);
	
	return ret;
}
int recv_loop(SOCKET s, u8 mode)
{	
	int ret=1;
	uint16 len;
	u8 rcv[512];
	
	u8 one_command=0;
	
	recv_one_command(s, mode, rcv, &one_command);
	if(one_command)
	{
		len = rcv[0];
		if(rcv[1] == NET_TCP_SET_DETECTOR_SENS)
			;
		else if(rcv[1] == NET_TCP_ALIVE_RSP)
			printf("recv tcp live in recv_loop\n");
		else if(rcv[1] == NET_TCP_ARMING)
		{
			printf("recv arming order in recv_loop\n");
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{
				arming = *(u32*)(rcv+2);
				printf("recv arming: 0x%x\n", arming);
				save_arming_to_flash(arming);
				do_send_ok(s, mode);
			}
		}
		else if(rcv[1] == NET_TCP_DISARMING)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
			{
				arming = *(u32*)(rcv+2);
				printf("recv disarming: 0x%x\n", arming);
				save_arming_to_flash(arming);
				do_send_ok(s, mode);
			}
		}
		else if(rcv[1] == NET_TCP_SET_TIME)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				ds1302_set_time(rcv);
				;
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
				do_tcp_link_set(s, rcv);
		}	

		else if(rcv[1] == NET_TCP_SET_CELL_NUM)
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_set_cell_num(s, rcv);
		else if(rcv[1] == NET_TCP_SET_TEXT_NUM)
		{
			if(rcv[len+2] == calcfcs(rcv, len+2))
				do_tcp_set_cell_text(s, rcv);;
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
		else
			;
	}
	
	ret = OK;
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
		gsm_usart2_send(cmd, cmd[1]+4);
	//do_common_rsp(s);
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
		gsm_usart2_send(cmd, cmd[1]+4);
	do_common_rsp(s);
	return OK;
	
}

int do_tcp_battery_low_power(SOCKET s, u8 mode)
{
	u8 cmd[5];
	
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
		gsm_usart2_send(cmd, cmd[1]+4);
	do_common_rsp(s);
	return OK;
}

int do_tcp_net_loss(SOCKET s, u8 mode)
{	
	u8 cmd[4];
	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 0;
	cmd[2] = NET_TCP_ALARM_NET_LOSS;
	cmd[3] = calcfcs(cmd+1,2);

	if(mode)
		send(s, cmd, cmd[1]+4, (bool)0);
	else
		gsm_usart2_send(cmd, cmd[1]+4);
	do_common_rsp(s);
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
		gsm_usart2_send(cmd, cmd[1]+4);
	do_common_rsp(s);
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
		gsm_usart2_send(cmd, cmd[1]+4);
	do_common_rsp(s);
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
	int ret, len, time_flag;
	static u8 i=0;
	
	if(mode)
		cmd[0] = FRAME_HEAD;
	else
		cmd[0] = GPRS_FRAME_HEAD;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = calcfcs(cmd+1,2);
	


	if (!mode && (gprs_flag > 5))
		printf("cannot recv gprs live\n");


	//if (mode && (tcp_flag > 5))
	//	printf("cannot recv tcp live\n");
	
	
	if (time_return()- keep_alive_time > (tick_second*1))
	{	
		if(!mode)
		{
			gsm_usart2_send(cmd, cmd[1]+4);
			if(parse_gprs_live_rsp())
				gprs_flag++;
			else
				gprs_flag=0;

			//check if connected server in tcp mode every ten second when in gprs mode
			if(i==5)
			{	
				i = 0;
				cmd[0] = FRAME_HEAD;
				ret = send(s, cmd, 4, (bool)0);
				
				time_flag=10;
				while((len=getSn_RX_RSR(s))== 0 && time_flag)
				{	
					time_flag--;
					//printf("wait for live rsp in while\n");
					Delay_ms(100);
				}
				if(!time_flag)
					printf("recv tcp live timeout\n");
				if ((len = getSn_RX_RSR(s)) > 0) 
				{	
					ret = recv(s, cmd, len);
					ret = check_live_rsp(cmd, ret);
					if (ret == OK)
					{
						tcp_flag = 0;
						printf("get tcp live in tcp mode\n");
					}
					else
						tcp_flag++;	
				}
				else
					tcp_flag++;
			}
			else
				i++;
		}
		else
		{
			ret = send(s, cmd, 4, (bool)0);
			time_flag=10;
			while((len=getSn_RX_RSR(s))== 0 && time_flag)
			{	
				time_flag--;
				//printf("wait for live rsp in while\n");
				Delay_ms(100);
			}
			if(!time_flag)
				printf("recv tcp live timeout\n");
			if ((len = getSn_RX_RSR(s)) > 0) 
			{	
				ret = recv(s, cmd, len);
				ret = check_live_rsp(cmd, ret);
				if (ret == OK)
				{
					tcp_flag = 0;
					printf("get tcp live in tcp mode\n");
				}
				else
					tcp_flag++;	
			}
			else
				tcp_flag++;
		}
		
		keep_alive_time = time_return();
	}
	
	return OK;
}

u8 get_alarm_stat(SOCKET s, u8 mode)
{
	u8 i;
	u32 stat=0xffff0000;

	for(i=0; i<16; i++)
		stat |= (get_alarm_in(i)<<i);
	//printf("send alarm, stat: 0x%x\n", stat);
	//if ((stat & 0xffff) != 0xffff)
	{
		stat = ~stat;
		printf("send alarm stat: 0x%x\n", stat);
		do_tcp_alarm_input_stat(s,stat, mode);
	}

	return OK;
}

u8 get_poweroff_stat(SOCKET s, u8 mode)
{
	u8 ret;
	ret = is_poweroff();
	if (ret)
		do_tcp_poweroff(s, mode);

	return OK;
}

u8 get_arm_stat(SOCKET s, u8 mode)
{
	u8 ret;
	ret = get_key(0);
	if (ret == 1)
		do_tcp_upload_arm_stat(s, 0, mode);

	ret = get_key(1);
	if (ret == 1)
		do_tcp_upload_arm_stat(s, 1, mode);
	return OK;
}

u8 cruise_io(SOCKET s, u8 mode)
{
	if(arming != 0)
	{	
		//printf("in arimng cruise\n");
		if (time_return()- cruise_interval > (tick_second*1))
		{	
			get_alarm_stat(s, mode);
			cruise_interval = time_return();
		}
	}

	//get_poweroff_stat(s);
	//get_arm_stat(s);
	return OK;
}

//0:GPRS mode 1:TCP mode

u8 check_net_mode(SOCKET s, u8* mode)
{
	u8 ret=ERROR;
	static u8 check=0;

	if (check > 5)
	{
		*mode = 0;
		printf("in gprs mode\n");
	}

	if (time_return()- net_check_time > (tick_second*1))
	{
		//if not connected
		if (ch_status[s] != 2)
			check++;
		/*if connected without receiving tcp live*/
		else if((ch_status[s] == 2)&& (tcp_flag>5))
			check++;
		else
		{	
			*mode = 1;
			check=0;
			printf("in tcp mode, ch_status[s]: %d\n", ch_status[s]);
		}
		net_check_time = time_return();
	}
	ret = OK;
	return ret;
}

///*********************** Set Socket n in TCP Server mode ************************/
//unsigned int Socket_Listen(SOCKET s)
//{
//	/* Set Socket n in TCP mode */
//	Write_SOCK_1_Byte(s, Sn_MR, MR_TCP);
//	/* Open Socket n */
//	Write_SOCK_1_Byte(s, Sn_CR, OPEN);
//
//	Delay_ms(5);	/* Wait for a moment */
//	if (Read_SOCK_1_Byte(s, Sn_SR) != SOCK_INIT)
//	{
//		Write_SOCK_1_Byte(s, Sn_CR, CLOSE);		/* Close Socket n */
//		return FALSE;
//	}
//
//	/* Set Socket n in Server mode */
//	Write_SOCK_1_Byte(s, Sn_CR, LISTEN);
//	Delay_ms(5);	/* Wait for a moment */
//	if (Read_SOCK_1_Byte(s, Sn_SR) != SOCK_LISTEN)
//	{
//		Write_SOCK_1_Byte(s, Sn_CR, CLOSE);		/* Close Socket n */
//		return FALSE;
//	}
//	return TRUE;
//}

void loopback_tcpc(SOCKET s, uint16 port, u8 mode)
{
	uint16 RSR_len;
	uint8 * data_buf = TX_BUF;

	
	//Socket n 的状态
	u8 res = getSn_SR(s);
	switch (res)
	{
	case SOCK_ESTABLISHED:                 /* if connection is established */
		//printf("SOCK_ESTABLISHED\n");
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			presentTime = time_return();
			//printf("SOCK_ESTABLISHED\n");
		}
		if(ch_status[s] == 1 || ch_status[s] == 4)
		{
			 printf("\r\n%d : Connected",s);
			 ch_status[s] = 2;
		}

		#if 0
		if ((RSR_len = getSn_RX_RSR(s)) > 0)         
		{
			printf("RSR_len: 0x%x\n", RSR_len);
			 if (RSR_len > TX_RX_MAX_BUF_SIZE) 
			 	RSR_len = TX_RX_MAX_BUF_SIZE;   
			                                                                   
			 received_len = recv(s, data_buf, RSR_len); 
			 send(s, data_buf, received_len, (bool)0);
		}
		#endif

		//do_tcp_alive(s, mode);
		//cruise_io(s, mode);
		recv_loop(s, mode);

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
		if(socket(s, Sn_MR_TCP, any_port++, 0x00) == 0)    /* reinitialize the socket */
		{
			 printf("\a%d : Fail to create socket.",s);
			 ch_status[s] = 0;
		}else{
			 printf("\a  : create socket %d %d success ",s,s);
		}
		break;
	case SOCK_INIT:     /* if a socket is initiated */
		//printf("SOCK_INIT\n");
		if(ch_status[s] == 2)
			ch_status[s] = 4;
		if(time_return() - presentTime >= (tick_second * 2)) 
		{  
			connect(s, Chconfig_Type_Def.destip, Chconfig_Type_Def.port);
			presentTime = time_return();
			printf("SOCK %d is connect \n",s);
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

uint8_t NET_parseData(struct msgStu *pNmsgR)
{
//    uint8_t i = 0;
//    uint8_t instr;
//    uint8_t *pmac;
//    uint8_t *pion;
//    uint8_t *piomode;
//    uint8_t *pname;
//    struct devTable *pdevTbs = NULL;
//		uint8_t procymode;
//    //uint8_t ctr;
//    // uint8_t *msgStu = (uint8_t *) & (pNmsgR->data[0]);
//    pNmsgR->usable = 0;
//    instr = pNmsgR->data[0];
	
		printf("NET_parseData ");
//    switch (instr)
//    {

//    case NET_CMD_REQ_ALL_DEV:
//        for (i = 0; i < MAX_DEVTABLE_NUM; i++)
//        {
//            if (devTbs[i].devstate)
//            {
//                Net_send_device(&devTbs[i]);
//            }
//        }
//        zigbee_updateAllDevice();
//        break;

//    case NET_CMD_REQ_ONE_DEV:
//        // (02 [MAC] [Ion])
//        //??MAC???? ????
//        //??MAC?????
//        pmac = &pNmsgR->data[1];
//        pion = &pNmsgR->data[9];
//        pdevTbs = getDevTbsByMac(pmac);
//        if (pdevTbs != NULL)
//        {
//            pdevTbs->ion = *pion;
//            Net_send_device(pdevTbs);
//            // ??Zigbee??
//            zigbee_inquire(pdevTbs, ZIGEBE_FORCE, ZIGEBE_IMMEDIATE);
//            // zigbee_remote_req_net_io(pdevTbs->netId, pdevTbs->ion , ZIGEBE_IMMEDIATE);
//        }
//        else
//        {
//            //TODO: ?????,????????
//        }
//        break;
//    // ??????
//    case NET_CMD_SET_DEV_NAME:
//        // ([04] [MAC] [Ion] [name])
//        pmac = &pNmsgR->data[1];
//        pion = &pNmsgR->data[9];
//        pname = &pNmsgR->data[10];
//        pdevTbs = getDevTbsByMac(pmac);
//        if (pdevTbs != NULL)
//        {
//            pdevTbs->ion = *pion;
//            for (i = 0; i < DEV_NAME_SIZE; i++)
//                pdevTbs->statetables[pdevTbs->ion].name[i] = *pname++;
//        }
//        else
//        {
//            //TODO: ?????,????????
//        }
//        break;
//    // ??????
//    case NET_CMD_SET_ON_OFF:
//        // ([08] [MAC] [Ion] [iomode])
//        pmac = &pNmsgR->data[1];
//        pion = &pNmsgR->data[9];
//        //poperate = &pNmsgR->data[10];
//        piomode = &pNmsgR->data[10];
//        pdevTbs = getDevTbsByMac(pmac);
//        if (pdevTbs != NULL)
//        {
//            pdevTbs->ion = *pion;
//            pdevTbs->statetables[pdevTbs->ion].iomode = *piomode;
//            pdevTbs->priority = PRIORITY_HIGHEST; //?????????,??????
//            // ??????
//            zigbee_operate(pdevTbs, PRIORITY_HIGHEST, ZIGEBE_FORCE, ZIGEBE_IMMEDIATE);
//        }
//        else
//        {
//            //TODO: ?????,????????
//        }
//        break;
//    // ????
//    case NET_CMD_PROCY_UPDATE:
//        break;
//		//????
//		    case NET_CMD_MODE_SWITCH:
//						procymode = pNmsgR->data[1];
//						policy_mode_switch(procymode);
//        break;
//    }
    return OK;
}

void NET_parseInstruction(struct msgStu *pNmsgR)
{
    uint16_t sn;
    if (pNmsgR->usable != 2)
        return;
#if BIGENDIAN
    sn = (uint16_t)(((pNmsgR->sn[0] << 8 | pNmsgR->sn[1])) );
#else
    sn = (uint16_t)(((pNmsgR->sn[1] << 8 | pNmsgR->sn[0])) );
#endif
    if (sn == 0) //??sn=0
    {
			
    }
    else if (sn % 2 == 0)
    {

 //       Net_Ans(pNmsgR);

        NET_parseData(pNmsgR);
    }
    else if (sn % 2)
    {

       // Ans_parse(pNmsgR);
    }
    pNmsgR->usable = 0;
}



struct msgStu *get_NetRecvDataCMDS()
{
    uint32_t i = 0;
    while (i < RECV_CMDS_NUM)
    {
        if (netRecvDataCMDS[i].usable == 0)
        {
            return &netRecvDataCMDS[i];
        }
        i++;
    }
    return NULL;
}


