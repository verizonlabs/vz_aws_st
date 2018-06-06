/**
  ******************************************************************************
  * @file    Application\at_service\custom\modem_bg96\inc\at_custom_modem_api.h
  * @author  MCD Application Team
  * @brief   This file defines all the parameters specific to the
  *          BG96 Quectel modem (3G)
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef AT_CUSTOM_MODEM_API_H
#define AT_CUSTOM_MODEM_API_H

#include "at_core.h"
#include "at_custom_common.h"
#include "sysctrl.h"

/* Exported functions ------------------------------------------------------- */

at_status_t      atcma_init_at_func_ptrs(atcustom_funcPtrs_t *funcPtrs);
sysctrl_status_t atcma_init_sysctrl_func_ptrs(sysctrl_funcPtrs_t *funcPtrs);

#endif /* AT_CUSTOM_MODEM_API_H */
