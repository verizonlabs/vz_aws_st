/**
  ******************************************************************************
  * @file    Middlewares\ST\core\ipc\src\ipc_rxfifo.c
  * @author  MCD Application Team
  * @version V0.4
  * @date    29-June-2017
  * @brief   This file provides common code for managing IPC RX FIFO
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
#include "ipc_common.h"
#include "ipc_rxfifo.h"
#include "string.h"

#if (USE_TRACE_INTERFACE_IPC == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_IPC, DBL_LVL_P0, "IPC:" format "\r\n", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_IPC, DBL_LVL_P1, "IPC:" format "\r\n", ## args)
#define PrintErr(format, args...)  TracePrint(DBG_CHAN_IPC, DBL_LVL_ERR, "IPC ERROR:" format "\r\n", ## args)
#define PrintBuf(pbuf, size)       TracePrintBufChar(DBG_CHAN_ATCMD, DBL_LVL_P0, (char *)pbuf, size)
#define PrintBufHexa(pbuf, size)   TracePrintBufHex(DBG_CHAN_ATCMD, DBL_LVL_P0, (char *)pbuf, size)
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("IPC:" format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("IPC:" format "\r\n", ## args)
#define PrintErr(format, args...)   printf("IPC ERROR:" format "\r\n", ## args)
#define PrintBuf(pbuf, size)        do {} while(0)
#define PrintBufHexa(pbuf, size)    do {} while(0)
#elif (USE_CONFIGPRINTF_IPC == 1)
#define PrintINFO(format, args...)  printf("IPC:" format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("IPC:" format "\r\n", ## args)
#define PrintErr(format, args...)   printf("IPC ERROR:" format "\r\n", ## args)
#define PrintBuf(pbuf, size)        do {} while(0)
#define PrintBufHexa(pbuf, size)    do {} while(0)
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#define PrintErr(format, args...)   do {} while(0)
#define PrintBuf(pbuf, size)        do {} while(0)
#define PrintBufHexa(pbuf, size)    do {} while(0)
#endif

/* static functions prototypes  ------------------------------------------------*/
static void RXFIFO_incrementTail(IPC_Handle_t *hipc, uint16_t inc_size);
static void RXFIFO_incrementHead(IPC_Handle_t *hipc);
static void RXFIFO_readMsgHeader(IPC_Handle_t *hipc, IPC_RxHeader_t *header);
static void RXFIFO_updateMsgHeader(IPC_Handle_t *hipc);
static void RXFIFO_prepareNextMsgHeader(IPC_Handle_t *hipc);
static void rearm_RX_IT(IPC_Handle_t *hipc);

/* Exported functions --------------------------------------------------------*/
void RXFIFO_init(IPC_Handle_t *hipc)
{
    memset(hipc->RxQueue.data, 0, sizeof(uint8_t) * IPC_RXBUF_MAXSIZE);
    hipc->RxQueue.index_read = 0;
    hipc->RxQueue.index_write = IPC_RXMSG_HEADER_SIZE;
    hipc->RxQueue.current_msg_index = 0;
    hipc->RxQueue.current_msg_size = 0;
    hipc->RxQueue.nb_unread_msg = 0;

#if (DBG_IPC_RX_FIFO != 0)
    /* init debug infos */
    hipc->dbgRxQueue.total_RXchar = 0;
    hipc->dbgRxQueue.cpt_RXPause = 0;
    hipc->dbgRxQueue.free_bytes = IPC_RXBUF_MAXSIZE;
    hipc->dbgRxQueue.queue_pos = 0;
    hipc->dbgRxQueue.msg_info_queue[0].start_pos = hipc->RxQueue.index_read;
    hipc->dbgRxQueue.msg_info_queue[0].size = 0;
    hipc->dbgRxQueue.msg_info_queue[0].complete = 0;
#endif /* DBG_IPC_RX_FIFO */
}

