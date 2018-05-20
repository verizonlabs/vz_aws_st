/**
  ******************************************************************************
  * @file    ST/net/com/inc/com_sockets_err_compat.h
  * @author  MCD Application Team
  * @version V1.0
  * @date    28-Mar-2018
  * @brief   This file defines Communication Socket Error Compatibility Interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_SOCKETS_ERR_COMPAT_H__
#define __COM_SOCKETS_ERR_COMPAT_H__ 1

#ifdef __cplusplus 
extern "C" { 
#endif 

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (COM_SOCKETS_ERRNO_COMPAT == 1) || (USE_SOCKETS_MODEM == 0)
#include "lwip\errno.h"
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Definitions for error constants */
typedef enum {
  COM_SOCKETS_ERR_OK          =   0, /* Everything OK              */
  COM_SOCKETS_ERR_GENERAL     =  -1, /* General / Low level error  */
  COM_SOCKETS_ERR_DESCRIPTOR  =  -2, /* Invalid socket descriptor  */
  COM_SOCKETS_ERR_PARAMETER   =  -3, /* Invalid parameter          */
  COM_SOCKETS_ERR_WOULDBLOCK  =  -4, /* Operation would block      */
  COM_SOCKETS_ERR_NOMEMORY    =  -5, /* Out of memory error        */
  COM_SOCKETS_ERR_CLOSING     =  -6, /* Connection is closing      */
  COM_SOCKETS_ERR_LOCKED      =  -7, /* Address in use             */
  COM_SOCKETS_ERR_TIMEOUT     =  -8, /* Timeout                    */
  COM_SOCKETS_ERR_INPROGRESS  =  -9, /* Operation in progress      */
  COM_SOCKETS_ERR_NONAME      = -10, /* Host Name not existing     */

  /* Added value */  
  COM_SOCKETS_ERR_NONETWORK   = -11, /* No network to proceed      */
  COM_SOCKETS_ERR_UNSUPPORTED = -12, /* Unsupported functionnality */
  COM_SOCKETS_ERR_STATE       = -13,  /* Connect rqt but Socket is already connected
                                        Send/Recv rqt but Socket is not connected
                                        ... */
} com_sockets_err_t;

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/* Used only in com_sockets_ip_modem 
   in getsockopt(SO_ERRNO) to translate an error com_sockets enum to errno enum */
int32_t com_sockets_err_to_errno(com_sockets_err_t err);

#ifdef __cplusplus 
} 
#endif 

#endif /* __COM_SOCKETS_ERR_COMPAT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

