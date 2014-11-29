
#include "config.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"

#include "gsm.h"

#include "stm32f10x_it.h"


extern sys_para sys;




int gsm_usart2_send(u8* cmd, u8 len)
{
	int i;

	printf("------send %s\n", cmd);

	for(i=0; i<len; i++)
	{
		// Write a character to the USART
		 USART_SendData(USART1, cmd[i]);
		 
		 //  Loop until the end of transmission
		 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		 {
		 }
	}
 	return 0;
}




int gsm_send_cmd(u8* cmd, u8 len, u8* ack, u16 time)
{
	gsm_usart2_send(cmd, len);
	if(ack)
	{
		while(time--)
		{
			Delay_ms(10);
			parse_ack(ack, 500, 6);
		}
	}
	return OK;
}

int gsm_send_text(u8* cell, u8* text)
{
	u8 ret=ERROR;
	u8 call_buf[24];
	u8 snd_ok=0x1a;
	
	gsm_usart2_send("AT\r", 3);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CPIN?\r", 9);
	if(parse_ack("+CPIN:READY", 500, 15))
		goto error;
	if(parse_ack("OK", 500, 6))
		goto error;
	
	gsm_usart2_send("AT+CSCS=\"GSM\"\r", 14);
	ret = parse_ack("OK", 500, 6);
	if(ret)
		goto error;

	gsm_usart2_send("AT+CMGF=1\r", 10);
	ret = parse_ack("OK", 500, 6);
	if(ret)
		goto error;
	
	call_buf[22] = 0;
	sprintf((char*)call_buf, "AT+CMGS=\"%s\"\r", cell);
	gsm_usart2_send(call_buf, 22);
	if(parse_text_pro(500))
		goto error;

	gsm_usart2_send(text, strlen((char*)text));
	gsm_usart2_send("\r", 1);
	Delay_ms(500);

	gsm_usart2_send(&snd_ok, 1);
	gsm_usart2_send("\r", 1);
	
	ret = OK;
error:
		return ret;
}

int gsm_snd_text_trail()
{	
	u8 ret=ERROR;
	
	if(parse_text_pro(500))
		goto error;
	ret = parse_ack("+CMGS", 500, 9);
	if(ret)
		goto error;
	ret = parse_ack("OK", 500, 6);
	if(ret)
		goto error;
error:
		return ret;
}
int gsm_call_init(u8* cell)
{	
	u8 ret=ERROR;
	u8 a=60;
	u8 b;
	u8 call_buf[18];
	
	gsm_usart2_send("AT\r", 3);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CPIN?\r", 9);
	if(parse_ack("+CPIN:READY", 500, 15))
		goto error;
	if(parse_ack("OK", 500, 6))
		goto error;

	//show phone num 
	gsm_usart2_send("AT+COLP=1\r", 10);
	if(parse_ack("OK", 500, 6))
		goto error;

	call_buf[15] = 0;
	sprintf((char*)call_buf, "ATD%s;\r", cell);
	gsm_usart2_send(call_buf, 16);
	Delay_ms(500);
	Delay_ms(500);


	printf("before +colp\n");
	b=parse_dial_rsp("+COLP", 500, 4);
	printf("after +colp\n");
	if (b==OK)
	{
		ret = parse_ack("OK", 500, 6);
		if(ret)
			goto error;
	}
	else
		goto error;
	
	ret = OK;
error:
		return ret;

}


int gsm_call_end()
{	
	u8 ret;

	wait_for_end_call();
	#if 0
	gsm_usart2_send("ATH\r", 4);
	ret = parse_ack("OK", 500);
	if(ret)
		goto error;
	#endif
	ret = OK;
error:
		return ret;
}

