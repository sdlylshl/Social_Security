#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"

#include "stm32f10x_flash.h"  
#include "stm32f10x_it.h"




//#define  STARTADDR  0x08010000  //STM32F103
#define		STARTADDR 0x0800C000
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE; 
#define IP_OFFSET 		0x0
#define GATE_OFFSET 	0x4
#define MASK_OFFSET 	0x8
#define DEST_OFFSET 	0xA
#define PORT_OFFSET 	0xc
#define ID_OFFSET		0x10
#define MAC_OFFSET 		0x14
#define ARM_OFFSET		0x18
#define LINK_IN_OFFSET		0x2A
#define LINK_OUT_OFFSET		0x2C
#define LINK_XOR_OFFSET		0x30

#define CELL_NUM1_OFFSET		0xea
#define CELL_NUM2_OFFSET		0xf0
#define CELL_NUM3_OFFSET		0xf8
#define CELL_NUM4_OFFSET		0xfc
#define CELL_NUM5_OFFSET		0x104

#define TEXT_NUM1_OFFSET		0x10A
#define TEXT_NUM2_OFFSET		0x110
#define TEXT_NUM3_OFFSET		0x118
#define TEXT_NUM4_OFFSET		0x11C
#define TEXT_NUM5_OFFSET		0x124


#define TEXT_CONTENT1_OFFSET		0x12a
#define TEXT_CONTENT2_OFFSET		0x1b6
#define TEXT_CONTENT3_OFFSET		0x242
#define TEXT_CONTENT4_OFFSET		0x2ce
#define TEXT_CONTENT5_OFFSET		0x35a










int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum) 
{
        int DataNum = 0;
		ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
        while(DataNum < ReadNum) 
		{
           *(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++;
           DataNum++;
        }
        return DataNum;
}


void WriteFlashOneWord(uint32_t WriteAddress,uint32_t WriteData)
{
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData);    //flash.c 中API函数
		//FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //需要写入更多数据时开启
		//FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //需要写入更多数据时开启
	}
	FLASH_LockBank1();
} 

int save_ip_to_flash(u8* addr, u8 sep)
{	
	u32 ip;

	
	ip = ((addr[0]&0xff)<<24)|((addr[1]&0xff)<<16)|((addr[2]&0xff)<<8)|(addr[3]&0xff);
	if (sep == IP_START)
		WriteFlashOneWord(IP_OFFSET, ip);
	else if(sep == GATE_IP)
		WriteFlashOneWord(GATE_OFFSET, ip);
	else if(sep == NETMASK_IP)
		WriteFlashOneWord(MASK_OFFSET, ip);
	else if(sep == DEST_IP)
		WriteFlashOneWord(DEST_OFFSET, ip);
	return OK;
}

int save_port_to_flash(u16 port)
{
	WriteFlashOneWord(PORT_OFFSET, port);
	return OK;
}

int save_id_to_flash(u32 id)
{
	WriteFlashOneWord(ID_OFFSET, id);
	return OK;
}

int save_mac_to_flash(u8* addr)
{
	u32 ip;
	ip = ((addr[0]&0xff)<<24)|((addr[1]&0xff)<<16)|((addr[2]&0xff)<<8)|(addr[3]&0xff);
	WriteFlashOneWord(MAC_OFFSET, ip);

	ip = ((addr[4]&0xff)<<24)|((addr[5]&0xff)<<16);
	WriteFlashOneWord(MAC_OFFSET+4, ip);
	return OK;
}


int get_ip(u8 offset, u32* ip)
{
	u8 i;
	u8 data[4];

	ReadFlashNBtye(offset, data, 4);
	for(i=0; i<4; i++)
		printf("data[%d]: %d", i, data[i]);
	printf("\n");
	((*ip) ) |= data[3];
	((*ip)) |= data[2] <<  8;
	((*ip)) |= data[1] <<  16;
	((*ip)) |= data[0] <<  24;
	return OK;
}

u8 get_ip_addr(u32* ip)
{
	get_ip(IP_OFFSET, ip);
	return OK;
}

int get_port()
{
	u8 i;
	u8 data[4];

	ReadFlashNBtye(PORT_OFFSET, data, 4);
	for(i=0; i<4; i++)
		printf("data[%d]: %d", i, data[i]);
	printf("\n");
	return OK;
}

int get_mac()
{	
	u8 i;
	u8 data[6];

	ReadFlashNBtye(MAC_OFFSET, data, 6);
	for(i=0; i<6; i++)
		printf("data[%d]: %d", i, data[i]);
	printf("\n");
	return OK;
}

