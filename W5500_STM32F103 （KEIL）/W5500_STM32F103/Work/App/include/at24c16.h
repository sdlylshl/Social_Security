#ifndef __AT24C16_H
#define __AT24C16_H
#include "types.h"

void at24c16_write(uint16 addr, unsigned char val);
unsigned char at24c16_read(uint16 addr);

void eep_block_write(uint16 eepAddr, uint8* dat, uint16 index, uint16 len);
#endif