int gsm_gprs_init()
{
	u8 ret = ERROR;

	//printf("gsm_gprs_init()\n");
	
	gsm_usart2_send("AT\r", 3);
	parse_ack("OK", 500, 6);

	gsm_usart2_send("AT+CPIN?\r", 9);
	if(parse_ack("+CPIN:READY", 500, 15))
		goto error;
	if(parse_ack("OK", 500, 6))
		goto error;

	
	gsm_usart2_send("AT+COPS?\r", 9);
	if(parse_ack("MOBILE", 500, 9))
		sys.tele_operator = 1; //UNICOM
	else
		sys.tele_operator = 0; //MOBILE

	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CGCLASS=\"B\"\r", 15);
	if(parse_ack("OK", 500, 6))
		goto error;

	if(sys.tele_operator)
	{
		gsm_usart2_send("AT+CGDCONT=1,\"IP\",\"UNINET\"\r", 27);
		if(parse_ack("OK", 500, 6))
			goto error;
	}
	else
	{
		gsm_usart2_send("AT+CGDCONT=1,\"IP\",\"CMNET\"\r", 26);
		if(parse_ack("OK", 500, 6))
			goto error;
	}

	gsm_usart2_send("AT+CGATT=1\r", 11);
	if(parse_ack("OK", 500, 6))
		goto error;
	if(sys.tele_operator)
	{
		gsm_usart2_send("AT+CIPCSGP=1,\"UNINET\"\r", 22);
		if(parse_ack("OK", 500, 6))
			goto error;
	}
	else
	{
		gsm_usart2_send("AT+CIPCSGP=1,\"CMNET\"\r", 21);
		if(parse_ack("OK", 500, 6))
			goto error;
	}
		

	gsm_usart2_send("AT+CLPORT=\"TCP\",\"2000\"\r", 23);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CIPSTART=\"TCP\",\"112.233.240.70\",\"8081\"\r", 42);

	if(parse_ack("OK", 500, 6))
		goto error;
    if(parse_ack("CONNECTOK", 20000, 13))
		goto error;


	ret = OK;
error:
	return ret;
}


int gsm_gprs_send(u8* dst, u16 len)
{
	u8 ret=ERROR;
	u8 snd_ok = 0x1a;
	int i;

	gsm_usart2_send("AT+CIPSEND\r", 11);
	if(parse_text_pro(500))
		goto error;
	
	printf("gsm_gprs_send: ", dst);
	for(i=0; i<len; i++)
		printf("%x ", dst[i]);
	printf("\n");
	gsm_usart2_send(dst, len);
	//gsm_usart2_send("a\r", 2);

	Delay_ms(500);
	gsm_usart2_send(&snd_ok, 1);
	gsm_usart2_send("\r", 1);

	if(parse_ack("OK", 1000, 6))
		goto error;

	ret = OK;

error:
		return ret;
}


int gsm_read_text(u8 *text)
{	
	u8 ret=ERROR;
	u8 text_num[8]={0};
	u8 text_num_len;
	u8 snd[16]={0};
	if(parse_text_hint("+CMTI", 500, 16, text_num, &text_num_len) == OK)
	{	
		sprintf(snd, "AT+CMGR=%s\r", text_num);
		gsm_usart2_send(snd, 9+text_num_len);
		parse_text_content("+CMGR", 500, 9, text);
		parse_ack("OK",500,6);
		ret = OK;
	}
error:
	return ret;
}

int gsm_get_balance(u8* l_text)
{	
	u8 ret;
	//u8 l_text[512]={0};
	
	gsm_usart2_send("AT\r", 3);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CPIN?\r", 9);
	if(parse_ack("+CPIN:READY", 500, 15))
		goto error;
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+COPS?\r", 9);
	if(parse_ack("MOBILE", 500, 9))
		sys.tele_operator = 1; //UNICOM
	else
		sys.tele_operator = 0; //MOBILE

	if(parse_ack("OK", 500, 6))
		goto error;

	if(sys.tele_operator)
		gsm_send_text("10010", "102");
	else
		gsm_send_text("10086", "ye");

	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);

	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);

	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);

	if(!gsm_snd_text_trail())
	{
		printf("parse text trail ok\n");
	}
	else
		printf("parse text trail error\n");

	if(gsm_read_text(l_text))
		goto error;

	ret = OK;
	
	
error:
	return ret;
}

//do not use cause of the require result is unstable
int gsm_get_l_num(u8* cell)
{
	u8 ret=ERROR;
	
	gsm_usart2_send("AT\r", 3);
	if(parse_ack("OK", 500, 6))
		goto error;
	
	gsm_usart2_send("AT+CNUM\r", 8);
	ret = parse_ack("OK", 500, 6);
	if(ret)
		goto error;
	else
		ret = ERROR;
error:
	return ret;
}


