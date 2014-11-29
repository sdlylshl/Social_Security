#ifndef __GSM_H
#define __GSM_H

int gsm_test(void);
int gprs_loop(u8 mode);
int gsm_usart2_send(u8* cmd, u8 len);


#endif

