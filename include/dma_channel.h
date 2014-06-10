/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file dma_channel.h
 *
 * @brief This file is used to include the correct version of a library file. It will select the
 * correct compiler/hardware version of a file depending on certain macros defined within the
 * build environment.
 *
 * @author Liam Bucci
 * @date 6/9/2014
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

/**
 * @ingroup DMAChannel
 *
 * @{
 */

// Include guard
#ifndef _DMA_CHANNEL_H
#define _DMA_CHANNEL_H

// Compiler check
#if defined(__XC16) || defined(__XC16__) || defined(XC16)
// 16-bit compiler in use

// DMA check
#if defined(__HAS_DMA__) || defined(__HAS_DMA_DMAV2__)

// Chip has DMA available
#include <dma_channel_xc16.h>

#else
#error "DMA_CHANNEL: This chip is not DMA enabled!"
#endif // DMA check

#else
#error "DMA_CHANNEL: The dma_channel object requires the XC16 compiler or similar!"
#endif // Compiler check

#endif //_DMA_CHANNEL_H
