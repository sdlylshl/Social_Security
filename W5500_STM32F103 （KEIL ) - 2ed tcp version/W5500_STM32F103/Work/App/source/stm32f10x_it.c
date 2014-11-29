/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "config.h"
#include "util.h"
#include "flash.h"
#include "gsm.h"
#include "APPs\loopback.h"

#include <stdio.h>

#include "stdarg.h"	 	 
 	 
#include "string.h"	
#include "i2c_ee.h"


extern sys_para sys;

#define MAX_SERIAL_PARA_BUF 64
u8 buf[MAX_SERIAL_PARA_BUF] ;
u16 read = 0;
u16 write = 0;
u8 cmd[32];

u8 find_one_cmd=0;
u8 u2_buf[512];

#define MAX_GSM_BUF 512
u8 gsm_buf[MAX_GSM_BUF] ;
u16 gsm_read = 0;
u16 gsm_write = 0;
u8 gsm_cmd[64];



/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//extern vu32 Timer2_Counter;
//extern u32 my_time;

extern void Timer3_ISR(void);

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	#if 0
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		Timer2_ISR();
		
	}
	#endif
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		Timer3_ISR();
		
	}
}





/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
	u8 c;
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)  
	{		 
		 //USART_SendData(USART1, USART_ReceiveData(USART1)); 
		#if 0
		c=USART1->DR;
		if (c != ' ')
		{
			buf[write] = c;
			if (write == 63)
				write = 0;
			else
				write++;
		}
		#endif
		c=USART1->DR;
		if (c != ' ')
		{
			gsm_buf[gsm_write] = c;
			if (gsm_write == MAX_GSM_BUF-1)
				gsm_write = 0;
			else
				gsm_write++;
		}
	}
}

int read_pos_backward()
{
	if (read == MAX_SERIAL_PARA_BUF-1)
		read = 0;
	else
		read--;

	return OK;
}


int read_pos_forward()
{
	if (read == MAX_SERIAL_PARA_BUF-1)
		read = 0;
	else
		read++;

	
	return OK;
}

//parse string "#192.168.088.035#"
int parse_ip_bit(u8*buf,u8*ip, u8 bit, u8* is_ch)
{	
	u8 j;
	u8 help[3] = {100, 10, 1};

	
	for(j=0; j<3; j++)
	{	
		//printf("buf[4*bit+j+1]: %c, bit: %d, j: %d\n", buf[4*bit+j+1], bit, j);
		if (buf[4*bit+j+1] == '0')
			ip[bit] += 0*help[j];
		else if (buf[4*bit+j+1] == '1')
			ip[bit] += 1*help[j];
		else if (buf[4*bit+j+1] == '2')
			ip[bit] += 2*help[j];
		else if (buf[4*bit+j+1] == '3')
			ip[bit] += 3*help[j];
		else if (buf[4*bit+j+1] == '4')
			ip[bit] += 4*help[j];
		else if (buf[4*bit+j+1] == '5')
			ip[bit] += 5*help[j];
		else if (buf[4*bit+j+1] == '6')
			ip[bit] += 6*help[j];
		else if (buf[4*bit+j+1] == '7')
			ip[bit] += 7*help[j];
		else if (buf[4*bit+j+1] == '8')
			ip[bit] += 8*help[j];
		else if (buf[4*bit+j+1] == '9')
			ip[bit] += 9*help[j];
		else 
			*is_ch = 1;
		//printf("ip[bit]: %d, bit: %d\n", ip[bit], bit);
	}

	return OK;
		
}


//parse string "#192.168.088.035#"
int parse_ip(u8*buf, u8 len, u8* addr, u8 sep)
{	
	u8 i,is_ch=0;
	
	if (buf[0] == sep && buf[16] == sep && buf[4] == '.' &&
		buf[8] == '.' && buf[12] == '.')
	{	
		for(i=0; i<4; i++)
		{
			addr[i] = 0;
			parse_ip_bit(buf, addr, i, &is_ch);
		}
		if(!is_ch)
		{
			printf("set addr: %d.%d.%d.%d\n", addr[0], addr[1], addr[2], addr[3]);
			save_ip_to_eeprom(addr, sep);
			save_default_flag_eeprom();
		}
		
		//printf("set ip: %d.%d.%d.%d, is_ch:%d\n", ip[0], ip[1], ip[2], ip[3], is_ch);
		for(i=0; i<4; i++)
			addr[i] = 0;
	}

	return OK;
}

