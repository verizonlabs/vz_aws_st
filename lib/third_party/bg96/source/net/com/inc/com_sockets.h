/**
  ******************************************************************************
  * @file    Application\net\com\inc\com_sockets.h
  * @author  MCD Application Team
  * @brief   This file defines Communication Socket Interface
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_SOCKETS_H__
#define __COM_SOCKETS_H__ 1

#ifdef __cplusplus 
extern "C" { 
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h> 
#include <stdbool.h> 

#include "com_sockets_addr_compat.h"
#include "com_sockets_net_compat.h"
#include "com_sockets_err_compat.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/* Com Init */
bool com_init(void);
void com_start(void);

int32_t com_socket(int32_t family, int32_t type, int32_t protocol);

int32_t com_setsockopt(int32_t sock, int32_t level, int32_t optname, const void *optval, int32_t optlen);
int32_t com_getsockopt(int32_t sock, int32_t level, int32_t optname, void *optval, int32_t *optlen);

int32_t com_bind(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen);
int32_t com_connect(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen);

int32_t com_send(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags);
int32_t com_recv(int32_t sock, com_char_t *buf, int32_t len, int32_t flags);

int32_t com_closesocket(int32_t sock);

int32_t com_listen(int32_t sock, int32_t backlog);
int32_t com_accept(int32_t sock, com_sockaddr_t *addr, int32_t *addrlen);

int32_t com_sendto(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags,
                   const com_sockaddr_t *to, int32_t tolen);
int32_t com_recvfrom(int32_t sock, com_char_t *buf, int32_t len, int32_t flags,
                     com_sockaddr_t *from, int32_t *fromlen);

int32_t com_getpeername(int32_t sock, com_sockaddr_t *name, int32_t *namelen);
int32_t com_getsockname(int32_t sock, com_sockaddr_t *name, int32_t *namelen);

int32_t com_gethostbyname(const com_char_t *name, com_sockaddr_t *addr);

#ifdef __cplusplus 
} 
#endif 

#endif /* __COM_SOCKETS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
