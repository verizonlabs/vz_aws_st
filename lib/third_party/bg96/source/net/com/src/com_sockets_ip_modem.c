/**
  ******************************************************************************
  * @file    ST/net/com/src/com_sockets_ip_modem.c
  * @author  MCD Application Team
  * @version V1.0
  * @date    28-Mar-2018
  * @brief   This file implements Socket IP on MODEM side
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


/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"
#include "com_sockets_err_compat.h"
#include "com_sockets_ip_modem.h"
#include "com_sockets_net_compat.h"
#include "cellular_service_os.h"
#include "com_sockets_ip_modem.h"

#include <string.h>


#if (USE_DATACACHE == 1)
#include "dc_service.h"
#include "dc_common.h"
#endif

#if (USE_TRACE_INTERFACE_COM_SOCKETS_IP == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_COM, DBL_LVL_P0, "COM: " format "\r\n", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_COM, DBL_LVL_P1, "COM: " format "\r\n", ## args)
#define PrintERR(format, args...)  TracePrint(DBG_CHAN_COM, DBL_LVL_ERR, "COM ERROR: " format "\r\n", ## args)
#define PrintSTAT(format, args...) TracePrint(DBG_CHAN_COM, DBL_LVL_P0, "" format "\r\n", ## args)
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("COM: " format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("COM: " format "\r\n", ## args)
#define PrintERR(format, args...)   printf("COM ERROR: " format "\r\n", ## args)
#define PrintSTAT(format, args...)  printf("" format "\r\n", ## args)
#elif (USE_CONFIGPRINTF_COM_SOCKETS_IP == 1)
#define PrintINFO(format, args...)  vLoggingPrintf("COM:" format "\r\n",## args)
#define PrintDBG(format, args...)   vLoggingPrintf("COM:" format "\r\n",## args)
#define PrintERR(format, args...)   vLoggingPrintf("COM ERROR:" format "\r\n",## args)
#define PrintSTAT(format, args...)  vLoggingPrintf("" format "\r\n",## args)
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#define PrintERR(format, args...)   do {} while(0)
#define PrintSTAT(format, args...)  do {} while(0)
#endif

/* Need a function at low level to obtain these values according to the modem */
#define MODEM_MAX_TX_DATA_SIZE ((uint32_t)1460)
#define MODEM_MAX_RX_DATA_SIZE ((uint32_t)1500)

#define COM_MIN(a,b) ((a)<(b) ? (a) : (b))

typedef enum
{
  SOCKET_DATA_RCV   = 0,
  SOCKET_CLOSING_RQT
} socket_msg_type_t;

typedef struct
{
  socket_msg_type_t type;
} socket_msg_t;

typedef enum {
  SOCKET_INVALID = 0,
  SOCKET_CREATING,
  SOCKET_CREATED,
  SOCKET_CONNECTED,
  SOCKET_SENDING,
  SOCKET_WAITING_RSP,
  SOCKET_CLOSING
} socket_state_t;

#define COM_SOCKET_INVALID_ID (-1)

#define COM_SOCKET_STATISTIC (0)

typedef enum {
  COM_SOCKET_STAT_INIT =0,
#if (USE_DATACACHE == 1)
  COM_SOCKET_STAT_NWK_UP,
  COM_SOCKET_STAT_NWK_DWN,
#endif
  COM_SOCKET_STAT_CRE_OK,
  COM_SOCKET_STAT_CRE_NOK,
  COM_SOCKET_STAT_CNT_OK,
  COM_SOCKET_STAT_CNT_NOK,
  COM_SOCKET_STAT_SND_OK,
  COM_SOCKET_STAT_SND_NOK,
  COM_SOCKET_STAT_RCV_OK,
  COM_SOCKET_STAT_RCV_NOK,
  COM_SOCKET_STAT_CLS_OK,
  COM_SOCKET_STAT_CLS_NOK,
  COM_SOCKET_STAT_PRINT
} com_socket_stat_t;

#if (COM_SOCKET_STATISTIC == 1)
#define COM_SOCKET_STAT_POLLING_PERIOD 10000
static osTimerId com_socket_stat_timer_handle;
static void com_socket_stat_timer_cb(void const * argument);

static const uint8_t* socket_state_string[] = {
  "Invalid",
  "Creating",
  "Created",
  "Connected",
  "Sending",
  "Receiving",
  "Closing"
};

typedef struct
{
#if (USE_DATACACHE == 1)
  uint16_t nwk_up;
  uint16_t nwk_dwn;
#endif
  uint16_t sock_cre_ok;
  uint16_t sock_cre_nok;
  uint16_t sock_cnt_ok;
  uint16_t sock_cnt_nok;
  uint16_t sock_snd_ok;
  uint16_t sock_snd_nok;
  uint16_t sock_rcv_ok;
  uint16_t sock_rcv_nok;
  uint16_t sock_cls_ok;
  uint16_t sock_cls_nok;
} com_socket_statistic_t;

static com_socket_statistic_t com_socket_statistic;
#endif

typedef struct socket_desc_t {
  socket_state_t       state;
  bool                 closing;
  int8_t               error;
  int32_t              id;
  uint16_t             port;
  uint32_t             snd_timeout;
  uint32_t             rcv_timeout;
  osMessageQId         queue;
  struct socket_desc_t *next;
} socket_desc_t;

socket_desc_t *socket_desc_list;
static osMutexId ComSocketsMutexHandle;

