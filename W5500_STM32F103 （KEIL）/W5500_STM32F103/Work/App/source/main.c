#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"
#include "ds1302.h"
#include "at24c16.h"
#include "alarm.h"
#include "gsm.h"
#include "adc.h"
#include "W5500\w5500.h"
#include "W5500\socket.h"
#include "W5500\spi2.h"
#include "APPs\loopback.h"
#include "keys.h"
#include "screan.h"
#include "voice.h"

#include <string.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def; 

// Configuration Network Information of W5500
uint8 Enable_DHCP = OFF;
uint8 MAC[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x02};//MAC Address
uint8 IP[4] = {192, 168, 88, 11};//IP Address
uint8 GateWay[4] = {192, 168, 88, 1};//Gateway Address
uint8 SubNet[4] = {255, 255, 255, 0};//SubnetMask Address

//TX MEM SIZE- SOCKET 0-7:2KB
//RX MEM SIZE- SOCKET 0-7:2KB
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

//FOR TCP Client
//Configuration Network Information of TEST PC
uint8 Dest_IP[4] = {192, 168, 88, 221}; //DST_IP Address 
uint16 Dest_PORT = 8081; //DST_IP port

uint8 ch_status[MAX_SOCK_NUM] = { 0, };	/** 0:close, 1:ready, 2:connected 3:init*/

uint8 TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications
uint8 RX_BUF[TX_RX_MAX_BUF_SIZE]; // RX Buffer for applications

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void Timer_Configuration(void);

/* Private functions ---------------------------------------------------------*/
__IO uint32_t Timer3_Counter;
uint32_t my_time;
uint32_t presentTime;
uint32_t keep_alive_time;
uint32_t cruise_interval;
uint32_t net_check_time;

__IO uint8_t Tx_Idx = 0;
uint8_t TxBuffer[1024];

void Timer3_ISR(void)
{
  if (Timer3_Counter++ > 1000) { // 1m x 1000 = 1sec
    Timer3_Counter = 0;
    my_time++;
  }
}

void WIZ_Config(void)
{
  // Call Set_network(): Set network configuration, Init. TX/RX MEM SIZE., and  Set RTR/RCR
  Set_network(); // at util.c
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


u8 serial[12];


u8 read_io_data()
{
	u16 input = 0;
	u8 i, ret;
	
	while(1)
	{	
		for(i=0; i<16; i++)
		{
			ret = GPIO_ReadInputDataBit(GPIOC,IO_INPUT_0<<i);
			if (ret)
				input |= 1<<i;
		}
	}
}
u8 write_io_data()
{	
	
	GPIO_WriteBit(GPIOC, IO_OUTPUT_0, Bit_SET);
	return OK;
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{  
	int i;
	//mode: 0:tcp mode 1:gprs mode
	u8 mode=1;
	keyProcessHandle = set_Page;
	RCC_Configuration(); // Configure the system clocks
	NVIC_Configuration(); // NVIC Configuration
	GPIO_Configuration();

	adc_init();
	DS1302_Init();
	USART1_Init();
	USART2_Init();
	Timer_Configuration();
	txsize[0]=at24c16_read(0x0100);
	txsize[1]=at24c16_read(0x1200);
	at24c16_write(0x0100,0x08);
	at24c16_write(0x1200,0x11);
	txsize[2]=at24c16_read(0x01);
	txsize[3]=at24c16_read(0x12);
	printf("run here: \n");
	
	// CRC Æô¶¯
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	Reset_W5500();
	WIZ_SPI_Init();	
	WIZ_Config(); // network config & Call Set_network ();
	presentTime = my_time; // For TCP client's connection request delay

	printf("run here: \n");
	GetSerialNum(serial);
	for(i=0; i<12; i++)
		printf("serial[%d]: 0x%x\n", i, serial[i]);

	while(1) 
	{	
		Delay_ms(300);
		//check_net_mode(4, &mode);
		//gprs_loop(mode);
		//loopback_tcpc(4, 5004, mode);    
		//parse_one_cmd();
		//parse_ack("OK");
		//alarm_in_test();
		//ds1302_get_time_test();
		//adc_test();
		//Delay_ms(300);
	//	led_green(0);
		//flash_test();
		
	}
  
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  // Port A output
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin   =  USART2_TX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // Configure the USART2_Rx as input floating
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin   = USART2_RX;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  
  // Configure the GPIO ports( USART1 Transmit and Receive Lines)
  // Configure the USART1_Tx as Alternate function Push-Pull
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin   =  USART1_TX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // Configure the USART1_Rx as input floating
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin   = USART1_RX | AD_IN1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //PortA
  GPIO_InitStructure.GPIO_Pin   = LED0_P| LED0_S| LED1_P|LED1_S;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // Port B
  GPIO_InitStructure.GPIO_Pin   = LED3_P | LED3_S|WIZ_SCS | PB8 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, WIZ_SCS);
  //GPIO_SetBits(GPIOB, WIZ_RESET);
  GPIO_SetBits(GPIOB, PB8);
  
  // SPI 2
  /* Configure SPIy pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin   = WIZ_SCLK | WIZ_MISO | WIZ_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //Port B bidirection SDA
  GPIO_InitStructure.GPIO_Pin = I2C_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //Port B SCK
  GPIO_InitStructure.GPIO_Pin = I2C_SCK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // Port C input
  //GPIO_InitStructure.GPIO_Pin   = WIZ_INT;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  //GPIO_Init(GPIOC, &GPIO_InitStructure);

	//
	  GPIO_InitStructure.GPIO_Pin   = LED2_P| LED2_S;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  	//ALARM IN
	GPIO_InitStructure.GPIO_Pin   = IO_INPUT_1|IO_INPUT_2 |
										IO_INPUT_3|IO_INPUT_4|IO_INPUT_5 |
										IO_INPUT_6|IO_INPUT_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = IO_INPUT_0 | IO_INPUT_8 |IO_INPUT_9|IO_INPUT_10|WIZ_INT|
										IO_INPUT_11 | IO_INPUT_12 |IO_INPUT_13|IO_INPUT_14|IO_INPUT_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = WIZ_RESET ;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD, WIZ_RESET);


	GPIO_InitStructure.GPIO_Pin   = IO_OUTPUT_0 | IO_OUTPUT_1 |IO_OUTPUT_2|IO_OUTPUT_3|
										IO_OUTPUT_4 | IO_OUTPUT_5 |IO_OUTPUT_6|IO_OUTPUT_7|
										KEY0|KEY1|KEY2|KEY3 | BEEP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  
  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
    
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  
  /* TIM2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3|RCC_APB1Periph_SPI2 |RCC_APB1Periph_USART2, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
                         |RCC_APB2Periph_AFIO  | RCC_APB2Periph_USART1 |RCC_APB2Periph_ADC1 |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
  
  
  
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
  
#else  /* VECT_TAB_FLASH  */
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  
  /* Enable the TIM2 global Interrupt */
  //NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  
  /* Set the Vector Table base location at 0x08002000 -> USE AIP*/ 
  // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);  
  // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);  
#endif
}



void Timer_Configuration(void)
{
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  /* Time base configuration */	
  TIM_TimeBaseStructure.TIM_Period = 1000;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0;      
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, 71, TIM_PSCReloadMode_Immediate);
  
  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
  
  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  
  


}





