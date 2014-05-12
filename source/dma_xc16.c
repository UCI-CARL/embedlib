/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file dma_channel.c
 *
 * @brief This module contains code to control a single DMA channel on a
 * dsPIC33F chip. It uses the base address of the DMA control registers along
 * with their defined offsets to set the various registers for that particular
 * channel.
 *
 * @author Liam Bucci
 * @date 12/3/2012
 * @carlnumber FIRM-0004
 * @version 0.3.0
 */

/**
 * @ingroup DMAChannel
 *
 * @{
 */

// Standard C include files
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Microchip peripheral libraries
#include <p33Fxxxx.h>

// Local include file
#include "../include/dma_channel.h"

/**
 * @brief The private data structure which holds the initialized data for a DMA channel.
 *
 * @details This private data structure holds the attributes and settings which have been used to
 * initialize a DMA channel. When the #dma_init function is called on a #dma_channel_t variable
 * the #dma_attr_s is copied into this structure for safe keeping. The attributes may only be
 * changed if the DMA channel is reinitialized.
 *
 * @private
 */
struct dma_private_s
{
    struct dma_attr_s attr;
};

/**
 * @brief Defines the offsets of all SFRs associated with a DMA channel.
 *
 * @details This enumeration defines the offsets for each special function register in a DMA
 * channel. The numbers represent the word offsets, not byte offsets.
 *
 * @private
 */
enum dma_offset_e
{
    DMA_OFFSET_DMAxCON = 0x0000, /**< Offset for DMAxCON register */
    DMA_OFFSET_DMAxREQ = 0x0001, /**< Offset for DMAxREQ register */
    DMA_OFFSET_DMAxSTA = 0x0002, /**< Offset for DMAxSTA register */
    DMA_OFFSET_DMAxSTB = 0x0003, /**< Offset for DMAxSTB register */
    DMA_OFFSET_DMAxPAD = 0x0004, /**< Offset for DMAxPAD register */
    DMA_OFFSET_DMAxCNT = 0x0005  /**< Offset for DMAxCNT register */
};

/**
 * @brief Defines the bitmasks for each bit (or set of bits) in the DMA SFRs.
 *
 * @details This enumeration defines the bitmasks for every bit (and set of bits) in all of the DMA
 * special function registers. Registers or sets of bits which are only ever used as a whole do not
 * have bitmasks for their individual bits.
 *
 * @private
 */
enum dma_bitmasks_e
{
    // DMAxCON bitmasks
    DMA_BITMASK_MODE   = 0x0003, /**< Bitmask for MODE bits of DMAxCON register */
    DMA_BITMASK_MODE0  = 0x0001, /**< Bitmask for MODE0 bit of DMAxCON register */
    DMA_BITMASK_MODE1  = 0x0002, /**< Bitmask for MODE1 bit of DMAxCON register */
    DMA_BITMASK_AMODE  = 0x0030, /**< Bitmask for AMODE bits of DMAxCON register */
    DMA_BITMASK_AMODE0 = 0x0010, /**< Bitmask for AMODE0 bit of DMAxCON register */
    DMA_BITMASK_AMODE1 = 0x0020, /**< Bitmask for AMODE1 bit of DMAxCON register */
    DMA_BITMASK_NULLW  = 0x0800, /**< Bitmask for NULLW bit of DMAxCON register */
    DMA_BITMASK_HALF   = 0x1000, /**< Bitmask for HALF bit of DMAxCON register */
    DMA_BITMASK_DIR    = 0x2000, /**< Bitmask for DIR bit of DMAxCON register */
    DMA_BITMASK_SIZE   = 0x4000, /**< Bitmask for SIZE bit of DMAxCON register */
    DMA_BITMASK_CHEN   = 0x8000, /**< Bitmask for CHEN bit of DMAxCON register */

    // DMAxREQ bitmasks
    DMA_BITMASK_IRQSEL = 0x007F, /**< Bitmask for IRQSEL bits of DMAxREQ register */
    DMA_BITMASK_FORCE  = 0x8000, /**< Bitmask for FORCE bit of DMAxREQ register */