typedef struct {
  CS_IPaddrType_t ip_type;
  /* In cellular_service socket_configure_remote()
     memcpy from char *addr to addr[] without knowing the length
     and by using strlen(char *addr) so ip_value must contain /0 */
  /* IPv4 : xxx.xxx.xxx.xxx=15+/0*/
  /* IPv6 : xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx = 39+/0*/
  uint8_t         ip_value[40];
  uint16_t        port;
} socket_addr_t;

#if (USE_DATACACHE == 1)
static bool network_is_up;
#endif

/* Use google as primary DNS service */
const com_char_t com_primary_dns_addr_str[]= "8.8.8.8";

#define SOCKET_SET_ERROR(socket, val) do {\
  if ((socket) != NULL) {\
    (socket)->error = (val); }\
  } while(0)

#define SOCKET_GET_ERROR(socket, val) do {\
  if ((socket) != NULL) {\
    (*(int32_t *)(val)) = (((socket)->error) == COM_SOCKETS_ERR_OK ? \
                                                (int32_t)COM_SOCKETS_ERR_OK : \
                                                com_sockets_err_to_errno((com_sockets_err_t)((socket)->error))); }\
  else {\
    (*(int32_t *)(val)) = com_sockets_err_to_errno(COM_SOCKETS_ERR_DESCRIPTOR); }\
  } while(0)


#if (COM_SOCKET_STATISTIC == 1)
static void com_socket_stat_update(com_socket_stat_t stat)
{
  switch(stat)
  {
#if (USE_DATACACHE == 1)
    case COM_SOCKET_STAT_NWK_UP:
    {
      com_socket_statistic.nwk_up++;
      break;
    }
    case COM_SOCKET_STAT_NWK_DWN:
    {
      com_socket_statistic.nwk_dwn++;
      break;
    }
#endif
    case COM_SOCKET_STAT_CRE_OK:
    {
      com_socket_statistic.sock_cre_ok++;
      break;
    }
    case COM_SOCKET_STAT_CRE_NOK:
    {
      com_socket_statistic.sock_cre_nok++;
      break;
    }
    case COM_SOCKET_STAT_CNT_OK:
    {
      com_socket_statistic.sock_cnt_ok++;
      break;
    }
    case COM_SOCKET_STAT_CNT_NOK:
    {
      com_socket_statistic.sock_cnt_nok++;
      break;
    }
    case COM_SOCKET_STAT_SND_OK:
    {
      com_socket_statistic.sock_snd_ok++;
      break;
    }
    case COM_SOCKET_STAT_SND_NOK:
    {
      com_socket_statistic.sock_snd_nok++;
      break;
    }
    case COM_SOCKET_STAT_RCV_OK:
    {
      com_socket_statistic.sock_rcv_ok++;
      break;
    }
    case COM_SOCKET_STAT_RCV_NOK:
    {
      com_socket_statistic.sock_rcv_nok++;
      break;
    }
    case COM_SOCKET_STAT_CLS_OK:
    {
      com_socket_statistic.sock_cls_ok++;
      break;
    }
    case COM_SOCKET_STAT_CLS_NOK:
    {
      com_socket_statistic.sock_cls_nok++;
      break;
    }
    case COM_SOCKET_STAT_INIT:
    {
      memset(&com_socket_statistic, 0, sizeof(com_socket_statistic_t));
      if(COM_SOCKET_STAT_POLLING_PERIOD != 0)
      {
        osTimerDef(com_socket_stat_timer, com_socket_stat_timer_cb);
        com_socket_stat_timer_handle = osTimerCreate(osTimer(com_socket_stat_timer),
                                                      osTimerPeriodic, NULL);
        osTimerStart(com_socket_stat_timer_handle, COM_SOCKET_STAT_POLLING_PERIOD);
      }
      break;
    }
    case COM_SOCKET_STAT_PRINT:
    {
      socket_desc_t *socket_desc;
      socket_desc = socket_desc_list;

      /* Check that at least one socket has run */
      if (com_socket_statistic.sock_cre_ok != 0)
      {
        PrintSTAT("*** Begin Socket Statistic ***");
#if (USE_DATACACHE == 1)
        dc_time_date_rt_info_t dc_time_date_rt_info;

        (void)dc_srv_get_time_date(&dc_time_date_rt_info,
                                   DC_DATE_AND_TIME);

        PrintSTAT("%02d/%02d/%04d - %02d:%02d:%02d",
                  dc_time_date_rt_info.mday,
                  dc_time_date_rt_info.month,
                  dc_time_date_rt_info.year,
                  dc_time_date_rt_info.hour,
                  dc_time_date_rt_info.min,
                  dc_time_date_rt_info.sec);

        PrintSTAT("Network: up: %5d dwn:  %5d tot: %6d",
                  com_socket_statistic.nwk_up,
                  com_socket_statistic.nwk_dwn,
                  (com_socket_statistic.nwk_up+com_socket_statistic.nwk_dwn));
#endif
        PrintSTAT("Create : ok: %5d nok:  %5d tot: %6d",
                  com_socket_statistic.sock_cre_ok,
                  com_socket_statistic.sock_cre_nok,
                  (com_socket_statistic.sock_cre_ok+com_socket_statistic.sock_cre_nok));
        PrintSTAT("Connect: ok: %5d nok:  %5d tot: %6d",
                  com_socket_statistic.sock_cnt_ok,
                  com_socket_statistic.sock_cnt_nok,
                  (com_socket_statistic.sock_cnt_ok+com_socket_statistic.sock_cnt_nok));
        PrintSTAT("Send   : ok: %5d nok:  %5d tot: %6d",
                  com_socket_statistic.sock_snd_ok,
                  com_socket_statistic.sock_snd_nok,
                  (com_socket_statistic.sock_snd_ok+com_socket_statistic.sock_snd_nok));
        PrintSTAT("Recv   : ok: %5d nok:  %5d tot: %6d",
                  com_socket_statistic.sock_rcv_ok,
                  com_socket_statistic.sock_rcv_nok,
                  (com_socket_statistic.sock_rcv_ok+com_socket_statistic.sock_rcv_nok));
        PrintSTAT("Close  : ok: %5d nok:  %5d tot: %6d",
                  com_socket_statistic.sock_cls_ok,
                  com_socket_statistic.sock_cls_nok,
                  (com_socket_statistic.sock_cls_ok+com_socket_statistic.sock_cls_nok));
        while (socket_desc != NULL)
        {
          PrintSTAT("Sock Id: %2d - State: %s - Status: %d",
                    socket_desc->id,
                    socket_state_string[socket_desc->state],
                    socket_desc->error);
          socket_desc = socket_desc->next;
        }
        PrintSTAT("*** End   Socket Statistic ***");
      }
      break;
    }
    default:
      break;
  }
}

