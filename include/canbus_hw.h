/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file canbus_hw.h
 *
 * @brief This file defines hardware specific macros for the canbus library. It should only be called
 * from within the canbus library.
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

#ifndef CANBUS_HW_H_
#define CANBUS_HW_H_

/* ***** Define Hardware Specific Constants ***** */

#if defined(__dsPIC33FJ128MC802__)
#define CANBUS_HW_NUMBER_OF_MODULES 1 /**< Number of CAN bus hardware modules on this chip. */
#endif

#if defined(__dsPIC33FJ128MC804__)
#define CANBUS_HW_NUMBER_OF_MODULES 1 /**< Number of CAN bus hardware modules on this chip. */
#endif

/* Set Base Addresses */

#define CANBUS_HW_BASE_ADDRESS_C1 &C1CTRL1 /**< Base address for C1 */
#define CANBUS_HW_BASE_ADDRESS_C2 &C2CTRL1 /**< Base address for C2 */



#endif //CANBUS_HW_H_

/**
 * @}
 */
