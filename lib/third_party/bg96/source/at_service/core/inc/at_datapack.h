/**
  ******************************************************************************
  * @file    Application\at_service\core\inc\at_datapack.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for
  *          packing/unpacking datas
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef AT_DATAPACK_H_
#define AT_DATAPACK_H_

#include "at_core.h"

#define DATAPACK_MAX_BUF_SIZE (ATCMD_MAX_BUF_SIZE)

typedef enum
{
  DATAPACK_OK = 0,
  DATAPACK_ERROR,
} DataPack_Status_t;

typedef struct
{
    void* structptr;
}datapack_structptr_t;

DataPack_Status_t DATAPACK_writePtr(uint8_t *p_buf, uint16_t msgtype, void *p_data);
DataPack_Status_t DATAPACK_writeStruct(uint8_t *p_buf, uint16_t msgtype, uint16_t size, void *p_data);
DataPack_Status_t DATAPACK_readPtr(uint8_t *p_buf, uint16_t msgtype, void **p_data);
DataPack_Status_t DATAPACK_readStruct(uint8_t *p_buf, uint16_t msgtype, uint16_t size, void *p_data);
uint16_t               DATAPACK_readMsgType(uint8_t *p_buf);
uint16_t               DATAPACK_readSize(uint8_t *p_buf);

#endif /* AT_DATAPACK_H_ */
