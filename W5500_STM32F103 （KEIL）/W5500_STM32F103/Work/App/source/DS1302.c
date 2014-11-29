#include "ds1302.h"
#include <stdio.h>



#if 0
void RTC_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;
	RCC_APB2PeriphClockCmd(DS1302_IORCC, ENABLE );	
						 
	//GPIOC->CRH&=0XFFF00FFF;
	//GPIOC->CRH|=0X00033000;	   
	GPIO_InitStructure.GPIO_Pin = (DS1302_CE_PIN | DS1302_SCK_PIN);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);
	//GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	//GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = (DS1302_IO_PIN);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;    
	
	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);

	DS1302_WriteByte(WrControl,0x00);  
	 Delay_us(10);
	if(DS1302_ReadByte(RdTrickleCharge) != 0xA6)
	{
		Delay_us(10);
		DS1302_WriteByte(WrTrickleCharge,0xA6);
	}	
	Delay_us(10);
	DS1302_WriteByte(WrControl,0x80);  
}


void DS1302_Write8bit(u8 code)
{
	u8 i;
	DS1302_IO_OUT()
	DS1302_CLRSCK();
	//DS13CLK =0;
	for(i=0;i<8;i++)
	{
		  //Delay_ms(1);
		  Delay_us(5);
		  if(code&0x01) (DS1302_SETIO());
		  else (DS1302_CLRIO());
		  Delay_us(5);
		  //DS13CLK =1;
		  DS1302_SETSCK();
		  Delay_us(5);
		  //DS13CLK =0;
		  DS1302_CLRSCK();
		  code = code >> 1;
	}
}


u8 DS1302_Read8bit(void)
{
	u8 i,code;
	DS1302_IO_IN()
	code=0;
	DS1302_CLRSCK();
	Delay_us(5);
	//DS13CLK =0;
	//DS13IO  =1;
	for(i=0;i<8;i++)
	{
		  //Delay_ms(1);
		  code = code >>1;
		  if(DS1302_READ_SDA()) code = code|0x80;
		  Delay_us(5);
		  DS1302_SETSCK();
		  Delay_us(5);
		  //DS13CLK =1;
		  DS1302_CLRSCK();
		 // DS13CLK =0;
	}
	return code;
}

u8 DS1302_ReadByte(u8 con)
{
	u8 code;
	DS1302_CLRCE();		   
	Delay_us(5);       
	DS1302_CLRSCK();
	Delay_us(5);
	//DS13CLK =0;
	DS1302_SETCE();          
	Delay_us(5);
	DS1302_Write8bit(con);   
	code=DS1302_Read8bit();  
	Delay_us(5);
	DS1302_SETSCK();
	Delay_us(5);
	DS1302_CLRCE();        
	return code;
}



void DS1302_WriteByte(u8 con,u8 code)
{
	DS1302_CLRCE();      
	Delay_us(5);
	DS1302_CLRSCK();
	Delay_us(5);
	//DS13CLK =0;
	DS1302_SETCE();        
	Delay_us(5);
	DS1302_Write8bit(con);    
	DS1302_Write8bit(code); 
	Delay_us(5);
	DS1302_SETSCK();
	Delay_us(5);
	DS1302_CLRCE();        

}

void DS1302_WrintTime(TIME_TypeDef* time)
{
	DS1302_WriteByte(WrControl,0x00);  
		
	DS1302_WriteByte(WrYear,time->year);
	DS1302_WriteByte(WrMonth,time->month);
	DS1302_WriteByte(WrDate,time->date);
	DS1302_WriteByte(WrWeek,time->week);
	DS1302_WriteByte(WrHour,time->hour);
	DS1302_WriteByte(WrMin,time->min);
	DS1302_WriteByte(WrSec,time->sec);

	DS1302_WriteByte(WrControl,0x80);  

}




void DS1302_ReadTime(TIME_TypeDef* time)
{
	time->year = DS1302_ReadByte(RdYear);
	time->month = DS1302_ReadByte(RdMonth);	
	time->date = DS1302_ReadByte(RdDate);
	time->week = DS1302_ReadByte(RdWeek);

	time->hour = DS1302_ReadByte(RdHour);
	time->min = DS1302_ReadByte(RdMin);
	time->sec = DS1302_ReadByte(RdSec);
}

u8 ds1302_set_time(u8* src)
{
	TIME_TypeDef time;

	time.year = *(u16*)(src+2);
	time.month = src[4];
	time.date = src[5];

	time.hour = src[6];
	time.min = src[7];
	time.sec = src[8];

	DS1302_WrintTime(&time);
}