/* osTimer callback function */
static void com_socket_stat_timer_cb(void const * argument)
{
  com_socket_stat_update(COM_SOCKET_STAT_PRINT);
}

#else
static void com_socket_stat_update(com_socket_stat_t socket_stat)
{
}
#endif

static void com_ip_modem_init_socket_desc(socket_desc_t *socket_desc)
{
  socket_desc->state       = SOCKET_INVALID;
  socket_desc->closing     = false;
  socket_desc->id          = COM_SOCKET_INVALID_ID;
  socket_desc->port        = 0;
  socket_desc->rcv_timeout = osWaitForever;
  socket_desc->snd_timeout = osWaitForever;
  socket_desc->error       = COM_SOCKETS_ERR_OK;
}

static socket_desc_t *com_ip_modem_create_socket_desc(void)
{
  socket_desc_t * socket_desc;

  socket_desc = (socket_desc_t *)pvPortMalloc(sizeof(socket_desc_t));
  if (socket_desc != NULL)
  {
    osMessageQDef(MODEM_QUEUE, 2, uint32_t);
    socket_desc->queue = osMessageCreate(osMessageQ(MODEM_QUEUE), NULL);
    if (socket_desc->queue == NULL)
    {
      free(socket_desc);
    }
    else
    {
      com_ip_modem_init_socket_desc(socket_desc);
    }
  }

  return socket_desc;
}

static socket_desc_t *com_ip_modem_provide_socket_desc(void)
{
  osMutexWait(ComSocketsMutexHandle, osWaitForever);

  socket_desc_t *socket_desc;

  socket_desc = socket_desc_list;

  /* Need to create a new socket_desc ? */
  while (   (socket_desc->state != SOCKET_INVALID)
         && (socket_desc->next  != NULL))
  {
    socket_desc = socket_desc->next;
  }
  /* Find an empty socket ? */
  if (socket_desc->state != SOCKET_INVALID)
  {
    /* No empty socket - create new one */
    socket_desc = com_ip_modem_create_socket_desc();
    if (socket_desc != NULL)
    {
      socket_desc->state = SOCKET_CREATING;
      socket_desc->next = socket_desc_list;
      socket_desc_list  = socket_desc;
    }
  }
  else
  {
    /* Find an empty place */
    socket_desc->state = SOCKET_CREATING;
  }

  osMutexRelease(ComSocketsMutexHandle);

  return socket_desc;
}

static void com_ip_modem_delete_socket_desc(int32_t sock)
{
  osMutexWait(ComSocketsMutexHandle, osWaitForever);

  bool found;
  socket_desc_t *socket_desc;
  socket_desc_t *socket_desc_previous;

  found = false;

  socket_desc = socket_desc_list;
  socket_desc_previous = NULL;

  while (   (socket_desc != NULL)
         && (found != true))
  {
    if (socket_desc->id == sock)
    {
      found = true;
    }
    else
    {
      socket_desc_previous = socket_desc;
      socket_desc = socket_desc->next;
    }
  }
  if (found == true)
  {
    /* Always keep at least one socket */
    if (socket_desc->next == NULL)
    {
      com_ip_modem_init_socket_desc(socket_desc);
    }
    else
    {
      if (socket_desc_previous != NULL)
      {
        socket_desc_previous->next = socket_desc->next;
      }
      else
      {
        socket_desc_list = socket_desc->next;
      }
      osMessageDelete(socket_desc->queue);
      free(socket_desc);
    }
  }

  osMutexRelease(ComSocketsMutexHandle);
}

static socket_desc_t *com_ip_modem_find_socket(int32_t sock)
{
  socket_desc_t *socket_desc;
  bool found;

  socket_desc = socket_desc_list;
  found = false;

  while (   (socket_desc != NULL)
         && (found != true))
  {
    if (socket_desc->id == sock)
    {
      found = true;
    }
    else
    {
      socket_desc = socket_desc->next;
    }
  }

  return socket_desc;
}

