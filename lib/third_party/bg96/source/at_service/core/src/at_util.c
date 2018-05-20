/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\src\at_util.c
  * @author  MCD Application Team
  * @brief   This file provides code for atcore utilities
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

#include "plf_config.h"
#include "at_util.h"
#include "string.h"


uint32_t ATutil_ipow(uint32_t base, uint32_t exp)
{
    /* implementation of power function */
    uint32_t result = 1;
    while (exp)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }

    return result;
}

uint32_t ATutil_convertStringToInt(uint8_t *p_string, uint32_t size)
{
    uint16_t idx, nb_digit_ignored = 0, loop = 0;
    uint32_t conv_nbr = 0;

    /* auto-detect if this is an hexa value (format: 0x....) */
    if (p_string[1] == 120)
    {
        /* check if the value exceed maximum size */
        if (size > 10)
        {
            /* PrintErr("String to convert exceed maximum size"); */
            return(0);
        }

       /* hexa value */
        nb_digit_ignored = 2;
        for (idx=2; idx < size; idx++)
        {
            /* consider only the numbers */
            if ((p_string[idx] >= 48) && (p_string[idx] <= 57))
            {
                /* 0 to 9 */
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 48) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
            }
            else if ((p_string[idx] >= 97) && (p_string[idx] <= 102))
            {
                /* a to f */
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 97 + 10) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
            }
            else if ((p_string[idx] >= 65) && (p_string[idx] <= 70))
            {
                /* A to F */
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 65 + 10) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
            }
            else
            {
                nb_digit_ignored++;
            }
        }
    }
    else
    {
        /* decimal value */
        for (idx=0; idx < size; idx++)
        {
            /* consider only the numbers */
            if ((p_string[idx] >= 48) && (p_string[idx] <= 57))
            {
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 48) * ATutil_ipow(10, (size - loop - nb_digit_ignored)));
            }
            else
            {
                nb_digit_ignored++;
            }
        }
    }

    return(conv_nbr);
}

uint64_t ATutil_convertStringToInt64(uint8_t *p_string, uint32_t size)
{
    uint16_t idx, nb_digit_ignored = 0, loop = 0;
    uint64_t conv_nbr = 0;

    /* auto-detect if this is an hexa value (format: 0x....) */
    if (p_string[1] == 120)
    {
        /* check if the value exceed maximum size */
        if (size > 18)
        {
            /* PrintErr("String to convert exceed maximum size"); */
            return(0);
        }

       /* hexa value */
        nb_digit_ignored = 2;
        for (idx=2; idx < size; idx++)
        {
            /* consider only the numbers */
            if ((p_string[idx] >= 48) && (p_string[idx] <= 57))
            {
                /* 0 to 9 */
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 48) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
            }
            else if ((p_string[idx] >= 97) && (p_string[idx] <= 102))
            {
                /* a to f */
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 97 + 10) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
            }
            else if ((p_string[idx] >= 65) && (p_string[idx] <= 70))
            {
                /* A to F */
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 65 + 10) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
            }
            else
            {
                nb_digit_ignored++;
            }

        }
    }
    else
    {
        /* decimal value */
        for (idx=0; idx < size; idx++)
        {
            /* consider only the numbers */
            if ((p_string[idx] >= 48) && (p_string[idx] <= 57))
            {
                loop++;
                conv_nbr = conv_nbr + ((p_string[idx] - 48) * ATutil_ipow(10, (size - loop - nb_digit_ignored)));
            }
            else
            {
                nb_digit_ignored++;
            }
        }
    }

    return(conv_nbr);
}

uint64_t ATutil_convertHexaStringToInt64(uint8_t *p_string, uint32_t size)
{
    uint16_t idx, nb_digit_ignored = 0, loop = 0;
    uint64_t conv_nbr = 0;
    uint16_t max_str_size;

    /* this function assumes that the string value is an hexadecimal value with or without Ox prefix */

    /* auto-detect if 0x is present */
    if (p_string[1] == 120)
    {
        /* 0x is present */
        max_str_size = 18;
        nb_digit_ignored = 2;
    }
    else
    {
        /* 0x is not present */
        max_str_size = 16;
        nb_digit_ignored = 0;
    }


    /* check if the value exceed maximum size */
    if (size > max_str_size)
    {
        /* PrintErr("String to convert exceed maximum size %d", max_str_size); */
        return(0);
    }

    /* hexa value */
    for (idx=nb_digit_ignored; idx < size; idx++)
    {
        /* consider only the numbers */
        if ((p_string[idx] >= 48) && (p_string[idx] <= 57))
        {
            /* 0 to 9 */
            loop++;
            conv_nbr = conv_nbr + ((p_string[idx] - 48) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
        }
        else if ((p_string[idx] >= 97) && (p_string[idx] <= 102))
        {
            /* a to f */
            loop++;
            conv_nbr = conv_nbr + ((p_string[idx] - 97 + 10) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
        }
        else if ((p_string[idx] >= 65) && (p_string[idx] <= 70))
        {
            /* A to F */
            loop++;
            conv_nbr = conv_nbr + ((p_string[idx] - 65 + 10) * ATutil_ipow(16, (size - loop - nb_digit_ignored)));
        }
        else
        {
            nb_digit_ignored++;
        }

    }

    return(conv_nbr);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
