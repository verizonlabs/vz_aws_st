/**
  ******************************************************************************
  * @file    ST/net/com/inc/com_sockets_lwip_mcu.h
  * @author  MCD Application Team
  * @version V1.0
  * @date    28-Mar-2018
  * @brief   This file defines Communication Socket LwIP on MCU side
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
#ifndef __COM_SOCKETS_LWIP_MCU_H__
#define __COM_SOCKETS_LWIP_MCU_H__ 1

#ifdef __cplusplus 
extern "C" { 
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h> 
#include <stdbool.h> 

#include "com_sockets_addr_compat.h"
  
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/   
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/* Com Init for LwIP MCU */
bool com_init_lwip_mcu(void);
void com_start_lwip_mcu(void);  

int32_t com_socket_lwip_mcu(int32_t family, int32_t type, int32_t protocol);
int32_t com_setsockopt_lwip_mcu(int32_t sock, int32_t level, int32_t optname, const void *optval, int32_t optlen);
int32_t com_getsockopt_lwip_mcu(int32_t sock, int32_t level, int32_t optname, void *optval, int32_t *optlen);
int32_t com_bind_lwip_mcu(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen);
int32_t com_connect_lwip_mcu(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen);

int32_t com_send_lwip_mcu(int32_t sock, const com_char_t *buf, size_t len, int32_t flags);
int32_t com_recv_lwip_mcu(int32_t sock, com_char_t *buf, size_t len, int32_t flags);

int32_t com_closesocket_lwip_mcu(int32_t sock);

int32_t com_listen_lwip_mcu(int32_t sock, int32_t backlog);
int32_t com_accept_lwip_mcu(int32_t sock, com_sockaddr_t *addr, int32_t *addrlen);

int32_t com_sendto_lwip_mcu(int32_t sock, const com_char_t *buf, size_t len, int32_t flags,
                        const com_sockaddr_t *to, int32_t tolen);
int32_t com_recvfrom_lwip_mcu(int32_t sock, com_char_t *buf, size_t len, int32_t flags,
                          com_sockaddr_t *from, int32_t *fromlen);

int32_t com_getpeername_lwip_mcu(int32_t sock, com_sockaddr_t *name, int32_t *namelen);
int32_t com_getsockname_lwip_mcu(int32_t sock, com_sockaddr_t *name, int32_t *namelen);

int32_t com_gethostbyname_lwip_mcu(const com_char_t *name, com_sockaddr_t *addr);

#ifdef __cplusplus 
} 
#endif 

#endif /* __COM_SOCKETS_LWIP_MCU_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
