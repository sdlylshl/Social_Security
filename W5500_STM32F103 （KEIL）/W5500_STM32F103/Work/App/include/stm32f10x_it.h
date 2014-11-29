/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM2_IRQHandler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
int parse_ack(u8* ack);
int parse_sep(void);
int parse_ATE0(void);
int parse_text_pro(void);
u8 parse_gprs_live_rsp(void);
u8 recv_one_cmd_from_gprs(u8* rcv, u8* one_cmd);



#define IP_START '#'
#define IP_END '#'
#define SEPARATOR '.'

#define DEST_IP '!'
#define DEST_END '!'

#define GATE_IP '$'
#define GATE_END '$'

#define NETMASK_IP '%'
#define NETMASK_END '%'

#define ID_START '&'
#define ID_END '&'

#define MAC_START '*'
#define MAC_END '*'

#define PORT_START '('
#define PORT_END '('

#define GSM_FRAME_S1 '\r'
#define GSM_FRAME_S2 '\n'

#define GSM_FRAME_E1 '\r'
#define GSM_FRAME_E2 '\n'

#define GSM_TEXT_PRO '>'

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
