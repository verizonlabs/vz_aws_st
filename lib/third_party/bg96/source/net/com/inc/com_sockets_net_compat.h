/**
  ******************************************************************************
  * @file    Application\net\com\inc\com_sockets_net_compat.h
  * @author  MCD Application Team
  * @brief   This file defines Communication Socket Net Compatibility Interface
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_SOCKETS_NET_COMPAT_H__
#define __COM_SOCKETS_NET_COMPAT_H__ 1

#ifdef __cplusplus 
extern "C" { 
#endif 

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

/* Exported constants --------------------------------------------------------*/
#if (USE_SOCKETS_MODEM == 1)
#define COM_INADDR_ANY          ((uint32_t)0x00000000UL) /* All IP adresses accepted */

/* Socket Familly */
#define COM_AF_UNSPEC       0 /* Unspecified */
#define COM_AF_INET         1 /* Internet Address Family */
#define COM_AF_INET6        3 /* Internet Address Family version 6 */

/* Socket Type TCP/UDP/RAW */
#define COM_SOCK_STREAM     1 /* Supported */
#define COM_SOCK_DGRAM      2 /* Not supported */
#define COM_SOCK_RAW        3 /* Not supported */

/* Socket Protocol */
#define COM_IPPROTO_IP      0  /* IPv4 Level   */
#define COM_IPPROTO_ICMP    1  
#define COM_IPPROTO_UDP     2  /* UDP Protocol */
#define COM_IPPROTO_IPV6    3  /* IPv6 Level   */
#define COM_IPPROTO_TCP     6  /* TCP Protocol */
  
/*
 * Socket Options
 */
/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define COM_SOL_SOCKET     0xfff  /* options for socket level */

#define COM_SO_SNDTIMEO    0x1005 /* send timeout */
#define COM_SO_RCVTIMEO    0x1006 /* receive timeout */
#define COM_SO_ERROR       0x1007 /* get error status and clear */

/* Flags used with recv. */
#define COM_MSG_WAIT       0x00    /* Blocking    */
#define COM_MSG_DONTWAIT   0x01    /* Nonblocking */
  
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#else
#include "lwip/sockets.h"
  
#define COM_INADDR_ANY      INADDR_ANY

/* Socket Familly */
#define COM_AF_UNSPEC       AF_UNSPEC /* Unspecified */
#define COM_AF_INET         AF_INET   /* Internet Address Family */
#define COM_AF_INET6        AF_INET6  /* Internet Address Family version 6 */

/* Socket Type TCP/UDP/RAW */
#define COM_SOCK_STREAM     SOCK_STREAM
#define COM_SOCK_DGRAM      SOCK_DGRAM
#define COM_SOCK_RAW        SOCK_RAW

/* Socket Protocol */
#define COM_IPPROTO_IP      IPPROTO_IP
#define COM_IPPROTO_ICMP    IPPROTO_ICMP
#define COM_IPPROTO_UDP     IPPROTO_UDP
#define COM_IPPROTO_IPV6    IPPROTO_IPV6
#define COM_IPPROTO_TCP     IPPROTO_TCP

/*
 * Socket Options
 */
/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define COM_SOL_SOCKET     SOL_SOCKET  /* options for socket level */

#define COM_SO_SNDTIMEO    SO_SNDTIMEO /* send timeout */
#define COM_SO_RCVTIMEO    SO_RCVTIMEO /* receive timeout */
#define COM_SO_ERROR       SO_ERROR    /* get error status and clear */

/* Flags used with recv. */
#define COM_MSG_WAIT       MSG_WAIT      /* Blocking    */
#define COM_MSG_DONTWAIT   MSG_DONTWAIT  /* Nonblocking */
#endif
  
#ifdef __cplusplus 
} 
#endif 

#endif /* __COM_SOCKETS_NET_COMPAT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

