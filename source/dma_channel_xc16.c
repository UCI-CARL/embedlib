/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file dma_channel_xc16.c
 *
 * @brief This module contains code to control a single DMA channel on a
 * dsPIC33F chip. It uses the base address of the DMA control registers along
 * with their defined offsets to set the various registers for that particular
 * channel.
 *
 * @author Liam Bucci
 * @date 12/3/2012
 * @carlnumber FIRM-0009
 * @version 0.4.0
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

// Local library include files
#include <bitops.h>

// Microchip peripheral libraries
#include <p33Fxxxx.h>

// Module include file
#include <dma_channel.h>


/**
 * @brief The private data structure which holds the data for a DMA channel.
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
    struct dma_attr_s _attr;

    volatile unsigned int * base_address;
};


/**
 * @brief Defines the offsets of all SFRs associated with a DMA channel.
 *
 * @details This enumeration defines the offsets for each special function register in a DMA
 * channel. The numbers represent the word offsets, not byte offsets.
 *
 * @private
 */
enum dma_sfr_offsets_e
{
    DMA_SFR_OFFSET_DMAxCON = 0x0000, /**< Offset for DMAxCON register */
    DMA_SFR_OFFSET_DMAxREQ = 0x0001, /**< Offset for DMAxREQ register */
    DMA_SFR_OFFSET_DMAxSTA = 0x0002, /**< Offset for DMAxSTA register */
    DMA_SFR_OFFSET_DMAxSTB = 0x0003, /**< Offset for DMAxSTB register */
    DMA_SFR_OFFSET_DMAxPAD = 0x0004, /**< Offset for DMAxPAD register */
    DMA_SFR_OFFSET_DMAxCNT = 0x0005  /**< Offset for DMAxCNT register */
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
enum dma_sfr_bitmasks_e
{
    // DMAxCON bitmasks
    DMA_SFR_BITMASK_MODE   = 0x0003, /**< Bitmask for MODE bits of DMAxCON register */
    DMA_SFR_BITMASK_MODE0  = 0x0001, /**< Bitmask for MODE0 bit of DMAxCON register */
    DMA_SFR_BITMASK_MODE1  = 0x0002, /**< Bitmask for MODE1 bit of DMAxCON register */
    DMA_SFR_BITMASK_AMODE  = 0x0030, /**< Bitmask for AMODE bits of DMAxCON register */
    DMA_SFR_BITMASK_AMODE0 = 0x0010, /**< Bitmask for AMODE0 bit of DMAxCON register */
    DMA_SFR_BITMASK_AMODE1 = 0x0020, /**< Bitmask for AMODE1 bit of DMAxCON register */
    DMA_SFR_BITMASK_NULLW  = 0x0800, /**< Bitmask for NULLW bit of DMAxCON register */
    DMA_SFR_BITMASK_HALF   = 0x1000, /**< Bitmask for HALF bit of DMAxCON register */
    DMA_SFR_BITMASK_DIR    = 0x2000, /**< Bitmask for DIR bit of DMAxCON register */
    DMA_SFR_BITMASK_SIZE   = 0x4000, /**< Bitmask for SIZE bit of DMAxCON register */
    DMA_SFR_BITMASK_CHEN   = 0x8000, /**< Bitmask for CHEN bit of DMAxCON register */

    // DMAxREQ bitmasks
    DMA_SFR_BITMASK_IRQSEL = 0x007F, /**< Bitmask for IRQSEL bits of DMAxREQ register */
    DMA_SFR_BITMASK_FORCE  = 0x8000, /**< Bitmask for FORCE bit of DMAxREQ register */