void RXFIFO_writeCharacter(IPC_Handle_t* hipc, uint8_t rxChar)
{
    hipc->RxQueue.data[hipc->RxQueue.index_write]= rxChar;

    hipc->RxQueue.current_msg_size++;

#if (DBG_IPC_RX_FIFO != 0)
    hipc->dbgRxQueue.msg_info_queue[hipc->dbgRxQueue.queue_pos].size = hipc->RxQueue.current_msg_size;
#endif /* DBG_IPC_RX_FIFO */

    RXFIFO_incrementHead(hipc);

    if (hipc->State != IPC_STATE_PAUSED)
    {
        /* rearm RX Interrupt */
        rearm_RX_IT(hipc);
    }

    /* check if the char received is an end of message */
    if ((*hipc->CheckEndOfMsgCallback)(rxChar) == 1)
    {
        hipc->RxQueue.nb_unread_msg++;

        /* update header for message received */
        RXFIFO_updateMsgHeader(hipc);

        /* save start position of next message */
        hipc->RxQueue.current_msg_index = hipc->RxQueue.index_write;

        /* reset current msg size */
        hipc->RxQueue.current_msg_size = 0;

        /* reserve place for next msg header */
        RXFIFO_prepareNextMsgHeader(hipc);

        /* msg received: call client callback */
        (* hipc->RxClientCallback)((void*)hipc);
    }

    return;
}

int16_t RXFIFO_read(IPC_Handle_t *hipc, IPC_RxMessage_t *o_Msg)
{
    IPC_RxHeader_t header;
    uint16_t oversize = 0;

#if (DBG_IPC_RX_FIFO != 0)
    PrintDBG(" *** start pos=%d ", hipc->RxQueue.index_read);
#endif /* DBG_IPC_RX_FIFO */

    /* read message header */
    RXFIFO_readMsgHeader(hipc, &header);

    if (header.complete != 1)
    {
        /* error: trying to read an uncomplete message */
        return(-1);
    }

    /* jump header */
    RXFIFO_incrementTail(hipc, IPC_RXMSG_HEADER_SIZE);

#if (DBG_IPC_RX_FIFO != 0)
    PrintDBG(" *** data pos=%d ", hipc->RxQueue.index_read);
    PrintDBG(" *** size=%d ", header.size);
    PrintDBG(" *** free bytes before read=%d ", hipc->dbgRxQueue.free_bytes);
#endif /* DBG_IPC_RX_FIFO */

    /* update size in output structure */
    o_Msg->size = header.size;

    /* copy msg content to output structure */
    if ((hipc->RxQueue.index_read + header.size) > IPC_RXBUF_MAXSIZE)
    {
        /* message is split in 2 parts in the circular buffer */
        oversize = (hipc->RxQueue.index_read + header.size - IPC_RXBUF_MAXSIZE);
        memcpy((void *)&(o_Msg->buffer[0]),
               (void *)&(hipc->RxQueue.data[hipc->RxQueue.index_read]),
               header.size - oversize);
        memcpy((void *)&(o_Msg->buffer[header.size - oversize]),
               (void *)&(hipc->RxQueue.data[0]),
               oversize);

#if (DBG_IPC_RX_FIFO != 0)
        PrintDBG("override end of buffer");
#endif /* DBG_IPC_RX_FIFO */
    }
    else
    {
        /* message is contiguous in the circular buffer */
        memcpy((void *)o_Msg->buffer, (void *)&(hipc->RxQueue.data[hipc->RxQueue.index_read]), header.size);
    }

    /* increment tail index to the next message */
    RXFIFO_incrementTail(hipc, header.size);

#if (DBG_IPC_RX_FIFO != 0)
    /* update free_bytes infos */
    hipc->dbgRxQueue.free_bytes = RXFIFO_getFreeBytes(hipc);
    PrintDBG(" *** free after read bytes=%d ", hipc->dbgRxQueue.free_bytes);
#endif /* DBG_IPC_RX_FIFO */

    /* msg has been read */
    hipc->RxQueue.nb_unread_msg--;

    return(hipc->RxQueue.nb_unread_msg);
}

#if (IPC_USE_STREAM_MODE == 1)
void RXFIFO_stream_init(IPC_Handle_t *hipc)
{
    memset(hipc->RxBuffer.data, 0,  sizeof(uint8_t) * IPC_RXBUF_STREAM_MAXSIZE);
    hipc->RxBuffer.index_read = 0;
    hipc->RxBuffer.index_write = 0;
    hipc->RxBuffer.available_char = 0;
    hipc->RxBuffer.total_rcv_count = 0;
}

void RXFIFO_writeStream(IPC_Handle_t* hipc, uint8_t rxChar)
{
    hipc->RxBuffer.data[hipc->RxBuffer.index_write] = rxChar;

    /* rearm RX Interrupt */
    rearm_RX_IT(hipc);

    hipc->RxBuffer.index_write++;
    hipc->RxBuffer.total_rcv_count++;

    if(hipc->RxBuffer.index_write >= IPC_RXBUF_STREAM_MAXSIZE)
    {
        hipc->RxBuffer.index_write=0;
    }
    hipc->RxBuffer.available_char++;

    (* hipc->RxClientCallback)((void*)hipc);

}
#endif /* IPC_USE_STREAM_MODE */