    // DMACS0 bitmasks
    DMA_BITMASK_XWCOL0 = 0x0001, /**< Bitmask for XWCOL0 bit of DMACS0 register */
    DMA_BITMASK_XWCOL1 = 0x0002, /**< Bitmask for XWCOL1 bit of DMACS0 register */
    DMA_BITMASK_XWCOL2 = 0x0004, /**< Bitmask for XWCOL2 bit of DMACS0 register */
    DMA_BITMASK_XWCOL3 = 0x0008, /**< Bitmask for XWCOL3 bit of DMACS0 register */
    DMA_BITMASK_XWCOL4 = 0x0010, /**< Bitmask for XWCOL4 bit of DMACS0 register */
    DMA_BITMASK_XWCOL5 = 0x0020, /**< Bitmask for XWCOL5 bit of DMACS0 register */
    DMA_BITMASK_XWCOL6 = 0x0040, /**< Bitmask for XWCOL6 bit of DMACS0 register */
    DMA_BITMASK_XWCOL7 = 0x0080, /**< Bitmask for XWCOL7 bit of DMACS0 register */
    DMA_BITMASK_PWCOL0 = 0x0100, /**< Bitmask for PWCOL0 bit of DMACS0 register */
    DMA_BITMASK_PWCOL1 = 0x0200, /**< Bitmask for PWCOL1 bit of DMACS0 register */
    DMA_BITMASK_PWCOL2 = 0x0400, /**< Bitmask for PWCOL2 bit of DMACS0 register */
    DMA_BITMASK_PWCOL3 = 0x0800, /**< Bitmask for PWCOL3 bit of DMACS0 register */
    DMA_BITMASK_PWCOL4 = 0x1000, /**< Bitmask for PWCOL4 bit of DMACS0 register */
    DMA_BITMASK_PWCOL5 = 0x2000, /**< Bitmask for PWCOL5 bit of DMACS0 register */
    DMA_BITMASK_PWCOL6 = 0x4000, /**< Bitmask for PWCOL6 bit of DMACS0 register */
    DMA_BITMASK_PWCOL7 = 0x8000, /**< Bitmask for PWCOL7 bit of DMACS0 register */

    // DMACS1 bitmasks
    DMA_BITMASK_PPST0  = 0x0001, /**< Bitmask for PPST0 bit of DMACS1 register */
    DMA_BITMASK_PPST1  = 0x0002, /**< Bitmask for PPST1 bit of DMACS1 register */
    DMA_BITMASK_PPST2  = 0x0004, /**< Bitmask for PPST2 bit of DMACS1 register */
    DMA_BITMASK_PPST3  = 0x0008, /**< Bitmask for PPST3 bit of DMACS1 register */
    DMA_BITMASK_PPST4  = 0x0010, /**< Bitmask for PPST4 bit of DMACS1 register */
    DMA_BITMASK_PPST5  = 0x0020, /**< Bitmask for PPST5 bit of DMACS1 register */
    DMA_BITMASK_PPST6  = 0x0040, /**< Bitmask for PPST6 bit of DMACS1 register */
    DMA_BITMASK_PPST7  = 0x0080, /**< Bitmask for PPST7 bit of DMACS1 register */
    DMA_BITMASK_LSTCH  = 0x0F00  /**< Bitmask for LSTCH bits of DMACS1 register */
};

/**
 * @brief Defines the default values of all DMA channel SFRs.
 *
 * @details This enumeration defines the default values of all DMA channel special function
 * registers. These values are used during initialization to make sure the channel starts out in a
 * consistent, predictable state.
 *
 * @private
 */
enum dma_defaults_e
{
    DMA_DEFAULT_DMAxCON = 0x0000, /**< Default value of DMAxCON register */
    DMA_DEFAULT_DMAxREQ = 0x0000, /**< Default value of DMAxREQ register */
    DMA_DEFAULT_DMAxSTA = 0x0000, /**< Default value of DMAxSTA register */
    DMA_DEFAULT_DMAxSTB = 0x0000, /**< Default value of DMAxSTB register */
    DMA_DEFAULT_DMAxPAD = 0x0000, /**< Default value of DMAxPAD register */
    DMA_DEFAULT_DMAxCNT = 0x0000  /**< Default value of DMAxCNT register */
};