    // DMACS0 bitmasks
    DMA_SFR_BITMASK_XWCOL0 = 0x0001, /**< Bitmask for XWCOL0 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL1 = 0x0002, /**< Bitmask for XWCOL1 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL2 = 0x0004, /**< Bitmask for XWCOL2 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL3 = 0x0008, /**< Bitmask for XWCOL3 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL4 = 0x0010, /**< Bitmask for XWCOL4 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL5 = 0x0020, /**< Bitmask for XWCOL5 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL6 = 0x0040, /**< Bitmask for XWCOL6 bit of DMACS0 register */
    DMA_SFR_BITMASK_XWCOL7 = 0x0080, /**< Bitmask for XWCOL7 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL0 = 0x0100, /**< Bitmask for PWCOL0 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL1 = 0x0200, /**< Bitmask for PWCOL1 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL2 = 0x0400, /**< Bitmask for PWCOL2 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL3 = 0x0800, /**< Bitmask for PWCOL3 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL4 = 0x1000, /**< Bitmask for PWCOL4 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL5 = 0x2000, /**< Bitmask for PWCOL5 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL6 = 0x4000, /**< Bitmask for PWCOL6 bit of DMACS0 register */
    DMA_SFR_BITMASK_PWCOL7 = 0x8000, /**< Bitmask for PWCOL7 bit of DMACS0 register */

