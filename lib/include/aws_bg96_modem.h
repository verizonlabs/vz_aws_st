/*

 */


/**
 * @file aws_BG96_modem.h
 * @brief BG96 modem interface.
 */

#ifndef _AWS_BG96_MODEM_H_
#define _AWS_BG96_MODEM_H_

#include <stdint.h>
#include "cmsis_os.h"


typedef enum {
	network_is_up = 1,
	network_is_down = 0
} BG96_network_status_t;

BG96_network_status_t BG96_network_status = network_is_down;

osMessageQId BG96_modem_queue;


/**
 * @brief Return code from various APIs.
 */


/**
 * @brief Initialize the BG96 HAL.
 *
 * This function initializes the low level hardware drivers and must be called
 * before calling any other modem API
 *
 */

void BG96_HAL_Init(void);

void BG96_Modem_Init(void);

void BG96_Modem_Start (void);

#endif /* __AWS_BG96_MODEM_H_ */
