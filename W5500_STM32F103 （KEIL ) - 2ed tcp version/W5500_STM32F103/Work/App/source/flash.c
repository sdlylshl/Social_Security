#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"

#include "stm32f10x_flash.h"  
#include "stm32f10x_it.h"




//#define  STARTADDR  0x08010000  //STM32F103
#define		STARTADDR 0x08050000

volatile FLASH_Status FLASHStatus = FLASH_COMPLETE; 

#define IP_OFFSET 		0x0
#define GATE_OFFSET 	0x800
#define MASK_OFFSET 	0x1000
#define DEST_OFFSET 	0x1800
#define PORT_OFFSET 	0x2000
#define ID_OFFSET		0x2800
#define MAC_OFFSET 		0x3000
#define ARM_OFFSET		0x3800
#define LINK_IN_OFFSET		0x4000
#define LINK_OUT_OFFSET		0x4800
#define LINK_XOR_OFFSET		0x5000

#define CELL_NUM1_OFFSET		0x5800
#define CELL_NUM2_OFFSET		0x6000
#define CELL_NUM3_OFFSET		0x6800
#define CELL_NUM4_OFFSET		0x7000
#define CELL_NUM5_OFFSET		0x7800

#define TEXT_NUM1_OFFSET		0x8000
#define TEXT_NUM2_OFFSET		0x8800
#define TEXT_NUM3_OFFSET		0x9000
#define TEXT_NUM4_OFFSET		0x9800
#define TEXT_NUM5_OFFSET		0xa000


#define TEXT_CONTENT1_OFFSET		0xa800
#define TEXT_CONTENT2_OFFSET		0xb000
#define TEXT_CONTENT3_OFFSET		0xb800
#define TEXT_CONTENT4_OFFSET		0xc000
#define TEXT_CONTENT5_OFFSET		0xc800

#define LINK_OFFSET					0xd000










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
    FLASHStatus = FLASH_ErasePage(STARTADDR+WriteAddress);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData);    //flash.c 中API函数
		//FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //需要写入更多数据时开启
		//FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //需要写入更多数据时开启
	}
	FLASH_LockBank1();
} 

void WriteFlashTwoWord(uint32_t WriteAddress,uint32_t WriteData, uint32_t WriteData2)
{
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR+WriteAddress);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData); 
		
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress+4, WriteData2);    
		//FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                     
		//FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      
	}
	FLASH_LockBank1();
}

void WriteFlashThreeWord(uint32_t WriteAddress,uint32_t WriteData, uint32_t WriteData2, uint32_t WriteData3)
{
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR+WriteAddress);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData); 
		
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress+4, WriteData2);   
		
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress+8, WriteData3); 
		//FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                     
		//FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      
	}
	FLASH_LockBank1();
}

void WriteFlashNWord(uint32_t WriteAddress,uint32_t* WriteData, u8 n)
{	
	u8 i;
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR+WriteAddress);

	if (n<512)
	{
		if(FLASHStatus == FLASH_COMPLETE)
		{	
			for(i=0; i<n; i++)
				FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress+i*4, *(WriteData+i*4));    //flash.c 中API函数
		}
	}
	FLASH_LockBank1();
} 


void WriteFlashNHalfWord(uint32_t WriteAddress,uint16_t* WriteData, u8 n)
{	
	u8 i;
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR+WriteAddress);

	if (n<512)
	{
		if(FLASHStatus == FLASH_COMPLETE)
		{	
			for(i=0; i<n; i++)
			{
				FLASHStatus = FLASH_ProgramHalfWord(STARTADDR + WriteAddress+i*2, *(WriteData+i));
				printf("WriteData): %s\n",(char*) (WriteData+i));
			}
			
		}
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
	u32 ip1, ip2;
	ip1 = ((addr[0]&0xff)<<24)|((addr[1]&0xff)<<16)|((addr[2]&0xff)<<8)|(addr[3]&0xff);
	

	ip2 = ((addr[4]&0xff)<<24)|((addr[5]&0xff)<<16);
	WriteFlashTwoWord(MAC_OFFSET, ip1, ip2);
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
	WriteFlashThreeWord(LINK_OFFSET+0x800*i, in, out, xor);
	return OK;
}

u8 get_link_from_flash(u8 i, u32* in, u16* out, u8* xor)
{
	u8 data[4];

	ReadFlashNBtye(LINK_OFFSET+0x800*i, data, 4);

	((*in) ) |= data[3];
	((*in)) |= data[2] << 8;
	((*in)) |= data[1] << 16;
	((*in)) |= data[0] << 24;

	ReadFlashNBtye(LINK_OFFSET+0x800*i+4, data, 4);

	((*out) ) |= data[3];
	((*out)) |= data[2] << 8;



	ReadFlashNBtye(LINK_OFFSET+0x800*i+8, data, 4);

	((*xor) ) |= data[3];
	return OK;

}


