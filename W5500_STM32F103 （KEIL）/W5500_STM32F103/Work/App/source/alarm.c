#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"


u8 get_alarm_in(u8 in)
{
	u8 ret;
	if (in == 0)
		ret = GPIO_ReadInputDataBit(GPIOD,IO_INPUT_0);
	else if(in>0 && in<8)
		ret = GPIO_ReadInputDataBit(GPIOC,IO_INPUT_1<<(in-1));
	else
		ret = GPIO_ReadInputDataBit(GPIOD,IO_INPUT_8<<(in-8));
	return ret;
}

u8 get_alarm_out(u8 out)
{
	u8 ret;
	ret = GPIO_ReadInputDataBit(GPIOE, IO_OUTPUT_0<<out);
	return ret;
}

u8 set_alarm_out(u8 out, u8 val)
{	
	if(val)
		GPIO_WriteBit(GPIOE, IO_OUTPUT_0<<out, Bit_SET);
	else
		GPIO_WriteBit(GPIOE, IO_OUTPUT_0<<out, Bit_RESET);

	return OK;
}

u8 get_key(u8 key)
{
	u8 ret;
	ret = GPIO_ReadInputDataBit(GPIOE, KEY0<<key);
	return ret;
}

u8 led_red(u8 led)
{	
	if (led == 0)
	{
		GPIO_WriteBit(GPIOA, LED0_P, Bit_RESET);
		GPIO_WriteBit(GPIOA, LED0_S, Bit_RESET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOA, LED1_P, Bit_RESET);
		GPIO_WriteBit(GPIOA, LED1_S, Bit_RESET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOC, LED2_P, Bit_RESET);
		GPIO_WriteBit(GPIOC, LED2_S, Bit_RESET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOB, LED3_P, Bit_RESET);
		GPIO_WriteBit(GPIOB, LED3_S, Bit_RESET);
	}
	return OK;
}

u8 led_green(u8 led)
{
	if (led == 0)
	{
		GPIO_WriteBit(GPIOA, LED0_P, Bit_RESET);
		GPIO_WriteBit(GPIOA, LED0_S, Bit_SET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOA, LED1_P, Bit_RESET);
		GPIO_WriteBit(GPIOA, LED1_S, Bit_SET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOC, LED2_P, Bit_RESET);
		GPIO_WriteBit(GPIOC, LED2_S, Bit_SET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOB, LED3_P, Bit_RESET);
		GPIO_WriteBit(GPIOB, LED3_S, Bit_SET);
	}
	return OK;

}

u8 led_off(u8 led)
{
	if (led == 0)
	{
		GPIO_WriteBit(GPIOA, LED0_P, Bit_SET);
		GPIO_WriteBit(GPIOA, LED0_S, Bit_SET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOA, LED1_P, Bit_SET);
		GPIO_WriteBit(GPIOA, LED1_S, Bit_SET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOC, LED2_P, Bit_SET);
		GPIO_WriteBit(GPIOC, LED2_S, Bit_SET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOB, LED3_P, Bit_SET);
		GPIO_WriteBit(GPIOB, LED3_S, Bit_SET);
	}
	return OK;
}


u8 get_ad_in()
{
	u8 ret;
	ret = GPIO_ReadInputDataBit(GPIOA, AD_IN1);
	return ret;
}


u8 is_power_low()
{
	return 0;
}


u8 alarm_in_test()
{	
	u8 i;
	u32 stat=0;

	for(i=0; i<16; i++)
		stat |= (get_alarm_in(i)<<i);

	printf("alarm in stat: 0x%x\n", stat);
	return OK;
}

u8 alarm_out_test()
{
	u8 i;

	for(i=0; i<8; i++)
		set_alarm_out(i, 0);

	return OK;
}

