#include "voice.h"
#include "stdint.h"


void voice_output(uint8_t *dat,uint8_t len)
{
	int i = 0;
	printf("voice : ");
	for (; i < len; i++)
	{
		printf("%2X ", dat[i]);
	}
	printf("\n");

}



