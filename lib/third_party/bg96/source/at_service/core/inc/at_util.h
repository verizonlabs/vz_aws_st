/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\inc\at_util.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for utilities
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

#ifndef AT_UTIL_H_
#define AT_UTIL_H_

#include "plf_config.h"

uint32_t ATutil_ipow(uint32_t base, uint32_t exp);
uint32_t ATutil_convertStringToInt(uint8_t *p_string, uint32_t size);
uint64_t ATutil_convertStringToInt64(uint8_t *p_string, uint32_t size);
uint64_t ATutil_convertHexaStringToInt64(uint8_t *p_string, uint32_t size);

#endif /* AT_UTIL_H_ */