static bool com_translate_ip_address(const com_sockaddr_t *addr,
                                     int32_t              addrlen,
                                     socket_addr_t        *socket_addr)
{
  bool result;

  result = false;

  if (   (addr != NULL)
      && (socket_addr != NULL))
  {
    if (addr->sa_family == COM_AF_INET)
    {
      socket_addr->ip_type = CS_IPAT_IPV4;
      if (((com_sockaddr_in_t *)addr)->sin_addr.s_addr == COM_INADDR_ANY)
      {
        memcpy(&socket_addr->ip_value[0],"0.0.0.0", strlen("0.0.0.0"));
      }
      else
      {
        uint32_t IPaddress;
        uint8_t  iptab[4];

        IPaddress = ((com_sockaddr_in_t *)addr)->sin_addr.s_addr;
        iptab[0] = (uint8_t)(IPaddress >> 24);
        iptab[1] = (uint8_t)(IPaddress >> 16);
        iptab[2] = (uint8_t)(IPaddress >> 8);
        iptab[3] = (uint8_t)(IPaddress);
        sprintf((char *)&socket_addr->ip_value[0],"%d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);
      }
      socket_addr->port = com_ntohs(((com_sockaddr_in_t *)addr)->sin_port);
      result = true;
    }
    else if (addr->sa_family == COM_AF_INET6)
    {
      /* To do support AF_INET6 */
    }
  }

  return result;
}

static bool com_convert_IPString_to_sockaddr(const com_char_t *ipaddr_str,
                                             com_sockaddr_t   *sockaddr)
{
  bool result;
  int ip_addr[4];
  uint8_t count;
  uint8_t start;

  start = (ipaddr_str[0] == '"') ? 1 : 0;

  count = sscanf((char *)(&ipaddr_str[start]), "%3d.%3d.%3d.%3d",
                                               &ip_addr[0], &ip_addr[1],
                                               &ip_addr[2], &ip_addr[3]);

  result = false;
  if (count == 4)
  {
    if (   (ip_addr[0] <= 255)
        && (ip_addr[1] <= 255)
        && (ip_addr[2] <= 255)
        && (ip_addr[3] <= 255))
    {
      memset(sockaddr, 0, sizeof(com_sockaddr_t));
      sockaddr->sa_family = COM_AF_INET;
      sockaddr->sa_len    = sizeof(com_sockaddr_in_t);
      ((com_sockaddr_in_t *)sockaddr)->sin_port = 0;
      ((com_sockaddr_in_t *)sockaddr)->sin_addr.s_addr =  (ip_addr[3] << 24)
                                                        + (ip_addr[2] << 16)
                                                        + (ip_addr[1] << 8)
                                                        + (ip_addr[0]);
      result = true;
    }
  }

  return(result);
}

static bool com_ip_modem_is_network_up(void)
{
  bool result;

#if (USE_DATACACHE == 1)
  result = network_is_up;
#else
  /* Feature not supported without Datacache
     Do not block -=> consider network is up */
  result = true;
#endif

  return result;
}

/**
  * @brief Socket data ready callback
  */
static void com_ip_modem_data_ready_cb(socket_handle_t sock)
{
  PrintDBG("callback socket data ready called");

  socket_desc_t *socket_desc;
  socket_msg_t msg;

  socket_desc = com_ip_modem_find_socket(sock);

  if (   (socket_desc != NULL)
      && (socket_desc->closing == false))
//      && (socket_desc->state   == SOCKET_WAITING_RSP))
  {
    msg.type = SOCKET_DATA_RCV;
    PrintINFO("callback socket data ready called: data ready");
    osMessagePut(socket_desc->queue, *(uint32_t*)&msg, 0);
  }
  else
  {
    PrintINFO("!!! PURGE callback socket data ready !!!");
    if (socket_desc == NULL)
    {
      PrintERR("callback socket data ready called: unknown socket");
    }
    if (socket_desc->closing == true)
    {
      PrintERR("callback socket data ready called: socket is closing");
    }
    if (socket_desc->state != SOCKET_WAITING_RSP)
    {
      PrintERR("callback socket data ready called: socket_state:%i NOK", socket_desc->state);
    }
  }
}

/**
  * @brief Socket closing callback
  */
static void com_ip_modem_closing_cb(socket_handle_t sock)
{
  PrintDBG("callback socket closing called");

  socket_desc_t *socket_desc;
  socket_msg_t msg;

  socket_desc = com_ip_modem_find_socket(sock);
  if (socket_desc != NULL)
  {
    PrintINFO("callback socket closing called: close rqt");
    if (socket_desc->closing == false)
    {
      socket_desc->closing = true;
      PrintINFO("callback socket closing: close rqt");
    }
    if (socket_desc->state == SOCKET_WAITING_RSP)
    {
      PrintERR("!!! callback socket closing called: data_expected !!!");
      msg.type = SOCKET_CLOSING_RQT;
      osMessagePut(socket_desc->queue, *(uint32_t*)&msg, 0);
    }
  }
  else
  {
    PrintERR("callback socket closing called: unknown socket");
  }
}

#if (USE_DATACACHE == 1)
/**
  * @brief Socket datacache callback
  */
static void com_socket_datacache_cb(dc_com_event_id_t dc_event_id, void* private_gui_data)
{
  switch (dc_event_id)
  {
    case DC_COM_NIFMAN_INFO:
    {
      dc_nifman_rt_info_t  dc_nifman_rt_info;

      dc_com_read(&dc_com_db, DC_COM_NIFMAN_INFO,
                  (void *)&dc_nifman_rt_info, sizeof(dc_nifman_rt_info));

      if (dc_nifman_rt_info.rt_state == DC_SERVICE_ON)
      {
        network_is_up = true;
        com_socket_stat_update(COM_SOCKET_STAT_NWK_UP);
      }
      else
      {
        if (network_is_up == true)
        {
          network_is_up = false;
          com_socket_stat_update(COM_SOCKET_STAT_NWK_DWN);
        }
      }
      break;
    }
    default:
      break;
  }
}
#endif

int32_t com_socket_ip_modem(int32_t family, int32_t type, int32_t protocol)
{
  int32_t sock;
  /* result is either the socket number assigned or an error */
  int32_t result;

  CS_IPaddrType_t IPaddrType;
  CS_TransportProtocol_t TransportProtocol;
  CS_PDN_conf_id_t PDN_conf_id;

  /* These values will be updated further along. If sock is different from COM_SOCKET_INVALID_ID
   * the software will assign that value to result (e.g a valid socket number),
   * otherwise result will return an error */

  result = COM_SOCKETS_ERR_OK;
  sock = COM_SOCKET_INVALID_ID;

  if (family == COM_AF_INET)
  {
    /* address family IPv4 */
    IPaddrType = CS_IPAT_IPV4;
  }
  else if (family == COM_AF_INET6)
  {
    /* address family IPv6 */
    IPaddrType = CS_IPAT_IPV6;
    result = COM_SOCKETS_ERR_PARAMETER;
  }
  else
  {
    result = COM_SOCKETS_ERR_PARAMETER;
  }

  if (   (type == COM_SOCK_STREAM)
      && (   (protocol == COM_IPPROTO_TCP)
          || (protocol == COM_IPPROTO_IP)))
  {
    /* IPPROTO_TCP = must be used with SOCK_STREAM */
    TransportProtocol = CS_TCP_PROTOCOL;
  }
  else if (   (type == COM_SOCK_DGRAM)
           && (   (protocol == COM_IPPROTO_UDP)
               || (protocol == COM_IPPROTO_IP)))
  {
    /* IPPROTO_UDP = must be used with SOCK_DGRAM */
    TransportProtocol = CS_UDP_PROTOCOL;
    result = COM_SOCKETS_ERR_PARAMETER;
  }
  else
  {
    result = COM_SOCKETS_ERR_PARAMETER;
  }

  PDN_conf_id = CS_PDN_CONFIG_DEFAULT;

  if (result == COM_SOCKETS_ERR_OK)
  {
    result = COM_SOCKETS_ERR_GENERAL;
    PrintDBG("socket create request");

    sock = osCDS_socket_create(IPaddrType,
                               TransportProtocol,
                               0, /* the modem assigns local port automatically */
                               PDN_conf_id);

    if (sock != COM_SOCKET_INVALID_ID)
    {
      socket_desc_t *socket_desc;
      PrintINFO("create socket ok low level");

      /* Need to create a new socket_desc ? */
      socket_desc = com_ip_modem_provide_socket_desc();
      if (socket_desc == NULL)
      {
        result = COM_SOCKETS_ERR_NOMEMORY;
        PrintERR("create socket NOK no memory");
        /* Socket descriptor is not existing in COM
           must close directly the socket */
        if (   osCDS_socket_close(sock, 0)
            == CELLULAR_OK)
        {
          PrintINFO("close socket ok low level");
        }
        else
        {
          PrintERR("close socket NOK low level");
        }
      }
      else
      {
        socket_desc->id    = sock;
        socket_desc->state = SOCKET_CREATED;

        if (   osCDS_socket_set_callbacks(sock,
                                          com_ip_modem_data_ready_cb,
                                          NULL,
                                          com_ip_modem_closing_cb)
            == CELLULAR_OK)
        {
          /* returns the socket number that has been assigned */
          result = sock;
        }
        else
        {
          PrintERR("rqt close socket issue at creation");
          if (   com_closesocket_ip_modem(sock)
              == CELLULAR_OK)
          {
            PrintINFO("close socket ok low level");
          }
          else
          {
            PrintERR("close socket NOK low level");
          }
        }
      }
    }
    else
    {
      PrintERR("create socket NOK low level");
    }
    /* Stat only the socket whose parameters are supported */
    com_socket_stat_update((result == COM_SOCKETS_ERR_OK) ? COM_SOCKET_STAT_CRE_OK : COM_SOCKET_STAT_CRE_NOK);
  }
  else
  {
    PrintERR("create socket NOK parameter NOK");
  }

  /* if (result < 0 || sock < 0) socket has not been created no need to call SOCKET_SET_ERROR */

  return result;
}

int32_t com_setsockopt_ip_modem(int32_t sock, int32_t level, int32_t optname, const void *optval, int32_t optlen)
{
  int32_t result;
  socket_desc_t *socket_desc;

  result = COM_SOCKETS_ERR_PARAMETER;
  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    if ((optval != NULL) && (optlen != 0))
    {
      if (level == COM_SOL_SOCKET)
      {
        switch(optname)
        {
          case COM_SO_RCVTIMEO :
          {
            if (optlen <= sizeof(socket_desc->rcv_timeout))
            {
              socket_desc->rcv_timeout = *(uint32_t *)optval;
              result = COM_SOCKETS_ERR_OK;
            }
            break;
          }
          case COM_SO_SNDTIMEO :
          {
            if (optlen <= sizeof(socket_desc->rcv_timeout))
            {
              /* There is already a tempo in AT-Core in case of non-response */
              /* Why redefined here a new tempo ? */
              socket_desc->snd_timeout = *(uint32_t *)optval;
              result = COM_SOCKETS_ERR_OK;
            }
            break;
          }
          case COM_SO_ERROR :
          {
            /* Set for this option is NOK */
            break;
          }
          default :
          {
            /* Other options are not yet supported */
            break;
          }
        }
      }
      else
      {
        /* Other level than SOL_SOCKET not yet supported */
      }
    }
  }
  else
  {
    result = COM_SOCKETS_ERR_DESCRIPTOR;
  }

  SOCKET_SET_ERROR(socket_desc, result);
  return result;
}