    // DMACS1 bitmasks
    DMA_SFR_BITMASK_PPST0  = 0x0001, /**< Bitmask for PPST0 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST1  = 0x0002, /**< Bitmask for PPST1 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST2  = 0x0004, /**< Bitmask for PPST2 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST3  = 0x0008, /**< Bitmask for PPST3 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST4  = 0x0010, /**< Bitmask for PPST4 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST5  = 0x0020, /**< Bitmask for PPST5 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST6  = 0x0040, /**< Bitmask for PPST6 bit of DMACS1 register */
    DMA_SFR_BITMASK_PPST7  = 0x0080, /**< Bitmask for PPST7 bit of DMACS1 register */
    DMA_SFR_BITMASK_LSTCH  = 0x0F00  /**< Bitmask for LSTCH bits of DMACS1 register */
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
enum dma_sfr_defaults_e
{
    DMA_SFR_DEFAULT_DMAxCON = 0x0000, /**< Default value of DMAxCON register */
    DMA_SFR_DEFAULT_DMAxREQ = 0x0000, /**< Default value of DMAxREQ register */
    DMA_SFR_DEFAULT_DMAxSTA = 0x0000, /**< Default value of DMAxSTA register */
    DMA_SFR_DEFAULT_DMAxSTB = 0x0000, /**< Default value of DMAxSTB register */
    DMA_SFR_DEFAULT_DMAxPAD = 0x0000, /**< Default value of DMAxPAD register */
    DMA_SFR_DEFAULT_DMAxCNT = 0x0000  /**< Default value of DMAxCNT register */
};


int dma_init(dma_channel_t *dma_channel,
             dma_attr_t *attr)
{
    // Check for valid dma_channel
    if( dma_channel == NULL \
        || dma_channel->channel_number > 7 \
        || dma_channel->buffer_a == NULL \
        || dma_channel->buffer_a_size == 0 \
        || (dma_channel->buffer_b == NULL && dma_channel->buffer_b_size > 0) )
    {// Invalid DMA channel
        // Return error code
        return DMA_E_CHANNEL;
    }

    
    // Check for uninitialized dma_channel
    if( dma_channel->private != NULL )
    {// DMA channel has already been initialized
        // Return error code
        return DMA_E_INPUT;
    }
        
    
    // Check for valid attr
    if( attr == NULL )
    {// Invalid attr
        // Return error code
        return DMA_E_INPUT;
    }

    
    // Allocate the private structure
    dma_channel->private = malloc(sizeof(struct dma_private_s));
    if(dma_channel->private == NULL)
    {// Failed to allocate memory!
        return DMA_E_ALLOC;
    }

    // Save the attr struct to the private data object
    ((struct dma_private_s *)dma_channel->private)->attr = *attr;

    switch(dma_channel->channel_number)
    {
    case 0:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA0CON;
        break;
    case 1:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA1CON;
        break;
    case 2:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA2CON;
        break;
    case 3:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA3CON;
        break;
    case 4:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA4CON;
        break;
    case 5:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA5CON;
        break;
    case 6:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA6CON;
        break;
    case 7:
        *((struct dma_private_s)(dma_channel->private)->base_address) = DMA7CON;
        break;
    default:
        // Invalid channel number (should have already been checked!)
        // Assertion failed!
        return DMA_E_ASSERT;
    }
        
    
    // Set all registers to default values
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON) \
        = DMA_SFR_DEFAULT_DMAxCON;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxREQ) \
        = DMA_SFR_DEFAULT_DMAxREQ;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxSTA) \
        = DMA_SFR_DEFAULT_DMAxSTA;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxSTB) \
        = DMA_SFR_DEFAULT_DMAxSTB;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxPAD) \
        = DMA_SFR_DEFAULT_DMAxPAD;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCNT) \
        = DMA_SFR_DEFAULT_DMAxCNT;

    
    // Configure DMA operating mode
    if( (attr->config & DMA_CONFIG_OPMODE_BITMASK) == DMA_CONFIG_OPMODE_ONESHOT )
    {// One-shot mode
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_MODE0);
    }
    else
    {// Otherwise default to Continuous
    }

    
    // Configure Ping-Pong mode
    if( (attr->config & DMA_CONFIG_PINGPONG_BITMASK) == DMA_CONFIG_PINGPONG_EN )
    {// Enable Ping-Pong mode
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_MODE1);

        // Set up buffers and initial block size
        *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxSTA) \
            = __builtin_dmaoffset(dma_channel->buffer_a);
        *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxSTB) \
            = __builtin_dmaoffset(dma_channel->buffer_b);
        *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCNT) \
            = dma_channel->buffer_a_size;
    }
    else
    {// Otherwise default to disabled
        // Set up buffer and block size
        *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxSTA) \
            = __builtin_dmaoffset(dma_channel->buffer);
        *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCNT) \
            = dma_channel->buffer_a_size;
    }
    

    // Configure Addressing mode
    if( (attr->config & DMA_CONFIG_ADDRMODE_BITMASK) == DMA_CONFIG_ADDRMODE_PERIPHERAL_IND )
    {// Peripheral indirect mode
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_AMODE1);
    }
    else if( (attr->config & DMA_CONFIG_ADDRMODE_BITMASK) == DMA_CONFIG_ADDRMODE_REGIND_NOPOSTINC )
    {// Register indirect w/o post-increment
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_AMODE0);
    }
    else
    {// Default to register indirect w/ post-increment
    }

    
    // Configure Null Write
    if( (attr->config & DMA_CONFIG_NULLWRITE_BITMASK) == DMA_CONFIG_NULLWRITE_EN )
    {// Enable Null Write
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_NULLW);
    }
    else
    {// Otherwise default to disabled
    }

    
    // Configure Transfer Direction
    if( (attr->config & DMA_CONFIG_DIR_BITMASK) == DMA_CONFIG_DIR_TO_PERIPHERAL )
    {// Transfer to peripheral
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_DIR);
    }
    else
    {// Otherwise default to transfer from peripheral
    }

    // Configure Data Size
    if( (attr->config & DMA_CONFIG_DATASIZE_BITMASK) == DMA_CONFIG_DATASIZE_BYTE )
    {// Data size is byte
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_SIZE);
    }
    else
    {// Otherwise default to word size
    }
    
    // Set DMAxREQ register (interrupt which triggers a DMA transfer)
    WRITE_BITS_INSERT(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxREQ), attr->irq, DMA_SFR_BITMASK_IRQSEL);

    // Set DMAxPAD register (peripheral address the DMA transfer writes to/reads from)
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxPAD) \
        = (volatile unsigned int)attr->peripheral_address;

    
    // Return successfully
    return DMA_E_NONE;
}


int dma_enable(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Enable DMA channel
    WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_CHEN);

    // Return successfully
    return DMA_E_NONE;
}


