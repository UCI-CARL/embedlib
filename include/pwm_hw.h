/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file pwm_hw.h
 *
 * @brief This file defines hardware specific macros for the pwm library. It should only be called
 * from within the pwm library.
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

#ifndef PWM_HW_H_
#define PWM_HW_H_

/* ***** Define Hardware Specific Constants ***** */

#if defined(__dsPIC33FJ128MC802__)
#define PWM_HW_NUMBER_OF_MODULES 2 /**< Number of PWM hardware modules on this chip. */
#define PWM_HW_MCPWM1_CHANNELS 3 /**< Number of PWM channels in MCPWM1 module. */
#undef PWM_HW_LOCKED /**< No PWMLOCK bit or PWMKEY register. */
#endif

#if defined(__dsPIC33FJ128MC804__)
#define PWM_HW_NUMBER_OF_MODULES 2 /**< Number of PWM hardware modules on this chip. */
#define PWM_HW_MCPWM1_CHANNELS 3 /**< Number of PWM channels in MCPWM1 module. */
#undef PWM_HW_LOCKED /**< No PWMLOCK bit or PWMKEY register. */
#endif


#endif //PWM_HW_H_

/**
 * @}
 */