int32_t com_getsockopt_ip_modem(int32_t sock, int32_t level, int32_t optname, void *optval, int32_t *optlen)
{
  int32_t result;
  socket_desc_t *socket_desc;

  result = COM_SOCKETS_ERR_PARAMETER;
  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    if ((optval != NULL) && (optlen != NULL))
    {
      if (level == COM_SOL_SOCKET)
      {
        switch(optname)
        {
          case COM_SO_RCVTIMEO :
          {
            /* Force optval to be on 4 bytes to be compliant with lwip */
            if (*optlen == sizeof(uint32_t))
            {
              *(uint32_t *)optval = socket_desc->rcv_timeout;
              result = COM_SOCKETS_ERR_OK;
            }
            break;
          }
          case COM_SO_SNDTIMEO :
          {
            /* Force optval to be on 4 bytes to be compliant with lwip */
            if (*optlen == sizeof(uint32_t))
            {
              /* There is already a tempo in AT-Core in case of non-response */
              /* Why redefined here a new tempo ? */
              *(uint32_t *)optval = socket_desc->snd_timeout;
              result = COM_SOCKETS_ERR_OK;
            }
            break;
          }
          case COM_SO_ERROR :
          {
            /* Force optval to be on 4 bytes to be compliant with lwip */
            if (*optlen == sizeof(uint32_t))
            {
              SOCKET_GET_ERROR(socket_desc, optval);
              socket_desc->error = COM_SOCKETS_ERR_OK;
              result = COM_SOCKETS_ERR_OK;
            }
            break;
          }
          default :
          {
            /* Other options are not yet supported */
            break;
          }
        }
      }
      else
      {
        /* Other level than SOL_SOCKET not yet supported */
      }
    }
  }
  else
  {
    result = COM_SOCKETS_ERR_DESCRIPTOR;
  }

  SOCKET_SET_ERROR(socket_desc, result);
  return result;
}