int dma_init(dma_channel_t *dma_channel,
             struct dma_attr_s *attr)
{
    // Check for valid dma_channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        // Return error code
        return DMA_E_CHANNEL;
    }

    // Check for valid attr
    if( attr == NULL )
    {// Invalid attr
        // Return error code
        return DMA_E_INPUT;
    }

    // Set all registers to default values
    *(dma_channel->base_address + DMA_OFFSET_DMAxCON) = DMA_DEFAULT_DMAxCON;
    *(dma_channel->base_address + DMA_OFFSET_DMAxREQ) = DMA_DEFAULT_DMAxREQ;
    *(dma_channel->base_address + DMA_OFFSET_DMAxSTA) = DMA_DEFAULT_DMAxSTA;
    *(dma_channel->base_address + DMA_OFFSET_DMAxSTB) = DMA_DEFAULT_DMAxSTB;
    *(dma_channel->base_address + DMA_OFFSET_DMAxPAD) = DMA_DEFAULT_DMAxPAD;
    *(dma_channel->base_address + DMA_OFFSET_DMAxCNT) = DMA_DEFAULT_DMAxCNT;

    // Configure DMA operating mode
    if( attr->control_settings & DMA_BITMASK_ONESHOT )
    {// One-shot mode
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_MODE0;
    }// Otherwise default to Continuous

    // Configure Ping-Pong mode
    if( attr->control_settings & DMA_PINGPONG_EN )
    {// Enable Ping-Pong mode
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_MODE1;
    }// Otherwise default to disabled

    // Configure Addressing mode
    if( attr->control_settings & DMA_ADDRMODE_PERIPHERAL_IND )
    {// Peripheral indirect mode
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_AMODE1;
    }
    else
    {// Otherwise default to Register Indirect mode
        // Configure Post-increment
        if( attr->control_settings & DMA_ADDRMODE_POSTINC_DIS )
        {// Disable Post-increment
            *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_AMODE0;
        }// Otherwise default to Post-increment enabled
    }

    // Configure Null Write
    if( attr->control_settings & DMA_NULLWRITE_EN )
    {// Enable Null Write
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_NULLW;
    }// Otherwise default to disabled
    
    // Configure Block Transfer Interrupt
    if( attr->control_settings & DMA_INT_HALF )
    {// Interrupt on half
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_HALF;
    }// Otherwise default to interrupt on all

    // Configure Transfer Direction
    if( attr->control_settings & DMA_DIR_TOPERIPHERAL )
    {// Transfer to peripheral
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_DIR;
    }// Otherwise default to transfer from peripheral

    // Configure Data Size
    if( attr->control_settings & DMA_DATASIZE_BYTE )
    {// Data size is byte
        *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_SIZE;
    }// Otherwise default to word size
    
    // Set DMAxREQ register
    *(dma_channel->base_address + DMA_OFFSET_DMAxREQ) = (attr->irq & DMA_BITMASK_IRQSEL);

    // Set DMAxPAD register
    *(dma_channel->base_address + DMA_OFFSET_DMAxPAD) = (volatile unsigned int)attr->peripheral_address;

    // Configure Ping-pong buffers and block size if applicable
    if( attr->control_settings & DMA_PINGPONG_EN )
    {// Ping-pong is enabled
        *(dma_channel->base_address + DMA_OFFSET_DMAxSTA) = (unsigned int)(dma_channel->buffer) - (unsigned int)(&_DMA_BASE);
        *(dma_channel->base_address + DMA_OFFSET_DMAxSTB) = (unsigned int)(dma_channel->buffer+(dma_channel->buffer_size/2)) - (unsigned int)&_DMA_BASE;
        *(dma_channel->base_address + DMA_OFFSET_DMAxCNT) = (dma_channel->buffer_size/2);
    }
    else
    {// Ping-pong is disabled
        *(dma_channel->base_address + DMA_OFFSET_DMAxSTA) = (unsigned int)(dma_channel->buffer) - (unsigned int)(&_DMA_BASE);
        *(dma_channel->base_address + DMA_OFFSET_DMAxCNT) = dma_channel->buffer_size;
    }

    // Allocate and save the attributes to the private structure
    dma_channel->private = malloc(sizeof(struct dma_private_s));
    ((struct dma_private_s *)dma_channel->private)->attr = *attr;
    
    // Return successfully
    return DMA_E_NONE;
}