u8 save_cell_num(u8 i, char* src)
{
	//unsigned __int64 num;
	u8 j;
	u32 data[2] = {0, 0};
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
	printf("data[0]: %d\n", data[0]);
	

	
	for(j=5; j<11; j++)
	{
		if (src[j] == '0')
			;
		else if(src[j] == '1')
			data[1] += help2[j-5];
		else if(src[j] == '2')
			data[1] += 2*help2[j-5];
		else if(src[j] == '3')
			data[1] += 3*help2[j-5];
		else if(src[j] == '4')
			data[1] += 4*help2[j-5];
		else if(src[j] == '5')
			data[1] += 5*help2[j-5];
		else if(src[j] == '6')
			data[1] += 6*help2[j-5];
		else if(src[j] == '7')
			data[1] += 7*help2[j-5];
		else if(src[j] == '8')
			data[1] += 8*help2[j-5];
		else if(src[j] == '9')
			data[1] += 9*help2[j-5];
	}
	printf("data[1]: %d\n", data[1]);
	WriteFlashTwoWord(CELL_NUM1_OFFSET+0x800*i, data[0], data[1]);
	return OK;
}

u8 save_text_content(u8 i, u8 len, char* src)
{
	u8 j;
	j = len/2;

	printf("save_text_content: %s, len: %d\n", src, len);
	WriteFlashNHalfWord(TEXT_CONTENT1_OFFSET+i*0x800, (u16*)src, j);
	return OK;
}

u8 get_text_content(u8 i, u8 len, char* src)
{
	u8 data[4];
	u8 a, j, k;

	
	a = len/4;
	printf("a: %d, len :%d\n", a, len);

	for(j=0; j<a; j++)
	{
		ReadFlashNBtye(TEXT_CONTENT1_OFFSET+0x800*i+4*j, data, 4);

		src[j*4+0] = data[0];
		src[j*4+1] = data[1];
		src[j*4+2] = data[2];
		src[j*4+3] = data[3];

		for(k=0; k<4; k++)
		//;
			printf("data[%d]: %c\n", k, data[k]);
		printf("get_text_content: %s\n", src);
	}
	
	return OK;
}

u8 save_text_num(u8 i, char* src)
{	
	u8 j;
	u32 data[2] = {0, 0};
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

	
	for(j=5; j<11; j++)
	{
		if (src[j] == '0')
			;
		else if(src[j] == '1')
			data[1] += help2[j-5];
		else if(src[j] == '2')
			data[1] += 2*help2[j-5];
		else if(src[j] == '3')
			data[1] += 3*help2[j-5];
		else if(src[j] == '4')
			data[1] += 4*help2[j-5];
		else if(src[j] == '5')
			data[1] += 5*help2[j-5];
		else if(src[j] == '6')
			data[1] += 6*help2[j-5];
		else if(src[j] == '7')
			data[1] += 7*help2[j-5];
		else if(src[j] == '8')
			data[1] += 8*help2[j-5];
		else if(src[j] == '9')
			data[1] += 9*help2[j-5];
	}
	WriteFlashTwoWord(TEXT_NUM1_OFFSET+0x800*i, data[0], data[1]);
	return OK;
}

u8 get_cell_num(u8 i, char* src)
{
	u8 data[4];
	u8 data1[4];

	ReadFlashNBtye(CELL_NUM1_OFFSET+0x800*i, data, 4);
	//for(j=0; j<4; j++)
		//printf("data[%d]: 0x%x\n", j, data[j]);

	Delay_ms(20);
	ReadFlashNBtye(CELL_NUM1_OFFSET+4+0x800*i, data1, 4);
	//for(j=0; j<4; j++)
		//printf("data[%d]: 0x%x\n", j, data1[j]);

	sprintf(src, "%5d%06d", *(u32*)data, *(u32*)data1);
	printf("src: %s\n", src);

	return OK;
}

u8 get_text_num(u8 i, char* src)
{
	u8 data[4];
	u8 data1[4];

	ReadFlashNBtye(TEXT_NUM1_OFFSET+0x800*i, data, 4);
	
	Delay_ms(20);
	
	ReadFlashNBtye(TEXT_NUM1_OFFSET+4+0x800*i, data1, 4);


	sprintf(src, "%5d%06d", *(u32*)data, *(u32*)data1);
	printf("src: %s\n", src);

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

    WriteFlashTwoWord(CELL_NUM1_OFFSET,0x2356, 0x78912);    
	
	Delay_ms(20);

	ReadFlashNBtye(CELL_NUM1_OFFSET, Temp_Data,4); 
	
             
	Delay_ms(10);

	for(i=0; i<4; i++)
		printf("Temp_Data[%d]: 0x%x\n", i, Temp_Data[i]);



	ReadFlashNBtye(CELL_NUM1_OFFSET+4, Temp_Data,4); 
	
             
	Delay_ms(10);

	for(i=0; i<4; i++)
		printf("Temp_Data[%d]: 0x%x\n", i, Temp_Data[i]);
	
}


