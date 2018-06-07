/**
  ******************************************************************************
  * @file    Application\radio_service\cellular\inc\cellular_service_task.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for
  *          cellular service task
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef CELLULAR_SERVICE_TASK_H_
#define CELLULAR_SERVICE_TASK_H_


/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "cellular_service.h"
//Added by AVK
#define MDM_SIM_SELECT_0_Pin       GPIO_PIN_4
#define MDM_SIM_SELECT_0_GPIO_Port GPIOA
#define MDM_SIM_SELECT_1_Pin       GPIO_PIN_3
#define MDM_SIM_SELECT_1_GPIO_Port GPIOA
#define BUTTON_Pin                 GPIO_PIN_13
#define BUTTON_GPIO_Port           GPIOC
#define BUTTON_EXTI_IRQn           EXTI15_10_IRQn
#define MDM_PWR_EN_Pin       GPIO_PIN_4
#define MDM_PWR_EN_GPIO_Port GPIOB
#define MDM_RST_Pin          GPIO_PIN_15
#define MDM_RST_GPIO_Port    GPIOA
#define LED2_Pin       GPIO_PIN_14
#define LED2_GPIO_Port GPIOB
#define MDM_DTR_Pin GPIO_PIN_1
#define MDM_DTR_GPIO_Port GPIOB
//End
/* external variable definition ----------------------------------------------------------*/
typedef enum
{
    CST_MODEM_INIT_STATE           ,
    CST_MODEM_RESET_STATE          ,
    CST_MODEM_ON_STATE             ,
    CST_MODEM_ON_ONLY_STATE        ,
    CST_MODEM_POWERED_ON_STATE     ,
    CST_WAITING_FOR_SIGNAL_QUALITY_OK_STATE,
    CST_WAITING_FOR_NETWORK_STATUS_STATE,
    CST_NETWORK_STATUS_OK_STATE    ,
    CST_MODEM_REGISTERED_STATE      ,
    CST_MODEM_PDN_ACTIVATE_STATE    ,
    CST_MODEM_PDN_ACTIVATED_STATE ,
    CST_MODEM_PPP_CONFIG_ON_GOING_STATE,
    CST_MODEM_DATA_READY_STATE  ,
    CST_MODEM_FAIL_STATE
} CST_state_t;

/* external function definition ----------------------------------------------------------*/
extern osThreadId CST_cellularServiceThreadId;

extern CST_state_t CST_get_state(void);

extern CS_Status_t  CST_radio_on(void);

extern CS_Status_t CST_modem_power_on(void);

extern CS_Status_t CST_cellular_service_init(void);

extern CS_Status_t CST_cellular_service_start(void);



#endif /* CELLULAR_SERVICE_TASK_H_ */