void RXFIFO_print_data(IPC_Handle_t *hipc, uint16_t index, uint16_t size, uint8_t readable)
{
    PrintINFO("DUMP RX QUEUE: (index=%d) (size=%d)", index, size);
    if ((index + size) > IPC_RXBUF_MAXSIZE)
    {
        /* in case buffer loops back to index 0 */
        /* print first buffer part (until end of queue) */
        PrintBuf((const char *)&hipc->RxQueue.data[index], (IPC_RXBUF_MAXSIZE - index));
        /* print second buffer part */
        PrintBuf((const char *)&hipc->RxQueue.data[0], (size - (IPC_RXBUF_MAXSIZE - index)));

        PrintINFO("dump same in hexa:");
        /* print first buffer part (until end of queue) */
        PrintBufHexa((const char *)&hipc->RxQueue.data[index], (IPC_RXBUF_MAXSIZE - index));
        /* print second buffer part */
        PrintBufHexa((const char *)&hipc->RxQueue.data[0], (size - (IPC_RXBUF_MAXSIZE - index)));
    }
    else
    {
        PrintBuf((const char *)&hipc->RxQueue.data[index], size);
        PrintINFO("dump same in hexa:");
        PrintBufHexa((const char *)hipc->RxQueue.data[index], size);
    }
    PrintINFO("\r\n");
}

void RXFIFO_readMsgHeader_at_pos(IPC_Handle_t *hipc, IPC_RxHeader_t *header, uint16_t pos)
{
    uint8_t header_byte1, header_byte2;
    uint16_t index;

#if (DBG_IPC_RX_FIFO != 0)
    PrintDBG("DBG RXFIFO_readMsgHeader: index_read = %d", hipc->RxQueue.index_read);
#endif /* DBG_IPC_RX_FIFO */

    /* read header bytes */
    index =  pos; /* hipc->RxQueue.index_read; */
    header_byte1 = hipc->RxQueue.data[index];
    index = (index + 1) % IPC_RXBUF_MAXSIZE;
    header_byte2 = hipc->RxQueue.data[index];

    PrintDBG("header_byte1[0x%x] header_byte2[0x%x]",header_byte1,header_byte2);

    /* get msg complete bit */
    header->complete = (IPC_RXMSG_HEADER_COMPLETE_MASK & header_byte1) >> 7;
    /* get msg size */
    header->size = ((IPC_RXMSG_HEADER_SIZE_MASK & header_byte1) << 8);
    header->size = header->size + header_byte2;

    PrintDBG("complete=%d size=%d",header->complete, header->size);

    return;
}

uint16_t RXFIFO_getFreeBytes(IPC_Handle_t *hipc)
{
    uint16_t free_bytes;

    if (hipc->RxQueue.index_write > hipc->RxQueue.index_read)
    {
        free_bytes = (IPC_RXBUF_MAXSIZE - hipc->RxQueue.index_write +  hipc->RxQueue.index_read);
    }
    else
    {
        free_bytes =  hipc->RxQueue.index_read - hipc->RxQueue.index_write;
    }

    return(free_bytes);
}

/* Static functions --------------------------------------------------------*/

static void RXFIFO_incrementTail(IPC_Handle_t *hipc, uint16_t inc_size)
{
    hipc->RxQueue.index_read = (hipc->RxQueue.index_read + inc_size) % IPC_RXBUF_MAXSIZE;
}

static void RXFIFO_incrementHead(IPC_Handle_t *hipc)
{
    uint16_t free_bytes = 0;

    hipc->RxQueue.index_write = (hipc->RxQueue.index_write + 1) % IPC_RXBUF_MAXSIZE;

    free_bytes = RXFIFO_getFreeBytes(hipc);

#if (DBG_IPC_RX_FIFO != 0)
    hipc->dbgRxQueue.free_bytes = free_bytes;
#endif /* DBG_IPC_RX_FIFO */

    if (free_bytes <= IPC_RXBUF_THRESHOLD)
    {
        hipc->State = IPC_STATE_PAUSED;

#if (DBG_IPC_RX_FIFO != 0)
        hipc->dbgRxQueue.cpt_RXPause++;
#endif /* DBG_IPC_RX_FIFO */
    }
}

