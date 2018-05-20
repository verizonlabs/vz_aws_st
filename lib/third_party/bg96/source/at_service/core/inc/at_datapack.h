/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\inc\at_datapack.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for
  *          packing/unpacking datas
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
