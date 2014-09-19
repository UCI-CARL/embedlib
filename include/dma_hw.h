/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file dma_hw.h
 *
 * @brief This file contains all the hardware specific definitions and macros needed by the DMA
 * library code.
 *
 * @author Liam Bucci
 * @date 6/18/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @addtogroup dma_channel
 *
 * @{
 */

// Include guard
#ifndef _DMA_HW_H
#define _DMA_HW_H

/**
 * @todo Add hardware specific if/elif/else statements.
 */

#define DMA_HW_NUMBER_OF_CHANNELS 8 /**< The number of DMA channels in this chip */

#define DMA_HW_BASE_ADDRESS_CH0 &DMA0CON /**< Base address for channel 0 */
#define DMA_HW_BASE_ADDRESS_CH1 &DMA1CON /**< Base address for channel 1 */
#define DMA_HW_BASE_ADDRESS_CH2 &DMA2CON /**< Base address for channel 2 */
#define DMA_HW_BASE_ADDRESS_CH3 &DMA3CON /**< Base address for channel 3 */
#define DMA_HW_BASE_ADDRESS_CH4 &DMA4CON /**< Base address for channel 4 */
#define DMA_HW_BASE_ADDRESS_CH5 &DMA5CON /**< Base address for channel 5 */
#define DMA_HW_BASE_ADDRESS_CH6 &DMA6CON /**< Base address for channel 6 */
#define DMA_HW_BASE_ADDRESS_CH7 &DMA7CON /**< Base address for channel 7 */

#endif //_DMA_HW_H

/**
 * @}
 */
