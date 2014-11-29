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
			printf("һ�ڴ����\n");
			printf("2014-10-9 11��40\n");
			printf("���ù�����(22/50)\n");
		}
			break;
		case SAVE_SELECTPAGE:
		{
			printf("���\n");
			printf("1�����Ա\n");
			printf("2�������\n");
			printf("3����ʱ���\n");
		}
			break;
		case GET_SELECTPAGE:
		{
			printf("1�����\n");
			printf("2�����Ա\n");
			printf("3���ռ���\n");
		}

			break;

			
		default:
			break;
	}
	screan_output("-------- end --------");
}




