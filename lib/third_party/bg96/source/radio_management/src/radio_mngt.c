/**
  ******************************************************************************
  * @file    Application\radio_management\src\radio_mngt.c
  * @author  MCD Application Team
  * @brief   This file contains radio management
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#include "error_handler.h"
#include "radio_mngt.h"
#include "cellular_service_task.h"


radio_mngt_status_t radio_mngt_radio_on()
{
  return  (radio_mngt_status_t)CST_radio_on();
}


