#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
//=================================================


//Port A
#define AD_IN1				GPIO_Pin_1
#define LED0_P				GPIO_Pin_4
#define LED0_S				GPIO_Pin_5
#define LED1_P				GPIO_Pin_6
#define LED1_S				GPIO_Pin_7
#define USART2_TX		        GPIO_Pin_2	// out
#define USART2_RX		        GPIO_Pin_3	// in 
#define PA4				GPIO_Pin_4	// 
#define PA5				GPIO_Pin_5	// 
#define PA6				GPIO_Pin_6	// 
#define PA7				GPIO_Pin_7	// 
#define PA8				GPIO_Pin_8	// 
#define USART1_TX		        GPIO_Pin_9	// out
#define USART1_RX		        GPIO_Pin_10	// in 
#define PA11				GPIO_Pin_11	// 
#define PA12				GPIO_Pin_12	// 
#define PA13				GPIO_Pin_13	// 
#define PA14				GPIO_Pin_14	// out
#define PA15				GPIO_Pin_15	// out

//Port B

#define LED3_P				GPIO_Pin_0
#define LED3_S				GPIO_Pin_1
#define PB0			        GPIO_Pin_0	// in
#define PB1 				GPIO_Pin_1	// 
#define PB2				GPIO_Pin_2	// 
#define PB3				GPIO_Pin_3	// 
#define PB4				GPIO_Pin_4	// 
#define PB5				GPIO_Pin_5	// 
#define I2C_SCK	                        GPIO_Pin_6  //out
#define I2C_SDA	                        GPIO_Pin_7  //bidirection
#define PB8		                GPIO_Pin_8	// out
//#define WIZ_RESET		        GPIO_Pin_9	// out
#define PB10			  	GPIO_Pin_10	// 
#define PB11				GPIO_Pin_11	// 
#define WIZ_SCS			        GPIO_Pin_12	// out
#define WIZ_SCLK			GPIO_Pin_13	// out
#define WIZ_MISO			GPIO_Pin_14	// in
#define WIZ_MOSI			GPIO_Pin_15	// out

//Port C

#define LED2_P				GPIO_Pin_4
#define LED2_S				GPIO_Pin_5
#define IO_INPUT_1			GPIO_Pin_6
#define IO_INPUT_2			GPIO_Pin_7
#define IO_INPUT_3			GPIO_Pin_8
#define IO_INPUT_4			GPIO_Pin_9
#define IO_INPUT_5			GPIO_Pin_10
#define IO_INPUT_6			GPIO_Pin_11
#define IO_INPUT_7			GPIO_Pin_12
//#define WIZ_INT				GPIO_Pin_13	//
#define PC13				GPIO_Pin_13	//
#define PC14				GPIO_Pin_14	//
#define PC15				GPIO_Pin_15	//


// Port D
#define IO_INPUT_0			GPIO_Pin_15
#define IO_INPUT_8			GPIO_Pin_0
#define IO_INPUT_9			GPIO_Pin_1
#define IO_INPUT_10			GPIO_Pin_2
#define IO_INPUT_11			GPIO_Pin_3
#define IO_INPUT_12			GPIO_Pin_4
#define IO_INPUT_13			GPIO_Pin_5
#define IO_INPUT_14			GPIO_Pin_6
#define IO_INPUT_15			GPIO_Pin_7
#define WIZ_INT		    	GPIO_Pin_8
#define WIZ_RESET		    GPIO_Pin_9




//Port E
#define KEY0				GPIO_Pin_2
#define KEY1				GPIO_Pin_3
#define KEY2				GPIO_Pin_4
#define KEY3				GPIO_Pin_5
#define BEEP				GPIO_Pin_7

#define IO_OUTPUT_0			GPIO_Pin_8			
#define IO_OUTPUT_1			GPIO_Pin_9	
#define IO_OUTPUT_2			GPIO_Pin_10	
#define IO_OUTPUT_3			GPIO_Pin_11	
#define IO_OUTPUT_4			GPIO_Pin_12			
#define IO_OUTPUT_5			GPIO_Pin_13	
#define IO_OUTPUT_6			GPIO_Pin_14	
#define IO_OUTPUT_7			GPIO_Pin_15	






//=================================================
typedef struct _CONFIG_MSG
{
	uint8 Mac[6];
	uint8 Lip[4];
	uint8 Sub[4];
	uint8 Gw[4];
	uint8 DNS_Server_IP[4];	
	uint8  DHCP;
}
CONFIG_MSG;


typedef struct _CONFIG_TYPE_DEF
{
	uint16 port;
	uint8 destip[4];
}CHCONFIG_TYPE_DEF;

	
#define SOCK_CONFIG		2	// UDP
#define SOCK_DNS		2	// UDP
#define SOCK_DHCP		3	// UDP

#define MAX_BUF_SIZE		1460
#define KEEP_ALIVE_TIME	30	// 30sec

#define ON	1
#define OFF	0

#define HIGH		1
#define LOW		0

//#define __GNUC__

// SRAM address range is 0x2000 0000 ~ 0x2000 4FFF (20KB)
#define TX_RX_MAX_BUF_SIZE	2048
//#define TX_BUF	0x20004000
//#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)
extern uint8 TX_BUF[TX_RX_MAX_BUF_SIZE];
extern uint8 RX_BUF[TX_RX_MAX_BUF_SIZE];


#define ApplicationAddress 	0x08004000


#endif

