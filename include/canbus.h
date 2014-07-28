/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file canbus.h
 *
 * @brief This file is used to include the correct version of the canbus library files. It will
 * select the correct file depending on the compiler/hardware and set macros which will be
 * used within the code to set up the hardware correctly.
 *
 * @author Liam Bucci
 * @date 7/25/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @ingroup canbus
 *
 * @{
 */

// Include guard
#ifndef CANBUS_H_
#define CANBUS_H_

// Compiler Check
#if defined(__XC16) || defined(__XC16__) || defined(XC16)
// 16-bit compiler in use

#include <canbus_xc16.h>

#else
#error "CANBUS: Unknown compiler!"
#endif // Compiler check

#endif //CANBUS_H_

/**
 * @}
 */
