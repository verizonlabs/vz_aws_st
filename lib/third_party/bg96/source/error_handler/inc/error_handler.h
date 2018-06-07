/**
  ******************************************************************************
  * @file    error_handler.h
  * @author  MCD Application Team
  * @brief   This file contains error define utilities
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "stdint.h"

typedef enum {
  DBG_CHAN_GENERIC = 0,
  DBG_CHAN_MAIN,
  DBG_CHAN_ATCMD,
  DBG_CHAN_COM,
  DBG_CHAN_HTTP,
  DBG_CHAN_IPC,
  DBG_CHAN_COAP,
  DBG_CHAN_PPPOSIF,
  DBG_CHAN_CELLULAR_SERVICE,
  DBG_CHAN_NIFMAN,
  DBG_CHAN_DATA_CACHE,
  DBG_CHAN_UTILITIES,
  DBG_CHAN_ERROR_LOGGER,
  DBG_CHAN_MAX_VALUE        /* keep last */
} dbg_channels_t;

typedef enum {
  ERROR_NO = 0,
  ERROR_DEBUG,
  ERROR_WARNING,
  ERROR_FATAL,
} error_gravity_t;

typedef struct
{
    dbg_channels_t  channel; /* channel where error occured */
    int             errorId; /* number identifying the error in the channel */
    error_gravity_t gravity; /* error gravity */
    uint32_t        count;   /* count how many errors have been logged since the beginning */

} error_handler_decript_t;

void ERROR_Handler_Init(void);
void ERROR_Handler(dbg_channels_t chan, int errorId, error_gravity_t gravity);
void ERROR_Dump_All(void);
void ERROR_Dump_Last(void);

#endif /* ERROR_HANDLER_H */
