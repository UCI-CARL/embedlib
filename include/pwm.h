/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file pwm.h
 *
 * @brief This file is used to include the correct version of the pwm library files. It will
 * select the correct file depending on the compiler/hardware and set macros which will be
 * used within the code to set up the hardware correctly.
 *
 * @author Liam Bucci
 * @date 8/15/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @ingroup pwm
 *
 * @{
 */

// Include guard
#ifndef PWM_H_
#define PWM_H_

// Compiler Check
#if defined(__XC16) || defined(__XC16__) || defined(XC16)
// 16-bit compiler in use

#include <pwm_xc16.h>

#else
#error "PWM: Unknown compiler!"
#endif // Compiler check

#endif //PWM_H_

/**
 * @}
 */
