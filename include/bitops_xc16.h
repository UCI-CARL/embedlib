/* -*- mode: c; tab-width: 4; -*- */
/**
 * @file bitops.h
 *
 * @brief This file contains various bit operations, both in preprocessor macro form and function
 * form.
 *
 * @author     Liam Bucci
 * @date       12/14/2012
 * @carlnumber FIRM-0004
 * @version    0.3.0
 */



// Set bit
#define BIT_SET(in,bit)           ((in)|(1<<(bit)))
// Clear bit
#define BIT_CLEAR(in,bit)         ((in)&(~(1<<(bit))))
// Set bits
#define BITS_SET(in,mask)         ((in)|(mask))
// Clear bits
#define BITS_CLEAR(in,mask)       ((in)&(~(mask)))
// Insert value
#define BITS_INSERT(in,mask,val)  (((in)&(~(mask)))|((val)&(mask)))

