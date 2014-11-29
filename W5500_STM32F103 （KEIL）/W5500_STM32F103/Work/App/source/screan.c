#include "screan.h"
#include "stdint.h"



void screan_output(uint8_t *dat)
{
	printf("\nscrean : %s \n",dat);
}


void set_Page(uint16_t type)
{
	switch (type)
	{
		case 0xff:
			change_page_by_type(HOMEPAGE);
			break;
		case 1:
			change_page_by_type(SAVE_SELECTPAGE);
			break;
		case 2:
			change_page_by_type(GET_SELECTPAGE);
			break;

		default:
			break;
	}
}

void change_page_by_type(PageType type)
{
	screan_output("-------- start --------");
	switch (type)
	{
		case HOMEPAGE:
		{
			printf("一邻储物柜\n");
			printf("2014-10-9 11：40\n");
			printf("可用柜量：(22/50)\n");
		}
			break;
		case SAVE_SELECTPAGE:
		{
			printf("存件\n");
			printf("1、快递员\n");
			printf("2、发快递\n");
			printf("3、临时存件\n");
		}
			break;
		case GET_SELECTPAGE:
		{
			printf("1、存件\n");
			printf("2、快递员\n");
			printf("3、收件人\n");
		}

			break;

			
		default:
			break;
	}
	screan_output("-------- end --------");
}




