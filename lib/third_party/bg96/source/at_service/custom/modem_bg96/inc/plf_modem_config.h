/**
  ******************************************************************************
  * @file    plf_modem_config.h
  * @author  MCD Application Team
  * @brief   This file contains the modem configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLF_MODEM_CONFIG_H
#define __PLF_MODEM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/
#if defined(HWREF_AKORIOT_BG96)
/* explicitly defined: using BG96 AkorIot board */
#elif defined(HWREF_B_CELL_BG96_V2)
/* explicitly defined: using BG96 B_CELL_V2 board */
#else
/* default: no board specified -> using BG96 B_CELL_V2 board */
#define HWREF_B_CELL_BG96_V2
#endif

#if defined(HWREF_AKORIOT_BG96)
#define USE_MODEM_BG96
#define CONFIG_MODEM_UART_BAUDRATE (115200)
#define CONFIG_MODEM_UART_RTS_CTS  (0)
#define CONFIG_MODEM_USE_ARDUINO_CONNECTOR

#else /* default case is HWREF_B_CELL_BG96_V2 */
#define USE_MODEM_BG96
#define CONFIG_MODEM_UART_BAUDRATE (115200)
#define CONFIG_MODEM_UART_RTS_CTS  (0)
#define CONFIG_MODEM_USE_STMOD_CONNECTOR

#endif /* modem board choice */

/* PDP context parameters (for AT+CGDONT) BEGIN */
#define PDP_CONTEXT_DEFAULT_MODEM_CID          1   /* CID numeric value */
#define PDP_CONTEXT_DEFAULT_MODEM_CID_STRING  "1"  /* CID string value */
#define PDP_CONTEXT_DEFAULT_TYPE              "IP" /* defined in project config files */
#define PDP_CONTEXT_DEFAULT_APN               ""   /* defined in project config files */
/* PDP context parameters (for AT+CGDONT) END */

/* Pre-defined BG96 band configs(!!! flags are exclusive)
*  if no bands flag is set, we use actual modem settings without modification
*/
#define  BG96_BAND_CONFIG_CAT_M1_ONLY_FULL_BANDS          (0)
#define  BG96_BAND_CONFIG_CAT_NB1_ONLY_FULL_BANDS         (0)
#define  BG96_BAND_CONFIG_GSM_ONLY_FULL_BANDS             (0)
#define  BG96_BAND_CONFIG_CAT_M1_GSM_FALLBACK_FULL_BANDS  (0)
#define  BG96_BAND_CONFIG_CAT_NB1_GSM_FALLBACK_FULL_BANDS (0)
#define  BG96_BAND_CONFIG_DEFAULT                         (0)
/* Custom BG96 band configs (!!! pre-defined configs above have to be commented if using custom config) */
#define BG96_BAND_CONFIG_CUSTOM                   (0) /* customizable by user (modify config below */
#define BG96_BAND_CONFIG_VERIZON                  (0) /* pre-defined VERIZON config (M1 only, band 4 + band 13) */
#define BG96_BAND_CONFIG_VODAFONE                 (0) /* pre-defined VODAFONE config (NB1 only, band 8 + band 20) */
#define BG96_BAND_CONFIG_MOne                     (0) /* pre-defined M1 (sgp) config (NB1 only, band 8) */
#define BG96_BAND_CONFIG_Orange                   (0) /* pre-defined Orange (Fr) config (M1 only, band 3 & band 20) */

