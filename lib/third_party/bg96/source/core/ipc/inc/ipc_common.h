/**
  ******************************************************************************
  * @file    Middlewares\ST\core\ipc\inc\ipc_common.h
  * @author  MCD Marketing / Vertical Application Team
  * @version V0.4
  * @date    29-June-2017
  * @brief   This file contains all the functions prototypes common for IPC
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifndef __IPC_COMMON_H_
#define __IPC_COMMON_H_

#include "plf_config.h"
#include "ipc_config.h"

/* ipc_config.h must define following flags:

* - IPC_USE_STREAM_MODE: set to 0 if IPC stream mode not supported (ie using modem IP stack, with sockets)
*                        set to 1 if IPC stream mode needed (ie using MCU IP stack like Lwip)
* - IPC_RXBUF_MAXSIZE: size of the queue to receive characters from the modem
* - IPC_RXBUF_STREAM_MAXSIZE:  size of the queue to receive characters from the modem in stream mode
*                              NOTE: need to define only if (USE_SOCKETS_TYPE == USE_SOCKETS_LWIP)
* - IPC_RXBUF_THRESHOLD: if free space in RX queue is < to this value, the interface (UART,..) will be paused
*                        until enough free space (ie previous msg have been read)
* - IPC_USE_UART: set to 1 is IPC uses UART (ONLY UART IS SUPPORTED ACTUALLY)
* - IPC_USE_SPI: 0
* - IPC_USE_I2C: 0
* - DBG_IPC_RX_FIFO: set to 1 for additional debug informations
*/

#if (USER_DEFINED_IPC_MAX_DEVICES != 0)
#define IPC_MAX_DEVICES  ((uint8_t) USER_DEFINED_IPC_MAX_DEVICES)
#else
#define IPC_MAX_DEVICES  ((uint8_t) 1)
#endif /* USER_DEFINED_IPC_MAX_DEVICES) */


#if (RTOS_USED == 1)
#include "cmsis_os.h"

#define mySemaphoreDef(name,index)  \
const osSemaphoreDef_t os_semaphore_def_##name##index = { 0 }
#define mySemaphore(name,index)  \
&os_semaphore_def_##name##index

#endif /* RTOS_USED */

/* IPC message format stored in circular buffer:
*  <HEADER><PAYLOAD>
*  - Header = 2 bytes
*    bit 7  6  5  4  3  2  1  0
*       |C| S  S  S  S  S  S  S |
*    bit 7  6  5  4  3  2  1  0
*       |S  S  S  S  S  S  S  S |
*   C: message complete (1 bit: 0 for msg not complete, 1 for msg complete)
*   S: message size (15 bits, maximum size = 32767)
*
*  - Payload
*    message received
*/
#define  IPC_RXMSG_HEADER_SIZE            ((uint16_t) 2U)
#define  IPC_RXMSG_HEADER_COMPLETE_MASK   ((uint8_t) 0x80U)
#define  IPC_RXMSG_HEADER_SIZE_MASK       ((uint8_t) 0x7FU)

#define  IPC_DEVICE_NOT_FOUND  (0xFFU)

/* Debug types ------------------------------------------------------------*/
#if (DBG_IPC_RX_FIFO != 0U)
typedef struct
{
    uint16_t start_pos;
    uint16_t size;
    uint16_t complete;
}dbg_msg_info_t;

typedef struct
{
    uint32_t          total_RXchar;
    uint32_t          cpt_RXPause;
    uint32_t          free_bytes;
    dbg_msg_info_t    msg_info_queue[DBG_QUEUE_SIZE];
    uint16_t          queue_pos;
}dbg_rx_queue_info_t;
#endif /* DBG_IPC_RX_FIFO */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    IPC_DEVICE_0 = 0x00,
    IPC_DEVICE_1 = 0x01,
    IPC_DEVICE_2 = 0x02,
}IPC_Device_t;

typedef enum
{
    IPC_INTERFACE_UNINITIALIZED = 0x00,
    IPC_INTERFACE_UART          = 0x01,
    IPC_INTERFACE_SPI           = 0x02,
    IPC_INTERFACE_I2C           = 0x03,
}IPC_Interface_t;

typedef struct
{
  IPC_Interface_t  interface_type;
  /* only one of the following interface is active for a device */
#if (IPC_USE_UART == 1U)
  UART_HandleTypeDef     *h_uart;
#endif
#if (IPC_USE_SPI == 1U)
  SPI_HandleTypeDef      *h_spi;
#endif
#if (IPC_USE_I2C == 1U)
  I2C_HandleTypeDef      *h_i2c;
#endif
}IPC_PhysicalInterface_t;

typedef enum
{
    IPC_MODE_UART_CHARACTER  = 0x00,
    IPC_MODE_UART_STREAM     = 0x01,
    IPC_MODE_SPI_MASTER      = 0x02,
    IPC_MODE_SPI_ROLE_SLAVE  = 0x03,
}IPC_Mode_t;

typedef enum
{
    IPC_OK = 0x00,
    IPC_ERROR,
    IPC_RXQUEUE_EMPTY,
    IPC_RXQUEUE_MSG_AVAIL,
}IPC_Status_t;

