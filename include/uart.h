/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file uart.h
 *
 * @brief This file is used to include the correct version of the uart library files. It will
 * select the correct file depending on the compiler/hardware and set macros which will be
 * used within the code to set up the hardware correctly.
 *
 * @author Liam Bucci
 * @date 6/14/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @ingroup uart_module
 *
 * @{
 */

// Include guard
#ifndef _UART_H
#define _UART_H

// Compiler Check
#if defined(__XC16) || defined(__XC16__) || defined(XC16)
// 16-bit compiler in use

#include <uart_xc16.h>

#else
#error "UART: Unknown compiler!"
#endif // Compiler check

#endif //_UART_H

/**
 * @}
 */
