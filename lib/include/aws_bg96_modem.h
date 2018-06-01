/*
 * Copyright (C) 2018 pureIntegration. All Rights Reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://pureintegration.com
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

/* Message queue which signals modem status */

osMessageQId BG96_modem_queue;

/**
 * @brief Initialize the BG96 HAL.
 *
 * This function initializes the low level hardware drivers and must be called
 * before calling any other modem API
 *
 */

void BG96_HAL_Init(void);

/**
 * @brief Initialize the BG96 drivers.
 *
 * This function initializes the low level drivers and must be called
 * before calling BG96_Modem_Start
 *
 */

void BG96_Modem_Init(void);

/**
 * @brief Starts the BG96 drivers.
 *
 * This function starts the low level  drivers and must be called last in the sequence
 * e.g. after BG96_HAL_Init and BG96_Modem_Init
 *
 */

void BG96_Modem_Start (void);

#endif /* __AWS_BG96_MODEM_H_ */