int gsm_get_stat()
{
	u8 ret=ERROR;
	
	gsm_usart2_send("AT\r", 3);
	if(parse_ack("OK", 500, 6))
		goto error;
	
	gsm_usart2_send("AT+CPIN?\r", 9);
	if(parse_ack("+CPIN:READY", 500, 15))
		goto error;
	if(parse_ack("OK", 500, 6))
		goto error;
error:
	return ret;
}

int gsm_gprs_shutdown()
{
	u8 ret=ERROR;
	
	//printf("gsm_gprs_shutdown()\n");
	gsm_usart2_send("AT+CIPCLOSE=1\r", 14);
	//Delay_ms(500);
	//Delay_ms(500);
	if(parse_ack("OK", 1000, 6))
		goto error;

	gsm_usart2_send("AT+CIPSHUT\r", 11);
	//Delay_ms(500);
	//Delay_ms(500);
	if(parse_ack("OK", 1000, 6))
		goto error;

	ret = OK;

error:
		return ret;
}


int gsm_init()
{
	u8 ret=ERROR;

	gsm_usart2_send("AT\r", 3);
	Delay_ms(500);
	if(parse_sep(500))
		goto error;
	
	if(parse_ack("OK", 500, 6))
		goto error;
	gsm_usart2_send("ATE0\r", 5);

	Delay_ms(500);
	parse_ATE0(500);
	if(parse_ack("OK", 500, 6))
		goto error;

	//block call in
	gsm_usart2_send("AT+GSMBUSY=1\r", 13);
	if(parse_ack("OK", 500, 6))
		goto error;
	
	ret = OK;
error:
	return ret;
}


int gprs_loop(u8 mode)
{
	u8 ret = ERROR;
	static u8 gprs_init = 0;
	if((!mode) && (!gprs_init))
	{
		if(gsm_gprs_init())
		{
			printf("gsm_gprs_init error\n");
			goto error;
		}
		else
		{	
			printf("gsm_gprs_init ok\n");
			gprs_init = 1;
			sys.gprs_init_flag = 1;
		}
		//do_tcp_net_loss();
	}
	else if(mode && gprs_init)
	{
		if(gsm_gprs_shutdown())
		{	
			printf("gsm_gprs_shutdown error\n");
			goto error;
		}
		else
		{	
			printf("gsm_gprs_shutdown ok\n");
			gprs_init = 0;
			sys.gprs_init_flag = 0;
		}
	}
	
	ret = OK;
	
error:
	return ret;
}
int gsm_gprs_test()
{
	u8 ret =1;
	u8 snd_ok = 0x1a;
	
	gsm_usart2_send("AT\r", 3);
	//Delay_ms(500);
	parse_sep(500);

	parse_ack("OK", 500, 6);
	//Delay_ms(500);
	gsm_usart2_send("ATE0\r", 5);
    Delay_ms(500);
	parse_ATE0(500);
	parse_ack("OK", 500, 6);
	
	//Delay_ms(500);
	gsm_usart2_send("AT\r", 3);
    //Delay_ms(500);
	parse_ack("OK", 500, 6);

	gsm_usart2_send("AT+CPIN?\r", 9);
    //Delay_ms(500);
	if(parse_ack("+CPIN:READY", 500, 15))
		goto error;
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CGCLASS=\"B\"\r", 15);
    //Delay_ms(500);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CGDCONT=1,\"IP\",\"UNINET\"\r", 27);
    //Delay_ms(500);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CGATT=1\r", 11);
    //Delay_ms(500);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CIPCSGP=1,\"UNINET\"\r", 22);
    //Delay_ms(500);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CLPORT=\"TCP\",\"2000\"\r", 23);
    //Delay_ms(500);
	if(parse_ack("OK", 500, 6))
		goto error;

	gsm_usart2_send("AT+CIPSTART=\"TCP\",\"112.233.240.70\",\"8081\"\r", 42);
    //Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
   // Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	if(parse_ack("OK", 10000, 6))
		goto error;
    if(parse_ack("OK", 5000, 6))
		goto error;

	gsm_usart2_send("AT+CIPSEND\r", 11);
    //Delay_ms(500);
	if(parse_text_pro(500))
		goto error;
	gsm_usart2_send("hello\r", 6);
    //Delay_ms(500);
	//Delay_ms(500);
	//Delay_ms(500);
	Delay_ms(500);
	gsm_usart2_send(&snd_ok, 1);
	gsm_usart2_send("\r", 1);
	//Delay_ms(500);
	//Delay_ms(500);

	if(parse_ack("OK", 500, 6))
		goto error;

	ret = OK;

