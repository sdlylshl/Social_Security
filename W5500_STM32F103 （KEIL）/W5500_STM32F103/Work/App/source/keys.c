#include "keys.h"
#include "stdint.h"
#include "screan.h"
#include "voice.h"

uint16_t keysBuf[MAX_KEYS_NUM];
uint8_t rdPtr = 0;
uint8_t wrPtr = 0;
KeyProcessPtr keyProcessHandle;
void nullHandle(uint16_t key)
{
	printf("nullHandle + %2x",key);

}

void key_process(uint16_t key)
{
	switch (key)
	{
	case 0xff:

		break;
	case (uint8_t)'s':

		break;
	case (uint8_t)'g':

		break;
	default:
		break;
	}
}
/**
@brief   扫描当前是否有按键发生
@return  keysBuf[rdPtr] for success else 0xffff.
*/
uint16_t loopKeysMgr()
{
	
	return 0;
}

