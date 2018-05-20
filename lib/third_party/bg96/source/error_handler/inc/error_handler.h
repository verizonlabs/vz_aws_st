/**
  ******************************************************************************
  * File Name          : error_handler.h
  * Description        : This file contains error define utilities
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
