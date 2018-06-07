/**
  ******************************************************************************
  * @file    project_config/inc/stack_size.h
  * @author  MCD Application Team
  * @brief   This file contains the software configuration of the platform
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLF_STACK_SIZE_H
#define __PLF_STACK_SIZE_H

#include "features.h"

#define TCPIP_THREAD_STACK_SIZE             512
#define PPPOSIF_CLIENT_THREAD_STACK_SIZE    640
#define PPPOSIF_SERVER_THREAD_STACK_SIZE    768
#define DC_CTRL_THREAD_STACK_SIZE           256
#define DC_TEST_THREAD_STACK_SIZE           256
#define DC_MEMS_THREAD_STACK_SIZE           256
#define DEFAULT_THREAD_STACK_SIZE           192
#define DC_EMUL_THREAD_STACK_SIZE           300
#define ATCORE_THREAD_STACK_SIZE            256
#define IAC_COAP_RECEIVE_THREAD_STACK_SIZE  512
#define COAP_RESOURCES_THREAD_STACK_SIZE    384
#define CELLULAR_SERVICE_THREAD_STACK_SIZE  512
#define NIFMAN_THREAD_STACK_SIZE            256
#define HRM_THREAD_STACK_SIZE               300
#define HTTPCLIENT_THREAD_STACK_SIZE        384
#define HTTPSERVER_THREAD_STACK_SIZE        512
#define LIVEBOX_THREAD_STACK_SIZE           2048
#define PINGCLIENT_THREAD_STACK_SIZE        256
#define FREERTOS_TIMER_THREAD_STACK_SIZE    256
#define FREERTOS_IDLE_THREAD_STACK_SIZE     128

#define USED_DC_CTRL_THREAD_STACK_SIZE           DC_CTRL_THREAD_STACK_SIZE
#define USED_ATCORE_THREAD_STACK_SIZE            ATCORE_THREAD_STACK_SIZE
#define USED_CELLULAR_SERVICE_THREAD_STACK_SIZE  CELLULAR_SERVICE_THREAD_STACK_SIZE
#define USED_NIFMAN_THREAD_STACK_SIZE            NIFMAN_THREAD_STACK_SIZE
#define USED_DEFAULT_THREAD_STACK_SIZE           DEFAULT_THREAD_STACK_SIZE
#define USED_FREERTOS_TIMER_THREAD_STACK_SIZE    FREERTOS_TIMER_THREAD_STACK_SIZE
#define USED_FREERTOS_IDLE_THREAD_STACK_SIZE     FREERTOS_IDLE_THREAD_STACK_SIZE

#define USED_DC_CTRL_THREAD           1
#define USED_ATCORE_THREAD            1
#define USED_NIFMAN_THREAD            1
#define USED_CELLULAR_SERVICE_THREAD  1
#define USED_DEFAULT_THREAD_THREAD    1
#define USED_FREERTOS_TIMER_THREAD    1
#define USED_FREERTOS_IDLE_THREAD     1


#if (USE_SOCKETS_MODEM == 0)
#define USED_TCPIP_THREAD_STACK_SIZE           TCPIP_THREAD_STACK_SIZE
#define USED_TCPIP_THREAD                        1
#else
#define USED_TCPIP_THREAD_STACK_SIZE             0
#define USED_TCPIP_THREAD                        0
#endif

#if (USE_PPPOSIF_CLIENT == 1)
#define USED_PPPOSIF_CLIENT_THREAD_STACK_SIZE           PPPOSIF_CLIENT_THREAD_STACK_SIZE
#define USED_PPPOSIF_CLIENT_THREAD                        1
#else
#define USED_PPPOSIF_CLIENT_THREAD_STACK_SIZE             0
#define USED_PPPOSIF_CLIENT_THREAD                        0
#endif

#if (USE_PPPOSIF_SERVER == 1)
#define USED_PPPOSIF_SERVER_THREAD_STACK_SIZE           PPPOSIF_SERVER_THREAD_STACK_SIZE
#define USED_PPPOSIF_SERVER_THREAD                        1
#else
#define USED_PPPOSIF_SERVER_THREAD_STACK_SIZE             0
#define USED_PPPOSIF_SERVER_THREAD                        0
#endif

#if (USE_DC_TEST == 1)
#define USED_DC_TEST_THREAD_STACK_SIZE           DC_TEST_THREAD_STACK_SIZE
#define USED_DC_TEST_THREAD                        1
#else
#define USED_DC_TEST_THREAD_STACK_SIZE             0
#define USED_DC_TEST_THREAD                        0
#endif

#if (USE_DC_MEMS == 1)
#define USED_DC_MEMS_THREAD_STACK_SIZE           DC_MEMS_THREAD_STACK_SIZE
#define USED_DC_MEMS_THREAD                        1
#else
#define USED_DC_MEMS_THREAD_STACK_SIZE             0
#define USED_DC_MEMS_THREAD                        0
#endif

#if (USE_DC_EMUL == 1)
#define USED_DC_EMUL_THREAD_STACK_SIZE           DC_EMUL_THREAD_STACK_SIZE
#define USED_DC_EMUL_THREAD                        1
#else
#define USED_DC_EMUL_THREAD_STACK_SIZE             0
#define USED_DC_EMUL_THREAD                        0
#endif


#if (USE_COAP == 1)
#define USED_IAC_COAP_RECEIVE_THREAD_STACK_SIZE  IAC_COAP_RECEIVE_THREAD_STACK_SIZE
#define USED_COAP_RESOURCES_THREAD_STACK_SIZE    COAP_RESOURCES_THREAD_STACK_SIZE
#define USED_COAP_RESOURCES_THREAD                     1
#define USED_IAC_COAP_RECEIVE_THREAD                   1
#else
#define USED_COAP_RESOURCES_THREAD_STACK_SIZE          0
#define USED_IAC_COAP_RECEIVE_THREAD_STACK_SIZE        0
#define USED_COAP_RESOURCES_THREAD                     0
#define USED_IAC_COAP_RECEIVE_THREAD                   0
#endif


#if (USE_HRM == 1)
#define USED_HRM_THREAD_STACK_SIZE               HRM_THREAD_STACK_SIZE
#define USED_HRM_THREAD                          1
#else
#define USED_HRM_THREAD_STACK_SIZE               0
#define USED_HRM_THREAD                          0
#endif


#if (USE_HTTP_CLIENT == 1)
#define USED_HTTPCLIENT_THREAD_STACK_SIZE        HTTPCLIENT_THREAD_STACK_SIZE
#define USED_HTTPCLIENT_THREAD                   1
#else
#define USED_HTTPCLIENT_THREAD_STACK_SIZE        0
#define USED_HTTPCLIENT_THREAD                   0
#endif


#if (USE_HTTP_SERVER == 1)
#define USED_HTTPSERVER_THREAD_STACK_SIZE        HTTPSERVER_THREAD_STACK_SIZE
#define USED_HTTPSERVER_THREAD                   1
#else
#define USED_HTTPSERVER_THREAD_STACK_SIZE        0
#define USED_HTTPSERVER_THREAD                   0
#endif


#if (USE_LIVEOBJECTS == 1)
#define USED_LIVEBOX_THREAD_STACK_SIZE           LIVEBOX_THREAD_STACK_SIZE
#define USED_LIVEBOX_THREAD                      1
#else
#define USED_LIVEBOX_THREAD_STACK_SIZE           0
#define USED_LIVEBOX_THREAD                      0
#endif

#if (USE_PING_CLIENT == 1)
#define USED_PINGCLIENT_THREAD_STACK_SIZE           PINGCLIENT_THREAD_STACK_SIZE
#define USED_PINGCLIENT_THREAD                      1
#else
#define USED_PINGCLIENT_THREAD_STACK_SIZE           0
#define USED_PINGCLIENT_THREAD                      0
#endif

#define TOTAL_THREAD_STACK_SIZE                            \
              USED_TCPIP_THREAD_STACK_SIZE                 \
             +USED_DEFAULT_THREAD_STACK_SIZE               \
             +USED_PPPOSIF_CLIENT_THREAD_STACK_SIZE        \
             +USED_PPPOSIF_SERVER_THREAD_STACK_SIZE        \
             +USED_DC_CTRL_THREAD_STACK_SIZE               \
             +USED_ATCORE_THREAD_STACK_SIZE                \
             +USED_NIFMAN_THREAD_STACK_SIZE                \
             +USED_COAP_RESOURCES_THREAD_STACK_SIZE        \
             +USED_IAC_COAP_RECEIVE_THREAD_STACK_SIZE      \
             +USED_DC_TEST_THREAD_STACK_SIZE               \
             +USED_DC_MEMS_THREAD_STACK_SIZE               \
             +USED_DC_EMUL_THREAD_STACK_SIZE               \
             +USED_HRM_THREAD_STACK_SIZE                   \
             +USED_HTTPCLIENT_THREAD_STACK_SIZE            \
             +USED_HTTPSERVER_THREAD_STACK_SIZE            \
             +USED_LIVEBOX_THREAD_STACK_SIZE               \
             +USED_PINGCLIENT_THREAD_STACK_SIZE            \
             +USED_FREERTOS_TIMER_THREAD_STACK_SIZE        \
             +USED_FREERTOS_IDLE_THREAD_STACK_SIZE         \
             +USED_CELLULAR_SERVICE_THREAD_STACK_SIZE
               
               
#define THREAD_NUMBER                            \
               USED_TCPIP_THREAD                 \
              +USED_DEFAULT_THREAD               \
              +USED_PPPOSIF_CLIENT_THREAD        \
              +USED_PPPOSIF_SERVER_THREAD        \
              +USED_DC_CTRL_THREAD               \
              +USED_ATCORE_THREAD                \
              +USED_NIFMAN_THREAD                \
              +USED_COAP_RESOURCES_THREAD        \
              +USED_IAC_COAP_RECEIVE_THREAD      \
              +USED_DC_TEST_THREAD               \
              +USED_DC_EMUL_THREAD               \
              +USED_HRM_THREAD                   \
              +USED_HTTPCLIENT_THREAD            \
              +USED_HTTPSERVER_THREAD            \
              +USED_LIVEBOX_THREAD               \
              +USED_PINGCLIENT_THREAD            \
              +USED_FREERTOS_TIMER_THREAD        \
              +USED_FREERTOS_IDLE_THREAD         \
              +USED_CELLULAR_SERVICE_THREAD

#define PARTIAL_HEAP_SIZE                        8196
#define TOTAL_HEAP_SIZE                         ((size_t)(TOTAL_THREAD_STACK_SIZE)*4+PARTIAL_HEAP_SIZE)

#endif /* __PLF_STACK_SIZE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
