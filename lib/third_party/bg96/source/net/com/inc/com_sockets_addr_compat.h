/**
  ******************************************************************************
  * @file    Application\net\com\inc\com_sockets_addr_compat.h
  * @author  MCD Application Team
  * @brief   This file defines Communication Socket Address Compatibility Interface
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_SOCKETS_ADDR_COMPAT_H__
#define __COM_SOCKETS_ADDR_COMPAT_H__ 1

#ifdef __cplusplus 
extern "C" { 
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "string.h"
#include <stdlib.h>

#include "plf_config.h"

#if (USE_SOCKETS_MODEM == 1)

/* Exported constants --------------------------------------------------------*/
  
/* Exported types ------------------------------------------------------------*/
typedef uint32_t com_addr_t;

typedef struct {
  com_addr_t s_addr;         
} com_in_addr_t;

typedef struct {
  com_addr_t addr;
} com_ip4_addr_t ;

/* IF IPv4 and IPv6 has to be managed */
#if 0
enum com_ip_addr_type {
  /* IPv4 */
  COM_IPADDR_TYPE_V4 =   0U,
  /* IPv6 */
  COM_IPADDR_TYPE_V6 =   6U,
  /* IPv4+IPv6 ("dual-stack") */
  COM_IPADDR_TYPE_ANY = 46U
};

typedef struct {
  com_addr_t addr[4];
} com_ip6_addr_t;

typedef struct  {
  union {
    com_ip6_addr_t ip6;
    com_ip4_addr_t ip4;
  } u_addr;
  com_ip_addr_type type;
} com_ip_addr_t;
#endif

typedef com_ip4_addr_t com_ip_addr_t;

typedef struct  {
  uint8_t  sa_len;
  uint8_t  sa_family;
  char     sa_data[14];
} com_sockaddr_t;

/* IPv4 Socket Address structure */
typedef struct
{
  uint8_t       sin_len;		
  uint8_t       sin_family;		
  uint16_t      sin_port;
  com_in_addr_t sin_addr;
  char          sin_zero[8];
} com_sockaddr_in_t;

typedef uint8_t com_char_t;

/* Exported macro ------------------------------------------------------------*/
#define COM_SOCKETS_LITTLE_ENDIAN (0)
#define COM_SOCKETS_BIG_ENDIAN    (1)

#ifndef COM_SOCKETS_BYTE_ORDER
#define COM_SOCKETS_BYTE_ORDER COM_SOCKETS_LITTLE_ENDIAN
#endif 

#if (COM_SOCKETS_BYTE_ORDER == COM_SOCKETS_BIG_ENDIAN)
#define com_htonl(x)   (x)
#define com_ntohl(x)   (x)
#define com_htons(x)   (x)
#define com_ntohs(x)   (x)
#else /* COM_SOCKETS_BYTE_ORDER != COM_SOCKETS_BIG_ENDIAN */
#define com_htonl(x)   ((((uint32_t)(x) & 0xff000000) >> 24) | \
                        (((uint32_t)(x) & 0x00ff0000) >> 8) | \
                        (((uint32_t)(x) & 0x0000ff00) << 8) | \
                        (((uint32_t)(x) & 0x000000ff) << 24))
                 
#define com_ntohl(x)    com_htonl(x)

#define com_htons(x)    ((((unsigned long)(x) & 0xff00) >> 8) | \
                         (((unsigned long)(x) & 0x00ff) << 8))

#define com_ntohs(x)    com_htons(x)
#endif /* COM_SOCKETS_BYTE_ORDER */

/* Exported functions ------------------------------------------------------- */
#else

#include "lwip\sockets.h"
#if (USE_IAC_THROUGH_PCB == 1)
#include "lwip\udp.h"
#endif

typedef ip_addr_t com_ip_addr_t;

typedef in_addr_t com_in_addr_t;

typedef struct sockaddr com_sockaddr_t;
typedef struct sockaddr_in com_sockaddr_in_t;

typedef uint8_t com_char_t;

/* Exported macro ------------------------------------------------------------*/
#define com_htonl(A)   htonl((A))

#define com_ntohl(A)   ntonl((A))

#define com_htons(A)   htons((A))
#define com_ntohs(A)   htons((A))

#endif

#ifdef __cplusplus 
} 
#endif 

#endif /* __COM_SOCKETS_ADDR_COMPAT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
