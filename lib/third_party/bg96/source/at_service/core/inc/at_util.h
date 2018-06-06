/**
  ******************************************************************************
  * @file    Application\at_service\core\inc\at_util.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for utilities
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
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
