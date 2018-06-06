/**
  ******************************************************************************
  * @file    Application\radio_management\inc\radio_mngt.h
  * @author  MCD Application Team
  * @brief   This file contains radio management protoype
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef __RADIO_MNGT_H
#define __RADIO_MNGT_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "error_handler.h"

typedef enum
{
    RADIO_MNGT_OK = 0x00,
    RADIO_MNGT_KO
}radio_mngt_status_t;


extern radio_mngt_status_t radio_mngt_radio_on(void);

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_MNGT_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/