u8 ds1302_get_time(u8* src)
{
	TIME_TypeDef time;
	DS1302_ReadTime(&time);

	*(u16*)src = time.year;
	src[2] = time.month;
	src[3] = time.date;

	src[4] = time.hour;
	src[5] = time.min;
	src[6] = time.sec;

}


u8 ds1302_get_time_test()
{
	u8 src[7];
	u8 dst[7];

	dst[0] = 0x0a;
	dst[1] = 0x5;

	dst[2] = 0x0a;
	dst[3] = 0x5;

	dst[4] = 0x0a;
	dst[5] = 0x5;

	dst[6] = 0x0a;
	
	
	ds1302_set_time(dst);
	
	ds1302_get_time(src);

	
	printf("year: 0x%x, mon: 0x%x, data: 0x%x, h: 0x%x, m: 0x%x, s: 0x%x\n", 
		*(u16*)src, src[2], src[3], src[4], src[5], src[6]);
}
#endif

#if 0
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "ds1302.h"

#define DS1302_RESET_PORT            GPIOD
#define DS1302_RESET_BIT             GPIO_Pin_12
#define DS1302_SCLK_PORT             GPIOD
#define DS1302_SCLK_BIT              GPIO_Pin_10
#define DS1302_DATA_PORT             GPIOD
#define DS1302_DATA_BIT              GPIO_Pin_11


unsigned char CommunicationByte(unsigned char value)
{
    unsigned char i, temp;
   
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0;
    temp = 0;
    i = 0;
    for(i = 0; i < 8; i++)
    {
        //接收数据，数据是紧接着控制字最后一位输出的
        asm("NOP");asm("NOP");asm("NOP");asm("NOP");
        temp >>= 1;//低位在前
        if (GPIO_ReadInputDataBit(DS1302_DATA_PORT, DS1302_DATA_BIT) == 1)
        {
            temp |= 0x80;
        }
      
        //发送数据
        if ((value & 0x01) != 0)
        {
            GPIO_WriteBit(DS1302_DATA_PORT, DS1302_DATA_BIT, Bit_SET);
        }
        else
        {
            GPIO_WriteBit(DS1302_DATA_PORT, DS1302_DATA_BIT, Bit_RESET);
        }
        GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_SET);//sclk = 1, 上升沿
        value >>= 1;//低位在前
        asm("NOP");asm("NOP");asm("NOP");asm("NOP");
        GPIO_WriteBit(DS1302_DATA_PORT, DS1302_DATA_BIT, Bit_SET);//拉高DATA IO,避免影响芯片输出
        
        GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0，下降沿
    }
   
    return temp;
}


void DS1302WriteRegister(unsigned char reg, unsigned char value)
{
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_SET);//只有在SCLK为低电平时候，才允许将rst置位高电平
   
    CommunicationByte(reg);
    CommunicationByte(value);
   
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_RESET);//reset = 0;
}

unsigned char DS1302ReadRegister(unsigned char reg)
{
    unsigned char temp;
   
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_SET);//只有在SCLK为低电平时候，才允许将rst置位高电平
   
    CommunicationByte(reg);
    temp = CommunicationByte(0xff);
   
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_SET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_RESET);//reset = 0;
   
    return temp;
}

void DS1302WriteTime(unsigned char *data)
{
    unsigned char i;

    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_SET);//reset = 1;

    CommunicationByte(0xbe);//连续写 日历/时钟寄存器
   
    for (i = 0; i < 8; i++)
    {
        CommunicationByte(data[i]);
    }
   
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_RESET);//reset = 0;
}


void DS1302TimeConfig(void)
{
    unsigned char temp;
    unsigned char data[8] = {0x01,0x10,0x11,0x30,0x11,0x05,0x12};//for test
   
    temp = DS1302ReadRegister(0xc1);//读取ds1302ram 数据
    if (temp != 0xaa)//第一次上电
    {
        DS1302WriteRegister(0x8e, 0x00);//允许写操作
        DS1302WriteRegister(0xc0, 0xaa);//置标志位
        
        DS1302WriteTime(data);
        DS1302WriteRegister(0x8e, 0x80);//禁止写操作
        
        DS1302WriteRegister(0x80,0x01);//设置秒
        DS1302WriteRegister(0x82,0x18);//设置分
        DS1302WriteRegister(0x84,0x17);//设置时
        DS1302WriteRegister(0x86,0x29);//设置日
        DS1302WriteRegister(0x88,0x11);//设置月
        DS1302WriteRegister(0x8a,0x04);//设置星期
        DS1302WriteRegister(0x8c,0x12);//设置年
        
    }
}


