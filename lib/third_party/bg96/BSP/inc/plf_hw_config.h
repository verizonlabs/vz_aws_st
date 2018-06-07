/**
  ******************************************************************************
  * @file    project_config/inc/plf_hw_config.h
  * @author  MCD Application Team
  * @brief   This file contains the hardware configuration of the platform
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLF_HW_CONFIG_H
#define __PLF_HW_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "main.h"
#include "plf_modem_config.h"

/* Platform defines ------------------------------------------------------------*/

/*
Hardware configuration for the NBIOT board B-L475E-IOT01 (using STM32L475xx)
----------------------------------------------------------------------------
common config (AkorIot and B-CELL)
UART used to communicate with the modem = UART4
UART used for traces = USART1
Test LED = LED2 (PB14)

* If using AkorIoT BG96 modem board
just plug it to the arduino connector + power the AkorIoT board with ext. 5V
in project options, need to define HWREF_AKORIOT_BG96
Modem PWR_EN pin = PA4 (arduino pin D7)
Modem RST pin = PB1    (arduino pin D6)

* If using HWREF_B_CELL_BG96_V2 modem board
in project options, need to define HWREF_B_CELL_BG96_V2
additional pins needed for B_CELL board:
Modem PWR_EN pin = PA4 (arduino pin D7)
Modem RST pin = PB1 (arduino pin D6)
Modem DTR pin = PB4 (arduino pin D5)
Modem SIM select 1 pin = PA3 (arduino pin D4) -> to GND
Modem SIM select 0 pin = PB0 (arduino pin D3) -> to GND

  HOW TO CONNECT  "HWREF_B_CELL_BG96_V2" to "B-L475E-IOT01"
  STMOD+ modem connector of HWREF_B_CELL_BG96_V2 <-> B-L475E-IOT01 board (arduino connectors)
  pin  1- RTSS -> UART-CTS                       <-> LEAVE NOT CONNECTED
  pin  2- RXDS -> UART-TX                        <-> PA0 (arduino connector D1)
  pin  3- TXDS -> UART-RX                        <-> PA1 (arduino connector D0)
  pin  4- CTSS -> UART-RTS                       <-> LEAVE NOT CONNECTED
  pin  5- GND                                    <-> GND
  pin  6- VCC +5V                                <-> +5V
  pin  7- not used                               <-> LEAVE NOT CONNECTED
  pin  8- Sim_select1                            <-> PA3 (arduino pin D4)
  pin  9- PWRen                                  <-> PA4 (arduino pin D7)
  pin 10- not used                               <-> LEAVE NOT CONNECTED
  pin 11- RIS                                    <-> LEAVE NOT CONNECTED
  pin 12- RST                                    <-> PB1 (arduino pin D6)
  pin 13- sim_CLK                                <-> LEAVE NOT CONNECTED
  pin 14- DTR                                    <-> PB4 (arduino pin D5)
  pin 15- VCC +5V                                <-> +5V
  pin 16- GND                                    <-> GND
  pin 17- sim_RST                                <-> LEAVE NOT CONNECTED
  pin 18- Sim_select0                            <-> PB0 (arduino pin D3)
  pin 19- sim_DATA                               <-> LEAVE NOT CONNECTED
  pin 20- same as pin 18                         <-> LEAVE NOT CONNECTED

*/

/* MODEM configuration */
#if defined(STM32L476xx)
extern UART_HandleTypeDef       huart1;
#define MODEM_UART_HANDLE       huart1
#define MODEM_UART_INSTANCE     USART1
#define MODEM_UART_AUTOBAUD     (1)
#elif defined(STM32L475xx)
extern UART_HandleTypeDef       huart4;
#define MODEM_UART_HANDLE       huart4
#define MODEM_UART_INSTANCE     UART4
#define MODEM_UART_AUTOBAUD     (0)
#else
#error Modem connector not specified
#endif

#define MODEM_UART_BAUDRATE     (CONFIG_MODEM_UART_BAUDRATE)
#if (CONFIG_MODEM_UART_RTS_CTS == 1)
#define MODEM_UART_HWFLOWCTRL   UART_HWCONTROL_RTS_CTS
#else
#define MODEM_UART_HWFLOWCTRL   UART_HWCONTROL_NONE
#endif
#define MODEM_UART_WORDLENGTH   UART_WORDLENGTH_8B
#define MODEM_UART_STOPBITS     UART_STOPBITS_1
#define MODEM_UART_PARITY       UART_PARITY_NONE
#define MODEM_UART_MODE         UART_MODE_TX_RX

/* ---- MODEM other pins configuration ---- */
#if defined(STM32L476xx) /* NUCLEO L476 NOT TESTED */
#if defined(HWREF_AKORIOT_BG96)
#define MODEM_RST_GPIO_Port             GPIOB       /* MDM_RST_GPIO_Port */
#define MODEM_RST_Pin                   GPIO_PIN_10 /* MDM_RST_Pin */
#define MODEM_PWR_EN_GPIO_Port          GPIOA       /* MDM_PWR_EN_GPIO_Port */
#define MODEM_PWR_EN_Pin                GPIO_PIN_8  /* MDM_PWR_EN_Pin */
#error NUCLEO L476 BOARD not tested

