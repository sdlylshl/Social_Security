/*
*
@file		util.h
@brief	
*/

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>


void Set_network(void);

void Reset_W5500(void);

void LED1_onoff(uint8_t on_off);
void LED2_onoff(uint8_t on_off);
void LED3_onoff(uint8_t on_off);
void LED4_onoff(uint8_t on_off);

void USART1_Init(void);
void USART2_Init(void);

void Delay_us(uint8 time_us);
void Delay_ms(uint16 time_ms);

//int putchar(int ch);
//int getchar(void);
int fputc(int ch, FILE *f);
//int fgetc(int ch, FILE *f);


uint32_t time_return(void);

#endif
