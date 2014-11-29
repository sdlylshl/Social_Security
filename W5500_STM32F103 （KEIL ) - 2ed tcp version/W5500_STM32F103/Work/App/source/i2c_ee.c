/********************************************************************************
 * 文件名  ：i2c_ee.c
 * 描述    ：i2c EEPROM(AT24C02)应用函数库         
 * 实验平台：野火STM32开发板
 * 硬件连接：-----------------
 *          |                 |
 *          |  PB6-I2C1_SCL	  |
 *          |  PB7-I2C1_SDA   |
 *          |                 |
 *           -----------------
 * 库版本  ：ST3.5.0
 * 作者    ：保留 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
	
#include "config.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"


#include "i2c_ee.h"
#include "stm32f10x_i2c.h"

#include "stm32f10x_it.h"

#include "APPs\loopback.h"


const u8 TEXT_Buffer[]={"WarShipSTM32 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)
extern try_again[];

void IIC_Init(void)
{

}

void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_SET); 	  
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET);
	Delay_us(10);
 	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_RESET);;//START:when CLK is high,DATA change form high to low 
	Delay_us(10);
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;//钳住I2C总线，准备发送或接收数据 
}	  

void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;
	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_RESET);;//STOP:when CLK is high DATA change form low to high
 	Delay_us(10);
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET); 
	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_SET);;//STOP:when CLK is high DATA change form low to high
	//发送I2C总线结束信号
	Delay_us(10);							   	
}

u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_SET);;Delay_us(10);	   
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET);Delay_us(10);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;//时钟输出0 	   
	return 0;  
} 

void IIC_Ack(void)
{
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;
	SDA_OUT();
	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_RESET);;
	Delay_us(10);
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET);
	Delay_us(10);
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;
}
		    
void IIC_NAck(void)
{
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;
	SDA_OUT();
	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_SET);;
	Delay_us(10);
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET);
	Delay_us(10);
	GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;
}					 				     

			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {    
    	if((txd&0x80)>>7)
        	GPIO_WriteBit(GPIOB, I2C_SDA, Bit_SET);
		else
			GPIO_WriteBit(GPIOB, I2C_SDA, Bit_RESET);
        txd<<=1; 	  
		Delay_us(10);   //对TEA5767这三个延时都是必须的
		GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET);
		Delay_us(10); 
		GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);;	
		Delay_us(10);
    }	 
} 

u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        GPIO_WriteBit(GPIOB, I2C_SCL, Bit_RESET);; 
        Delay_us(10);
		GPIO_WriteBit(GPIOB, I2C_SCL, Bit_SET);
        receive<<=1;
        if(READ_SDA)receive++;   
		Delay_us(10); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


void AT24CXX_Init(void)
{
 	IIC_Init();
}

u8 AT24CXX_ReadOneByte(u16 ReadAddr) 
{     
	u8 temp=0;                         
	IIC_Start(); 
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
	
	}
	else
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据  

	IIC_Wait_Ack();

	IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();    
	IIC_Start();       
	IIC_Send_Byte(0XA1);           //进入接收模式     
	IIC_Wait_Ack(); 
	temp=IIC_Read_Byte(0);    
	IIC_Stop();//产生一个停止条件    
 	return temp;
}


void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{                               
	IIC_Start(); 
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);     //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		  
	}
	else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据
	} 
	IIC_Wait_Ack();   
	IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack();                   
	IIC_Send_Byte(DataToWrite);     //发送字节         
	IIC_Wait_Ack();           
	IIC_Stop();//产生一个停止条件
	Delay_ms(10); 
}


void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{   
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}               
}

u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{   
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);         
	}
	return temp;               
}

u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX     
	if(temp==0X55)
		return 0;    
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255);  
		printf("temp: 0x%x\n", temp);
		if(temp==0X55)
			return 0;
	}
	return 1;            
}


void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++); 
		NumToRead--;
	}
} 



void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}



int save_ip_to_eeprom(u8* addr, u8 sep)
{	
	u8 i;
	for(i=0; i<4; i++)
	{
		if (sep == IP_START)
			AT24CXX_WriteOneByte(EEPROM_IP_OFFSET+i, addr[i]);
		else if(sep == GATE_IP)
			AT24CXX_WriteOneByte(EEPROM_GATE_OFFSET+i, addr[i]);
		else if(sep == NETMASK_IP)
			AT24CXX_WriteOneByte(EEPROM_MASK_OFFSET+i, addr[i]);
		else if(sep == DEST_IP)
			AT24CXX_WriteOneByte(EEPROM_DEST_OFFSET+i, addr[i]);
	}
	return OK;
}

int get_ip_from_eeprom(u8 offset, u8* ip)
{
	u8 i;

	for(i=0; i<4; i++)
	{
		ip[i] = AT24CXX_ReadOneByte(offset+i);
		printf("ip[%d]: %d ", i, ip[i]);
	}
	printf("\n");
	return OK;
}

u8 get_ip_addr_from_eeprom(u8* ip)
{
	get_ip_from_eeprom(EEPROM_IP_OFFSET, ip);
	return OK;
}

u8 get_gate_from_eeprom(u8* ip)
{
	get_ip_from_eeprom(EEPROM_GATE_OFFSET, ip);
	return OK;
}

u8 get_mask_from_eeprom(u8* ip)
{
	get_ip_from_eeprom(EEPROM_MASK_OFFSET, ip);
	return OK;
}

u8 get_dest_ip_from_eeprom(u8* ip)
{
	get_ip_from_eeprom(EEPROM_DEST_OFFSET, ip);
	return OK;
}





int save_port_to_eeprom(u16 port)
{	
	u8 i;
	u8 ch[2];

	for(i=0; i<2; i++)
	{	
		ch[i] = (port>>(8*(1-i)))&0xff;
		AT24CXX_WriteOneByte(EEPROM_PORT_OFFSET+i, ch[i]);
		printf("ch[%d]: %d", i, ch[i]);
	}
	return OK;
}

int get_port_from_eeprom(u16* port)
{
	u8 i;
	u8 data[2];

	for(i=0; i<2; i++)
	{
		data[i] = AT24CXX_ReadOneByte(EEPROM_PORT_OFFSET+i);
		*port += data[i] << (8*(1-i));
		printf("data[%d]: %d", i, data[i]);
	}

	return OK;
}


int save_id_to_eeprom(u32 id)
{
	u8 i;
	u8 ch[4];

	for(i=0; i<4; i++)
	{	
		ch[i] = (id>>(8*(3-i)))&0xff;
		AT24CXX_WriteOneByte(EEPROM_ID_OFFSET+i, ch[i]);
	
	}
	return OK;
}

int get_id_from_eeprom(u32* id)
{
	u8 i;
	u8 ch[4];
	
	for(i=0; i<4; i++)
	{	
		ch[i] = AT24CXX_ReadOneByte(EEPROM_ID_OFFSET+i);
		*id += ch[i] << (8*(3-i));
	}

	return OK;
}


int save_mac_to_eeprom(u8* addr)
{
	u8 i;

	for(i=0; i<6; i++)
	{	
		AT24CXX_WriteOneByte(EEPROM_MAC_OFFSET+i, addr[i]);
	}
	return OK;
}

int get_mac_from_eeprom(u8* addr)
{	
	u8 i;
	
	for(i=0; i<6; i++)
	{
		addr[i] = AT24CXX_ReadOneByte(EEPROM_MAC_OFFSET+i);
		printf("addr: %d ", addr[i]);
	}
	printf("\n");
	return OK;
}

u8 save_arming_to_eeprom(u32 arm)
{
	u8 i;
	u8 ch[4];

	for(i=0; i<4; i++)
	{	
		ch[i] = (arm>>(8*(3-i)))&0xff;
		printf("write ch[%d]: 0x%x\n", i, ch[i]);
		AT24CXX_WriteOneByte(EEPROM_ARM_OFFSET+i, ch[i]);
	
	}
	return OK;
}

u8 get_arming_from_eeprom(u32* arm)
{
	u8 i;
	u8 ch[4];
	
	for(i=0; i<4; i++)
	{	
		ch[i] = AT24CXX_ReadOneByte(EEPROM_ARM_OFFSET+i);
		printf("get ch[%d]: 0x%x\n", i, ch[i]);
		*arm += ch[i] << (8*(3-i));
	}

	return OK;
}

u8 save_link_to_eeprom(u8 i, u32 in, u16 out, u8 xor)
{	
	u8 j;
	u8 ch[4];

	for(j=0; j<4; j++)
	{	
		ch[j] = (in>>(8*(3-j)))&0xff;
		AT24CXX_WriteOneByte(EEPROM_LINK_IN_OFFSET+i*12, ch[j]);
	
	}

	for(j=0; j<2; j++)
	{	
		ch[j] = (in>>(8*(1-j)))&0xff;
		AT24CXX_WriteOneByte(EEPROM_LINK_OUT_OFFSET+i*12, ch[j]);
	
	}

	AT24CXX_WriteOneByte(EEPROM_LINK_XOR_OFFSET+i*12, xor);
	

	return OK;
}

u8 get_link_from_eeprom(u8 i, u32* in, u16* out, u8* xor)
{
	u8 j;
	u8 ch[4];
	
	for(j=0; j<4; j++)
	{	
		ch[j] = AT24CXX_ReadOneByte(EEPROM_LINK_IN_OFFSET+i*12);
		*in += ch[j] << (8*(3-j));
	}

	for(j=0; j<2; j++)
	{	
		ch[j] = AT24CXX_ReadOneByte(EEPROM_LINK_OUT_OFFSET+i*12);
		*in += ch[j] << (8*(1-j));
	}

	*xor = AT24CXX_ReadOneByte(EEPROM_LINK_XOR_OFFSET+i*12);
	return OK;

}

u8 save_link_group_to_eeprom(u8 group)
{

	AT24CXX_WriteOneByte(EEPROM_LINK_GROUP_OFFSET, group);
	

	return OK;
}

u8 get_link_group_from_eeprom(u8* group)
{
	
	*group = AT24CXX_ReadOneByte(EEPROM_LINK_GROUP_OFFSET);

	return OK;
}

u8 save_text_enable_to_eeprom(u8 enable)
{

	AT24CXX_WriteOneByte(EEPROM_TEXT_ENABLE_OFFSET, enable);
	

	return OK;
}

u8 get_text_enable_from_eeprom(u8* enable)
{
	
	*enable = AT24CXX_ReadOneByte(EEPROM_TEXT_ENABLE_OFFSET);

	return OK;
}

u8 save_cell_enable_to_eeprom(u8 enable)
{

	AT24CXX_WriteOneByte(EEPROM_CELL_ENABLE_OFFSET, enable);
	

	return OK;
}

u8 get_cell_enable_from_eeprom(u8* enable)
{
	
	*enable = AT24CXX_ReadOneByte(EEPROM_CELL_ENABLE_OFFSET);

	return OK;
}



u8 save_cell_group_to_eeprom(u8 group)
{

	AT24CXX_WriteOneByte(EEPROM_CELL_GROUP_OFFSET, group);
	

	return OK;
}

u8 get_cell_group_from_eeprom(u8* group)
{
	
	*group = AT24CXX_ReadOneByte(EEPROM_CELL_GROUP_OFFSET);

	return OK;
}


u8 save_cell_xor_to_eeprom(u8 group)
{

	AT24CXX_WriteOneByte(EEPROM_CELL_XOR_OFFSET, group);
	

	return OK;
}

u8 get_cell_xor_from_eeprom(u8* group)
{
	
	*group = AT24CXX_ReadOneByte(EEPROM_CELL_XOR_OFFSET);

	return OK;
}


u8 save_text_group_to_eeprom(u8 group)
{

	AT24CXX_WriteOneByte(EEPROM_TEXT_GROUP_OFFSET, group);
	

	return OK;
}

u8 get_text_group_from_eeprom(u8* group)
{
	
	*group = AT24CXX_ReadOneByte(EEPROM_TEXT_GROUP_OFFSET);

	return OK;
}

u8 save_text_length_to_eeprom(u8 i, u8 len)
{

	AT24CXX_WriteOneByte(EEPROM_TEXT_LENGTH_OFFSET+i, len);
	return OK;
}

u8 get_text_length_from_eeprom(u8 i, u8* len)
{
	
	*len = AT24CXX_ReadOneByte(EEPROM_TEXT_LENGTH_OFFSET+i);

	return OK;
}


u8 save_default_flag_eeprom()
{

	AT24CXX_WriteOneByte(EEPROM_PARA_DEFAULT_FLAG_OFFSET, 0x55);
	return OK;
}

u8 get_default_flag_from_eeprom(u8* flag)
{
	
	*flag = AT24CXX_ReadOneByte(EEPROM_PARA_DEFAULT_FLAG_OFFSET);

	return OK;
}




u8 save_cell_num_to_eeprom(u8 i, u8* src)
{
	u8 j;
	
	for(j=0; j<11; j++)
	{
		AT24CXX_WriteOneByte(EEPROM_CELL_NUM1_OFFSET+i*16, src[i]);
		printf("addr: %c",  src[i]);
	}
	return OK;
}

u8 get_cell_num_from_eeprom(u8 i, u8* src)
{
	u8 j;
	
	for(j=0; j<11; j++)
	{	
		src[i] = AT24CXX_ReadOneByte(EEPROM_CELL_NUM1_OFFSET+i*16+j);
	}

	return OK;
}


u8 save_text_num_to_eeprom(u8 i, u8* src)
{	
	u8 j;
	
	for(j=0; j<11; j++)
	{
		AT24CXX_WriteOneByte(EEPROM_TEXT_NUM1_OFFSET+i*16, src[i]);
		printf("addr: %c", src[i]);
	}
	return OK;

}



u8 get_text_num_from_eeprom(u8 i, u8* src)
{
	u8 j;
	
	for(j=0; j<11; j++)
	{	
		src[i] = AT24CXX_ReadOneByte(EEPROM_TEXT_NUM1_OFFSET+i*16+j);
	}

	return OK;

}


u8 save_text_content_to_eeprom(u8 i, u8 len, u8* src)
{
	u8 j;
	
	for(j=0; j<len; j++)
	{
		AT24CXX_WriteOneByte(EEPROM_TEXT_CONTENT1_OFFSET+i*64+j, src[i]);
		printf("addr: %c", src[i]);
	}
	return OK;

}

u8 get_text_content_from_eeprom(u8 i, u8 len, u8* src)
{
	u8 j;
	
	for(j=0; j<len; j++)
	{	
		src[i] = AT24CXX_ReadOneByte(EEPROM_TEXT_CONTENT1_OFFSET+i*64+j);
	}

	return OK;
}

u8 get_para(sys_para* sys)
{
	u8 i;
	u8 default_flag;

	sys->net_para.any_port = 1000;
	sys->gprs_flag = 0;
	sys->tcp_flag = 0;
	sys->cell_alarm = 0;
	sys->text_alarm = 0;
	sys->need_in_net = 0;
	sys->gprs_init_flag = 0;

	get_default_flag_from_eeprom(&default_flag);
	if(default_flag == 0x55)
	{
		get_mac_from_eeprom(sys->net_para.Mac);
		get_ip_addr_from_eeprom(sys->net_para.Lip);
		get_gate_from_eeprom(sys->net_para.Gw);
		get_mask_from_eeprom(sys->net_para.Sub);

		get_dest_ip_from_eeprom(sys->dest_net_para.destip);
		get_port_from_eeprom(&(sys->dest_net_para.port));

		for(i=0; i<6; i++)
			printf("mac: %d\n", sys->net_para.Mac[i]);
		for(i=0; i<4; i++)
			printf("ip: %d\n", sys->net_para.Lip[i]);
		for(i=0; i<4; i++)
			printf("gw: %d\n", sys->net_para.Gw[i]);
		for(i=0; i<4; i++)
			printf("sub: %d\n", sys->net_para.Sub[i]);
		for(i=0; i<4; i++)
			printf("dest ip: %d\n", sys->dest_net_para.destip[i]);
		
		printf("port: %d\n", sys->dest_net_para.port);


		get_id_from_eeprom(&sys->machine_id);
		printf("machine id: %d\n", sys->machine_id);
		//get link para
		get_link_group_from_eeprom(&(sys->link.link_group));
		if(sys->link.link_group < 17)
		{
			for(i=0; i<sys->link.link_group; i++)
			{
				get_link_from_eeprom(i, &(sys->link.in[i]), &(sys->link.out[i]), &(sys->link.xor[i]));
			}
		}
		
		//get cell para
		get_cell_group_from_eeprom(&(sys->cell.cell_group));
		get_cell_enable_from_eeprom(&(sys->cell.cell_enable));
		get_cell_xor_from_eeprom(&(sys->cell.cell_xor));

		for(i=0; i<5; i++)
		{	
			if(sys->cell.cell_enable & 1<<i)
				get_cell_num_from_eeprom(i, sys->cell.cell[i]);
		}
	
		//get text para
		get_text_group_from_eeprom(&(sys->text.text_group));
		get_text_enable_from_eeprom(&(sys->text.text_enable));

		for(i=0; i<5; i++)
		{
			if(sys->text.text_enable & 1<<i)
			{
				get_text_num_from_eeprom(i,sys->text.text[i]);
				get_text_length_from_eeprom(i,&(sys->text.text_length[i]));
				get_text_content_from_eeprom(i,sys->text.text_length[i], sys->text.text_content[i]);
				sys->text.send_ok[i] = ERROR;
				set_current_time(&(sys->text.time[i]));
			}
		}
			
		
		get_arming_from_eeprom(&sys->arming);
		printf("arming: 0x%x\n", sys->arming);
	}
	else
	{	
		printf("default para enable\n");
		sys->net_para.Mac[0] = 0x00;
		sys->net_para.Mac[1] = 8;
		sys->net_para.Mac[2] = 202;
		sys->net_para.Mac[3] = 1;
		sys->net_para.Mac[4] = 5;
		sys->net_para.Mac[5] = 3;

		sys->net_para.Lip[0] = 192;
		sys->net_para.Lip[1] = 168;
		sys->net_para.Lip[2] = 88;
		sys->net_para.Lip[3] = 101;

		sys->net_para.Gw[0] = 192;
		sys->net_para.Gw[1] = 168;
		sys->net_para.Gw[2] = 88;
		sys->net_para.Gw[3] = 1;

		sys->net_para.Sub[0] = 255;
		sys->net_para.Sub[1] = 255;
		sys->net_para.Sub[2] = 255;
		sys->net_para.Sub[3] = 0;

		sys->dest_net_para.destip[0] = 192;
		sys->dest_net_para.destip[1] = 168;
		sys->dest_net_para.destip[2] = 88;
		sys->dest_net_para.destip[3] = 81;

		sys->dest_net_para.port = 8081;
		sys->machine_id = 1234567891;

		sys->arming = 0x00;
		sys->text.text_group = 0;
		sys->cell.cell_group = 0;
		sys->link.link_group = 0;
	}

	for(i=0; i<MAX_NEED_RECV; i++)
		try_again[i] = 0;

	return OK;
}






void I2C_Test(void)
{
	u8 datatemp[SIZE];
	u32 test;
	
	while(AT24CXX_Check())//检测不到24c02
	{
		printf("check at24c256 error\n");
	}

	AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
	
	AT24CXX_Read(0,datatemp,SIZE);
	printf("datatemp: %s\n", datatemp);

	save_arming_to_eeprom(0xaa);
	get_arming_from_eeprom(&test);
	printf("test: 0x%x\n", test);
	
}


/*************************END OF FILE*************************************/
