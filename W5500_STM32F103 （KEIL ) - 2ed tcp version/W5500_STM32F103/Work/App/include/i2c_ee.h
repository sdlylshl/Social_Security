#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "stm32f10x.h"

#define AT24C01  127
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64     8191
#define AT24C128 16383
#define AT24C256 32767 

#define EE_TYPE AT24C256


#define I2C_SCL	                        GPIO_Pin_6  //out
#define I2C_SDA	                        GPIO_Pin_7  //bidirection


#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}

	 
#define READ_SDA   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))  

#define PAGE_ALGIN 64


#define EEPROM_IP_OFFSET 			0x0000
#define EEPROM_GATE_OFFSET 			0x0004
#define EEPROM_MASK_OFFSET 	 		0x0008
#define EEPROM_DEST_OFFSET 			0x000c
#define EEPROM_PORT_OFFSET 			0x0010
#define EEPROM_ID_OFFSET			0x0014
#define EEPROM_MAC_OFFSET 			0x0018
#define EEPROM_ARM_OFFSET			0x0020
#define EEPROM_LINK_IN_OFFSET		0x0024
#define EEPROM_LINK_OUT_OFFSET		0x0028
#define EEPROM_LINK_XOR_OFFSET		0x002c


#define EEPROM_CELL_ENABLE_OFFSET			0x00e0
#define EEPROM_TEXT_ENABLE_OFFSET			0x00e1

#define EEPROM_LINK_GROUP_OFFSET	0x00f0
#define EEPROM_CELL_GROUP_OFFSET	0x00f1
#define EEPROM_CELL_XOR_OFFSET		0x00f2
#define EEPROM_TEXT_GROUP_OFFSET	0x00f3
#define EEPROM_TEXT_LENGTH_OFFSET	0x00f4
#define EEPROM_PARA_DEFAULT_FLAG_OFFSET 0x00f9


#define EEPROM_CELL_NUM1_OFFSET		0x0100
#define EEPROM_CELL_NUM2_OFFSET		0x0110
#define EEPROM_CELL_NUM3_OFFSET		0x0120
#define EEPROM_CELL_NUM4_OFFSET		0x0130
#define EEPROM_CELL_NUM5_OFFSET		0x0140

#define EEPROM_TEXT_NUM1_OFFSET		0x0150
#define EEPROM_TEXT_NUM2_OFFSET		0x0160
#define EEPROM_TEXT_NUM3_OFFSET		0x0170
#define EEPROM_TEXT_NUM4_OFFSET		0x0180
#define EEPROM_TEXT_NUM5_OFFSET		0x01a0


#define EEPROM_TEXT_CONTENT1_OFFSET		0x0200
#define EEPROM_TEXT_CONTENT2_OFFSET		0x0240
#define EEPROM_TEXT_CONTENT3_OFFSET		0x0280
#define EEPROM_TEXT_CONTENT4_OFFSET		0x02c0
#define EEPROM_TEXT_CONTENT5_OFFSET		0x0300


      
u8 AT24CXX_ReadOneByte(u16 ReadAddr);      
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);  
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);     
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite); 
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);    
u8 AT24CXX_Check(void);  
void AT24CXX_Init(void);
void I2C_Test(void);
void IIC_Init(void);                			 
void IIC_Start(void);				
void IIC_Stop(void);	  			
void IIC_Send_Byte(u8 txd);			
u8 IIC_Read_Byte(unsigned char ack);
u8 IIC_Wait_Ack(void); 				
void IIC_Ack(void);					
void IIC_NAck(void);				
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	

u8 get_ip_addr_from_eeprom(u8* ip);
int get_id_from_eeprom(u32* id);
u8 save_link_to_eeprom(u8 i, u32 in, u16 out, u8 xor);
u8 save_cell_num_to_eeprom(u8 i, u8* src);
u8 save_text_content_to_eeprom(u8 i, u8 len, u8* src);
u8 save_text_num_to_eeprom(u8 i, u8* src);
u8 save_arming_to_eeprom(u32 arm);

int save_ip_to_eeprom(u8* addr, u8 sep);
int save_mac_to_eeprom(u8* addr);
int save_port_to_eeprom(u16 port);
int save_id_to_to_eeprom(u32 id);


u8 get_ip_addr_from_eeprom(u8* ip);
u8 get_cell_num_from_eeprom(u8 i, u8* src);
u8 get_text_num_from_eeprom(u8 i, u8* src);

u8 get_text_content_from_eeprom(u8 i, u8 len, u8* src);
int save_port_to_eeprom(u16 port);
u8 get_para(sys_para* sys);
u8 save_default_flag_eeprom(void);


#endif /* __I2C_EE_H */
