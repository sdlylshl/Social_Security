#ifndef __KEYS_H
#define __KEYS_H

#include "stdint.h"
#define MAX_KEYS_NUM 100

typedef void(*KeyProcessPtr)(uint16_t);
extern KeyProcessPtr keyProcessHandle;
void nullHandle(uint16_t key);



#endif

