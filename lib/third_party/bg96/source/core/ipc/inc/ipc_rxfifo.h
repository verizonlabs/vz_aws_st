/**
  ******************************************************************************
  * @file    Application\core\ipc\inc\ipc_rxfifo.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes common for managing
  *          IPC RX FIFO
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef __IPC_RXFIFO_H_
#define __IPC_RXFIFO_H_

#include "ipc_common.h"

void RXFIFO_init(IPC_Handle_t *hipc);
void RXFIFO_writeCharacter(IPC_Handle_t* hipc, uint8_t rxChar);
int16_t RXFIFO_read(IPC_Handle_t *hipc, IPC_RxMessage_t *o_Msg);

#if (IPC_USE_STREAM_MODE == 1)
void RXFIFO_stream_init(IPC_Handle_t *hipc);
void RXFIFO_writeStream(IPC_Handle_t* hipc, uint8_t rxChar);
#endif /* IPC_USE_STREAM_MODE */

/* debug functions */
void RXFIFO_print_data(IPC_Handle_t *hipc, uint16_t index, uint16_t size, uint8_t readable);
void RXFIFO_readMsgHeader_at_pos(IPC_Handle_t *hipc, IPC_RxHeader_t *header, uint16_t pos);
uint16_t RXFIFO_getFreeBytes(IPC_Handle_t *hipc);

#if (DBG_IPC_RX_FIFO != 0)
void dump_RX_dbg_infos(IPC_Handle_t *hipc, uint8_t databuf, uint8_t queue);
#endif /* DBG_IPC_RX_FIFO */

#endif /* __IPC_RXFIFO_H_ */
