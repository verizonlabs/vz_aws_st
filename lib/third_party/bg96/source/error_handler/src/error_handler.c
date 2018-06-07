/**
  ******************************************************************************
  * @file    error_handler.c
  * @author  MCD Application Team
  * @brief   This file contains error utilities
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#include "error_handler.h"
#include "plf_config.h"
#include "cmsis_os.h"


#if (USE_TRACE_INTERFACE_ERROR_HANDLER == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_ERROR_LOGGER, DBL_LVL_P0, "" format "\r\n", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_ERROR_LOGGER, DBL_LVL_1, "" format "\r\n", ## args)
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("EH:" format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("EH:" format "\r\n", ## args)
#elif (USE_CONFIGPRINTF_ERROR_HANDLER == 1)
#define PrintINFO(format, args...)  vLoggingPrintf("EH:" format "\r\n", ## args)
#define PrintDBG(format, args...)   vLoggingPrintf("EH:" format "\r\n", ## args)
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#endif

#define MAX_ERROR_ENTRIES (10)     /* log only last MAX_ERROR_ENTRIES errors */
#define MAX_ERROR_COUNTER (0xFFFF) /* count how many errors have been logged since the beginning */

static error_handler_decript_t errors_table[MAX_ERROR_ENTRIES]; /* error array */
static uint16_t error_index = 0; /* current error index */
static uint16_t error_counter = 0; /* total number of errors */

void ERROR_Handler_Init(void)
{
    /* initialize error array */
    for (uint32_t i=0; i<MAX_ERROR_ENTRIES; i++)
    {
        errors_table[i].channel = DBG_CHAN_ERROR_LOGGER; /* default value = self (ie no error) */
        errors_table[i].errorId = 0;
        errors_table[i].gravity = ERROR_NO;
    }

}

void ERROR_Handler(dbg_channels_t chan, int errorId, error_gravity_t gravity)
{
    /* if this is the very first error, init error array */
    if (error_counter == 0)
    {
        ERROR_Handler_Init();
    }

    /* log the error */
    error_counter = (error_counter + 1) % MAX_ERROR_COUNTER;
    errors_table[error_index].count = error_counter;
    errors_table[error_index].channel = chan;
    errors_table[error_index].errorId = errorId;
    errors_table[error_index].gravity = gravity;

    PrintINFO("LOG ERROR #%d: channel=%d / errorId=%d / gravity=%d", error_counter, chan, errorId, gravity);

    /* endless loop if error is fatal */

    if(gravity == ERROR_FATAL)
    {
    	BSP_LED_Toggle( LED_GREEN );
        HAL_Delay(1000);
        NVIC_SystemReset();
        while(1);
    }

    /* increment error index */
    error_index = (error_index + 1)%  MAX_ERROR_ENTRIES;
}

void ERROR_Dump_All(void)
{
    if (error_counter)
    {
        /* Dump errors array */
        for (uint16_t i=0; i<MAX_ERROR_ENTRIES; i++)
        {
            if (errors_table[i].gravity != ERROR_NO)
            {
                PrintINFO("DUMP ERROR[%u (#%u): channel=%d / errorId=%d / gravity=%d",
                		  i,
						  (uint16_t)errors_table[i].count,
                          errors_table[i].channel,
                          errors_table[i].errorId,
                          errors_table[i].gravity );
            }
        }
    }
}

void ERROR_Dump_Last(void)
{
    if (error_counter)
    {
        /* get last error index */
    	uint16_t previous_index = (error_index + (MAX_ERROR_ENTRIES - 1)) % MAX_ERROR_ENTRIES;

        PrintINFO("DUMP LAST ERROR[%u] (#%u): channel=%d / errorId=%d / gravity=%d",
                  previous_index,
				  (uint16_t)errors_table[previous_index].count,
                  errors_table[previous_index].channel,
                  errors_table[previous_index].errorId,
                  errors_table[previous_index].gravity );
    }
}
