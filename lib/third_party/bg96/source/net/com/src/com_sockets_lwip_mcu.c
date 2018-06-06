/**
  ******************************************************************************
  * @file    Application\net\com\src\com_sockets_lwip_mcu.c
  * @author  MCD Application Team
  * @brief   This file implements Socket LwIP on MCU side
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#include "com_sockets_lwip_mcu.h"

#if (USE_SOCKETS_MODEM == 0)

#if (USE_IAC_THROUGH_SOCKET == 1)
  #include "lwip\tcpip.h"
#else
  #include "lwip\init.h"
#endif

#include "lwip\sockets.h"
#include "lwip\api.h" /* for netconn_gethostbyname */

int32_t com_socket_lwip_mcu(int32_t family, int32_t type, int32_t protocol)
{
  return lwip_socket(family, type, protocol);
}

int32_t com_setsockopt_lwip_mcu(int32_t sock, int32_t level, int32_t optname, const void *optval, int32_t optlen)
{
  return lwip_setsockopt(sock, level, optname, optval, optlen);
}

int32_t com_getsockopt_lwip_mcu(int32_t sock, int32_t level, int32_t optname, void *optval, int32_t *optlen)
{
  return lwip_getsockopt(sock, level, optname, optval, optlen);
}

int32_t com_bind_lwip_mcu(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen)
{
  return lwip_bind(sock, (struct sockaddr *)addr, addrlen);
}

int32_t com_connect_lwip_mcu(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen)
{
  return lwip_connect(sock, (struct sockaddr *)addr, addrlen);
}

int32_t com_send_lwip_mcu(int32_t sock, const com_char_t *buf, size_t len, int32_t flags)
{
  return lwip_send(sock, buf, len, flags);
}

int32_t com_recv_lwip_mcu(int32_t sock, com_char_t *buf, size_t len, int32_t flags)
{
  return lwip_recv(sock, buf, len, flags);
}

int32_t com_closesocket_lwip_mcu(int32_t sock)
{
  return lwip_close(sock);
}

int32_t com_listen_lwip_mcu(int32_t sock, int32_t backlog)
{
  return lwip_listen(sock, backlog);
}

int32_t com_accept_lwip_mcu(int32_t sock, com_sockaddr_t *addr, int32_t *addrlen)
{
  return lwip_accept(sock, (struct sockaddr *)addr, addrlen);
}

int32_t com_sendto_lwip_mcu(int32_t sock, const com_char_t *buf, size_t len, int32_t flags,
                            const com_sockaddr_t *to, int32_t tolen)
{
  return lwip_sendto(sock, buf, len, flags, (struct sockaddr *)to, tolen);
}

int32_t com_recvfrom_lwip_mcu(int32_t sock, com_char_t *buf, size_t len, int32_t flags,
                              com_sockaddr_t *from, int32_t *fromlen)
{
  return lwip_recvfrom(sock, buf, len, flags, (struct sockaddr *)from, fromlen);
}

int32_t com_getpeername_lwip_mcu(int32_t sock, com_sockaddr_t *name, int32_t *namelen)
{
  return lwip_getpeername(sock, (struct sockaddr *)name, namelen);
}

int32_t com_getsockname_lwip_mcu(int32_t sock, com_sockaddr_t *name, int32_t *namelen)
{
  return lwip_getsockname(sock, (struct sockaddr *)name, namelen);
}

int32_t com_gethostbyname_lwip_mcu(const com_char_t *name, com_sockaddr_t *addr)
{
  int32_t result;
  com_ip_addr_t ip_addr;

  result = netconn_gethostbyname((const char *)name, &ip_addr);
  if (result == 0)
  {
    memset(addr, 0, sizeof(com_sockaddr_t));
    addr->sa_family = AF_UNSPEC;
    addr->sa_len    = sizeof(com_sockaddr_in_t);
    ((com_sockaddr_in_t *)addr)->sin_port = 0;
    ((com_sockaddr_in_t *)addr)->sin_addr.s_addr = ip_addr.addr;
  }

  return result;
}

bool com_init_lwip_mcu(void)
{
#if (USE_IAC_THROUGH_SOCKET == 1)
  tcpip_init(NULL, NULL);
#else
  lwip_init();
#endif
  return true;
}

void com_start_lwip_mcu(void)
{
}

#endif /* USE_SOCKETS_MODEM */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
