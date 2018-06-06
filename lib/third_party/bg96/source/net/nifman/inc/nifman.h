/**
  ******************************************************************************
  * @brief   Application\net\nifman\inc\nifman.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for NIFMAN
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef __NIFMAN_H
#define __NIFMAN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

typedef enum
{
    NIFMAN_OK = 0x00,
    NIFMAN_ERROR
}nifman_status_t;

/* Exported vars ------------------------------------------------------------*/
extern osThreadId  nifman_ThreadId;


/* Exported functions ------------------------------------------------------- */
nifman_status_t nifman_init  (void);
nifman_status_t nifman_start  (void);

#ifdef __cplusplus
}
#endif

#endif /* __NIFMANLIB_H */
/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/