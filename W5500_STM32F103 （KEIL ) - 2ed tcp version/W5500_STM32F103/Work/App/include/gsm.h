#ifndef __GSM_H
#define __GSM_H

int gsm_test(void);
int gprs_loop(u8 mode);
int gsm_usart2_send(u8* cmd, u8 len);
int do_gsm_cell_alarm(u8* cell_num);
int do_gsm_text_alarm(u8* text_num, u8* text_content);
int gsm_init(void);
int do_cell_alarm(void);
int text_alarm(void);


#endif