#if (BG96_BAND_CONFIG_CUSTOM == 1)
/* --- start user customization --- */
#define BG96_SET_BANDS    (1)
#define BG96_BAND_GSM     ((uint16_t) _QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t) _QCFGbandCatM1_B20)
#define BG96_BAND_CAT_NB1 ((uint64_t) _QCFGbandCatNB1_B20)
#define BG96_IOTOPMODE    ((uint16_t) _QCFGiotopmodeCatM1)
#define BG96_SCANSEQ      ((uint32_t) _QCFGscanseqAuto)
#define BG96_SCANMODE     ((uint16_t) _QCFGscanmodeAuto)
/* --- end user customization --- */
#elif (BG96_BAND_CONFIG_VERIZON == 1)
/* pre-defined VERIZON config - do not modify */
#define BG96_SET_BANDS    (1)
#define BG96_BAND_GSM     ((uint16_t) _QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t) ( _QCFGbandCatM1_B4 | _QCFGbandCatM1_B13 ))
#define BG96_BAND_CAT_NB1 ((uint64_t) _QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint16_t) _QCFGiotopmodeCatM1)
#define BG96_SCANSEQ      ((uint32_t) _QCFGscanseq_M1_NB1_GSM)
#define BG96_SCANMODE     ((uint16_t) _QCFGscanmodeAuto)
#elif (BG96_BAND_CONFIG_VODAFONE == 1)
/* pre-defined VODAFONE config - do not modify */
#define BG96_SET_BANDS    (1)
#define BG96_BAND_GSM     ((uint16_t) _QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t) _QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t) (_QCFGbandCatNB1_B8 | _QCFGbandCatNB1_B20))
#define BG96_IOTOPMODE    ((uint16_t) _QCFGiotopmodeCatNB1)
#define BG96_SCANSEQ      ((uint32_t) _QCFGscanseq_NB1_M1_GSM)
#define BG96_SCANMODE     ((uint16_t) _QCFGscanmodeAuto)
#elif (BG96_BAND_CONFIG_MOne == 1)
/* pre-defined M1 (sgp) config - do not modify */
#define BG96_SET_BANDS    (1)
#define BG96_BAND_GSM     ((uint16_t) _QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t) _QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t) _QCFGbandCatNB1_B8)
#define BG96_IOTOPMODE    ((uint16_t) _QCFGiotopmodeCatNB1)
#define BG96_SCANSEQ      ((uint32_t) _QCFGscanseq_NB1_M1_GSM)
#define BG96_SCANMODE     ((uint16_t) _QCFGscanmodeAuto)
#elif (BG96_BAND_CONFIG_Orange == 1)
/* pre-defined Orange (Fr) config - do not modify */
#define BG96_SET_BANDS    (1)
#define BG96_BAND_GSM     ((uint16_t) _QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t) ( _QCFGbandCatM1_B3 | _QCFGbandCatM1_B20 ))
#define BG96_BAND_CAT_NB1 ((uint64_t) _QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint16_t) _QCFGiotopmodeCatM1)
#define BG96_SCANSEQ      ((uint32_t) _QCFGscanseq_M1_NB1_GSM)
#define BG96_SCANMODE     ((uint16_t) _QCFGscanmodeAuto)
/* pre-defined configs */
#elif (BG96_BAND_CONFIG_CAT_M1_ONLY_FULL_BANDS == 1)
#define BG96_SET_BANDS    (1)
#elif (BG96_BAND_CONFIG_CAT_NB1_ONLY_FULL_BANDS == 1)
#define BG96_SET_BANDS    (1)
#elif (BG96_BAND_CONFIG_GSM_ONLY_FULL_BANDS == 1)
#define BG96_SET_BANDS    (1)
#elif (BG96_BAND_CONFIG_CAT_M1_GSM_FALLBACK_FULL_BANDS == 1)
#define BG96_SET_BANDS    (1)
#elif (BG96_BAND_CONFIG_CAT_NB1_GSM_FALLBACK_FULL_BANDS == 1)
#define BG96_SET_BANDS    (1)
#elif (BG96_BAND_CONFIG_DEFAULT == 1)
#define BG96_SET_BANDS    (1)
#else
/* IF FALL HERE, MODEM BAND CONFIG NOT MODIFIED */
#define BG96_SET_BANDS    (0)
#endif


/* Power saving mode settings (PSM)
*  should we send AT+CPSMS write command ?
*/
#define BG96_SEND_READ_CPSMS        (0)
#define BG96_SEND_WRITE_CPSMS       (0)
#if (BG96_SEND_WRITE_CPSMS == 1)
#define BG96_ENABLE_PSM             (1) /* 1 if enabled, 0 if disabled */
#define BG96_CPSMS_REQ_PERIODIC_TAU ("00000100")  /* refer to AT commands reference manual v2.0 to set timer values */
#define BG96_CPSMS_REQ_ACTIVE_TIME  ("00001111")  /* refer to AT commands reference manual v2.0 to set timer values */
#else
#define BG96_ENABLE_PSM             (0)
#endif /* BG96_SEND_WRITE_CPSMS */

/* e-I-DRX setting (extended idle mode DRX)
* should we send AT+CEDRXS write command ?
*/
#define BG96_SEND_READ_CEDRXS       (0)
#define BG96_SEND_WRITE_CEDRXS      (0)
#if (BG96_SEND_WRITE_CEDRXS == 1)
#define BG96_ENABLE_E_I_DRX         (1) /* 1 if enabled, 0 if disabled */
#define BG96_CEDRXS_ACT_TYPE        (5) /* 1 for Cat.M1, 5 for Cat.NB1 */
#else
#define BG96_ENABLE_E_I_DRX         (0)
#endif /* BG96_SEND_WRITE_CEDRXS */

/* Network Information */
#define BG96_OPTION_NETWORK_INFO    (1)  /* 1 if enabled, 0 if disabled */

/* Engineering Mode */
#define BG96_OPTION_ENGINEERING_MODE    (0)  /* 1 if enabled, 0 if disabled */

#endif /* __PLF_MODEM_CONFIG_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
