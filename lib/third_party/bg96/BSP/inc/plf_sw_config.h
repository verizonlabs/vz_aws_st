/**
  ******************************************************************************
  * @file    project_config/inc/plf_sw_config.h
  * @author  MCD Marketing / Vertical Application Team
  * @brief   This file contains the software configuration of the platform
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLF_SW_CONFIG_H
#define __PLF_SW_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_modem_config.h"
#include "plf_stack_size.h"

/* Public define ------------------------------------------------------------*/
/* Compilation Flag  BEGIN */


/* Stack size trace analysis */
#define STACK_ANALYSIS_TRACE   (0)
#if (STACK_ANALYSIS_TRACE == 1)
#include "stack_analysis.h"
#endif /* STACK_ANALYSIS_TRACE */

/* Compilation Flag  END */

/* Stack Size BEGIN */
/* Number of threads in the Project */

/* Stack Size END */

/* Stack Priority BEGIN */
#define TCPIP_THREAD_PRIO                  osPriorityBelowNormal
#define PPPOSIF_CLIENT_THREAD_PRIO         osPriorityHigh
#define PPPOSIF_SERVER_THREAD_PRIO         osPriorityNormal
#define DC_CTRL_THREAD_PRIO                osPriorityNormal
#define DC_TEST_THREAD_PRIO                osPriorityNormal
#define DC_MEMS_THREAD_PRIO                osPriorityNormal
#define DC_EMUL_THREAD_PRIO                osPriorityNormal
#define ATCORE_THREAD_STACK_PRIO           osPriorityHigh  /*osPriorityNormal*/
#define IAC_COAP_RECEIVE_THREAD_PRIO       osPriorityNormal
#define COAP_RESOURCES_THREAD_PRIO         osPriorityNormal
#define CELLULAR_SERVICE_THREAD_PRIO       osPriorityNormal
#define NIFMAN_THREAD_PRIO                 osPriorityNormal
#define HRM_THREAD_STACK_PRIO              osPriorityNormal
#define HTTPCLIENT_THREAD_PRIO             osPriorityNormal
#define CTRL_THREAD_PRIO                   osPriorityAboveNormal
#define HTTPSERVER_THREAD_PRIO             osPriorityNormal
#define LIVEBOX_THREAD_PRIO                osPriorityNormal
#define PINGCLIENT_THREAD_PRIO             osPriorityNormal
/* Stack Priority END */

/* IAC config BEGIN */
#define IAC_COAP_SERVER_PORT         5683       /* define the IAC CoAP Server local connection port */
/* IAC config END */

/* IPC config BEGIN */
#define USER_DEFINED_IPC_MAX_DEVICES   (1)
#define USER_DEFINED_IPC_DEVICE_MODEM  (IPC_DEVICE_0)
/* IPC config END */

/* Network ADDR BEGIN */
#define PPP_SERVER_ADDR_HEX     0x0A00A8C0            /* 192.168.0.10 */
#define PPP_CLIENT_ADDR_HEX     0x0B00A8C0            /* 192.168.0.11 */
#define F7_UDP_CLIENT_ADDR_HEX  PPP_CLIENT_ADDR_HEX   /* 192.168.0.11 */

#define F7_UDP_CLIENT_PORT     5683
#define PPP_NETMASK_HEX        0x00FFFFFF    /* 255.255.255.0 */
/* Network ADDR END */

/* Polling modem period */
#if (USE_SOCKETS_MODEM == 1)
#define CST_MODEM_POLLING_PERIOD          10000 /* 0 to disable / 10000 for Polling period = 10s */
#else
#define CST_MODEM_POLLING_PERIOD          0      /* No polling for modem monitoring */
#endif

/* FLASH config mapping */
#define FEEPROM_UTILS_FLASH_USED      (1)
#define FEEPROM_UTILS_LAST_PAGE_ADDR  (FLASH_LAST_PAGE_ADDR)
#define FEEPROM_UTILS_APPLI_MAX       5

/* Trace flags BEGIN */
/* trace channels: ITM - UART */
#define TRACE_IF_TRACES_ITM     (0)
#define TRACE_IF_TRACES_UART    (1)

/* trace masks allowed */
/* P0, WARN and ERROR traces only */
#define TRACE_IF_MASK    (uint16_t)(DBL_LVL_P0 | DBL_LVL_WARN | DBL_LVL_ERR)
/* Full traces */
/* #define TRACE_IF_MASK    (uint16_t)(DBL_LVL_P0 | DBL_LVL_P1 | DBL_LVL_P2 | DBL_LVL_WARN | DBL_LVL_ERR) */

/* trace module flags, do not use these in AWS FreeRTOS implementation*/
#define USE_TRACE_INTERFACE_TEST                (0)
#define USE_TRACE_INTERFACE_SYSCTRL             (0)
#define USE_TRACE_INTERFACE_ATCORE              (0)
#define USE_TRACE_INTERFACE_ATCUSTOM_MODEM      (0)
#define USE_TRACE_INTERFACE_ATCUSTOM_COMMON     (0)
#define USE_TRACE_INTERFACE_ATDATAPACK          (0)
#define USE_TRACE_INTERFACE_ATPARSER            (0)
#define USE_TRACE_INTERFACE_CELLULAR_SERVICE    (0)
#define USE_TRACE_INTERFACE_ATCUSTOM_SPECIFIC   (0)
#define USE_TRACE_INTERFACE_COM_SOCKETS_IP      (0)
#define USE_TRACE_INTERFACE_HTTP_CLIENT         (0)
#define USE_TRACE_INTERFACE_IPC                 (0)
#define USE_TRACE_INTERFACE_PPPOSIF             (0)
#define USE_TRACE_INTERFACE_DCLIB               (0)
#define USE_TRACE_INTERFACE_DCMEMS              (0)
#define USE_TRACE_INTERFACE_ERROR_HANDLER		(0)
/* Trace flags END */

 /* AWS CONFIFPRINTF debug print module flags use these in AWS FreeRTOS implementation */
 #define USE_CONFIGPRINTF_SYSCTRL             (0)
 #define USE_CONFIGPRINTF_ATCORE              (0)
 #define USE_CONFIGPRINTF_ATCUSTOM_MODEM      (0)
 #define USE_CONFIGPRINTF_ATCUSTOM_COMMON     (0)
 #define USE_CONFIGPRINTF_ATDATAPACK          (0)
 #define USE_CONFIGPRINTF_ATPARSER            (0)
 #define USE_CONFIGPRINTF_CELLULAR_SERVICE    (0)
 #define USE_CONFIGPRINTF_ATCUSTOM_SPECIFIC   (0)
 #define USE_CONFIGPRINTF_COM_SOCKETS_IP      (0)
 #define USE_CONFIGPRINTF_IPC                 (0)
 #define USE_CONFIGPRINTF_DCLIB               (0)
 #define USE_CONFIGPRINTF_DCMEMS              (0)
 #define USE_CONFIGPRINTF_ERROR_HANDLER		  (0)
 /* AWS CONFIFPRINTF debug print module flags END */

#ifdef __cplusplus
}
#endif

#endif /* __PLF_SW_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
