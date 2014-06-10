/* -*- mode: c; tab-width: 4; -*- */
/**
 * @file bitops_xc16.h
 *
 * @brief This file contains various bit operations, both in preprocessor macro form and function
 * form.
 *
 * @author     Liam Bucci
 * @date       12/14/2012
 * @carlnumber FIRM-0009
 * @version    0.3.0
 */

// Include guard
#ifndef _BITOPS_XC16_H
#define _BITOPS_XC16_H

/**
 * @brief Set the nth bit of the given value.
 *
 * @details Retains all other bits in the given value, but sets the nth bit to a one.
 *
 * @param[in]  val
 *             The value to modify. May be any built in type length.
 * @param[in]  n
 *             The place of the bit to set. Must be between 0-16.
 * @returns The new value with the nth bit set.
 *
 * @public
 */
#define BIT_SET(val,n)           ((val)|(1<<(n)))

#define WRITE_BIT_SET(val,n)     (val) = BIT_SET((val),(n))

/**
 * @brief Clear the nth bit of the given value.
 *
 * @details Retains all other bits in the given value, but clears the nth bit to a zero.
 *
 * @param[in]  val
 *             The value to modify. May be any built in type length.
 * @param[in]  n
 *             The place of the bit to set. Must be between 0-16.
 * @returns The new value with the nth bit cleared.
 *
 * @public
 */
#define BIT_CLEAR(val,n)         ((val)&(~(1<<(n))))

#define WRITE_BIT_CLEAR(val,n)   (val) = BIT_CLEAR((val),(n))

/**
 * @brief Set all bits marked by bitmask.
 *
 * @details Retain all other bits in the given value, but set all bits marked by the given bitmask.
 * If a bit in the bitmask is one the resulting bit in the returned value will be a one regardless
 * of it's previous state. If a bit in the bitmask is zero the resulting bit in the returned value
 * will be the same as it was originally.
 *
 * @param[in]  val
 *             The value to modify. May be any built in type length.
 * @param[in]  mask
 *             The bitmask defining which bits to set. May be any built in type length.
 * @returns The new value with the requested bits set.
 *
 * @public
 */
#define MASK_SET(val,mask)         ((val)|(mask))

#define WRITE_MASK_SET(val,mask)   (val) = MASK_SET((val),(mask))

/**
 * @brief Clear all bits marked by bitmask.
 *
 * @details Retain all other bits in the given value, but clear all bits marked by the given
 * bitmask. If a bit in the bitmask is one the resulting bit in the returned value will be a zero
 * regardless of it's previous state. If a bit in the bitmask is zero the resulting bit in the
 * returned value will be the same as it was originally.
 *
 * @param[in]  val
 *             The value to modify. May be any built in type length.
 * @param[in]  mask
 *             The bitmask defining which bits to clear. May be any built in type length.
 * @returns The new value with the requested bits cleared.
 *
 * @public
 */
#define MASK_CLEAR(val,mask)       ((val)&(~(mask)))

#define WRITE_MASK_CLEAR(val,mask) (val) = MASK_CLEAR((val),(mask))

/**
 * @brief Insert specific bits into the given value.
 *
 * @details Insert the bit values from @em insert and masked by @em mask into the given @em value.
 * If a bit in the mask is a one then the corresponding bit in the @em insert value will overwrite
 * the corresponding original bit value. If a bit in the mask is a zero the original corresponding
 * bit will remain unchanged.
 *
 * @param[in]  val
 *             The value to modify. May be any built in type length.
 * @param[in]  mask
 *             The bitmask defining which bits of the new @em insert value should overwrite the
 *             original values.
 * @param[in]  insert
 *             The value to be masked and inserted into the original value.
 * @returns The new value with the requested bits inserted into it.
 *
 * @public
 */
#define BITS_INSERT(val,mask,insert)  (((val)&(~(mask)))|((insert)&(mask)))

#define WRITE_BITS_INSERT(val,mask,insert) (val) = BITS_INSERT((val),(mask),(insert))

#define IS_BIT_SET(val,n)          ( ((val)&(1<<(n))) != 0 )

#define IS_BIT_CLEAR(val,n)        ( ((~(val))&(1<<(n))) != 0 )

#define IS_MASK_SET(val,mask)      ( ((val)&(mask)) != 0 )

#define IS_MASK_CLEAR(val,mask)    ( ((~(val))&(mask)) != 0 )

// End include guard
#endif //_BITOPS_XC16_H
