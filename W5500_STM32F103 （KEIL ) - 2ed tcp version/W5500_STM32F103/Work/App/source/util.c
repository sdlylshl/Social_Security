#include "stm32f10x.h"
#include "config.h"
#include "util.h"
#include "W5500\w5500.h"
#include <stdio.h>
#include <stdarg.h>

/* Private functions ---------------------------------------------------------*/
__IO uint32_t Timer3_Counter;
uint32_t my_time;



#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


int reset_sim900a()
{	
	printf("reset sim900a\n");
	GPIO_ResetBits(GPIOA, SIM900A_RST);
	Delay_ms(500);
	GPIO_SetBits(GPIOA, SIM900A_POW);
  	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
  	GPIO_ResetBits(GPIOA, SIM900A_POW);
  	Delay_ms(500);
}


void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOA, WIZ_RESET);
  Delay_ms(1);
  GPIO_SetBits(GPIOA, WIZ_RESET);
  Delay_ms(10);
}


void SoftReset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

void sys_reset(void)
{
	
}

/*******************************************************************************
* USARTx configured as follow:
- BaudRate = 115200 baud
- Word Length = 8 Bits
- One Stop Bit
- No parity
- Hardware flow control disabled (RTS and CTS signals)
- Receive and transmit enabled
*******************************************************************************/
void USART1_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* Configure the USARTx */
  USART_Init(USART1, &USART_InitStructure);
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);
}


/*******************************************************************************
* USARTx configured as follow:
- BaudRate = 115200 baud
- Word Length = 8 Bits
- One Stop Bit
- No parity
- Hardware flow control disabled (RTS and CTS signals)
- Receive and transmit enabled
*******************************************************************************/
void USART2_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* Configure the USARTx */
  USART_Init(USART2, &USART_InitStructure);
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  
  /* Enable the USARTx */
  USART_Cmd(USART2, ENABLE);
}


__asm void nop(void)
{
    NOP

}


/*******************************************************************************
* Function Name  : Delay_us
* Description    : Delay per micro second.
* Input          : time_us
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_us( u8 time_us )
{
  register u8 i;
  register u8 j;
  for( i=0;i<time_us;i++ )
  {
    for( j=0;j<5;j++ )          // 25CLK
    {
      nop;       //1CLK
      nop;       //1CLK
      nop;       //1CLK
      nop;       //1CLK
      nop;       //1CLK
    }
  }                              // 25CLK*0.04us=1us
}

/*******************************************************************************
* Function Name  : Delay_ms
* Description    : Delay per mili second.
* Input          : time_ms
* Output         : None
* Return         : None
*******************************************************************************/

void Delay_ms( u16 time_ms )
{
  register u16 i;
  for( i=0;i<time_ms;i++ )
  {
    Delay_us(250);
    Delay_us(250);
    Delay_us(250);
    Delay_us(250);
  }
}

/*******************************************************************************
* Function Name  : PUTCHAR_PROTOTYPE
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
PUTCHAR_PROTOTYPE
{
// Write a character to the USART
USART_SendData(USART1, (uint8_t) ch);

//  Loop until the end of transmission
while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
{
  }

return ch;
}
*/
#if 0
int putchar(int ch)
{
  // Write a character to the USART
  USART_SendData(USART1, (uint8_t) ch);
  
  //  Loop until the end of transmission
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }
  
  return ch;
}
#endif
/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口 */
	USART_SendData(USART2, (unsigned char) ch);
	while (!(USART2->SR & USART_FLAG_TXE));
	
	return (ch);
}


#if 0
int fgetc(int ch, FILE *f)
{
	int ch;
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{
	}
	ch = USART_ReceiveData(USART1);
	return ch;

}



int getchar(void)
{
  int ch;
  while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
  {
  }
  ch = USART_ReceiveData(USART1);
  return ch;
}
#endif


void set_current_time(uint32_t* presentTime)
{
	*presentTime = my_time;
}


void Timer3_ISR(void)
{
  if (Timer3_Counter++ > 1000) { // 1m x 1000 = 1sec
    Timer3_Counter = 0;
    my_time++;
  }
}


/*******************************************************************************
* Function Name  : time_return
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

uint32_t time_return(void)
{
  return my_time;
}



void GetSerialNum(u8 *Serial)
{
	u16 Device_Serial0, Device_Serial1;
	u32 Device_Serial2, Device_Serial3;

	Device_Serial0 = *(u16*)(0x1FFFF7E8);
	Device_Serial1 = *(u16*)(0x1FFFF7EA);
	Device_Serial2 = *(u32*)(0x1FFFF7EC);
	Device_Serial3 = *(u32*)(0x1FFFF7F0);

	Serial[0] = (u8)((Device_Serial0 & 0x00FF));  
	Serial[1] = (u8)((Device_Serial0 & 0xFF00) >> 8);
	Serial[2] = (u8)((Device_Serial1 & 0x00FF) >> 8);
	Serial[3] = (u8)( Device_Serial1 & 0xFF00);  

	Serial[4] = (u8)((Device_Serial2 & 0x000000FF));
	Serial[5] = (u8)((Device_Serial2 & 0x0000FF00) >> 8);
	Serial[6] = (u8)((Device_Serial2 & 0x00FF0000) >> 16);
	Serial[7] = (u8)((Device_Serial2 & 0xFF000000) >> 16);  

	Serial[8] = (u8)((Device_Serial3 & 0x000000FF));
	Serial[9] = (u8)((Device_Serial3 & 0x0000FF00) >> 8);
	Serial[10] = (u8)((Device_Serial3 & 0x00FF0000) >> 16);
	Serial[11] = (u8)((Device_Serial3 & 0xFF000000) >> 16);   
}



#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif



