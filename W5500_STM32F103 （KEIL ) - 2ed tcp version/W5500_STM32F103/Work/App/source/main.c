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

#include <string.h>
#include <stdlib.h>
#include "i2c_ee.h"


sys_para sys;


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

	GPIO_InitStructure.GPIO_Pin   = SIM900A_POW | SIM900A_RST ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  
  // Port B
  GPIO_InitStructure.GPIO_Pin   = WIZ_SCS ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, WIZ_SCS);
 
  
  // SPI 1
  /* Configure SPIy pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin   = WIZ_SCLK | WIZ_MISO | WIZ_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = WIZ_INT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  
  GPIO_InitStructure.GPIO_Pin	= WIZ_RESET ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, WIZ_RESET);
  
  //Port B bidirection SDA
  GPIO_InitStructure.GPIO_Pin = I2C_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //Port B SCK
  GPIO_InitStructure.GPIO_Pin = I2C_SCK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


	//ALARM OUT
	GPIO_InitStructure.GPIO_Pin   = IO_OUTPUT_0|IO_OUTPUT_1|IO_OUTPUT_2|IO_OUTPUT_3;
										
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  	//ALARM IN
	GPIO_InitStructure.GPIO_Pin   = IO_INPUT_0|IO_INPUT_1|IO_INPUT_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = IO_INPUT_3 | IO_INPUT_4 |IO_INPUT_5|IO_INPUT_6|
										IO_INPUT_7 | IO_INPUT_8 |IO_INPUT_9|IO_INPUT_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = IO_INPUT_11|IO_INPUT_12|IO_INPUT_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = IO_INPUT_14|IO_INPUT_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	//KEY
	GPIO_InitStructure.GPIO_Pin   = KEY0|KEY1|KEY2|KEY3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//BEEP
	GPIO_InitStructure.GPIO_Pin   = BEEP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	  //LED
	GPIO_InitStructure.GPIO_Pin   = LED0_P| LED0_S| LED1_P|LED1_S|LED2_P| LED2_S| LED3_P|LED3_S;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
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
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3 |RCC_APB1Periph_USART2 , ENABLE);
  
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


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{  
	//int i;
	//mode: 1:tcp mode 0:gprs mode
	u8 mode=1;
	uint32_t gsm_init_time;
	static u8 l_gsm_init=0;
	
	//char cell_num[12];
	RCC_Configuration(); // Configure the system clocks
	NVIC_Configuration(); // NVIC Configuration
	GPIO_Configuration();

	//LED5_onoff(ON);
	//RTC_init();
	//DS1302Init();
	Adc_Init();
	beep_off();
	
	DS1302_Init();
	USART1_Init();
	USART2_Init();
	Timer_Configuration();

	Reset_W5500();
	//reset_sim900a();
	WIZ_SPI_Init();
	get_para(&sys);
	Set_network(&sys);// network config & Call Set_network ();
	set_tcp_utils(); // For TCP client's connection request delay
	set_current_time(&gsm_init_time);
	//GetSerialNum(serial);
	//for(i=0; i<12; i++)
	//	printf("serial[%d]: 0x%x\n", i, serial[i]);

	led_off(0);
	led_off(1);
	led_off(2);
	led_off(3);
	//led_test(0);


	//flash_save();
	//auto_keep_alive(4);
	//alarm_out_test();
	//gsm_test();

	//I2C_Test();
	//ds1302_set_time_test();
	//set_alarm_out(3, 1);
	
	while(0)
	{
		Delay_ms(500);
		led_off(0);
		//alarm_in_test();
		alarm_out_test();
		Delay_ms(500);
		led_green(0);
	}
	while(1) 
	{	
		#if 1
		if(!l_gsm_init)
		{
			if(time_return() - gsm_init_time >= (1 * 10)) 
			{  	
				if(gsm_init())
					printf("gsm_init error\n");
				else
				{	
					printf("gsm_init success\n");
					gsm_test();
					sys.gsm_init_flag = 1;
				}
				l_gsm_init = 1;
				gsm_init_time = time_return();
				
			}
		}
		#endif
		Delay_ms(500);
		led_off(0);
		//check_net_mode(4, &mode);

		if(sys.gsm_init_flag)
			//gprs_loop(mode);
		//parse_one_cmd();
		loopback_tcpc(4, 5004, mode);
		//adc_test();
		//loopback_tcp_test(5, 5104, mode);  
		
		//parse_ack("OK");
		//alarm_in_test();
		//ds1302_get_time_test();
		
		Delay_ms(500);
		led_green(0);
		//flash_test();
		
	}
  
}