int dma_enable(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Enable DMA channel
    *(dma_channel->base_address + DMA_OFFSET_DMAxCON) |= DMA_BITMASK_CHEN;

    // Return successfully
    return DMA_E_NONE;
}


int dma_disable(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Disable DMA channel
    *(dma_channel->base_address + DMA_OFFSET_DMAxCON) &= ~(DMA_BITMASK_CHEN);

    // Return successfully
    return DMA_E_NONE;
}


int dma_force(dma_channel_t *dma_channel)
{
    // Check for a valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Force a DMA transfer
    *(dma_channel->base_address + DMA_OFFSET_DMAxREQ) |= DMA_BITMASK_FORCE;

    // Return successfully
    return DMA_E_NONE;
}


int dma_get_bufferA(dma_channel_t *dma_channel,
                    volatile unsigned int **buffer)
{
    // Check for a valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    *buffer = dma_channel->buffer;

    if( ((struct dma_private_s *)dma_channel->private)->attr.control_settings & DMA_PINGPONG_EN )
    {// Ping-pong is enabled
        return (dma_channel->buffer_size/2);
    }
    else
    {// Ping-pong is disabled
        return dma_channel->buffer_size;
    }
}

int dma_get_bufferA_size(dma_channel_t *dma_channel)
{
    // Check for a valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    if( ((struct dma_private_s *)dma_channel->private)->attr.control_settings & DMA_PINGPONG_EN )
    {// Ping-pong is enabled
        return (dma_channel->buffer_size/2);
    }
    else
    {// Ping-pong is disabled
        return dma_channel->buffer_size;
    }
}

int dma_get_bufferB(dma_channel_t *dma_channel,
                    volatile unsigned int **buffer)
{
    // Check for a valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    if( ((struct dma_private_s *)dma_channel->private)->attr.control_settings & DMA_PINGPONG_EN )
    {// Ping-pong is enabled
        *buffer = (dma_channel->buffer + dma_channel->buffer_size/2);
        return (dma_channel->buffer_size/2);
    }
    else
    {// Ping-pong is disabled
        *buffer = NULL;
        return 0;
    }
}

int dma_get_bufferB_size(dma_channel_t *dma_channel)
{
    // Check for a valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    if( ((struct dma_private_s *)dma_channel->private)->attr.control_settings & DMA_PINGPONG_EN )
    {// Ping-pong is enabled
        return (dma_channel->buffer_size/2);
    }
    else
    {// Ping-pong is disabled
        return 0;
    }
}

int dma_set_block_size(dma_channel_t *dma_channel,
                       unsigned int block_size)
{
    // Check for valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Set the DMAxCNT register
    *(dma_channel->base_address + DMA_OFFSET_CNT) = block_size-1;

    // Return successfully
    return DMA_E_NONE;
}


int dma_get_block_size(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Return the DMAxCNT register
    return *(dma_channel->base_address + DMA_OFFSET_CNT) + 1;
}

int dma_get_attr(dma_channel_t *dma_channel,
                 struct dma_attr_s *attr)
{
    // Check for valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    // Copy attr struct
    *attr = ((struct dma_private_s *)dma_channel->private)->attr;

    return DMA_E_NONE;
}

int dma_cleanup(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( dma_channel == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    // Disable channel
    dma_disable(dma_channel);

    // Return channel to default settings
    *(dma_channel->base_address + DMA_OFFSET_DMAxCON) = DMA_DEFAULT_DMAxCON;
    *(dma_channel->base_address + DMA_OFFSET_DMAxREQ) = DMA_DEFAULT_DMAxREQ;
    *(dma_channel->base_address + DMA_OFFSET_DMAxSTA) = DMA_DEFAULT_DMAxSTA;
    *(dma_channel->base_address + DMA_OFFSET_DMAxSTB) = DMA_DEFAULT_DMAxSTB;
    *(dma_channel->base_address + DMA_OFFSET_DMAxPAD) = DMA_DEFAULT_DMAxPAD;
    *(dma_channel->base_address + DMA_OFFSET_DMAxCNT) = DMA_DEFAULT_DMAxCNT;
    
    // Free private struct
    free(dma_channel->private);

    return DMA_E_NONE;
}

/**
 * @}
 */ // End of DMAChannel group