static void RXFIFO_readMsgHeader(IPC_Handle_t *hipc, IPC_RxHeader_t *header)
{
    uint8_t header_byte1, header_byte2;
    uint16_t index;

#if (DBG_IPC_RX_FIFO != 0)
    PrintDBG("DBG RXFIFO_readMsgHeader: index_read = %d", hipc->RxQueue.index_read);
#endif /* DBG_IPC_RX_FIFO */

    /* read header bytes */
    index =  hipc->RxQueue.index_read;
    header_byte1 = hipc->RxQueue.data[index];
    index = (index + 1) % IPC_RXBUF_MAXSIZE;
    header_byte2 = hipc->RxQueue.data[index];

    /* get msg complete bit */
    header->complete = (IPC_RXMSG_HEADER_COMPLETE_MASK & header_byte1) >> 7;
    /* get msg size */
    header->size = ((IPC_RXMSG_HEADER_SIZE_MASK & header_byte1) << 8);
    header->size = header->size + header_byte2;

    return;
}

static void RXFIFO_updateMsgHeader(IPC_Handle_t *hipc)
{
    /* update header with the size of last complete msg received */
    uint8_t header_byte1, header_byte2;
    uint16_t index;

    /* set header byte 1:  complete bit + size (upper part)*/
    header_byte1 = (0x80 | ((hipc->RxQueue.current_msg_size >> 8) & 0x9F));
    /* set header byte 2:  size (lower part)*/
    header_byte2 =  hipc->RxQueue.current_msg_size & 0xFF;

    /* write header bytes */
    index = hipc->RxQueue.current_msg_index;
    hipc->RxQueue.data[index] = header_byte1;
    index = (index + 1) % IPC_RXBUF_MAXSIZE;
    hipc->RxQueue.data[index] = header_byte2;

#if (DBG_IPC_RX_FIFO != 0)
    hipc->dbgRxQueue.msg_info_queue[hipc->dbgRxQueue.queue_pos].complete = 1;
    hipc->dbgRxQueue.queue_pos = (hipc->dbgRxQueue.queue_pos + 1 ) % DBG_QUEUE_SIZE;
    hipc->dbgRxQueue.msg_info_queue[hipc->dbgRxQueue.queue_pos].start_pos = hipc->RxQueue.current_msg_index;
    hipc->dbgRxQueue.msg_info_queue[hipc->dbgRxQueue.queue_pos].complete = 0;
#endif /* DBG_IPC_RX_FIFO */

    return;
}

static void RXFIFO_prepareNextMsgHeader(IPC_Handle_t *hipc)
{
    uint8_t i;

    for (i=0; i < IPC_RXMSG_HEADER_SIZE; i++)
    {
        hipc->RxQueue.data[hipc->RxQueue.index_write] = 0;
        RXFIFO_incrementHead(hipc);
    }

    return;
}

static void rearm_RX_IT(IPC_Handle_t *hipc)
{
    /* TODO: specific to UART... should be in ipc_uart.c */
    /* rearm uart TX interrupt */
    if (hipc->Interface.interface_type == IPC_INTERFACE_UART)
    {
        HAL_UART_Receive_IT(hipc->Interface.h_uart, (uint8_t*)g_IPC_Devices_List[hipc->Device_ID].RxChar, 1);
    }
}

#if (DBG_IPC_RX_FIFO != 0)
void dump_RX_dbg_infos(IPC_Handle_t *hipc, uint8_t databuf, uint8_t queue)
{
    uint32_t i;

    if (databuf == 1)
    {
        PrintBuf((const char *)&hipc->RxQueue.data[0], IPC_RXBUF_MAXSIZE);
        /* PrintBufHexa((const char *)&hipc->RxQueue.data[0], IPC_RXBUF_MAXSIZE); */
    }

    PrintINFO("\r\n");

    if (queue == 1)
    {
        for (i=0; i <= hipc->dbgRxQueue.queue_pos; i++)
        {
            PrintINFO(" [index %d]:  start_pos=%d size=%d complete=%d",
                   i,
                   hipc->dbgRxQueue.msg_info_queue[i].start_pos,
                   hipc->dbgRxQueue.msg_info_queue[i].size,
                   hipc->dbgRxQueue.msg_info_queue[i].complete );
        }
    }
}
#endif /* DBG_IPC_RX_FIFO */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