error:
	return ret;
        
}

int do_gsm_cell_alarm(u8* cell_num)
{	
	u8 ret = ERROR;
	printf("do cell alarm: %s\n", cell_num);
	
	{
		if (gsm_call_init(cell_num))
		{
			printf("gsm_call_init error\n");
			goto error;
		}
		else
			ret = OK;

		gsm_call_end();
	}
error:
	return ret;
}
int do_gsm_text_alarm(u8* text_num, u8* text_content)
{
		u8 ret = ERROR;
		printf("do text alarm: %s, %s\n", text_num, text_content);

		if(gsm_send_text(text_num, text_content))
		{
			printf("do_gsm_text_alarm error\n");
			goto error;
		}
		else
			ret = OK;
	
	error:
		return ret;

}

int do_text_alarm(u8 i)
{
	u8 ret=ERROR;
	
	text_para text;
	text = sys.text;

	if(i==0 && (text.send_ok[text.text_group - 1] == OK))
	{
		if(!gsm_snd_text_trail())
			text.send_ok[text.text_group - 1] = ERROR;
	}
	else if(text.send_ok[i-1] == OK)
	{
		if(!gsm_snd_text_trail())
			text.send_ok[i-1] = ERROR;
	}
	ret = do_gsm_text_alarm(text.text[i], text.text_content[i]);
	if(ret != OK)
		goto error;
	else
	{
		text.send_ok[i] = OK;
		set_current_time(&text.time[i]);
	}
	
	
error:
	return ret;
}

int text_alarm()
{
	u8 ret=ERROR;
	u8 i;
		
	text_para text;
	text = sys.text;

	for(i=0; i<text.text_group; i++)
	{
		if(time_return() - text.time[i] > 10)
			do_text_alarm(i);
		else
			text.time[i] = time_return();
	}
	
error:
	return ret;
}
int do_cell_alarm()
{
	u8 i, ret=ERROR;
	cell_para cell;
	cell = sys.cell;
	
	for(i=0; i<cell.cell_group; i++)
	{
		ret = do_gsm_cell_alarm(cell.cell[i]);
		if(ret != OK)
			continue;
		else
		{
			if(!cell.cell_xor)
				break;
		}
	}
	
error:
	return ret;
}
int gsm_test()
{	
	u8 ret=ERROR;
	u8 l_text[512]={0};
#if 1

	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
#if 0
	if(gsm_init())
	{
		printf("gsm_init error\n");
		//reset_sim900a();
		goto error;
	}
#endif
#endif
#if 0	
	if(gsm_gprs_init())
		goto error;
	if(gsm_gprs_send("hi,mike", 7))
		goto error;
#endif
	printf("after gsm_init()\n");

#if 0
	if (gsm_call_init("15589107586"))
	{	
		printf("call error\n");
		//goto error;
	}
	else
	{
		gsm_call_end();
		printf("call success\n");
	}

	if (gsm_call_init("15589107586"))
	{	
		printf("call error\n");
		goto error;
	}
	else
		printf("call success\n");
#endif
#if 0	
	if(gsm_send_text("15589107586", "mike"))
	{
		printf("gsm text test error\n");
		//reset_sim900a();
		goto error;
	}
	printf("send text ok\n");
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);

	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);

	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);

	if(!gsm_snd_text_trail())
	{
		printf("parse text trail ok\n");
	}
	else
		printf("parse text trail error\n");
	
#endif
#if 1
	gsm_get_balance(l_text);
#endif
error:
	return ret;
}
