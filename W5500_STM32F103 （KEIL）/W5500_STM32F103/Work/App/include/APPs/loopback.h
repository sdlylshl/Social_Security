#ifndef _LOOPBACK_H
#define _LOOPBACK_H

#include "types.h"
#include "stdint.h"

#define NET_TCP_ALIVE 							0x0
#define NET_TCP_ALIVE_RSP 						0x1
#define NET_TCP_RSP_OK 							0x2
#define NET_TCP_RSP_ERROR 						0x3
#define NET_TCP_INPUT_ALARM_STAT 				0x4
#define NET_TCP_DETECTOR_LOSS 					0x6
#define NET_TCP_ALARM_BATTERY_LOW_POWER 		0x7
#define NET_TCP_ALARM_NET_LOSS					0x8
#define NET_TCP_ALARM_POWEROFF					0x9
#define NET_TCP_LINK_SET						0xA
#define NET_TCP_ARMING      					0xB
#define NET_TCP_DISARMING						0xC
#define NET_TCP_SET_TIME						0xD
#define NET_TCP_GET_TIME						0xE
#define NET_TCP_GET_TIME_RSP					0xF
#define NET_TCP_STAT_GET						0x10
#define NET_TCP_STAT_RSP						0x11
#define NET_TCP_SET_CELL_NUM					0x12
#define NET_TCP_SET_TEXT_NUM					0x13
#define NET_TCP_SET_DETECTOR_SENS				0x14
#define NET_TCP_GET_ARM_STAT					0x15
#define NET_TCP_GET_LINK_STAT					0x16
#define NET_TCP_GET_DETECTOR_SENS				0x17
#define NET_TCP_GET_ARM_STAT_RSP				0x18
#define NET_TCP_GET_LINK_STAT_RSP				0x19
#define NET_TCP_GET_DETECTOR_SENS_RSP			0x20
#define NET_TCP_UPLOAD_ARM_STAT					0x21

#define GPRS_FRAME_HEAD 0xfd
#define FRAME_HEAD 0xE0
#define FRAME_END  0x18
#define KEEP_ALIVE 0x55


// 指令最大长度
#define CMD_LEN 0x50
// 指令数据区最大长度 指令长度减去(FRAME_HEAD crc FRAME_END)
#define CMD_DATA_LEN (CMD_LEN-10)
//#define OK 0
//#define ERROR 0xFF
// 接收指令最大数量
#define RECV_CMDS_NUM 20
struct msgStu
{
    uint8_t    usable;
    uint8_t    head  ;//0xE0
    uint8_t    len;//data ???
    uint8_t    sn[2]  ;//
    uint8_t    data[CMD_DATA_LEN];
    uint8_t    crc[4];
    uint8_t    endl;//0x18
};


#endif
