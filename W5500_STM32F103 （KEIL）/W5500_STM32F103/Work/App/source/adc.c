#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"

#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"



#define ADC1_DR_Address ((u32)0x4001244C) 
u16 ADC_ConvertedValue; 


void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;//����DMA��ʼ���ṹ��
	DMA_DeInit(DMA1_Channel1);//��λDMAͨ��1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //���� DMAͨ���������ַ=ADC1_DR_Address
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue; //����DMAͨ���洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//ָ������ΪԴ��ַ
	DMA_InitStructure.DMA_BufferSize = 1;//����DMA��������С1
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��ǰ����Ĵ�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//��ǰ�洢����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�����������ݿ��16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //����洢�����ݿ��16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMAͨ������ģʽλ���λ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMAͨ�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//��ֹDMAͨ���洢�����洢������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��DMAͨ��1
	DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMAͨ��1
}


void ADC_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;//����ADC��ʼ���ṹ�����
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ʹ��ɨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADCת������������ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//���������ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת�������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;//ת��ͨ��Ϊͨ��1
	ADC_Init(ADC1, &ADC_InitStructure); //��ʼ��ADC
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5);
	//ADC1ѡ���ŵ�14,�������ȼ�1,����ʱ��239.5������
	ADC_DMACmd(ADC1, ENABLE);//ʹ��ADC1ģ��DMA
	ADC_Cmd(ADC1, ENABLE);//ʹ��ADC1
	ADC_ResetCalibration(ADC1); //����ADC1У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�ADC1У׼�������
	ADC_StartCalibration(ADC1);//��ʼADC1У׼
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADC1У׼���
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ADC1�����ʼת��
}

u8 adc_init()
{
	ADC_DMA_Config();
	ADC_Config();

	//printf("ADC = %X Volt = %d mv\r\n", ADC_ConvertedValue, ADC_ConvertedValue*3300/4096);
	return OK;
}

u8 is_poweroff()
{
	u32 thr;

	thr = ADC_ConvertedValue;

	printf("thr: %u\n", thr);
	if (thr < 4096*800/5000)
		return 1;
	else
		return 0;
}


u8 adc_test()
{
	is_poweroff();
	return OK;
}