#elif (defined(HWREF_B_CELL_BG96) || defined(HWREF_B_CELL_BG96_V2))
#define MODEM_RST_GPIO_Port             GPIOB       /* MDM_RST_GPIO_Port */
#define MODEM_RST_Pin                   GPIO_PIN_10 /* MDM_RST_Pin */
#define MODEM_PWR_EN_GPIO_Port          GPIOA       /* MDM_PWR_EN_GPIO_Port */
#define MODEM_PWR_EN_Pin                GPIO_PIN_8  /* MDM_PWR_EN_Pin */
#define MODEM_DTR_GPIO_Port             GPIOA       /* MDM_DTR_GPIO_Port */
#define MODEM_DTR_Pin                   GPIO_PIN_3  /* MDM_DTR_Pin */
#error NUCLEO L476 BOARD not tested

#else
#error HW Modem board not specified
#endif

#elif defined(STM32L475xx) /* B-L475E-IOT01 */
#if defined(HWREF_AKORIOT_BG96)
#define MODEM_RST_GPIO_Port             GPIOB       /* MDM_RST_GPIO_Port */
#define MODEM_RST_Pin                   GPIO_PIN_1  /* MDM_RST_Pin */
#define MODEM_PWR_EN_GPIO_Port          GPIOA       /* MDM_PWR_EN_GPIO_Port */
#define MODEM_PWR_EN_Pin                GPIO_PIN_4  /* MDM_PWR_EN_Pin */

#elif (defined(HWREF_B_CELL_BG96) || defined(HWREF_B_CELL_BG96_V2))
#if 0
//#define MODEM_RST_GPIO_Port             GPIOB       /* MDM_RST_GPIO_Port */
//#define MODEM_RST_Pin                   GPIO_PIN_1  /* MDM_RST_Pin */
//#define MODEM_PWR_EN_GPIO_Port          GPIOA       /* MDM_PWR_EN_GPIO_Port */
//#define MODEM_PWR_EN_Pin                GPIO_PIN_4  /* MDM_PWR_EN_Pin */
//#define MODEM_DTR_GPIO_Port             GPIOB       /* MDM_DTR_GPIO_Port */
//#define MODEM_DTR_Pin                   GPIO_PIN_4  /* MDM_DTR_Pin */
#else
/* USE ARDUINO TO STMOD ADAPTER BOARD */
/* PA15 = D9 = RESET */
#define MODEM_RST_GPIO_Port             GPIOA       /* MDM_RST_GPIO_Port */
#define MODEM_RST_Pin                   GPIO_PIN_15 /* MDM_RST_Pin */
/* PB4 = D5 = PWR EN */
#define MODEM_PWR_EN_GPIO_Port          GPIOB       /* MDM_PWR_EN_GPIO_Port */
#define MODEM_PWR_EN_Pin                GPIO_PIN_4  /* MDM_PWR_EN_Pin */
/* PB1 = D6 = DTR */
#define MODEM_DTR_GPIO_Port             GPIOB       /* MDM_DTR_GPIO_Port */
#define MODEM_DTR_Pin                   GPIO_PIN_1  /* MDM_DTR_Pin */
/* SIM SELECT see main.h and gpio.c
SIM-SEL0 PA4
SIM-SEL1 PA3
*/
#endif

#else
#error HW Modem board not specified
#endif

#else
#error Connector not specified
#endif

/* STM32-F7 link configuration - NOT USED */
#define PPPOS_LINK_UART_HANDLE   NULL
#define PPPOS_LINK_UART_INSTANCE NULL


/* Resource LED definition */
#if defined(STM32L476xx)
#define HTTP_LED_GPIO_PORT      GPIOA
#define HTTP_LED_PIN            GPIO_PIN_5
#define COAP_LED_GPIO_PORT      GPIOA /* LED2_GPIO_Port */
#define COAP_LED_PIN            GPIO_PIN_5 /* LED2_Pin */
#define LED_ON                  GPIO_PIN_SET
#define LED_OFF                 GPIO_PIN_RESET
#elif defined(STM32L475xx)
#define HTTP_LED_GPIO_PORT      GPIOB
#define HTTP_LED_PIN            GPIO_PIN_14
#define COAP_LED_GPIO_PORT      GPIOB /* LED2_GPIO_Port */
#define COAP_LED_PIN            GPIO_PIN_14 /* LED2_Pin */
#define LED_ON                  GPIO_PIN_SET
#define LED_OFF                 GPIO_PIN_RESET
#else
#error Connector not specified
#endif

/* Flash configuration   */
/* TODO : set correct values */
#if defined(STM32L476xx)
#define FLASH_LAST_PAGE_ADDR     ((uint32_t)0x080ff800) /* Base @ of Page 511, 2 Kbytes */
#define FLASH_LAST_PAGE_NUMBER     511
#elif defined(STM32L475xx)
#define FLASH_LAST_PAGE_ADDR     ((uint32_t)0x080ff800) /* Base @ of Page 511, 2 Kbytes */
#define FLASH_LAST_PAGE_NUMBER     511
#endif


/* PPPOSIF CLIENT LED definition */
#define PPPOSIF_CLIENT_LED_GPIO_PORT    GPIOB /* LED1_GPIO_Port */
#define PPPOSIF_CLIENT_LED_PIN          GPIO_PIN_14 /* LED1_Pin */

/* DEBUG INTERFACE CONFIGURATION */
#if defined(STM32L476xx)
extern UART_HandleTypeDef               huart2;
#define TRACE_INTERFACE_UART_HANDLE     huart2
#define TRACE_INTERFACE_INSTANCE        USART2
#elif defined(STM32L475xx)
extern UART_HandleTypeDef               huart1;
#define TRACE_INTERFACE_UART_HANDLE     huart1
#define TRACE_INTERFACE_INSTANCE        USART1
#else
#error Connector not specified
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PLF_HW_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