int dma_disable(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Disable DMA channel
    WRITE_MASK_CLEAR(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_CHEN);

    // Return successfully
    return DMA_E_NONE;
}

int dma_set_interrupt_on(dma_channel_t *dma_channel,
                         dma_interrupt_on_t int_on)
{
    // Check for a valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Set block transfer interrupt
    if( int_on == DMA_INTERRUPT_ON_HALF )
    {// Set interrupts to half block transfer
        WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_HALF);
    }
    else if( int_on == DMA_INTERRUPT_ON_FULL )
    {// Set interrupts to full block transfer
        WRITE_MASK_CLEAR(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_HALF);
    }
    else
    {// Unknown int_on value
        // Return an error
        return DMA_E_INPUT;
    }

    // Return successfully
    return DMA_E_NONE;
}

int dma_get_interrupt_on(dma_channel_t *dma_channel)
{
    // Check for a valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Check interrupt setting
    if( IS_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_SFR_OFFSET_DMAxCON), DMA_SFR_BITMASK_HALF) )
    {// Interrupt on half block transfer
        return DMA_INTERRUPT_ON_HALF;
    }
    else
    {// Interrupt on full block transfer
        return DMA_INTERRUPT_ON_FULL;
    }
}

int dma_pingpong_status(dma_channel_t *dma_channel)
{
    // Check for a valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Check ping-pong setting
    if( IS_BIT_SET(DMASCS1, dma_channel->channel_number) )
    {// Ping-pong buffer B selected
        return DMA_PINGPONG_BUFFER_B;
    }
    else
    {// Ping-pong buffer A selected
        return DMA_PINGPONG_BUFFER_A;
    }   
}
                      
int dma_force(dma_channel_t *dma_channel)
{
    // Check for a valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Force a DMA transfer
    WRITE_MASK_SET(*(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxREQ), DMA_BITMASK_FORCE);

    // Return successfully
    return DMA_E_NONE;
}


int dma_set_block_size(dma_channel_t *dma_channel,
                       unsigned int block_size)
{
    // Check for valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    /**
     * @todo Add buffer size checking so that block size cannot be larger than the buffer.
     */

    // Set the DMAxCNT register
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_CNT) = block_size-1;

    // Return successfully
    return DMA_E_NONE;
}


int dma_get_block_size(dma_channel_t *dma_channel)
{
    // Check for valid DMA channel
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        // Return unsuccessfully
        return DMA_E_CHANNEL;
    }

    // Return the DMAxCNT register
    return *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_CNT) + 1;
}


int dma_get_attr(dma_channel_t *dma_channel,
                 dma_attr_t *attr)
{
    // Check for valid DMA channel
    if( !dma_is_valid(dma_channel) )
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
    if( !dma_is_valid(dma_channel) )
    {// Invalid DMA channel
        return DMA_E_CHANNEL;
    }

    // Disable channel
    dma_disable(dma_channel);

    // Return channel to default settings
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxCON) = DMA_DEFAULT_DMAxCON;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxREQ) = DMA_DEFAULT_DMAxREQ;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxSTA) = DMA_DEFAULT_DMAxSTA;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxSTB) = DMA_DEFAULT_DMAxSTB;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxPAD) = DMA_DEFAULT_DMAxPAD;
    *(((struct dma_private_s)(dma_channel->private)->base_address) + DMA_OFFSET_DMAxCNT) = DMA_DEFAULT_DMAxCNT;
    
    // Free private struct
    free(dma_channel->private);
    dma_channel->private = NULL;

    return DMA_E_NONE;
}

inline bool dma_is_valid(dma_channel_t *dma_channel)
{
    return ( dma_channel == NULL \
             || dma_channel->channel_number > 7 \
             || dma_channel->buffer_a == NULL \
             || dma_channel->buffer_a_size == 0 \
             || (buffer_b == NULL && buffer_b_size != 0) \
             || dma_channel->private == NULL ) ? false : true;
}

/**
 * @}
 */ // End of DMAChannel group
