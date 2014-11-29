#ifndef __DS1302_H
#define __DS1302_H


#include "stm32f10x.h"


#if 0
#define   RdSec  0x81
#define   RdMin  0x83
#define   RdHour  0x85
#define   RdDate  0x87
#define   RdMonth  0x89
#define   RdWeek  0x8b
#define   RdYear  0x8d
#define   RdControl          0x8f
#define   RdTrickleCharge 0x91
#define   RdClockBurst  0xbf
#define   WrSec  0x80
#define   WrMin  0x82
#define   WrHour  0x84
#define   WrDate  0x86
#define   WrMonth  0x88
#define   WrWeek  0x8a
#define   WrYear  0x8c
#define   WrControl         0x8e
#define   WrTrickleCharge 0x90
#define   WrClockBurst  0xbe



#define DS1302_IORCC RCC_APB2Periph_GPIOD	

#define DS1302_PORT GPIOD

#define DS1302_SCK_PIN GPIO_Pin_10		
#define DS1302_IO_PIN GPIO_Pin_11
#define DS1302_CE_PIN GPIO_Pin_12

#define DS1302_CLRSCK() (GPIOD->BRR = GPIO_Pin_10)		
#define DS1302_SETSCK() (GPIOD->BSRR = GPIO_Pin_10)

#define DS1302_CLRIO() (GPIOD->BRR = GPIO_Pin_11)
#define DS1302_SETIO() (GPIOD->BSRR = GPIO_Pin_11)

#define DS1302_CLRCE() (GPIOD->BRR = GPIO_Pin_12)
#define DS1302_SETCE() (GPIOD->BSRR = GPIO_Pin_12)


#define DS1302_IO_IN()  {GPIOD->CRH&=0XFFFFFFF0;GPIOD->CRH|=8<<0;}  
#define DS1302_IO_OUT() {GPIOD->CRH&=0XFFFFFFF0;GPIOD->CRH|=3<<0;}

#define DS1302_READ_SDA()    (GPIO_ReadInputDataBit(DS1302_PORT, DS1302_IO_PIN))



typedef struct
{
	u8 year;
	u8 month;
	u8 date;
	u8 week;
	u8 hour;
	u8 min;
	u8 sec;
}TIME_TypeDef;	


void DS1302_Write8bit(u8 code);
u8 DS1302_Read8bit(void);

extern void RTC_init (void);
extern u8 DS1302_ReadByte(u8 con);
extern void DS1302_WriteByte(u8 con,u8 code);

extern void DS1302_WrintTime(TIME_TypeDef* time);
extern void DS1302_ReadTime(TIME_TypeDef* time);

#endif
#define DS1302_SCLK_GPIO  GPIOD
#define DS1302_SCLK_Pin   GPIO_Pin_10
#define DS1302_RST_GPIO   GPIOD
#define DS1302_RST_Pin    GPIO_Pin_12
#define DS1302_IO_GPIO    GPIOD
#define DS1302_IO_Pin     GPIO_Pin_11
#define DS1302_GPIO_CLOCK RCC_APB2Periph_GPIOD


#define DS1302_SCLK_H()   (GPIO_SetBits(DS1302_SCLK_GPIO, DS1302_SCLK_Pin))
#define DS1302_SCLK_L()   (GPIO_ResetBits(DS1302_SCLK_GPIO, DS1302_SCLK_Pin))
#define DS1302_RST_H()    (GPIO_SetBits(DS1302_RST_GPIO, DS1302_RST_Pin))
#define DS1302_RST_L()    (GPIO_ResetBits(DS1302_RST_GPIO, DS1302_RST_Pin))
#define DS1302_IO_H()     (GPIO_SetBits(DS1302_IO_GPIO, DS1302_IO_Pin))
#define DS1302_IO_L()     (GPIO_ResetBits(DS1302_IO_GPIO, DS1302_IO_Pin))
#define DS1302_IO_IN()    (SetDS1302IO_In())
#define DS1302_IO_OUT()   (SetDS1302IO_Out())
#define DS1302_IO_STATE() (GPIO_ReadInputDataBit(DS1302_IO_GPIO, DS1302_IO_Pin))

#define DS1302_SECOND     0x80
#define DS1302_MINUTE     0x82
#define DS1302_HOUR       0x84
#define DS1302_DAY        0x86
#define DS1302_MONTH      0x88
#define DS1302_WEEK       0x8A
#define DS1302_YEAR       0x8C
#define DS1302_PROTECT    0x8E

#define DS1302_RD       0x01
#define DS1302_WR       0x00

typedef struct
{
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t week;
  uint8_t day;
  uint8_t month;
  uint8_t year;
}STU_TIME;


static void SetDS1302IO_In(void);
static void SetDS1302IO_Out(void);
void DS1302_Init(void);
void Write1302(uint8_t addr, uint8_t dat);
uint8_t Read1302(uint8_t addr);
void ReadTime(void);
void Start1302(void);
void Stop1302(void);
u8 ds1302_set_time(u8* src);
u8 ds1302_get_time(u8* dst);



#endif
