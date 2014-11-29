#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
//=================================================


//Port A

#define WIZ_RESET		    GPIO_Pin_0
#define WIZ_INT		    	GPIO_Pin_1
#define USART2_TX		    GPIO_Pin_2	// out
#define USART2_RX		    GPIO_Pin_3	// in 
#define WIZ_SCS			    GPIO_Pin_4	// out
#define WIZ_SCLK			GPIO_Pin_5	// out
#define WIZ_MISO			GPIO_Pin_6	// in
#define WIZ_MOSI			GPIO_Pin_7	// out
#define SIM900A_RST			GPIO_Pin_8	// 
#define USART1_TX		    GPIO_Pin_9	// out
#define USART1_RX		    GPIO_Pin_10	// in 
#define SIM900A_POW			GPIO_Pin_11	// 
#define PA12				GPIO_Pin_12	// 
#define PA13				GPIO_Pin_13	// 
#define PA14				GPIO_Pin_14	// out
#define PA15				GPIO_Pin_15	// out

//Port B

#define PB0			        GPIO_Pin_0	// in
#define PB1 				GPIO_Pin_1	// 
#define PB2					GPIO_Pin_2	// 
#define PB3					GPIO_Pin_3	// 
#define PB4					GPIO_Pin_4	// 
#define PB5					GPIO_Pin_5	// 
#define IO_INPUT_11			GPIO_Pin_5
#define I2C_SCK	            GPIO_Pin_6  //out
#define I2C_SDA	            GPIO_Pin_7  //bidirection
#define PB8		            GPIO_Pin_8	// out
#define IO_INPUT_12			GPIO_Pin_8
#define IO_INPUT_13			GPIO_Pin_9
#define PB10			  	GPIO_Pin_10	// 
#define PB11				GPIO_Pin_11	// 



//Port C

#define AD_IN1				GPIO_Pin_1
#define AD_IN2				GPIO_Pin_2
#define IO_OUTPUT_0			GPIO_Pin_6
#define IO_OUTPUT_1			GPIO_Pin_7
#define IO_OUTPUT_2			GPIO_Pin_8
#define IO_OUTPUT_3			GPIO_Pin_9
#define IO_INPUT_0			GPIO_Pin_10
#define IO_INPUT_1			GPIO_Pin_11
#define IO_INPUT_2			GPIO_Pin_12
#define PC13				GPIO_Pin_13	//
#define PC14				GPIO_Pin_14	//
#define PC15				GPIO_Pin_15	//


// Port D

#define IO_INPUT_3			GPIO_Pin_0
#define IO_INPUT_4			GPIO_Pin_1
#define IO_INPUT_5			GPIO_Pin_2
#define IO_INPUT_6			GPIO_Pin_3
#define IO_INPUT_7			GPIO_Pin_4
#define IO_INPUT_8			GPIO_Pin_5
#define IO_INPUT_9			GPIO_Pin_6
#define IO_INPUT_10			GPIO_Pin_7


//Port E

#define IO_INPUT_14			GPIO_Pin_0
#define IO_INPUT_15			GPIO_Pin_1
#define KEY0				GPIO_Pin_2
#define KEY1				GPIO_Pin_3
#define KEY2				GPIO_Pin_4
#define KEY3				GPIO_Pin_5
#define BEEP				GPIO_Pin_7
#define LED0_P				GPIO_Pin_8
#define LED0_S				GPIO_Pin_9
#define LED1_P				GPIO_Pin_10
#define LED1_S				GPIO_Pin_11
#define LED2_P				GPIO_Pin_12
#define LED2_S				GPIO_Pin_13
#define LED3_P				GPIO_Pin_14
#define LED3_S				GPIO_Pin_15





//=================================================
typedef struct _CONFIG_MSG
{
	uint8 Mac[6];
	uint8 Lip[4];
	uint8 Sub[4];
	uint8 Gw[4];
	uint8 DNS_Server_IP[4];	
	uint8  DHCP;
	uint16 any_port;
}CONFIG_MSG;


typedef struct _CONFIG_TYPE_DEF
{
	uint16 port;
	uint8 destip[4];
}CHCONFIG_TYPE_DEF;

typedef struct
{
	u8 link_group;
	u8 xor[16];
	u16 out[16];
	u32 in[16];
}link_para;


typedef struct
{	
	u8 cell_enable;
	u8 cell_group;
	u8 cell_xor;
	u8 cell[5][12];
}cell_para;


typedef struct
{
	u8 text_group;
	u8 text_enable;
	u8 text_length[5];
	u8 text[5][12];
	u8 text_content[5][140];
	u8 send_ok[5];
	u32 time[5];
	
}text_para;



typedef struct
{
	u8 tcp_flag;
	u8 gprs_flag;
	u8 is_net_loss;
	u8 cell_alarm;
	u8 text_alarm;
	u8 gsm_init_flag;
	u8 gprs_init_flag;
	u8 need_in_net;
	u8 tele_operator; //0 chn-mobile 1-chn-unicom
	u32 arming;
	u32 machine_id;
	CONFIG_MSG net_para;
	CHCONFIG_TYPE_DEF dest_net_para;
	link_para link;
	cell_para cell;
	text_para text;
} sys_para;
	
#define SOCK_CONFIG		2	// UDP
#define SOCK_DNS		2	// UDP
#define SOCK_DHCP		3	// UDP

#define MAX_BUF_SIZE		1460
#define KEEP_ALIVE_TIME	30	// 30sec

#define ON	1
#define OFF	0

#define HIGH		1
#define LOW		0


#define MAX_ALARM_IN 	16
#define MAX_LINK_GROUP  16
#define MAX_LINK_IN	 	16
#define MAX_LINK_OUT	4
#define MAX_ALARM_OUT	4
#define MAX_KEY_NUM		4

#define POWER_OFF_THR	2800
#define BATTERY_LOWPOW_THR 2200

#define KEY_NORMAL 1
#define KEY_PRESSED 0
//#define __GNUC__

// SRAM address range is 0x2000 0000 ~ 0x2000 4FFF (20KB)
#define TX_RX_MAX_BUF_SIZE	2048
//#define TX_BUF	0x20004000
//#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)



#define ApplicationAddress 	0x08004000


#endif