int32_t com_bind_ip_modem(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen)
{
  int32_t result;

  socket_addr_t socket_addr;
  socket_desc_t *socket_desc;

  result = COM_SOCKETS_ERR_PARAMETER;
  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    if (socket_desc->state == SOCKET_CREATED)
    {
      if (   com_translate_ip_address(addr, addrlen, &socket_addr)
          == true)
      {
#if 0
        result = COM_SOCKETS_ERR_GENERAL;
        if (   osCDS_socket_bind(sock,
                                 socket_addr.ip_type,
                                 &socket_addr.ip_value[0],
                                 socket_addr.port)
            == CELLULAR_OK)
        {
#endif
          result = COM_SOCKETS_ERR_OK;
          socket_desc->port = socket_addr.port;
#if 0
        }
#endif
      }
    }
    else
    {
      result = COM_SOCKETS_ERR_STATE;
    }
  }

  SOCKET_SET_ERROR(socket_desc, result);

  return result;
}

int32_t com_connect_ip_modem(int32_t sock, const com_sockaddr_t *addr, int32_t addrlen)
{
  int32_t result;
  socket_addr_t socket_addr;
  socket_desc_t *socket_desc;

  result = COM_SOCKETS_ERR_PARAMETER;

  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    if (socket_desc->state == SOCKET_CREATED)
/*        || (socket_desc->state == SOCKET_CREATED_BIND)) */
    {
      if (   com_translate_ip_address(addr, addrlen, &socket_addr)
          == true)
      {
        /* Check Network status */
        if (com_ip_modem_is_network_up() == true)
        {
          if (    osCDS_socket_connect(socket_desc->id,
                                       socket_addr.ip_type,
                                       (char *)&socket_addr.ip_value[0],
                                       socket_addr.port)
               == CELLULAR_OK)
          {
            result = COM_SOCKETS_ERR_OK;
            PrintINFO("socket connect ok");
            socket_desc->state = SOCKET_CONNECTED;
          }
          else
          {
            result = COM_SOCKETS_ERR_GENERAL;
            PrintERR("socket connect NOK at low level");
          }
        }
        else
        {
          result = COM_SOCKETS_ERR_NONETWORK;
          PrintERR("socket connect NOK no network");
        }
      }
    }
    else
    {
      result = COM_SOCKETS_ERR_STATE;
      PrintERR("socket connect NOK state invalid");
    }

    com_socket_stat_update((result == COM_SOCKETS_ERR_OK) ? COM_SOCKET_STAT_CNT_OK : COM_SOCKET_STAT_CNT_NOK);
  }

  SOCKET_SET_ERROR(socket_desc, result);
  return result;
}