int parse_mac(u8*buf, u8 len, u8* addr)
{	
	u8 i,is_ch=0;
	
	if (buf[0] == MAC_START && buf[len-1] == MAC_END && buf[4] == '.' &&
		buf[8] == '.' && buf[12] == '.')
	{	
		for(i=0; i<6; i++)
		{
			addr[i] = 0;
			parse_ip_bit(buf, addr, i, &is_ch);
		}
		if(!is_ch)
		{
			printf("set mac: %d.%d.%d.%d.%d.%d\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
			save_mac_to_eeprom(addr);
		}
		
		//printf("set ip: %d.%d.%d.%d, is_ch:%d\n", ip[0], ip[1], ip[2], ip[3], is_ch);
		for(i=0; i<6; i++)
			addr[i] = 0;
	}

	return OK;
}


int parse_port(u8*buf, u8 len, u16* port, u8* is_ch)
{	
	u8 j;
	u16 help[6] = {0, 10000, 1000, 100, 10, 1};
	
	if(buf[0] == PORT_START&& buf[6] == PORT_END)
	{
		for(j=1; j<6; j++)
		{	
			//printf("buf[4*bit+j+1]: %c, bit: %d, j: %d\n", buf[4*bit+j+1], bit, j);
			if (buf[j] == '0')
				*port += 0*help[j];
			else if (buf[j] == '1')
				*port += 1*help[j];
			else if (buf[j] == '2')
				*port += 2*help[j];
			else if (buf[j] == '3')
				*port += 3*help[j];
			else if (buf[j] == '4')
				*port += 4*help[j];
			else if (buf[j] == '5')
				*port += 5*help[j];
			else if (buf[j] == '6')
				*port += 6*help[j];
			else if (buf[j] == '7')
				*port += 7*help[j];
			else if (buf[j] == '8')
				*port += 8*help[j];
			else if (buf[j] == '9')
				*port += 9*help[j];
			else 
				*is_ch = 1;
		//printf("Port: %u, j: %d\n", *port, j);
		}
		save_port_to_eeprom(*port);
	}

	return OK;
}

int parse_machine_id(u8* buf, u8 len, u32* machine_id, u8* is_ch)
{
	u8 j;
	u32 help[11] = {0, 1000000000, 100000000, 10000000,
		1000000, 100000, 10000, 1000, 100, 10, 1};
	
	if(buf[0] == ID_START && buf[11] == ID_END)
	{
		for(j=1; j<11; j++)
		{	
			//printf("buf[4*bit+j+1]: %c, bit: %d, j: %d\n", buf[4*bit+j+1], bit, j);
			if (buf[j] == '0')
				*machine_id += 0*help[j];
			else if (buf[j] == '1')
				*machine_id += 1*help[j];
			else if (buf[j] == '2')
				*machine_id += 2*help[j];
			else if (buf[j] == '3')
				*machine_id += 3*help[j];
			else if (buf[j] == '4')
				*machine_id += 4*help[j];
			else if (buf[j] == '5')
				*machine_id += 5*help[j];
			else if (buf[j] == '6')
				*machine_id += 6*help[j];
			else if (buf[j] == '7')
				*machine_id += 7*help[j];
			else if (buf[j] == '8')
				*machine_id += 8*help[j];
			else if (buf[j] == '9')
				*machine_id += 9*help[j];
			else 
				*is_ch = 1;
		printf("machine_id: %u, j: %d\n", *machine_id, j);
		}
		save_id_to_eeprom(*machine_id);
	}
	return OK;
}
int parse_one_cmd()
{
	u8 is_ch=0;
	int i;

	//printf("write: %d, read: %d\n", write, read);
	if ((write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 1)//wait for an shortest rsp
	{	
		//printf("2 read: %d\n", read);
		if (buf[read] == IP_START && (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 17)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<17; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			parse_ip(cmd, 17, sys.net_para.Lip, IP_START);
		}
		else if (buf[read] == GATE_IP&& (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 17)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<17; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			parse_ip(cmd, 17, sys.net_para.Gw, GATE_IP);
		}
		else if (buf[read] == NETMASK_IP&& (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 17)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<17; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			parse_ip(cmd, 17, sys.net_para.Sub, NETMASK_IP);
		}
		else if (buf[read] == DEST_IP&& (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 17)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<17; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			parse_ip(cmd, 17, sys.dest_net_para.destip, DEST_IP);
		}
		else if (buf[read] == PORT_START && (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 7)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<7; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			sys.dest_net_para.port = 0;
			parse_port(cmd, 7, &(sys.dest_net_para.port), &is_ch);
			if (!is_ch)
			{
				printf("Dest_PORT: %u\n", sys.dest_net_para.port);
				save_port_to_eeprom(sys.dest_net_para.port);
			}
			
		}
		else if (buf[read] == ID_START && (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 12)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<12; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			is_ch = 0;
			sys.machine_id = 0;
			parse_machine_id(cmd, 12, &sys.machine_id, &is_ch);
			if (!is_ch)
			{
				save_id_to_flash(sys.machine_id);
				printf("machine id: %u\n", sys.machine_id);
			}
		}
		else if (buf[read] == MAC_START && (write+MAX_SERIAL_PARA_BUF-read)%MAX_SERIAL_PARA_BUF >= 25)
		{	
			printf("write: %d, read: %d\n", write, read);
			for(i=0; i<25; i++)
			{
				cmd[i] = buf[read];
				read_pos_forward();
			}
			printf("cmd: %s\n", cmd);
			parse_mac(cmd, 25, sys.net_para.Mac);
		}
		else
			read_pos_forward();
			
	}
	return OK;
}




/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{	
	#if 0
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)  
	{		 
		 USART_SendData(USART2, USART_ReceiveData(USART2)); 			
	}
	#endif
	#if 1
	u8 c;
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)  
	{		 
		c=USART2->DR;
		#if 0
		if (c == '\r')
			USART_SendData(USART2, 0x0e);
		else if(c == '\n')
			USART_SendData(USART2, 0x0d);
		else
			USART_SendData(USART2, 0x0c);
		#endif
		#if 0
		if (c != ' ')
		{
			gsm_buf[gsm_write] = c;
			if (gsm_write == 511)
				gsm_write = 0;
			else
				gsm_write++;
		}
		#endif

		c=USART2->DR;
		if (c != ' ')
		{
			buf[write] = c;
			if (write == MAX_SERIAL_PARA_BUF-1)
				write = 0;
			else
				write++;
		}
	}
	#endif
}

