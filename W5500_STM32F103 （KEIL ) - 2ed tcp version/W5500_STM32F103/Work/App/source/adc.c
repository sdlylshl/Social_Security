#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"

#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"


#if 0
#define ADC1_DR_Address ((u32)0x4001244C) 
u16 ADC_ConvertedValue; 


void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;//定义DMA初始化结构体
	DMA_DeInit(DMA1_Channel1);//复位DMA通道1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //定义 DMA通道外设基地址=ADC1_DR_Address
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue; //定义DMA通道存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//指定外设为源地址
	DMA_InitStructure.DMA_BufferSize = 1;//定义DMA缓冲区大小1
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//当前外设寄存器地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//当前存储器地址不变
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //定义存储器数据宽度16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA通道操作模式位环形缓冲模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//禁止DMA通道存储器到存储器传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//初始化DMA通道1
	DMA_Cmd(DMA1_Channel1, ENABLE); //使能DMA通道1
}


void ADC_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;//定义ADC初始化结构体变量
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//有软件控制转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//转换通道为通道1
	ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_28Cycles5);
	//ADC1选择信道14,音序器等级1,采样时间239.5个周期
	ADC_DMACmd(ADC1, ENABLE);//使能ADC1模块DMA
	ADC_Cmd(ADC1, ENABLE);//使能ADC1
	ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
	ADC_StartCalibration(ADC1);//开始ADC1校准
	while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能ADC1软件开始转换
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


#endif


int test_data;

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = AD_IN1 | AD_IN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u8 is_poweroff(void)
{	
	u32 data;
	data = Get_Adc(11);
	if(data < POWER_OFF_THR)
		return 1;
	else
		return 0;
}

u8 is_battery_lowpow(void)
{	
	u32 data;
	data = Get_Adc(12);
	if(data < BATTERY_LOWPOW_THR)
		return 1;
	else
		return 0;
}



u16 adc_test()
{
	u32 temp_val=0;
	u8 t, i;

	for(i=0; i<2; i++)
	{
		temp_val+=Get_Adc(11+i);
		printf("adc%d : %d\n", i+11, temp_val);
		Delay_ms(100);
	}
	test_data = Get_Adc(12);
	
} 	 
























