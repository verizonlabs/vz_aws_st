/**
  ******************************************************************************
  * @file    Application\core\data_cache\Inc\dc_control.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for dc_control
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef __DC_CONTROL_H
#define __DC_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "dc_config.h"
#include "cmsis_os.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    DC_CTRL_OK = 0x00,
    DC_CTRL_ERROR
} dc_ctrl_status_t;


extern osThreadId dc_CtrlEventTaskId;

/* Exported functions ------------------------------------------------------------*/
dc_ctrl_status_t dc_ctrl_event_init(void);
dc_ctrl_status_t dc_ctrl_event_start(void);
void dc_ctrl_post_event_normal(dc_com_event_id_t event_id);
void dc_ctrl_post_event_debounce(dc_com_event_id_t event_id);

#ifdef __cplusplus
}
#endif

#endif /* __DC_CONTROL_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