int get_id(u32* id)
{
	u8 i;
	u8 data[4];

	ReadFlashNBtye(ID_OFFSET, data, 4);
	for(i=0; i<4; i++)
		printf("data[%d]: %d", i, data[i]);
	printf("\n");
	((*id) ) |= data[3];
	((*id)) |= data[2] << 8;
	((*id)) |= data[1] << 16;
	((*id)) |= data[0] << 24;
	return OK;
}

u8 save_arming_to_flash(u32 arm)
{
	WriteFlashOneWord(ARM_OFFSET, arm);
	return OK;
}

u8 save_arming_from_flash(u32* arm)
{
	u8 i;
	u8 data[4];

	ReadFlashNBtye(ARM_OFFSET, data, 4);
	for(i=0; i<4; i++)
		printf("data[%d]: %d", i, data[i]);

	((*arm) ) |= data[3];
	((*arm)) |= data[2] << 8;
	((*arm)) |= data[1] << 16;
	((*arm)) |= data[0] << 24;
	return OK;
}

u8 save_link_to_flash(u8 i, u32 in, u16 out, u8 xor)
{
	WriteFlashOneWord(LINK_IN_OFFSET+12*i, in);
	WriteFlashOneWord(LINK_OUT_OFFSET+12*i, out);
	WriteFlashOneWord(LINK_XOR_OFFSET+12*i, xor);
	return OK;
}

u8 get_link_from_flash(u8 i, u32* in, u16* out, u8* xor)
{
	u8 data[4];

	ReadFlashNBtye(LINK_IN_OFFSET+12*i, data, 4);

	((*in) ) |= data[3];
	((*in)) |= data[2] << 8;
	((*in)) |= data[1] << 16;
	((*in)) |= data[0] << 24;

	ReadFlashNBtye(LINK_OUT_OFFSET+12*i, data, 4);

	((*out) ) |= data[3];
	((*out)) |= data[2] << 8;



	ReadFlashNBtye(LINK_OUT_OFFSET+12*i, data, 4);

	((*xor) ) |= data[3];
	return OK;

}


u8 save_cell_num(u8 i, char* src)
{
	//unsigned __int64 num;
	u8 j;
	u32 data[2];
	u32 help1[5] = {10000, 1000, 100, 10, 1};
	u32 help2[6] = {100000, 10000, 1000, 100, 10, 1};

	for(j=0; j<5; j++)
	{
		if (src[j] == '0')
			;
		else if(src[j] == '1')
			data[0] += help1[j];
		else if(src[j] == '2')
			data[0] += 2*help1[j];
		else if(src[j] == '3')
			data[0] += 3*help1[j];
		else if(src[j] == '4')
			data[0] += 4*help1[j];
		else if(src[j] == '5')
			data[0] += 5*help1[j];
		else if(src[j] == '6')
			data[0] += 6*help1[j];
		else if(src[j] == '7')
			data[0] += 7*help1[j];
		else if(src[j] == '8')
			data[0] += 8*help1[j];
		else if(src[j] == '9')
			data[0] += 9*help1[j];
	}
	WriteFlashOneWord(CELL_NUM1_OFFSET+8*i, data[0]);

	
	for(j=0; j<6; j++)
	{
		if (src[j] == '0')
			;
		else if(src[j] == '1')
			data[1] += help2[j];
		else if(src[j] == '2')
			data[1] += 2*help2[j];
		else if(src[j] == '3')
			data[1] += 3*help2[j];
		else if(src[j] == '4')
			data[1] += 4*help2[j];
		else if(src[j] == '5')
			data[1] += 5*help2[j];
		else if(src[j] == '6')
			data[1] += 6*help2[j];
		else if(src[j] == '7')
			data[1] += 7*help2[j];
		else if(src[j] == '8')
			data[1] += 8*help2[j];
		else if(src[j] == '9')
			data[1] += 9*help2[j];
	}
	WriteFlashOneWord(CELL_NUM1_OFFSET+4+8*i, data[1]);
	return OK;
}

u8 save_text_content(u8 i, u8 len, char* src)
{
	u8 a, j, k;

	j = len/4;
	k = len%4;
	for(a=0; a<j; a++)
		WriteFlashOneWord(TEXT_CONTENT1_OFFSET+140*i, *(u32*)(src+a*4));
	if(k != 0)
		WriteFlashOneWord(TEXT_CONTENT1_OFFSET+140*j, *(u16*)(src+j*4));
	return OK;
}