int32_t com_send_ip_modem(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags)
{
  int32_t result;
  socket_desc_t *socket_desc;

  result = COM_SOCKETS_ERR_PARAMETER;
  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    if (socket_desc->state == SOCKET_CONNECTED)
    {
      /* closing maybe received, refuse to send data */
      if (socket_desc->closing == false)
      {
        /* network maybe down, refuse to send data */
        if (com_ip_modem_is_network_up() == false)
        {
          result = COM_SOCKETS_ERR_NONETWORK;
          PrintERR("snd data NOK no network");
        }
        else
        {
          uint32_t length_to_send;
          uint32_t length_send;

          result = COM_SOCKETS_ERR_GENERAL;
          length_send = 0;
          socket_desc->state = SOCKET_SENDING;

          if (flags == COM_MSG_DONTWAIT)
          {
            length_to_send = COM_MIN(len, MODEM_MAX_TX_DATA_SIZE);
            if (   osCDS_socket_send(socket_desc->id, (const char *)buf, length_to_send)
                == CELLULAR_OK)
            {
              length_send = length_to_send;
              result = length_send;
              PrintINFO("snd data DONTWAIT ok");
            }
            else
            {
              PrintERR("snd data DONTWAIT NOK at low level");
            }
            socket_desc->state = SOCKET_CONNECTED;
          }
          else
          {
/* Send all data of a big buffer - Whatever the size */
/* Not yet tested */
            while (   (length_send != len)
                   && (socket_desc->closing == false)
                   && (com_ip_modem_is_network_up() == true)
                   && (socket_desc->state == SOCKET_SENDING))
            {
              length_to_send = COM_MIN(len-length_send, MODEM_MAX_TX_DATA_SIZE);
              /* A tempo is already managed at low-level */
              if (   osCDS_socket_send(socket_desc->id, ((const char *)buf)+length_send, length_to_send)
                  == CELLULAR_OK)
              {
                length_send += length_to_send;
                PrintINFO("snd data ok");
              }
              else
              {
                socket_desc->state = SOCKET_CONNECTED;
                PrintERR("snd data NOK at low level");
              }
            }
            socket_desc->state = SOCKET_CONNECTED;
            result = length_send;
          }
        }
      }
      else
      {
        PrintERR("snd data NOK socket closing");
        result = COM_SOCKETS_ERR_CLOSING;
      }
    }
    else
    {
      PrintERR("snd data NOK err state");
      if (socket_desc->state < SOCKET_CONNECTED)
      {
        result = COM_SOCKETS_ERR_STATE;
      }
      else
      {
        result = (socket_desc->state == SOCKET_CLOSING) ? COM_SOCKETS_ERR_CLOSING : COM_SOCKETS_ERR_INPROGRESS;
      }
    }

    com_socket_stat_update((result >= 0) ? COM_SOCKET_STAT_SND_OK : COM_SOCKET_STAT_SND_NOK);
  }

  if (result >= 0)
  {
    SOCKET_SET_ERROR(socket_desc, COM_SOCKETS_ERR_OK);
  }
  else
  {
    SOCKET_SET_ERROR(socket_desc, result);
  }

  return(result);
}

int32_t com_recv_ip_modem(int32_t sock, com_char_t *buf, int32_t len, int32_t flags)
{
  int32_t result;
  int32_t len_rcv;
  osEvent event;
  socket_desc_t *socket_desc;
  socket_msg_t msg;

  result = COM_SOCKETS_ERR_PARAMETER;
  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    /* Closing maybe received or Network maybe done but still some data to read */
    if (socket_desc->state == SOCKET_CONNECTED)
    {
      uint32_t length_to_read;
      length_to_read = COM_MIN(len, MODEM_MAX_RX_DATA_SIZE);
      result = COM_SOCKETS_ERR_GENERAL;
      socket_desc->state = SOCKET_WAITING_RSP;

      if (flags == COM_MSG_DONTWAIT)
      {
        /* Application don't want to wait if there no data available */
        len_rcv = osCDS_socket_receive(socket_desc->id, (char *)buf, length_to_read);
        result = (len_rcv < 0) ? COM_SOCKETS_ERR_GENERAL : COM_SOCKETS_ERR_OK;
        socket_desc->state = SOCKET_CONNECTED;
        PrintINFO("rcv data DONTWAIT");
      }
      else
      {
        /* Maybe still some data available because application don't read all data with previous call */
        PrintDBG("rcv data waiting");
        len_rcv = osCDS_socket_receive(socket_desc->id, (char *)buf, length_to_read);
        PrintDBG("rcv data waiting exit");

        if (len_rcv == 0)
        {
          /* Waiting for Distant response or Closure Socket or Timeout */
          event = osMessageGet(socket_desc->queue, socket_desc->rcv_timeout);
          if (event.status == osEventTimeout)
          {
            result = COM_SOCKETS_ERR_TIMEOUT;
            socket_desc->state = SOCKET_CONNECTED;
            PrintINFO("rcv data exit timeout");
          }
          else
          {
            msg = *(socket_msg_t*)&event.value.v;
            switch(msg.type)
            {
              case SOCKET_DATA_RCV :
              {
                len_rcv = osCDS_socket_receive(socket_desc->id, (char *)buf, len);
                result = (len_rcv < 0) ? COM_SOCKETS_ERR_GENERAL : COM_SOCKETS_ERR_OK;
                socket_desc->state = SOCKET_CONNECTED;
                PrintINFO("rcv data exit with data");
                break;
              }
              case SOCKET_CLOSING_RQT :
              {
                result = COM_SOCKETS_ERR_CLOSING;
                socket_desc->state = SOCKET_CLOSING;
                PrintINFO("rcv data exit socket closing");
                break;
              }
              default :
              {
                /* Impossible case */
                result = COM_SOCKETS_ERR_GENERAL;
                socket_desc->state = SOCKET_CONNECTED;
                PrintERR("rcv data exit NOK impossible case");
                break;
              }
            }
          }
        }
        else
        {
          result = (len_rcv < 0) ? COM_SOCKETS_ERR_GENERAL : COM_SOCKETS_ERR_OK;
          socket_desc->state = SOCKET_CONNECTED;
          PrintINFO("rcv data exit data available or err low level");
        }
      }
    }
    else
    {
      PrintERR("recv data NOK err state");
      if (socket_desc->state < SOCKET_CONNECTED)
      {
        result = COM_SOCKETS_ERR_STATE;
      }
      else
      {
        result = (socket_desc->state == SOCKET_CLOSING) ? COM_SOCKETS_ERR_CLOSING : COM_SOCKETS_ERR_INPROGRESS;
      }
    }

    com_socket_stat_update((result == COM_SOCKETS_ERR_OK) ? COM_SOCKET_STAT_RCV_OK : COM_SOCKET_STAT_RCV_NOK);
  }

  SOCKET_SET_ERROR(socket_desc, result);
  return((result == COM_SOCKETS_ERR_OK) ? len_rcv : result);
}