int gsm_read_pos_backward()
{
	if (gsm_read == 0)
		gsm_read = MAX_GSM_BUF-1;
	else
		gsm_read--;

	return OK;
}




int gsm_read_pos_forward()
{
	if (gsm_read == MAX_GSM_BUF-1)
		gsm_read = 0;
	else
		gsm_read++;

	
	return OK;
}

int gsm_read_pos_for_and_len(u8* len)
{
	gsm_read_pos_forward();
	(*len)++;
	return OK;
}

int gsm_read_pos_back_and_len(u8* len)
{
	gsm_read_pos_forward();
	(*len)--;
	return OK;
}







void u2_printf(char* fmt,...)  
{  	
	int i;
	va_list ap;

	for(i=0; i<512; i++)
		u2_buf[i] = 0;
	va_start(ap,fmt);
	vsprintf((char*)u2_buf,fmt,ap);
	va_end(ap);
	gsm_usart2_send(u2_buf, strlen((char*)u2_buf));
}


int parse_ack_old(u8* ack, int time)
{	

	u8 i,len=0;
	u8 ack_len;
	u8 error = 1;
	u8 time_flag=10;

	ack_len = strlen(ack)+4;
	while(((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF < ack_len) && time_flag)
	{
		time_flag--;
		Delay_ms(time/10);
	}
	
	//gsm_usart2_send("PARSE OK", 8);
	printf("gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
	for(i=0; i<((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF); i++)
		printf("%c", gsm_buf[gsm_read+i]);
	printf("\n");
	if (((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 1) && gsm_buf[gsm_read] == GSM_FRAME_S1)
	{	
		gsm_read_pos_for_and_len(&len);
		while((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 1)
		{	
			gsm_read_pos_for_and_len(&len);
			if(gsm_buf[gsm_read] == GSM_FRAME_E1)
			{
				printf("find one cmd = 1 ");
				find_one_cmd = 1;
				break;
			}
		}
		//printf("find end 1 gsm_write: %d, gsm_read: %d, len: %d\n", gsm_write, gsm_read, len);
		for(i=0; i<len; i++)
			gsm_read_pos_backward();
		if (find_one_cmd)
		{
			//printf("find end 2 gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
			for(i=0; i<32; i++)
				gsm_cmd[i] = 0;
			for(i=0; i<len; i++)
			{	
				gsm_cmd[i] = gsm_buf[gsm_read];
				gsm_read_pos_forward();
			}
			gsm_read_pos_forward();
			gsm_read_pos_forward();
			printf("gsm_cmd: %s\n", gsm_cmd);
			if(strstr((char*)gsm_cmd, (char*)ack))
			{
				//gsm_usart2_send("PARSE OK", 8);
				return OK;
			}
			find_one_cmd = 0;
		}

	}


	return error;
}


int parse_sep(int time)
{	
	u8 ret=1, i, len;
	u8 time_flag=10;
	u8 l_cmd[64]={0};
	char* ptr=NULL;

	#if 0
	while(((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF < 2) && time_flag)
	{
		time_flag--;
		Delay_ms(time/10);
	}
	#endif
	for(i=0; i<((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF); i++)
		printf("%c", gsm_buf[gsm_read+i]);
	printf("\n");
	#if 0
	if (((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 2))
	{
		printf("in parse_sep find\n");
		if(gsm_buf[gsm_read] == '\r')
		{	
			gsm_read_pos_forward();
			if(gsm_buf[gsm_read] == '\n')
			{
				gsm_read_pos_backward();
				printf("find cmd bigin\n");
				return OK;
			}
		}
		else
			gsm_read_pos_forward();
	}
	#endif
	len = (gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF;
	memcpy(l_cmd, gsm_buf+gsm_read, len);
	ptr=strstr(l_cmd, "\r\n");
	if(ptr != NULL)
	{	
		//printf("find cmd bigin, gsm_buf: %d, ptr: %d\n", l_cmd, ptr);
		while((l_cmd+gsm_read)!= ptr)
			gsm_read_pos_forward();
		ret = OK;
	}
	
	return ret;
}


int parse_ATE0(int time)
{	
	u8 i, len=5, ret=1;
	u8 time_flag=10;
	char* ptr=NULL;
	
	while(((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF < 5) && time_flag)
	{
		time_flag--;
		Delay_ms(time/10);
	}
	if (((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 5))
	{
		for(i=0; i<32; i++)
				gsm_cmd[i] = 0;
		for(i=0; i<len; i++)
		{	
			gsm_cmd[i] = gsm_buf[gsm_read];
			gsm_read_pos_forward();
		}
		if(strstr((char*)gsm_cmd, "ATE0"))
			return OK;
		else
		{
			for(i=0; i<len; i++)
				gsm_read_pos_backward();
		}
	}

	return ret;
}



int parse_text_pro(int time)
{
	u8 ret=1, i, len=3;
	u8 time_flag=10;
	
	while(((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF < 3) && time_flag)
	{
		time_flag--;
		Delay_ms(time/10);
	}
	if((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 3)
	{	
		for(i=0; i<32; i++)
				gsm_cmd[i] = 0;
		for(i=0; i<len; i++)
		{	
			gsm_cmd[i] = gsm_buf[gsm_read];
			gsm_read_pos_forward();
		}
		if(strstr((char*)gsm_cmd, ">"))
			return OK;

	}
	return ret;
}


int parse_ack(u8* ack, int time, int need_recv_len)
{
	u8 i, j,len=0;
	u8 ack_len;
	u8 error = 1;
	u8 time_flag=10;
	char* ptr=NULL;
	char* ptr1=NULL;
	char* end_sep=NULL;
	u8 ret = ERROR;
	u8 diff;
	static u8 flag=0;

	//ack_len = strlen(ack)+4;

	while(((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF < need_recv_len) && time_flag)
	{
		time_flag--;
		Delay_ms(time/10);
	}
	
loop:
	len = (gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF;
	printf("gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
	for(i=0; i<len; i++)
		printf("%c", gsm_buf[gsm_read+i]);
	printf("\n");
	if(len >= need_recv_len)
	{
		memset(gsm_cmd, 0, sizeof(gsm_cmd));
		memcpy(gsm_cmd, gsm_buf+gsm_read, len);

		//printf("len: %d, gsm_cmd: %s\n", len, gsm_cmd);
		ptr = strstr(gsm_cmd, ack);
		if(ptr != NULL)
		{
			printf("find ack: %s\n", ack);
			i=0;
			while((gsm_cmd+i) != ptr)
			{
				i++;
				gsm_read_pos_forward();
			}
			end_sep = strstr(gsm_cmd+i, "\r\n");
			if(end_sep != NULL)
			{
				j=0;
				while((gsm_cmd+i+j) != end_sep)
				{
					j++;
					gsm_read_pos_forward();
				}
				for(i=0; i<2; i++)
					gsm_read_pos_forward();
			}
			ret = OK;
			goto error;
		}
		else
		{	
			if(!flag)
			{
				flag = 1;
				Delay_ms(500);
				goto loop;
			}
			else
				goto error;
		}
	}
error:
	return ret;
}

int parse_text_hint(u8* ack, int time, int need_recv_len, u8* text_num, u8* text_num_len)
{	
	u8 i, j,len=0;
	u8 ack_len;
	u8 error = 1;
	u8 time_flag=10;
	char* ptr=NULL;
	char* ptr1=NULL;
	char* end_sep=NULL;
	u8 ret = ERROR;
	u8 diff;
	static u8 flag=0;


	//ack_len = strlen(ack)+4;

	while(gsm_write == gsm_read)
	{
		Delay_ms(100);
	}
	Delay_ms(100);
	

	len = (gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF;
	printf("gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
	for(i=0; i<len; i++)
		printf("%c", gsm_buf[gsm_read+i]);
	printf("\n");
	if(len >= need_recv_len)
	{
		memset(gsm_cmd, 0, sizeof(gsm_cmd));
		memcpy(gsm_cmd, gsm_buf+gsm_read, len);

		//printf("len: %d, gsm_cmd: %s\n", len, gsm_cmd);
		ptr = strstr(gsm_cmd, ack);
		if(ptr != NULL)
		{
			printf("find ack: %s\n", ack);
			i=0;
			while((gsm_cmd+i) != ptr)
			{
				i++;
				gsm_read_pos_forward();
			}
			end_sep = strstr(gsm_cmd+i, "\r\n");
			ptr1=strstr(gsm_cmd+i, ",");
			if(end_sep != NULL)
			{
				j=0;
				while((gsm_cmd+i+j) != end_sep)
				{
					j++;
					gsm_read_pos_forward();
				}
				for(i=0; i<2; i++)
					gsm_read_pos_forward();
			}

			if(ptr1 != NULL)
			{	
				*text_num_len = end_sep-ptr1-1;
				memcpy(text_num, ptr1+1, *text_num_len);
			}
			
			ret = OK;
			goto error;
		}
		else
		{	
			for(i=0; i<len; i++)
				gsm_read_pos_forward();
		}
	}
error:
	return ret;
}

int parse_text_content(u8* ack, int time, int need_recv_len, u8* text)
{
	
	int i, j,k;
	int len;
	int ack_len;
	u8 error = 1;
	u8 time_flag=10;
	char* ptr=NULL;
	char* ptr1=NULL;
	char* ptr2=NULL;
	char* end_sep=NULL;
	u8 ret = ERROR;
	int diff;
	int lread;
	static u8 flag=0;
	u8 l_gsm_cmd[512];


	//ack_len = strlen(ack)+4;

	while(gsm_write == gsm_read)
	{
		Delay_ms(100);
	}
	Delay_ms(100);
	

	len = (gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF;
	printf("gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
	lread = gsm_read;
	for(i=0; i<len; i++)
	{	
		//printf("%d: ", i);
		if(lread+i > MAX_GSM_BUF-1)
			printf("%c", gsm_buf[lread+i-MAX_GSM_BUF]);
		else
			printf("%c", gsm_buf[lread+i]);
	}
	printf("\n");
	if(len >= need_recv_len)
	{
		memset(l_gsm_cmd, 0, sizeof(l_gsm_cmd));
		for(i=0; i<len; i++)
		{	
			if(lread+i > MAX_GSM_BUF-1)
				l_gsm_cmd[i] = gsm_buf[lread+i-MAX_GSM_BUF];
			else
			 	l_gsm_cmd[i] = gsm_buf[lread+i];
		}

		//printf("len: %d, gsm_cmd: %s\n", len, gsm_cmd);
		ptr = strstr(l_gsm_cmd, ack);
		if(ptr != NULL)
		{
			printf("find ack: %s\n", ack);
			i=0;
			while((l_gsm_cmd+i) != ptr)
			{
				i++;
				gsm_read_pos_forward();
			}
			end_sep = strstr(l_gsm_cmd+i, "\r\n");
			if(end_sep != NULL)
			{
				j=0;
				while((l_gsm_cmd+i+j) != end_sep)
				{
					j++;
					gsm_read_pos_forward();
				}
			}
			ptr1=strstr(l_gsm_cmd+i+j+2, "\r\n");
			if(ptr1 != NULL)
			{	
				k=0;
				while((l_gsm_cmd+i+j+k) != ptr1)
				{
					k++;
					gsm_read_pos_forward();
				}
				memcpy(text, l_gsm_cmd+i+j+2, k-2);
				printf("len: %d, text: %s\n", k-2, text);
				gsm_read_pos_forward();
				gsm_read_pos_forward();
			}
			
			ret = OK;
			goto error;
		}
		else
		{	
			for(i=0; i<len; i++)
				gsm_read_pos_forward();
		}
	}
error:
	return ret;
	
}

int wait_for_end_call()
{	
	u8 i, len;
	while(gsm_write == gsm_read)
	{
		Delay_ms(100);
	}
	Delay_ms(100);

	len = (gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF;
	printf("gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
	for(i=0; i<len; i++)
		printf("%c", gsm_buf[gsm_read+i]);
	printf("\n");

	for(i=0; i<len; i++)
		gsm_read_pos_forward();
}


int parse_dial_rsp(u8* ack, int time, int need_recv_len)
{
	u8 i, j,len=0;
	u8 ack_len;
	u8 error = 1;
	u8 time_flag=10;
	char* ptr=NULL;
	char* ptr1=NULL;
	char* end_sep=NULL;
	u8 ret = ERROR;
	u8 diff;
	static u8 flag=0;

	//ack_len = strlen(ack)+4;

	while(gsm_write == gsm_read)
	{
		Delay_ms(100);
	}
	Delay_ms(100);
	

	len = (gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF;
	printf("gsm_write: %d, gsm_read: %d\n", gsm_write, gsm_read);
	for(i=0; i<len; i++)
		printf("%c", gsm_buf[gsm_read+i]);
	printf("\n");
	if(len >= need_recv_len)
	{
		memset(gsm_cmd, 0, sizeof(gsm_cmd));
		memcpy(gsm_cmd, gsm_buf+gsm_read, len);

		//printf("len: %d, gsm_cmd: %s\n", len, gsm_cmd);
		ptr = strstr(gsm_cmd, ack);
		if(ptr != NULL)
		{
			printf("find ack: %s\n", ack);
			i=0;
			while((gsm_cmd+i) != ptr)
			{
				i++;
				gsm_read_pos_forward();
			}
			end_sep = strstr(gsm_cmd+i, "\r\n");
			if(end_sep != NULL)
			{
				j=0;
				while((gsm_cmd+i+j) != end_sep)
				{
					j++;
					gsm_read_pos_forward();
				}
				for(i=0; i<2; i++)
					gsm_read_pos_forward();
			}
			ret = OK;
			goto error;
		}
		else
		{	
			for(i=0; i<len; i++)
				gsm_read_pos_forward();
		}
	}
error:
	return ret;
}


u8 parse_gprs_live_rsp()
{
	u8 ret=ERROR;
	u8 buf[4], i;
	
	while((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 1)
	{
		if(gsm_buf[gsm_read] == GPRS_FRAME_HEAD)
		{
			gsm_read_pos_forward();	
			if(gsm_buf[gsm_read] == 0x00)
			{
				gsm_read_pos_forward();
				for(i=0; i<2; i++)
				{
					buf[i] = gsm_buf[gsm_read];
					gsm_read_pos_forward();
				}
				if((buf[0] == NET_TCP_ALIVE_RSP) && (buf[1] == 0x01))
					return OK;
			}
			else
				gsm_read_pos_backward();
			
		}
		gsm_read_pos_forward();	
	}
	
	return ret;
}

u8 recv_one_cmd_from_gprs(u8* rcv, u8* one_cmd)
{
	u8 ret=ERROR;
	u8 i, len;
	
	while((gsm_write+MAX_GSM_BUF-gsm_read)%MAX_GSM_BUF >= 2)
	{
		if(gsm_buf[gsm_read] == GPRS_FRAME_HEAD)
		{
			gsm_read_pos_forward();	
			len = gsm_buf[gsm_read];
			rcv[0] = len;
			gsm_read_pos_forward();
			printf("recv one command fd %x ", len);
			for(i=0; i<len+2; i++)
			{
				rcv[i+1] = gsm_buf[gsm_read];
				gsm_read_pos_forward();
				printf("%x ", rcv[i+1]);
			}
			printf("\n");
			*one_cmd = 1;
			return OK;
		}
		gsm_read_pos_forward();	
	}
	
	return ret;
}

u8 gsm_buf_init()
{
	int i=512;
	for(i=0; i<MAX_GSM_BUF; i++)
		gsm_buf[i] = 0;

	return OK;
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
