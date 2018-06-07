/**
  ******************************************************************************
  * @file    gpio.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for 
  *          the gpio  
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define MDM_SIM_SELECT_0_Pin       	GPIO_PIN_4
#define MDM_SIM_SELECT_0_GPIO_Port 	GPIOA
#define MDM_SIM_SELECT_1_Pin       	GPIO_PIN_3
#define MDM_SIM_SELECT_1_GPIO_Port 	GPIOA
#define BUTTON_Pin                 	GPIO_PIN_13
#define BUTTON_GPIO_Port           	GPIOC
#define BUTTON_EXTI_IRQn           	EXTI15_10_IRQn
#define MDM_PWR_EN_Pin       		GPIO_PIN_4
#define MDM_PWR_EN_GPIO_Port 		GPIOB
#define MDM_RST_Pin          		GPIO_PIN_15
#define MDM_RST_GPIO_Port    		GPIOA
#define LED2_Pin       				GPIO_PIN_14
#define LED2_GPIO_Port 				GPIOB
#define MDM_DTR_Pin 				GPIO_PIN_1
#define MDM_DTR_GPIO_Port 			GPIOB
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