typedef enum
{
    IPC_STATE_NOT_INITIALIZED = 0x00,
    IPC_STATE_INITIALIZED     = 0x01,
    IPC_STATE_ACTIVE          = 0x02,
    IPC_STATE_PAUSED          = 0x03,
}IPC_State_t;

typedef struct
{
    uint8_t     complete;
    uint16_t    size;
}IPC_RxHeader_t;

typedef struct
{
    uint8_t     buffer[IPC_RXBUF_MAXSIZE];
    uint16_t    size;
}IPC_RxMessage_t;

typedef struct
{
    uint8_t      data[IPC_RXBUF_MAXSIZE];
    uint16_t     index_read;
    uint16_t     index_write;
    uint16_t     current_msg_index;
    uint16_t     current_msg_size;
    uint8_t      nb_unread_msg;
}IPC_RxQueue_t ;

#if (IPC_USE_STREAM_MODE == 1U)
typedef struct
{
    uint8_t      data[IPC_RXBUF_STREAM_MAXSIZE];
    uint16_t     index_read;
    uint16_t     index_write;
    uint16_t     available_char;
    uint16_t     total_rcv_count;
}IPC_RxBuffer_t ;
#endif  /* IPC_USE_STREAM_MODE */

struct IPC_Handle_Typedef_struct;

typedef void    (*IPC_RxCallbackTypeDef)(struct IPC_Handle_Typedef_struct* hipc);
typedef void    (*IPC_TxCallbackTypeDef)(struct IPC_Handle_Typedef_struct* hipc);
typedef void    (*IPC_RXFIFO_writeTypeDef)(struct IPC_Handle_Typedef_struct* hipc, uint8_t rxChar);
typedef uint8_t (*IPC_CheckEndOfMsgCallbackTypeDef)(uint8_t rxChar);

typedef struct IPC_Handle_Typedef_struct
{
    IPC_Device_t            Device_ID;    /* IPC device ID */
    IPC_PhysicalInterface_t Interface;
    IPC_Mode_t              Mode;         /* Channel mode */
    IPC_State_t             State;        /* IPC state */
    IPC_RxQueue_t           RxQueue;      /* RX Queue */
#if (IPC_USE_STREAM_MODE == 1U)
    IPC_RxBuffer_t          RxBuffer;     /* RX Buffer for stream mode */
#endif  /* IPC_USE_STREAM_MODE */
    uint16_t                UartBusyFlag;
    IPC_RxCallbackTypeDef             RxClientCallback;
    IPC_TxCallbackTypeDef             TxClientCallback;
    IPC_CheckEndOfMsgCallbackTypeDef  CheckEndOfMsgCallback;
    IPC_RXFIFO_writeTypeDef           RxFifoWrite;

#if (DBG_IPC_RX_FIFO != 0U)
    dbg_rx_queue_info_t         dbgRxQueue;
#endif /* DBG_IPC_RX_FIFO */

} IPC_Handle_t;

typedef struct
{
  IPC_State_t              state;
  IPC_PhysicalInterface_t  phy_int;
  uint8_t                  RxChar[1];    /* RX DMA buffer (1 char) - common buffer for one physical interface  */
  IPC_Handle_t             *h_current_channel;   /* current active IPC channel */
  IPC_Handle_t             *h_inactive_channel;  /* other IPC channel (exists if not NULL), currently not active */
}IPC_ClientDescription_t;


/* Exported variables ------------------------------------------------------- */
extern IPC_ClientDescription_t g_IPC_Devices_List[IPC_MAX_DEVICES];

/* Exported functions ------------------------------------------------------- */
IPC_Status_t IPC_init(IPC_Device_t device, IPC_Interface_t itf_type, void *h_itf);
IPC_Status_t IPC_deinit(IPC_Device_t device);
IPC_Status_t IPC_open(IPC_Handle_t     *hipc,
                           IPC_Device_t     device,
                           IPC_Mode_t       mode,
                           IPC_RxCallbackTypeDef pRxClientCallback,
                           IPC_TxCallbackTypeDef pTxClientCallback,
                           IPC_CheckEndOfMsgCallbackTypeDef pCheckEndOfMsg);
IPC_Status_t IPC_close(IPC_Handle_t *hipc);
IPC_Status_t IPC_select(IPC_Handle_t *hipc);
IPC_Status_t IPC_reset(IPC_Handle_t *hipc);
IPC_Handle_t *IPC_get_other_channel(IPC_Handle_t *hipc);
IPC_Status_t IPC_send(IPC_Handle_t *hipc, uint8_t *p_TxBuffer, uint16_t bufsize);
IPC_Status_t IPC_receive(IPC_Handle_t *hipc, IPC_RxMessage_t *p_msg);
IPC_Status_t IPC_streamReceive(IPC_Handle_t *hipc,  uint8_t *p_buffer, int16_t *p_len);


void IPC_dump_RX_queue(IPC_Handle_t *hipc, uint8_t readable);

#endif /* __IPC_COMMON_H_ */