void DS1302Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//打开时钟模块
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS1302_RESET_BIT;//reset
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(DS1302_RESET_PORT, &GPIO_InitStructure);//复位引脚
   
    GPIO_InitStructure.GPIO_Pin = DS1302_SCLK_BIT;
    GPIO_Init(DS1302_SCLK_PORT, &GPIO_InitStructure);//SCLK
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);
   
    GPIO_InitStructure.GPIO_Pin = DS1302_DATA_BIT;//data
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出，通过外接上拉电阻，达到双向通信
    GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);
   
    DS1302TimeConfig();
}










void DS1302ReadTime(unsigned char *buffer)
{
    unsigned char i;

    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_RESET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_SET);//reset = 1;
    CommunicationByte(0xbf);//连续读 日历/时钟寄存器
   
    for (i = 0; i < 8; i++)
    {
        buffer[i] = CommunicationByte(0xff);
    }
   
    GPIO_WriteBit(DS1302_SCLK_PORT, DS1302_SCLK_BIT, Bit_SET);//sclk = 0;
    GPIO_WriteBit(DS1302_RESET_PORT, DS1302_RESET_BIT, Bit_RESET);//reset = 0;   
}




void DS1302TimeUpdate(unsigned char *data)
{
    //data 数据应为压缩bcd码
    DS1302WriteRegister(0x8e, 0x00);//允许写操作
    DS1302WriteRegister(0xc0, 0xaa);//置标志位
        
    DS1302WriteTime(data);//连续写入 时钟/日历 寄存器数据
    DS1302WriteRegister(0x8e, 0x80);//禁止写操作
}



u8 ds1302_get_time_test()
{
	u8 src[8];
	DS1302ReadTime(src);

	
	printf("year: 0x%x, year2: 0x%x, mon: 0x%x, data: 0x%x, h: 0x%x, m: 0x%x, s: 0x%x\n", 
		src[0], src[1], src[2], src[3], src[4], src[5], src[6]);
}

#endif


STU_TIME utime;

/**************************************
函数功能：设置DS1302 对应IO为输入模式
**************************************/
static void SetDS1302IO_In(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = DS1302_IO_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(DS1302_IO_GPIO, &GPIO_InitStructure);
}

/**************************************
函数功能：设置DS1302 对应IO为输出模式
**************************************/
static void SetDS1302IO_Out(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = DS1302_IO_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS1302_IO_GPIO, &GPIO_InitStructure);
}

/**************************************
函数功能：配置DS1302对应控制引脚
**************************************/
void DS1302_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(DS1302_GPIO_CLOCK, ENABLE);
  //DS1302 IO引脚初始化为推挽输出
  GPIO_InitStructure.GPIO_Pin = DS1302_IO_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS1302_IO_GPIO, &GPIO_InitStructure);
  //DS1302 RST引脚初始化为推挽输出
  GPIO_InitStructure.GPIO_Pin = DS1302_RST_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS1302_RST_GPIO, &GPIO_InitStructure);
  //DS1302 SCLK引脚初始化为推挽输出
  GPIO_InitStructure.GPIO_Pin = DS1302_SCLK_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS1302_SCLK_GPIO, &GPIO_InitStructure);
}
/**************************************
函数功能：向DS1302指定地址写入指定数据
**************************************/
void Write1302(uint8_t addr, uint8_t dat)
{
  uint8_t i;
  
  DS1302_RST_L();
  DS1302_SCLK_L();
  DS1302_RST_H();
  DS1302_IO_OUT();
  for(i = 8; i > 0; i--)
  {
    DS1302_SCLK_L();
    if(addr & 0x01)
      DS1302_IO_H();
    else
      DS1302_IO_L();
    addr >>= 1;
    DS1302_SCLK_H();
  }
  for(i = 8; i > 0; i--)
  {
    DS1302_SCLK_L();
    if(dat & 0x01)
      DS1302_IO_H();
    else
      DS1302_IO_L();
    dat >>= 1;
    DS1302_SCLK_H();
  }
  DS1302_RST_L();
}
/**************************************
函数功能：从DS1302指定地址读出数据
**************************************/
uint8_t Read1302(uint8_t addr)
{
  uint8_t i, RetVal = 0;
  
  DS1302_RST_L();
  DS1302_SCLK_L();
  DS1302_RST_H();
  DS1302_IO_OUT();
  for(i = 8; i > 0; i--)
  {
    DS1302_SCLK_L();
    if(addr & 0x01)
      DS1302_IO_H();
    else
      DS1302_IO_L();
    addr >>= 1;
    DS1302_SCLK_H();
  }
  DS1302_IO_IN();
  for(i = 8; i > 0; i--)
  {
    if(DS1302_IO_STATE())
      RetVal |= 0x80;
    else
      RetVal &= 0x7f;
    DS1302_SCLK_H();
    RetVal >>= 1;
    DS1302_SCLK_L();
  }
  DS1302_RST_L();
  
  return RetVal;
}
/**************************************
函数功能：从DS1302读取完整时间，并保存数据
**************************************/
void ReadTime(void)
{
  utime.sec = Read1302(DS1302_SECOND|DS1302_RD);
  utime.min = Read1302(DS1302_MINUTE|DS1302_RD);
  utime.hour = Read1302(DS1302_HOUR|DS1302_RD);
  utime.week = Read1302(DS1302_WEEK|DS1302_RD);
  utime.day = Read1302(DS1302_DAY|DS1302_RD);
  utime.month = Read1302(DS1302_MONTH|DS1302_RD);
  utime.year = Read1302(DS1302_YEAR|DS1302_RD);
}

