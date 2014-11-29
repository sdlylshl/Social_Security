
#include "config.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"

#include "gsm.h"

#include "stm32f10x_it.h"

int gsm_usart2_send(u8* cmd, u8 len)
{
	int i;

	for(i=0; i<len; i++)
	{
		// Write a character to the USART
		 USART_SendData(USART2, cmd[i]);
		 
		 //  Loop until the end of transmission
		 while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
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
			parse_ack(ack);
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
	Delay_ms(500);
	parse_sep();

	parse_ack("OK");
	Delay_ms(500);
	gsm_usart2_send("ATE0\r", 5);
        Delay_ms(500);
	parse_ATE0();
	parse_ack("OK");
	
	Delay_ms(500);
	gsm_usart2_send("AT\r", 3);
    Delay_ms(500);
	parse_ack("OK");

	gsm_usart2_send("AT+CPIN?\r", 9);
    Delay_ms(500);
	if(parse_ack("READY"))
		goto error;
	if(parse_ack("OK"))
		goto error;
	
	gsm_usart2_send("AT+CSCS=\"GSM\"\r", 14);
	Delay_ms(500);
	ret = parse_ack("OK");
	if(ret)
		goto error;

	gsm_usart2_send("AT+CMGF=1\r", 10);
	Delay_ms(500);
	ret = parse_ack("OK");
	if(ret)
		goto error;
	
	call_buf[22] = 0;
	sprintf((char*)call_buf, "AT+CMGS=\"%s\"\r", cell);
	gsm_usart2_send(call_buf, 22);
	Delay_ms(500);
	if(parse_text_pro())
		goto error;

	gsm_usart2_send(text, strlen((char*)text));
	gsm_usart2_send("\r", 1);
	Delay_ms(500);

	gsm_usart2_send(&snd_ok, 1);
	gsm_usart2_send("\r", 1);
	Delay_ms(500);
	Delay_ms(500);
	ret = parse_ack("+CMGS");
	if(ret)
		goto error;
	ret = parse_ack("OK");
	if(ret)
		goto error;
error:
		return ret;
}
int gsm_call_init(u8* cell)
{	
	u8 ret=ERROR;
	u8 a=60;
	u8 call_buf[18];
	
	gsm_usart2_send("AT\r", 3);
	Delay_ms(500);
	parse_sep();

	parse_ack("OK");
	Delay_ms(500);
	gsm_usart2_send("ATE0\r", 5);
        Delay_ms(500);
	parse_ATE0();
	if(parse_ack("OK"))
		goto error;
	
	Delay_ms(500);
	gsm_usart2_send("AT\r", 3);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CPIN?\r", 9);
    Delay_ms(500);
	if(parse_ack("READY"))
		goto error;
	if(parse_ack("OK"))
		goto error;

	
	call_buf[15] = 0;
	sprintf((char*)call_buf, "ATD%s;\r", cell);
	gsm_usart2_send(call_buf, 16);
	Delay_ms(500);
	Delay_ms(500);

	while(parse_ack("+COLP") && a)
	{
		Delay_ms(500);
		Delay_ms(500);
		a--;
	}
	if (a)
	{
		ret = parse_ack("OK");
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
	gsm_usart2_send("ATH\r", 4);
	ret = parse_ack("OK");
	if(ret)
		goto error;

error:
		return ret;
}

int gsm_gprs_init()
{
	u8 ret = ERROR;
	
	
	gsm_usart2_send("AT\r", 3);
	Delay_ms(500);
	parse_sep();

	parse_ack("OK");
	Delay_ms(500);
	gsm_usart2_send("ATE0\r", 5);
        Delay_ms(500);
	parse_ATE0();
	parse_ack("OK");
	
	Delay_ms(500);
	gsm_usart2_send("AT\r", 3);
    Delay_ms(500);
	parse_ack("OK");

	gsm_usart2_send("AT+CPIN?\r", 9);
    Delay_ms(500);
	if(parse_ack("READY"))
		goto error;
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CGCLASS=\"B\"\r", 15);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CGDCONT=1,\"IP\",\"UNINET\"\r", 27);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CGATT=1\r", 11);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPCSGP=1,\"UNINET\"\r", 22);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CLPORT=\"TCP\",\"2000\"\r", 23);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPSTART=\"TCP\",\"112.233.240.70\",\"8081\"\r", 42);
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
	if(parse_ack("OK"))
		goto error;
    if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPSEND\r", 11);
    Delay_ms(500);
	if(parse_text_pro())
		goto error;
	ret = OK;
error:
	return ret;
}


int gsm_gprs_send(u8* dst, u16 len)
{
	u8 ret=ERROR;
	u8 snd_ok = 0x1a;
	
	gsm_usart2_send(dst, len);
	gsm_usart2_send("\r", 1);

    Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	gsm_usart2_send(&snd_ok, 1);
	gsm_usart2_send("\r", 1);
	Delay_ms(500);
	Delay_ms(500);

	if(parse_ack("OK"))
		goto error;

	ret = OK;

error:
		return ret;
}

int gsm_gprs_shutdown()
{
	u8 ret=ERROR;
	
	
	gsm_usart2_send("AT+CIPCLOSE=1\r", 14);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPSHUT\r", 11);
	if(parse_ack("OK"))
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
			goto error;
		else
			gprs_init = 1;
	}
	else if(mode && gprs_init)
	{
		if(gsm_gprs_shutdown())
			goto error;
		else
			gprs_init = 0;
	}
	
	
	
error:
	return ret;
}
int gsm_gprs_test()
{
	u8 ret =1;
	u8 snd_ok = 0x1a;
	
	gsm_usart2_send("AT\r", 3);
	Delay_ms(500);
	parse_sep();

	parse_ack("OK");
	Delay_ms(500);
	gsm_usart2_send("ATE0\r", 5);
        Delay_ms(500);
	parse_ATE0();
	parse_ack("OK");
	
	Delay_ms(500);
	gsm_usart2_send("AT\r", 3);
    Delay_ms(500);
	parse_ack("OK");

	gsm_usart2_send("AT+CPIN?\r", 9);
    Delay_ms(500);
	if(parse_ack("READY"))
		goto error;
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CGCLASS=\"B\"\r", 15);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CGDCONT=1,\"IP\",\"UNINET\"\r", 27);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CGATT=1\r", 11);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPCSGP=1,\"UNINET\"\r", 22);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CLPORT=\"TCP\",\"2000\"\r", 23);
    Delay_ms(500);
	if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPSTART=\"TCP\",\"112.233.240.70\",\"8081\"\r", 42);
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
	if(parse_ack("OK"))
		goto error;
    if(parse_ack("OK"))
		goto error;

	gsm_usart2_send("AT+CIPSEND\r", 11);
    Delay_ms(500);
	if(parse_text_pro())
		goto error;
	gsm_usart2_send("hello\r", 6);
    Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	gsm_usart2_send(&snd_ok, 1);
	gsm_usart2_send("\r", 1);
	Delay_ms(500);
	Delay_ms(500);

	if(parse_ack("OK"))
		goto error;

	ret = OK;

error:
	return ret;
        
}



int gsm_test()
{	
	u8 ret=ERROR;
#if 1	
	if(gsm_gprs_init())
		goto error;
	if(gsm_gprs_send("hi,mike", 7))
		goto error;
#endif
#if 1
	if (gsm_call_init("18954954643"))
		goto error;
#endif
	gsm_send_text("18954954643", "hi,liu");
error:
	return ret;
}
