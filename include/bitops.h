/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file bitops.h
 *
 * @brief This file is used to include the correct version of a library file. It will select the
 * correct compiler/hardware version of a file depending on certain macros defined within the
 * build environment.
 *
 * @author Liam Bucci
 * @date 6/10/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @ingroup BitOps
 *
 * @{
 */

// Include guard
#ifndef _BITOPS_H
#define _BITOPS_H

// Compiler check
#if defined(__XC16) || defined(__XC16__) || defined(XC16)
// 16-bit compiler in use
#include <bitops_xc16.h>


#elif defined(__XC8) || defined(__XC8__) || defined(XC8)
// 8-bit compiler in use
#include <bitops_xc8.h>


#else
#error "BITOPS: The bit operations library requires the XC16 or XC8 compilers!"
#endif // Compiler check

#endif //_BITOPS_H