/*************************************************************
*************************************************************/
void Write_1302_Time(unsigned char *time) 
{
	unsigned char add=0x80;      
	unsigned char i; 
	    
	
	Write1302(0x8e,0x00);       
	
	for(i=0;i<7;i++) 
	{
		Write1302(add,time[i]); 
		add+=2;                    
	}
	
	Write1302(0x8e,0x80);       
} 



/**************************************
函数功能：启动ＤＳ1302
**************************************/
void Start1302(void)
{
	uint8_t temp;
	temp = Read1302(DS1302_SECOND | DS1302_RD);
	Write1302(DS1302_SECOND | DS1302_WR,temp&0x7f);
}
/**************************************
函数功能：暂停ＤＳ1302
**************************************/
void Stop1302(void)
{
	uint8_t temp;
	temp = Read1302(DS1302_SECOND | DS1302_RD);
	Write1302(DS1302_SECOND | DS1302_WR,temp|0x80);
}



u8 ds1302_set_time_test()
{
	u8 dst[7];

	dst[0] = 00;
	dst[1] = 00;

	dst[2] = 0x17;
	dst[3] = 0x24;

	dst[4] = 9;
	dst[5] = 3;

	dst[6] = 0x14;
	Write_1302_Time(dst);
	return OK;
}

u8 ds1302_set_time(u8* src)
{
	u8 ret=ERROR;
	u8 dst[7];

	dst[0] = src[8]&0xf0+src[8]&0x0f;
	dst[1] = src[7]&0xf0+src[7]&0x0f;

	dst[2] = src[6]&0xf0+src[6]&0x0f;
	dst[3] = src[5]&0xf0+src[5]&0x0f;;

	dst[4] = src[4]&0xf0+src[4]&0x0f;
	dst[5] = src[3];

	dst[6] = src[2]&0xf0+src[2]&0x0f;
	Write_1302_Time(dst);


	return ret;
}



u8 ds1302_get_time(u8* dst)
{
	u8 ret=ERROR;
	ReadTime();


	dst[0] = ((utime.year & 0xf0)>>4)*10 + (utime.year & 0x0f);
	dst[1] = utime.week;
	dst[2] = ((utime.month & 0xf0)>>4)*10 + (utime.month & 0x0f);
	dst[3] = ((utime.day & 0xf0)>>4)*10 + (utime.day & 0x0f);
	dst[4] = ((utime.hour & 0xf0)>>4)*10 + (utime.hour & 0x0f);
	dst[5] = ((utime.min& 0xf0)>>4)*10 + (utime.min & 0x0f);
	dst[6] = ((utime.sec& 0xf0)>>4)*10 + (utime.sec& 0x0f);

	
	return ret;
}




u8 ds1302_get_time_test()
{	
	u8 dst[7];
	ReadTime();


	dst[6] = ((utime.year & 0xf0)>>4)*10 + (utime.year & 0x0f);
	dst[5] = utime.week;
	dst[4] = ((utime.month & 0xf0)>>4)*10 + (utime.month & 0x0f);
	dst[3] = ((utime.day & 0xf0)>>4)*10 + (utime.day & 0x0f);
	dst[2] = ((utime.hour & 0xf0)>>4)*10 + (utime.hour & 0x0f);
	dst[1] = ((utime.min& 0xf0)>>4)*10 + (utime.min & 0x0f);
	dst[0] = ((utime.sec& 0xf0)>>4)*10 + (utime.sec& 0x0f);
	
	printf("year: %d, week: %d, mon: %d, day: %d, h: %d, m: %d, s: %d\n", 
		dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
	return OK;
}


