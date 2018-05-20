/**
  ******************************************************************************
  * @file    ST/net/com/inc/com_sockets_ip_modem.h
  * @author  MCD Application Team
  * @version V1.0
  * @date    28-Mar-2018
  * @brief   This file defines Communication Socket IP on MODEM side
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
#ifndef __COM_SOCKETS_IP_MODEM_H__
#define __COM_SOCKETS_IP_MODEM_H__ 1

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
/* Com Init for IP Modem */
bool com_init_ip_modem(void);  
void com_start_ip_modem(void);  

/* Only TCP IPv4 client mode is implemented at low level */

/* Implemented: family  : AF_INET
                type    : SOCK_STREAM
                protocol: IPPROTO_TCP */
int32_t com_socket_ip_modem(int32_t family, int32_t type, int32_t protocol);

/* Implemented : SO_SNDTIMEO : OK but value is not used because there is already a tempo at low level
                 SO_RCVTIMEO : OK to avoid to be blocked on recv while distant server does not send any data */
int32_t com_setsockopt_ip_modem(int32_t sock, int32_t level, int32_t optname, const void *optval, int32_t optlen);

/* Implemented : SO_SNDTIMEO : default value (WaitForever)
                 SO_RCVTIMEO : default value (WaitForever) but never used because there is a tempo at low level
                 SO_ERROR    : get last error and reset it */
int32_t com_getsockopt_ip_modem(int32_t sock, int32_t level, int32_t optname, void *optval, int32_t *optlen);

/* Bind : OK but value is not used - local port is always set to 0 */
int32_t com_bind_ip_modem(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen);

int32_t com_connect_ip_modem(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen);

int32_t com_send_ip_modem(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags);

int32_t com_recv_ip_modem(int32_t sock, com_char_t *buf, int32_t len, int32_t flags);

int32_t com_closesocket_ip_modem(int32_t sock);

/* Server and extra functionalities not yet implemented at low level */
int32_t com_listen_ip_modem(int32_t sock, int32_t backlog);
int32_t com_accept_ip_modem(int32_t sock, com_sockaddr_t *addr, int32_t *addrlen);
int32_t com_sendto_ip_modem(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags,
                            const com_sockaddr_t *to, int32_t tolen);
int32_t com_recvfrom_ip_modem(int32_t sock, com_char_t *buf, int32_t len, int32_t flags,
                              com_sockaddr_t *from, int32_t *fromlen);

int32_t com_getpeername_ip_modem(int32_t sock, com_sockaddr_t *name, int32_t *namelen);
int32_t com_getsockname_ip_modem(int32_t sock, com_sockaddr_t *name, int32_t *namelen);

int32_t com_gethostbyname_ip_modem(const com_char_t *name, com_sockaddr_t *addr);

#ifdef __cplusplus 
} 
#endif 

#endif /* __COM_SOCKETS_IP_MODEM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
