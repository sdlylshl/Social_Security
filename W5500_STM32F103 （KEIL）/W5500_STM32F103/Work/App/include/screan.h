#ifndef __SCREAN_H
#define __SCREAN_H
#include "stdint.h"

//#define WELCOMEPAGE 0xff
//#define save_selectPage 0x01
//#define get_selectpage 0x02
typedef enum pageType
{
	HOMEPAGE,
	SAVE_SELECTPAGE,
	GET_SELECTPAGE
} PageType;

void screan_output(uint8_t *dat);
void set_Page(uint16_t type);
void change_page_by_type(PageType type);


#endif

