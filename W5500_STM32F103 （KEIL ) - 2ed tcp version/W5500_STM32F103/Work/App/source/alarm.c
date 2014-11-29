#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"

extern sys_para sys;

u8 get_alarm_in(u8 in)
{
	u8 ret;
	if (in < 3)
		ret = GPIO_ReadInputDataBit(GPIOC,IO_INPUT_0<<(in));
	else if(in>=3 && in<=10)
		ret = GPIO_ReadInputDataBit(GPIOD,IO_INPUT_3<<(in-3));
	else if(in == 11)
		ret = GPIO_ReadInputDataBit(GPIOB,IO_INPUT_11);
	else if(in>11 && in<14)
		ret = GPIO_ReadInputDataBit(GPIOB,IO_INPUT_12<<(in-12));
	else 
		ret = GPIO_ReadInputDataBit(GPIOE,IO_INPUT_14<<(in-14));
	return ret;
}

u8 get_alarm_out(u8 out)
{
	u8 ret;
	ret = GPIO_ReadInputDataBit(GPIOC, IO_OUTPUT_0<<out);
	return ret;
}

u8 set_alarm_out(u8 out, u8 val)
{	
	if(val)
		GPIO_WriteBit(GPIOC, IO_OUTPUT_0<<out, Bit_SET);
	else
		GPIO_WriteBit(GPIOC, IO_OUTPUT_0<<out, Bit_RESET);

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
		GPIO_WriteBit(GPIOE, LED0_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED0_S, Bit_RESET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOE, LED1_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED1_S, Bit_RESET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOE, LED2_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED2_S, Bit_RESET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOE, LED3_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED3_S, Bit_RESET);
	}
	return OK;
}

u8 led_green(u8 led)
{
	if (led == 0)
	{
		GPIO_WriteBit(GPIOE, LED0_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED0_S, Bit_SET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOE, LED1_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED1_S, Bit_SET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOE, LED2_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED2_S, Bit_SET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOE, LED3_P, Bit_RESET);
		GPIO_WriteBit(GPIOE, LED3_S, Bit_SET);
	}
	return OK;

}

u8 led_off(u8 led)
{
	if (led == 0)
	{
		GPIO_WriteBit(GPIOE, LED0_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED0_S, Bit_SET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOE, LED1_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED1_S, Bit_SET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOE, LED2_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED2_S, Bit_SET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOE, LED3_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED3_S, Bit_SET);
	}
	return OK;
}

u8 led_test(u8 led)
{
	if (led == 0)
	{
		GPIO_WriteBit(GPIOE, LED0_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED0_S, Bit_SET);
	}

	if (led == 1)
	{
		GPIO_WriteBit(GPIOE, LED1_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED1_S, Bit_RESET);
	}

	if (led == 2)
	{
		GPIO_WriteBit(GPIOE, LED2_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED2_S, Bit_RESET);
	}

	if (led == 3)
	{
		GPIO_WriteBit(GPIOE, LED3_P, Bit_SET);
		GPIO_WriteBit(GPIOE, LED3_S, Bit_RESET);
	}
	return OK;
}
u8 get_ad_in()
{
	u8 ret;
	ret = GPIO_ReadInputDataBit(GPIOC, AD_IN1);
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

	for(i=0; i<MAX_ALARM_OUT; i++)
		set_alarm_out(i, 0);

	return OK;
}

u8 do_alarm_link(u32 status)
{
	u8 group, i, j;
	link_para link;

	link = sys.link;

	
	for(j=0; j<link.link_group; j++)
	{
		if(link.xor[j])
		{
			if((status & link.in[j]) != 0)
			{
				for(i=0; i<MAX_LINK_OUT; i++)
				{
					if(link.out[j] & (1 << i))
						set_alarm_out(i, 1);
				}
			}
		}
		else
		{
			if((status ^ link.in[j]) == 0)
			{
				for(i=0; i<MAX_LINK_OUT; i++)
				{
					if(link.out[j] & (1 << i))
						set_alarm_out(i, 1);
				}
			}
		}
	}
	return OK;
}


u8 do_disarm_link()
{
	u8 i;

	for(i=0; i<MAX_ALARM_OUT; i++)
		set_alarm_out(i, 0);
}


u8 beep_off()
{
	GPIO_WriteBit(GPIOE, BEEP, Bit_RESET);
}

u8 beep_on()
{	
	GPIO_WriteBit(GPIOE, BEEP, Bit_SET);
}



