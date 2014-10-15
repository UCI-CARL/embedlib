/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file uart_hw.h
 *
 * @brief This file defines hardware specific macros for the UART library. It should only be called
 * from within the UART library.
 *
 * @author Liam Bucci
 * @date 6/14/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @ingroup uart
 *
 * @{
 */

#ifndef _UART_HW_H
#define _UART_HW_H

/* ***** Define Hardware Specific Constants ***** */

#if defined(__dsPIC33FJ128MC802__)
#define UART_HW_NUMBER_OF_MODULES 2 /**< Number of UART hardware modules on this chip. */
#endif

#if defined(__dsPIC33FJ128MC804__)
#define UART_HW_NUMBER_OF_MODULES 2 /**< Number of UART hardware modules on this chip. */
#endif

#if defined(__PIC24EP512GU810__)
#define UART_HW_NUMBER_OF_MODULES 4 /**< Number of UART hardware modules on this chip. */
#endif

/* Set Base Addresses */

#define UART_HW_BASE_ADDRESS_UART1 &U1MODE /**< Base address for UART 1 */
#define UART_HW_BASE_ADDRESS_UART2 &U2MODE /**< Base address for UART 2 */
#define UART_HW_BASE_ADDRESS_UART3 &U3MODE /**< Base address for UART 3 */
#define UART_HW_BASE_ADDRESS_UART4 &U4MODE /**< Base address for UART 4 */

/* Set Baudrate Constants */

// _FCY_ must be defined to set baudrate, unless manually setting baudrate
#if !defined(UART_DEF_MANUAL_BAUDRATE) && !defined(_FCY_)
    #error "UART: Must supply _FCY_ in order to set baudrate correctly, or manually set baudrate."
#endif

// Define BRGH value
#define UART_HW_BRGH 1

// Define BRG values depending on Fcy
#if _FCY_ == 2000000 // Fcy is 2MHz
#define UART_HW_BRG_1200 415
#define UART_HW_BRG_2400 207
#define UART_HW_BRG_4800 103
#define UART_HW_BRG_9600 51
#define UART_HW_BRG_19200 25
#define UART_HW_BRG_38400 12
#define UART_HW_BRG_57600 7
#define UART_HW_BRG_115200 3
#define UART_HW_BRG_230400 1
#define UART_HW_BRG_460800 0
#define UART_HW_BRG_921600 0
#define UART_HW_BRG_1000000 0
#define UART_HW_BRG_1843200 0
#define UART_HW_BRG_2000000 0
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 3000000 // Fcy is 3MHz
#define UART_HW_BRG_1200 624
#define UART_HW_BRG_2400 311
#define UART_HW_BRG_4800 155
#define UART_HW_BRG_9600 77
#define UART_HW_BRG_19200 38
#define UART_HW_BRG_38400 18
#define UART_HW_BRG_57600 12
#define UART_HW_BRG_115200 5
#define UART_HW_BRG_230400 2
#define UART_HW_BRG_460800 0
#define UART_HW_BRG_921600 0
#define UART_HW_BRG_1000000 0
#define UART_HW_BRG_1843200 0
#define UART_HW_BRG_2000000 0
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 4000000 // Fcy is 4MHz
#define UART_HW_BRG_1200 832
#define UART_HW_BRG_2400 415
#define UART_HW_BRG_4800 207
#define UART_HW_BRG_9600 103
#define UART_HW_BRG_19200 51
#define UART_HW_BRG_38400 25
#define UART_HW_BRG_57600 16
#define UART_HW_BRG_115200 7
#define UART_HW_BRG_230400 3
#define UART_HW_BRG_460800 1
#define UART_HW_BRG_921600 0
#define UART_HW_BRG_1000000 0
#define UART_HW_BRG_1843200 0
#define UART_HW_BRG_2000000 0
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 6000000 // Fcy is 6MHz
#define UART_HW_BRG_1200 1249
#define UART_HW_BRG_2400 624
#define UART_HW_BRG_4800 311
#define UART_HW_BRG_9600 155
#define UART_HW_BRG_19200 77
#define UART_HW_BRG_38400 38
#define UART_HW_BRG_57600 25
#define UART_HW_BRG_115200 12
#define UART_HW_BRG_230400 5
#define UART_HW_BRG_460800 2
#define UART_HW_BRG_921600 0
#define UART_HW_BRG_1000000 0
#define UART_HW_BRG_1843200 0
#define UART_HW_BRG_2000000 0
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 10000000 // Fcy is 10MHz
#define UART_HW_BRG_1200 2082
#define UART_HW_BRG_2400 1040
#define UART_HW_BRG_4800 519
#define UART_HW_BRG_9600 259
#define UART_HW_BRG_19200 129
#define UART_HW_BRG_38400 64
#define UART_HW_BRG_57600 42
#define UART_HW_BRG_115200 20
#define UART_HW_BRG_230400 9
#define UART_HW_BRG_460800 4
#define UART_HW_BRG_921600 1
#define UART_HW_BRG_1000000 1
#define UART_HW_BRG_1843200 0
#define UART_HW_BRG_2000000 0
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 12000000 // Fcy is 12MHz
#define UART_HW_BRG_1200 2499
#define UART_HW_BRG_2400 1249
#define UART_HW_BRG_4800 624
#define UART_HW_BRG_9600 311
#define UART_HW_BRG_19200 155
#define UART_HW_BRG_38400 77
#define UART_HW_BRG_57600 51
#define UART_HW_BRG_115200 25
#define UART_HW_BRG_230400 12
#define UART_HW_BRG_460800 5
#define UART_HW_BRG_921600 2
#define UART_HW_BRG_1000000 2
#define UART_HW_BRG_1843200 0
#define UART_HW_BRG_2000000 0
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 20000000 // Fcy is 20MHz
#define UART_HW_BRG_1200 4165
#define UART_HW_BRG_2400 2082
#define UART_HW_BRG_4800 1040
#define UART_HW_BRG_9600 519
#define UART_HW_BRG_19200 259
#define UART_HW_BRG_38400 129
#define UART_HW_BRG_57600 85
#define UART_HW_BRG_115200 42
#define UART_HW_BRG_230400 20
#define UART_HW_BRG_460800 9
#define UART_HW_BRG_921600 4
#define UART_HW_BRG_1000000 4
#define UART_HW_BRG_1843200 1
#define UART_HW_BRG_2000000 1
#define UART_HW_BRG_3686400 0
#elif _FCY_ == 40000000 // Fcy is 40MHz
#define UART_HW_BRG_1200 8332
#define UART_HW_BRG_2400 4165
#define UART_HW_BRG_4800 2082
#define UART_HW_BRG_9600 1040
#define UART_HW_BRG_19200 519
#define UART_HW_BRG_38400 259
#define UART_HW_BRG_57600 172
#define UART_HW_BRG_115200 85
#define UART_HW_BRG_230400 42
#define UART_HW_BRG_460800 20
#define UART_HW_BRG_921600 9
#define UART_HW_BRG_1000000 9
#define UART_HW_BRG_1843200 4
#define UART_HW_BRG_2000000 4
#define UART_HW_BRG_3686400 1
#else // Unknown Fcy
#error "UART: Fcy is unknown and the baudrate should be set manually."
#endif

#endif //_UART_HW_H

/**
 * @}
 */