int32_t com_closesocket_ip_modem(int32_t sock)
{
  int32_t result;
  socket_desc_t *socket_desc;

  result = COM_SOCKETS_ERR_PARAMETER;
  socket_desc = com_ip_modem_find_socket(sock);

  if (socket_desc != NULL)
  {
    /* If socket is currently under process resfused to close it */
    if (   (socket_desc->state == SOCKET_SENDING)
        || (socket_desc->state == SOCKET_WAITING_RSP))
    {
      PrintERR("close socket NOK err state");
      result = COM_SOCKETS_ERR_INPROGRESS;
    }
    else
    {
      result = COM_SOCKETS_ERR_GENERAL;
      if (   osCDS_socket_close(sock, 0)
          == CELLULAR_OK)
      {
        com_ip_modem_delete_socket_desc(sock);
        result = COM_SOCKETS_ERR_OK;
        PrintINFO("close socket ok");
      }
      else
      {
        PrintINFO("close socket NOK low level");
      }
    }
    com_socket_stat_update((result == COM_SOCKETS_ERR_OK) ? COM_SOCKET_STAT_CLS_OK : COM_SOCKET_STAT_CLS_NOK);
  }

  return(result);
}

/* Server and extra functionalities not yet implemented at low level */
int32_t com_listen_ip_modem(int32_t sock, int32_t backlog)
{
  return COM_SOCKETS_ERR_UNSUPPORTED;
}

int32_t com_accept_ip_modem(int32_t sock, com_sockaddr_t *addr, int32_t *addrlen)
{
  return COM_SOCKETS_ERR_UNSUPPORTED;
}

int32_t com_sendto_ip_modem(int32_t sock, const com_char_t *buf, int32_t len, int32_t flags,
                            const com_sockaddr_t *to, int32_t tolen)
{
  return COM_SOCKETS_ERR_UNSUPPORTED;
}

int32_t com_recvfrom_ip_modem(int32_t sock, com_char_t *buf, int32_t len, int32_t flags,
                              com_sockaddr_t *from, int32_t *fromlen)
{
  return COM_SOCKETS_ERR_UNSUPPORTED;
}

int32_t com_getpeername_ip_modem(int32_t sock, com_sockaddr_t *name, int32_t *namelen)
{
  return COM_SOCKETS_ERR_UNSUPPORTED;
}

int32_t com_getsockname_ip_modem(int32_t sock, com_sockaddr_t *name, int32_t *namelen)
{
  return COM_SOCKETS_ERR_UNSUPPORTED;
}


int32_t com_gethostbyname_ip_modem(const com_char_t *name, com_sockaddr_t *addr)
{
  int32_t result;
  CS_PDN_conf_id_t PDN_conf_id;
  CS_DnsReq_t  dns_req;
  CS_DnsResp_t dns_resp;

  PDN_conf_id = CS_PDN_CONFIG_DEFAULT;

  result = COM_SOCKETS_ERR_PARAMETER;

  if (   (name != NULL)
      && (addr != NULL)
      && (strlen((const char *)name) <= sizeof(dns_req.host_name)))
  {
    result = COM_SOCKETS_ERR_GENERAL;
    strcpy((char *)&dns_req.host_name[0],
           (const char *)name);
    strcpy((char *)&dns_req.primary_dns_addr[0],
           (const char *)&com_primary_dns_addr_str[0]);
    result = osCDS_dns_request(PDN_conf_id, &dns_req, &dns_resp);
    if (result == CELLULAR_OK)
    {
      PrintINFO("DNS resolution OK: %s %s", name, dns_resp.host_addr);
      if (   com_convert_IPString_to_sockaddr((const com_char_t *)&dns_resp.host_addr[0], addr)
          == true)
      {
        PrintDBG("DNS conversion OK");
        result = COM_SOCKETS_ERR_OK;
      }
      else
      {
        PrintERR("DNS conversion NOK");
        result = COM_SOCKETS_ERR_GENERAL;
      }
    }
    else
    {
      PrintERR("DNS resolution NOK for %s", name);
    }
  }

  return(result);
}

bool com_init_ip_modem(void)
{
  bool result;

  result = false;

  osMutexDef(ComSocketsMutex);
  ComSocketsMutexHandle = osMutexCreate(osMutex(ComSocketsMutex));
  if (ComSocketsMutexHandle != NULL)
  {
    /* Create always the first element of the list */
    socket_desc_list = com_ip_modem_create_socket_desc();
    if (socket_desc_list != NULL)
    {
      socket_desc_list->next = NULL;
      socket_desc_list->state = SOCKET_INVALID;
      result = true;
    }

    com_socket_stat_update(COM_SOCKET_STAT_INIT);

#if (USE_DATACACHE == 1)
    network_is_up = false;
#endif
  }

  return result;
}

void com_start_ip_modem(void)
{
#if (USE_DATACACHE == 1)
    dc_com_register_gen_event_cb(&dc_com_db, com_socket_datacache_cb, (void*) NULL);
#endif
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
