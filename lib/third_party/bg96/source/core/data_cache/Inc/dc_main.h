/**
  ******************************************************************************
  * @file    Application\core\data_cache\Inc\dc_main.h
  * @author  MCD Application Team
  * @brief   This file contains data cache definitions
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef __DC_MAIN_H
#define __DC_MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    DC_MAIN_OK = 0x00,
    DC_MAIN_ERROR
} dc_main_status_t;


/* Exported functions ------------------------------------------------------------*/
dc_main_status_t dc_init(void);
void dc_start (void);

#ifdef __cplusplus
}
#endif

#endif /* __DC_MAIN_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
