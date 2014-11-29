#ifndef _LOOPBACK_H
#define _LOOPBACK_H

#include "types.h"


#define NET_TCP_ALIVE 							0x0
#define NET_TCP_ALIVE_RSP 						0x1
#define NET_TCP_RSP_OK 							0x2
#define NET_TCP_RSP_ERROR 						0x3


#define NET_TCP_INPUT_ALARM_STAT 				0x4
#define NET_TCP_DETECTOR_LOSS 					0x5
#define NET_TCP_ALARM_BATTERY_LOW_POWER 		0x6
#define NET_TCP_ALARM_NET_LOSS					0x7
#define NET_TCP_ALARM_POWEROFF					0x8
#define NET_TCP_LINK_SET						0x9
#define NET_TCP_ARMING      					0xA
#define NET_TCP_DISARMING						0xB
#define NET_TCP_SET_TIME						0xC
#define NET_TCP_GET_TIME						0xD
#define NET_TCP_STAT_GET						0xE
#define NET_TCP_SET_CELL_NUM					0xF
#define NET_TCP_SET_TEXT_NUM					0x10
#define NET_TCP_SET_DETECTOR_SENS				0x11
#define NET_TCP_GET_ARM_STAT					0x12
#define NET_TCP_GET_LINK_STAT					0x13
#define NET_TCP_GET_DETECTOR_SENS				0x14
#define NET_TCP_UPLOAD_ARM_STAT					0x15
#define NET_TCP_GET_BALANCE						0x16
#define NET_TCP_CANCEL_TEXT_ALARM				0x17
#define NET_TCP_CANCEL_CELL_ALARM				0x18


#define NET_TCP_INPUT_ALARM_STAT_RSP				0x54
#define NET_TCP_DETECTOR_LOSS_RSP 					0x55
#define NET_TCP_ALARM_BATTERY_LOW_POWER_RSP 		0x56
#define NET_TCP_ALARM_NET_LOSS_RSP					0x57
#define NET_TCP_ALARM_POWEROFF_RSP					0x58
#define NET_TCP_LINK_SET_RSP						0x59
#define NET_TCP_ARMING_RSP      					0x5A
#define NET_TCP_DISARMING_RSP						0x5B
#define NET_TCP_SET_TIME_RSP						0x5C
#define NET_TCP_GET_TIME_RSP						0x5D
#define NET_TCP_STAT_GET_RSP						0x5E
#define NET_TCP_SET_CELL_NUM_RSP					0x5F
#define NET_TCP_SET_TEXT_NUM_RSP					0x60
#define NET_TCP_SET_DETECTOR_SENS_RSP				0x61
#define NET_TCP_GET_ARM_STAT_RSP					0x62
#define NET_TCP_GET_LINK_STAT_RSP					0x63
#define NET_TCP_GET_DETECTOR_SENS_RSP				0x64
#define NET_TCP_UPLOAD_ARM_STAT_RSP					0x65
#define NET_TCP_GET_BALANCE_RSP						0x66
#define NET_TCP_CANCEL_TEXT_ALARM_RSP				0x67
#define NET_TCP_CANCEL_CELL_ALARM_RSP				0x68
#define NET_TCP_CMD_END								0x69


#define MAX_NEED_RECV			NET_TCP_CMD_END

#define NET_TCP_VALID_CMD						0xf0
#define NET_TCP_TEST							0xf1

#define GPRS_FRAME_HEAD 0xfd
#define FRAME_HEAD 0xfe
#define KEEP_ALIVE 0x55

#define TCP_MODE 1
#define GPRS_MODE 0

#define tick_second 1

typedef struct
{
	u8 head;
	u8 len;
	u8 cmd;
	u8 checkout;
}FRAME;


void loopback_tcps(SOCKET s, uint16 port);
void loopback_tcpc(SOCKET s, uint16 port, u8 mode);
void loopback_udp(SOCKET s, uint16 port);
u8 check_net_mode(SOCKET s, u8* mode);
int recv_loop(SOCKET s, u8 mode, u8 cmd);
u8 do_send_ok(SOCKET s, u8 mode, u8 cmd);
u8 get_para_for_forum(void);
u8 set_tcp_utils(void);

#endif