u8 get_text_content(u8 i, u8 len, char* src)
{
	u8 data[4];
	u8 a, j, k;

	j = len/4;
	k = len%4;

	for(a=0; a<j; a++)
	{
		ReadFlashNBtye(TEXT_CONTENT1_OFFSET+140*i+a*4, data, 4);

		src[a*4] = data[3];
		src[a*4+1] = data[2];
		src[a*4+2] = data[1];
		src[a*4+3] = data[0];
	}
	if (k != 0)
	{
		ReadFlashNBtye(TEXT_CONTENT1_OFFSET+140*i+j*4, data, 4);
		src[j*4] = data[3];
		src[j*4+1] = data[2];
	}
	return OK;
}

u8 save_text_num(u8 i, char* src)
{	
	u8 j;
	u32 data[2];
	u32 help1[5] = {10000, 1000, 100, 10, 1};
	u32 help2[6] = {100000, 10000, 1000, 100, 10, 1};

	for(j=0; j<5; j++)
	{
		if (src[j] == '0')
			;
		else if(src[j] == '1')
			data[0] += help1[j];
		else if(src[j] == '2')
			data[0] += 2*help1[j];
		else if(src[j] == '3')
			data[0] += 3*help1[j];
		else if(src[j] == '4')
			data[0] += 4*help1[j];
		else if(src[j] == '5')
			data[0] += 5*help1[j];
		else if(src[j] == '6')
			data[0] += 6*help1[j];
		else if(src[j] == '7')
			data[0] += 7*help1[j];
		else if(src[j] == '8')
			data[0] += 8*help1[j];
		else if(src[j] == '9')
			data[0] += 9*help1[j];
	}
	WriteFlashOneWord(TEXT_NUM1_OFFSET+8*i, data[0]);

	
	for(j=0; j<6; j++)
	{
		if (src[j] == '0')
			;
		else if(src[j] == '1')
			data[1] += help2[j];
		else if(src[j] == '2')
			data[1] += 2*help2[j];
		else if(src[j] == '3')
			data[1] += 3*help2[j];
		else if(src[j] == '4')
			data[1] += 4*help2[j];
		else if(src[j] == '5')
			data[1] += 5*help2[j];
		else if(src[j] == '6')
			data[1] += 6*help2[j];
		else if(src[j] == '7')
			data[1] += 7*help2[j];
		else if(src[j] == '8')
			data[1] += 8*help2[j];
		else if(src[j] == '9')
			data[1] += 9*help2[j];
	}
	WriteFlashOneWord(TEXT_NUM1_OFFSET+4+8*i, data[1]);
	return OK;
}

u8 get_cell_num(u8 i, char* src)
{
	u8 data[4];
	

	ReadFlashNBtye(CELL_NUM1_OFFSET+8*i, data, 4);

	src[0] = data[0]/10 + 48;
	src[1] = data[0]%10 + 48;
	src[2] = data[1]/10 + 48;
	src[3] = data[1]%10 + 48;
	src[4] = data[2]%10 + 48;

	
	ReadFlashNBtye(CELL_NUM1_OFFSET+8*i, data, 4);

	src[5] = data[0]/10 + 48;
	src[6] = data[0]%10 + 48;
	src[7] = data[1]/10 + 48;
	src[8] = data[1]%10 + 48;
	src[9] = data[2]/10 + 48;
	src[10] = data[2]%10 + 48;
	return OK;
}

u8 get_text_num(u8 i, char* src)
{
	u8 data[4];
	

	ReadFlashNBtye(TEXT_NUM1_OFFSET+8*i, data, 4);

	src[0] = data[0]/10 + 48;
	src[1] = data[0]%10 + 48;
	src[2] = data[1]/10 + 48;
	src[3] = data[1]%10 + 48;
	src[4] = data[2]%10 + 48;

	
	ReadFlashNBtye(TEXT_NUM1_OFFSET+8*i, data, 4);

	src[5] = data[0]/10 + 48;
	src[6] = data[0]%10 + 48;
	src[7] = data[1]/10 + 48;
	src[8] = data[1]%10 + 48;
	src[9] = data[2]/10 + 48;
	src[10] = data[2]%10 + 48;
	return OK;
}


int flash_test()
{
	//get_id();
	return OK;
}

void flash_save(void)
{  
	uint8_t Temp_Data[20] = {0};
	
	
	int i = 0;

    WriteFlashOneWord(0x4,0x12355678);    
	Delay_ms(10);                         
	ReadFlashNBtye(0x4, Temp_Data,4); 
	
             
	Delay_ms(10);

	for(i=0; i<4; i++)
		printf("Temp_Data[%d]: 0x%x\n", i, Temp_Data[i]);
	
}


