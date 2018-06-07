/**
  ******************************************************************************
  * @file    Application\net\com\src\com_sockets.c
  * @author  MCD Application Team
  * @brief   This file implements Communication Socket
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"
#include "com_sockets.h"

#if (USE_SOCKETS_MODEM == 1)
#include "com_sockets_ip_modem.h"
#else
#include "com_sockets_lwip_mcu.h"
#endif

#include "cellular_service_os.h"

int32_t com_socket(int32_t family, int32_t type, int32_t protocol)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_socket_ip_modem(family, type, protocol);
#else
  return com_socket_lwip_mcu(family, type, protocol);
#endif
}

int32_t com_setsockopt(int32_t sock, int32_t level, int32_t optname, const void *optval, int32_t optlen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_setsockopt_ip_modem(sock, level, optname, optval, optlen);
#else
  return com_setsockopt_lwip_mcu(sock, level, optname, optval, optlen);
#endif
}

int32_t com_getsockopt(int32_t sock, int32_t level, int32_t optname, void *optval, int32_t *optlen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_getsockopt_ip_modem(sock, level, optname, optval, optlen);
#else
  return com_getsockopt_lwip_mcu(sock, level, optname, optval, optlen);
#endif
}

int32_t com_bind(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_bind_ip_modem(sock, addr, addrlen);
#else
  return com_bind_lwip_mcu(sock, addr, addrlen);
#endif
}

int32_t com_connect(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_connect_ip_modem(sock, addr, addrlen);
#else
  return com_connect_lwip_mcu(sock, addr, addrlen);
#endif
}

int32_t com_send(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_send_ip_modem(sock, buf, len, flags);
#else
  return com_send_lwip_mcu(sock, buf, len, flags);
#endif
}

int32_t com_recv(int32_t sock, com_char_t *buf, int32_t len, int32_t flags)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_recv_ip_modem(sock, buf, len, flags);
#else
  return com_recv_lwip_mcu(sock, buf, len, flags);
#endif
}

int32_t com_closesocket(int32_t sock)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_closesocket_ip_modem(sock);
#else
  return com_closesocket_lwip_mcu(sock);
#endif
}

int32_t com_listen(int32_t sock, int32_t backlog)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_listen_ip_modem(sock, backlog);
#else
  return com_listen_lwip_mcu(sock, backlog);
#endif
}

int32_t com_accept(int32_t sock, com_sockaddr_t *addr, int32_t *addrlen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_accept_ip_modem(sock, addr, addrlen);
#else
  return com_accept_lwip_mcu(sock, addr, addrlen);
#endif
}

int32_t com_sendto(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags,
                   const com_sockaddr_t *to, int32_t tolen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_sendto_ip_modem(sock, buf, len, flags, to, tolen);
#else
  return com_sendto_lwip_mcu(sock, buf, len, flags, to, tolen);
#endif
}

int32_t com_recvfrom(int32_t sock, com_char_t *buf, int32_t len, int32_t flags,
                     com_sockaddr_t *from, int32_t *fromlen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_recvfrom_ip_modem(sock, buf, len, flags, from, fromlen);
#else
  return com_recvfrom_lwip_mcu(sock, buf, len, flags, from, fromlen);
#endif
}

int32_t com_getpeername(int32_t sock, com_sockaddr_t *name, int32_t *namelen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_getpeername_ip_modem(sock, name, namelen);
#else
  return com_getpeername_lwip_mcu(sock, name, namelen);
#endif
}

int32_t com_getsockname(int32_t sock, com_sockaddr_t *name, int32_t *namelen)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_getsockname_ip_modem(sock, name, namelen);
#else
  return com_getsockname_lwip_mcu(sock, name, namelen);
#endif
}

int32_t com_gethostbyname(const com_char_t *name, com_sockaddr_t *addr)
{
#if (USE_SOCKETS_MODEM == 1)
  return com_gethostbyname_ip_modem(name, addr);
#else
  return com_gethostbyname_lwip_mcu(name, addr);
#endif
}

bool com_init(void)
{
  bool result;
    
  osCDS_cellular_service_init();
  
#if (USE_SOCKETS_MODEM == 1)
  result = com_init_ip_modem();
#else
  result = com_init_lwip_mcu();
#endif

  return result;
}

void com_start(void)
{
#if (USE_SOCKETS_MODEM == 1)
  com_start_ip_modem();
#else
  com_start_lwip_mcu();
#endif
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
