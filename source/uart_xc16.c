/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file uart_module_xc16.c
 *
 * @brief This file defines all the functions of a UART module for the dsPIC33F microcontroller.
 *
 * @author     Liam Bucci
 * @date       6/6/2012
 * @carlnumber FIRM-0009
 * @version    0.4.0
 */

/**
 * @addtogroup uart_module
 *
 * @{
 */

// Include standard C library files
#include <stdlib.h>
#include <stdbool.h>

// Include Microchip Peripheral Library files
#include <xc.h>
#include <pps.h>

// Include board information
#include <board.def>

// Include local library code
#include <bitops.h>
#include <dma_channel.h>

// Include user definitions
#include <uart.def>

// Include private hardware definitions
#include <uart_hw.h>

// Include module declaration
#include <uart.h>

/**
 * @todo move to .def file.
 */
#define UART_DEF_LOCAL_ADDR_SIZE 8 /**< The maximum size of the local_addr_ array */

/* ***** Private Macros ***** */

/**
 * @brief Return a pointer to the base address of the given UART module.
 *
 * @private
 */
#define UART_GET_BASE_ADDRESS(module) ( ((uart_private_t *)((module)->private))->base_address_ )

/**
 * @brief Return the #_attr struct stored in the private object of the given UART module.
 *
 * @private
 */
#define UART_GET_ATTR(module) ( ((uart_private_t *)((module)->private))->attr_ )

/* ***** Private Constants ***** */



/* ***** Private Enumerations ***** */

/**
 * @brief This enumeration defines the offsets for a UART module's special function registers (SFR).
 *
 * @details The offsets defined by this enumeration are used to address each SFR of a UART module.
 * This is done using #_base_address from the private object and doing pointer addition to get a
 * final address of the register.
 *
 * @private
 */
enum uart_sfr_offsets_e
{
    UART_SFR_OFFSET_UxMODE  = 0x0000,
    UART_SFR_OFFSET_UxSTA   = 0x0001,
    UART_SFR_OFFSET_UxTXREG = 0x0002,
    UART_SFR_OFFSET_UxRXREG = 0x0003,
    UART_SFR_OFFSET_UxBRG   = 0x0004
};

/**
 * @brief This enumeration defines the UART special function registers' (SFR) bit masks.
 *
 * @details The values defined in this enumeration are bitmasks for each bit or group of bits in
 * the SFRs of a UART module.
 *
 * @private
 */
enum uart_sfr_bitmasks_e
{
    //UxMODE
    UART_SFR_BITMASK_STSEL    = 0x0001,
    UART_SFR_BITMASK_PDSEL0   = 0x0002,
    UART_SFR_BITMASK_PDSEL1   = 0x0004,
    UART_SFR_BITMASK_BRGH     = 0x0008,
    UART_SFR_BITMASK_URXINV   = 0x0010,
    UART_SFR_BITMASK_ABAUD    = 0x0020,
    UART_SFR_BITMASK_LPBACK   = 0x0040,
    UART_SFR_BITMASK_WAKE     = 0x0080,
    UART_SFR_BITMASK_UEN0     = 0x0100,
    UART_SFR_BITMASK_UEN1     = 0x0200,
    UART_SFR_BITMASK_RTSMD    = 0x0800,
    UART_SFR_BITMASK_IREN     = 0x1000,
    UART_SFR_BITMASK_USIDL    = 0x2000,
    UART_SFR_BITMASK_UARTEN   = 0x8000,
    
    //UxSTA
    UART_SFR_BITMASK_URXDA    = 0x0001,
    UART_SFR_BITMASK_OERR     = 0x0002,
    UART_SFR_BITMASK_FERR     = 0x0004,
    UART_SFR_BITMASK_PERR     = 0x0008,
    UART_SFR_BITMASK_RIDLE    = 0x0010,
    UART_SFR_BITMASK_ADDEN    = 0x0020,
    UART_SFR_BITMASK_URXISEL0 = 0x0040,
    UART_SFR_BITMASK_URXISEL1 = 0x0080,
    UART_SFR_BITMASK_TRMT     = 0x0100,
    UART_SFR_BITMASK_UTXBF    = 0x0200,
    UART_SFR_BITMASK_UTXEN    = 0x0400,
    UART_SFR_BITMASK_UTXBRK   = 0x0800,
    UART_SFR_BITMASK_UTXISEL0 = 0x2000,
    UART_SFR_BITMASK_UTXINV   = 0x4000,
    UART_SFR_BITMASK_UTXISEL1 = 0x8000
};

/**
 * @brief Default values for all UART module registers.
 *
 * @details This enumeration defines the default values for all special function registers of a
 * UART module.
 *
 * @private
 */
enum uart_sfr_defaults_e
{
    UART_SFR_DEFAULT_UxMODE = 0x0000, /**< The default value for the UxMODE SFR */
    UART_SFR_DEFAULT_UxSTA  = 0x0110, /**< The default value for the UxSTA SFR */
    UART_SFR_DEFAULT_UxBRG  = 0xFFFF  /**< The default value for the UxBRG SFR */
};

/* ***** Private Data Declaration ***** */
/**
 * @brief The private object of a UART module.
 *
 * @details The private object of the UART module stores internal settings, states, and pointers
 * for the specific UART module in which it is stored. These values should only be accessed and/or
 * modified by internal UART code. The private object is allocated and stored during #uart_init()
 * and is freed during #uart_cleanup().
 *
 * @private
 */
typedef struct uart_private_s
{
    /**
     * @brief The #_attr object stores immutable settings of the UART module.
     *
     * @details The private #_attr object stores the user supplied attributes given in #uart_init().
     * The struct passed into init is verified, then stored in the modules private #_attr object.
     * The settings stored in this object are immutable over the life of the module. In other words
     * the settings may only change if the module is re-initialized.
     *
     * @see uart_attr_t
     * @private
     */
    uart_attr_t attr_;

    /**
     * @brief The #_base_address points to the starting address of the current UART module.
     *
     * @details The #_base_address address is used, along with defined offsets, to address each
     * special function register (SFR) of the module. Each SFR is accessed relative to the stored
     * #_base_address.
     *
     * @private
     */
    volatile unsigned int *base_address_;

    /**
     * @brief The #_tx_dma pointer points to the user supplied DMA channel which should be used for
     * transmitting in DMA and hybrid buffer modes.
     *
     * @details This pointer is the assigned DMA channel for transmission. It must be valid if
     * either DMA or hybrid buffer modes is selected. The DMA channel will be initialized and
     * set up correctly by the UART module #uart_init() function.
     *
     * @see dma_channel_t
     * @private
     */
    dma_channel_t *tx_dma_;

    /**
     * @brief The #_rx_dma pointer points to the user supplied DMA channel which should be used for
     * receiving in DMA and hybrid buffer modes.
     *
     * @details This pointer is the assigned DMA channel for reception. It must be valid if either
     * DMA or hybrid buffer modes is selected. The DMA channel will be initialized and set up
     * correctly by the UART module #uart_init() function.
     *
     * @see dma_channel_t
     * @private
     */
    dma_channel_t *rx_dma_;

    /**
     * @brief The baud rate used by the module for transmission and reception of characters.
     *
     * @details The #_baudrate variable stores a coded value representing the baud rate of the
     * module.
     *
     * @see uart_baudrate_t
     * @private
     */
    uart_baudrate_t baudrate_;

    /**
     * @brief The current open state of the module, either RX only, TX and RX, or none.
     *
     * @details This variable stores the current open state of the module. It can be in RX only,
     * TX and RX, or none. The module cannot be in TX only as the hardware must enable RX to
     * enable TX.
     *
     * @see uart_direction_t
     * @private
     */
    uart_direction_t open_state_;
    
    void *tx_buffer_; /**< A pointer to the software TX buffer (either char or int). @private */
    void *tx_head_;   /**< A pointer to the last write into the TX software buffer. @private */
    void *tx_tail_;   /**< A pointer to the last read out of the TX software buffer. @private */

    void *rx_buffer_; /**< A pointer to the software RX buffer (either char or int). @private */
    void *rx_head_;   /**< A pointer to the last write into the RX software buffer. @private */
    void *rx_tail_;   /**< A pointer to the last read out of the RX software buffer. @private */

    char *local_addr_; /**< An array of addresses to accept in 9-bit, masked mode. @private */
    int local_addr_length_; /**< The length of the local_addr_ array. @private */

    int (*write_)(uart_module_t *module,
                  const void *buffer,
                  unsigned int length);

    int (*read_)(uart_module_t *module,
                 void *buffer,
                 unsigned int length);

    int (*flush_tx_)(uart_module_t *module);

    int (*flush_rx_)(uart_module_t *module);

    void (*tx_isr_)(uart_module_t *module);

    void (*rx_isr_)(uart_module_t *module);
    
} uart_private_t;


/* ***** Private Function Definitions ***** */

/**
 * @brief The private implementation of the UART write function for 8-bit mode and HW buffers only.
 *
 * @details This function defines the private implementation of the UART write function when the
 * module is initialized in standard (8-bit) mode and uses only the hardware FIFO buffer. During
 * initialization a pointer to this function is supplied in the uart_module_t::write() function
 * pointer of the initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained to the user in uart_module_t::write().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to write out to the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters written; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_write_8bit_hwonly(uart_module_t *module,
                                          const void *buffer,
                                          unsigned int length)
{
    int data_written = 0;
    const unsigned char *write_ptr = buffer;
    
    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check if TX enabled
    if( !uart_is_open(module, UART_DIRECTION_TX) )
    {// TX is closed
        return UART_E_CLOSED;
    }

    // If space avaliable in FIFO, write data
    while( !IS_MASK_SET( *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_UTXBF ) )
    {// Space available in TX FIFO buffer
        if( data_written < length )
        {// More data to write
            *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxTXREG) = *(write_ptr);
            write_ptr++;
            data_written++;
        }
        else
        {// No more data to write
            return data_written;
        }
    }

    return data_written;
}

/**
 * @brief The private implementation of the UART write function for 8-bit mode with DMA buffer.
 *
 * @details This function defines the private implementation of the UART write function when the
 * module is initialized in standard (8-bit) mode and uses a user provided DMA buffer. During
 * initialization a pointer to this function is supplied in the uart_module_t::write() function
 * pointer of the initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained to the user in uart_module_t::write().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to write out to the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters written; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_write_8bit_dma(uart_module_t *module,
                                       const void *buffer,
                                       unsigned int length)
{
    return UART_E_NONE;
}

/**
 * @brief The private implementation of the UART write function for 8-bit mode with a software
 * buffer.
 *
 * @details This function defines the private implementation of the UART write function when the
 * module is initialized in standard (8-bit) mode and uses a software buffer. During initialization
 * a pointer to this function is supplied in the uart_module_t::write() function pointer of the
 * initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained to the user in uart_module_t::write().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to write out to the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters written; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_write_8bit_soft(uart_module_t *module,
                                        const void *buffer,
                                        unsigned int length)
{
    return UART_E_NONE;
}

/**
 * @brief The private implementation of the UART write function for 8-bit mode and a hybrid --
 * software plus DMA -- buffer implementation.
 *
 * @details This function defines the private implementation of the UART write function when the
 * module is initialized in standard (8-bit) mode and uses both a software and DMA buffer. During
 * initialization a pointer to this function is supplied in the uart_module_t::write() function
 * pointer of the initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained for the user in uart_module_t::write().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to write out to the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters written; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_write_8bit_hybrid(uart_module_t *module,
                                          const void *buffer,
                                          unsigned int length)
{
    return UART_E_NONE;
}

/**
 * @brief The private implementation of the UART read function for 8-bit mode and HW buffer only.
 *
 * @details This function defines the private implementation of the UART read function when the
 * module is initialized in standard (8-bit) mode and uses only the hardware FIFO buffer. During
 * initialization a pointer to this function is supplied in the uart_module_t::read() function pointer of the
 * initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained for the user in uart_module_t::read().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to which to write any new data from the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters read in; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_read_8bit_hwonly(uart_module_t *module,
                                         void *buffer,
                                         unsigned int length)
{
    int data_read = 0;
    unsigned char *read_ptr = buffer;

    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check if RX enabled
    if( !uart_is_open(module, UART_DIRECTION_RX) )
    {// RX is closed
        return UART_E_CLOSED;
    }

    // If data is available in FIFO, read data
    while( !IS_MASK_SET( *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_URXDA ) )
    {// Data available in RX FIFO buffer
        if( data_read < length )
        {// More space available in user buffer
            *(read_ptr) = *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxRXREG);
            read_ptr++;
            data_read++;
        }
        else
        {// No space available in user buffer
            return data_read;
        }
    }

    return data_read;
}

/**
 * @brief The private implementation of the UART read function for 8-bit mode and DMA buffer.
 *
 * @details This function defines the private implementation of the UART read function when the
 * module is initialized in standard (8-bit) mode and uses a user supplied DMA buffer. During
 * initialization a pointer to this function is supplied in the uart_module_t::read() function pointer of the
 * initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained to the user in uart_module_t::read().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to which to write any new data from the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters read in; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_read_8bit_dma(uart_module_t *module,
                                      void *buffer,
                                      unsigned int length)
{
    return UART_E_NONE;
}

/**
 * @brief The private implementation of the UART read function for 8-bit mode and software buffer.
 *
 * @details This function defines the private implementation of the UART read function when the
 * module is initialized in standard (8-bit) mode and uses a software buffer. During initialization
 * a pointer to this function is supplied in the uart_module_t::read() function pointer of the
 * initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained to the user in uart_module_t::read().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to which to write any new data from the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters read in; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_read_8bit_soft(uart_module_t *module,
                                       void *buffer,
                                       unsigned int length)
{
    return UART_E_NONE;
}

/**
 * @brief The private implementation of the UART read function for 8-bit mode and a hybrid --
 * software plus DMA -- buffer.
 *
 * @details This function defines the private implementation of the UART read function when the
 * module is initialized in standard (8-bit) mode and uses a hybrid buffer, which implements both
 * software and DMA buffers. During initialization a pointer to this function is supplied in the
 * uart_module_t::read() function pointer of the initialized #uart_module_t object.
 *
 * The #buffer parameter is cast to a "const unsigned char", so the user must also provide the same
 * type when passing an array. This is explained to the user in uart_module_t::read().
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  buffer
 *             A pointer to the array to which to write any new data from the UART hardware.
 * @param[in]  length
 *             The length of the #buffer array.
 * @return If zero or positive, the number of characters read in; if negative, an error value
 * corresponding to one of #uart_error_e.
 *
 * @private
 */
static int uart_private_read_8bit_hybrid(uart_module_t *module,
                                         void *buffer,
                                         unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_9bit_hwonly(uart_module_t *module,
                                          const void *buffer,
                                          unsigned int length)
{
    int data_written = 0;
    const unsigned int *write_ptr = buffer;
    
    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check if TX enabled
    if( !uart_is_open(module, UART_DIRECTION_TX) )
    {// TX is closed
        return UART_E_CLOSED;
    }

    // If space avaliable in FIFO, write data
    while( !IS_MASK_SET( *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_UTXBF ) )
    {// Space available in TX FIFO buffer
        if( data_written <= length )
        {// More data to write
            *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxTXREG) = *write_ptr;
            write_ptr++;
            data_written++;
        }
        else
        {
            return data_written;
        }
    }

    return data_written;
}

static int uart_private_write_9bit_dma(uart_module_t *module,
                                       const void *buffer,
                                       unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_9bit_soft(uart_module_t *module,
                                        const void *buffer,
                                        unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_9bit_hybrid(uart_module_t *module,
                                          const void *buffer,
                                          unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_9bit_hwonly(uart_module_t *module,
                                         void *buffer,
                                         unsigned int length)
{
    int data_read = 0;
    unsigned int *read_ptr = buffer;

    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check if RX enabled
    if( !uart_is_open(module, UART_DIRECTION_RX) )
    {// RX is closed
        return UART_E_CLOSED;
    }

    // If data is available in FIFO, read data
    while( !IS_MASK_SET( *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_URXDA ) )
    {// Data available in RX FIFO buffer
        if( data_read < length )
        {// More space available in user buffer
            *read_ptr = *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxRXREG);
            read_ptr++;
            data_read++;
        }
        else
        {// No space available in user buffer
            return data_read;
        }
    }

    return data_read;
}

static int uart_private_read_9bit_dma(uart_module_t *module,
                                      void *buffer,
                                      unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_9bit_soft(uart_module_t *module,
                                       void *buffer,
                                       unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_9bit_hybrid(uart_module_t *module,
                                         void *buffer,
                                         unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_lin_hwonly(uart_module_t *module,
                                         const void *buffer,
                                         unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_lin_dma(uart_module_t *module,
                                      const void *buffer,
                                      unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_lin_soft(uart_module_t *module,
                                       const void *buffer,
                                       unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_write_lin_hybrid(uart_module_t * module,
                                         const void *buffer,
                                         unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_lin_hwonly(uart_module_t *module,
                                        void *buffer,
                                        unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_lin_dma(uart_module_t *module,
                                     void *buffer,
                                     unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_lin_soft(uart_module_t *module,
                                      void *buffer,
                                      unsigned int length)
{
    return UART_E_NONE;
}

static int uart_private_read_lin_hybrid(uart_module_t *module,
                                        void *buffer,
                                        unsigned int length)
{
    return UART_E_NONE;
}

/**
 * @brief Flushes any data in the TX buffer.
 *
 * @details The flush function forces any data in the TX buffers to be sent. However, in the
 * hardware only buffer mode any data that is in the FIFO buffer will automatically be sent, so
 * there is nothing for the flush function to do.
 *
 * @param[in]  module
 *             The UART module to work on.
 * @return A value corresponding to one of #uart_error_e.
 * @private
 */
static int uart_private_flush_tx_hwonly(uart_module_t *module)
{
    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check if TX is enabled
    if( !uart_is_open(module, UART_DIRECTION_TX) )
    {// TX is closed
        return UART_E_CLOSED;
    }

    return UART_E_NONE;
}

/**
 * @brief Flushes any data in the RX buffer.
 *
 * @details The flush function forces any data in the RX hardware buffers to be made available via
 * the uart_module_t::read() function. However, in the hardware only buffer mode all data in the
 * FIFO buffer is already accessible, so there is nothing for the flush function to do.
 *
 * @param[in]  module
 *             The UART module to work on.
 * @return A value corresponding to one of #uart_error_e.
 * @private
 */
static int uart_private_flush_rx_hwonly(uart_module_t *module)
{
    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check if RX is enabled
    if( !uart_is_open(module, UART_DIRECTION_RX) )
    {// RX is closed
        return UART_E_CLOSED;
    }

    return UART_E_NONE;
}

static int uart_private_flush_tx_dma(uart_module_t *module)
{
    return UART_E_NONE;
}

static int uart_private_flush_rx_dma(uart_module_t *module)
{
    return UART_E_NONE;
}

static int uart_private_flush_tx_soft(uart_module_t *module)
{
    return UART_E_NONE;
}

static int uart_private_flush_rx_soft(uart_module_t *module)
{
    return UART_E_NONE;
}

static int uart_private_flush_tx_hybrid(uart_module_t *module)
{
    return UART_E_NONE;
}

static int uart_private_flush_rx_hybrid(uart_module_t *module)
{
    return UART_E_NONE;
}

static void uart_private_tx_isr_hwonly(uart_module_t *module)
{
    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return;
    }

    // Check if TX enabled
    if( !uart_is_open(module, UART_DIRECTION_TX) )
    {// TX is closed
        return;
    }

    // Notify user by calling tx_callback
    if( module->tx_callback != NULL )
    {// Callback is valid
        module->tx_callback(module);
    }
}

static void uart_private_tx_isr_dma(uart_module_t *module)
{
}

static void uart_private_tx_isr_soft(uart_module_t *module)
{
}

static void uart_private_tx_isr_hybrid(uart_module_t *module)
{
}

static void uart_private_rx_isr_hwonly(uart_module_t *module)
{
    // Check for a valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return;
    }

    // Check if RX enabled
    if( !uart_is_open(module, UART_DIRECTION_RX) )
    {// RX is closed
        return;
    }

    // Notify user by calling rx_callback
    if( module->rx_callback != NULL )
    {// Callback is valid
        module->rx_callback(module);
    }   
}

static void uart_private_rx_isr_dma(uart_module_t *module)
{
}

static void uart_private_rx_isr_soft(uart_module_t *module)
{
}

static void uart_private_rx_isr_hybrid(uart_module_t *module)
{
}


/* ***** Public Function Definitions ***** */

int uart_init(uart_module_t *module,
              uart_attr_t *attr,
              dma_channel_t *tx_dma,
              dma_channel_t *rx_dma)
{
    unsigned int buffer_size = 0;
    
    // Check for a valid module pointer and module number
    if( module == NULL \
        || module->uart_number == 0 \
        || module->uart_number > UART_HW_NUMBER_OF_MODULES-1 )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Check for a valid attr pointer
    if( attr == NULL )
    {// Invalid attr pointer
        return UART_E_INPUT;
    }

    // Allocate a new private struct (initialize all values to zero)
    module->private = calloc(1,sizeof(uart_private_t));
    if( module->private == NULL )
    {// Allocation failed
        return UART_E_ALLOC;
    }

#if (UART_HW_NUMBER_OF_MODULES < 1) || (UART_HW_NUMBER_OF_MODULES > 4)
#error "UART(__file__,__line__): UART_HW_NUMBER_OF_MODULES is out of bounds. The UART library is \
expecting a number between 1 and 4."
#endif
    switch(module->uart_number)
    {
#if UART_HW_NUMBER_OF_MODULES >= 1
    case 1:
        UART_GET_BASE_ADDRESS(module) = UART_HW_BASE_ADDRESS_UART1;
        break;
#endif
#if UART_HW_NUMBER_OF_MODULES >= 2
    case 2:
        UART_GET_BASE_ADDRESS(module) = UART_HW_BASE_ADDRESS_UART2;
        break;
#endif
#if UART_HW_NUMBER_OF_MODULES >= 3       
    case 3:
        UART_GET_BASE_ADDRESS(module) = UART_HW_BASE_ADDRESS_UART3;
        break;
#endif
#if UART_HW_NUMBER_OF_MODULES >= 4
    case 4:
        UART_GET_BASE_ADDRESS(module) = UART_HW_BASE_ADDRESS_UART4;
        break;
#endif
    default:
        // Invalid module uart number (should have already been checked!)
        // Assertion failed!
        return UART_E_ASSERT;
    }

    // Save the attr struct to the private data object
    ((uart_private_t *)module->private)->attr_ = *attr;

    // Set all SFRs to default values (see datasheet for default values)
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE) = UART_SFR_DEFAULT_UxMODE;
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA)  = UART_SFR_DEFAULT_UxSTA;
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG)  = UART_SFR_DEFAULT_UxBRG;

    // Set baudrate_ to valid value
    ((uart_private_t *)module->private)->baudrate_ = UART_BAUDRATE_UNKNOWN;


    
    /* ***** Configure Module Settings ***** */
    
    // Configure CPU Idle settings
    if( (UART_GET_ATTR(module).module_settings & UART_MODULE_CPUIDLE_BITMASK) == UART_MODULE_CPUIDLE_STOP )
    {// Stop module operation during CPU idle
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_USIDL);
    }
    // Otherwise default to continue module operation during CPU idle

    // Configure Wake-up settings
    if( (UART_GET_ATTR(module).module_settings & UART_MODULE_WAKEUP_BITMASK) == UART_MODULE_WAKEUP_EN )
    {// Enable wake-up on start bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_WAKE);
    }
    // Otherwise default to disable wake-up on start bit

    // Configure Flow Control settings
    // Must occur before mode settings in the case IrDA external encoder is used. IrDA settings
    // will override any flow control with an external encoder.
    if( (UART_GET_ATTR(module).module_settings & UART_MODULE_FLOWCNTL_BITMASK) == UART_MODULE_FLOWCNTL_RTS )
    {// RTS pin only
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UEN0);
        WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UEN1);
    }
    else if( (UART_GET_ATTR(module).module_settings & UART_MODULE_FLOWCNTL_BITMASK) == UART_MODULE_FLOWCNTL_RTSCTS )
    {// RTS and CTS pins
        WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UEN0);
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UEN1);
    }
    // Otherwise default to no flow control

    // Configure RTS Mode settings
    if( (UART_GET_ATTR(module).module_settings & UART_MODULE_RTSMODE_BITMASK) == UART_MODULE_RTSMODE_SIMPLEX )
    {// Simplex mode
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_RTSMD);
    }
    // Otherwise default to flow control mode

    // Configure RX Idle state settings
    if( (UART_GET_ATTR(module).module_settings & UART_MODULE_RXIDLE_BITMASK) == UART_MODULE_RXIDLE_LOW )
    {// RX idle low
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_URXINV);
    }
    // Otherwise default to RX idle high

    // Configure TX Idle state settings
    if( (UART_GET_ATTR(module).module_settings & UART_MODULE_TXIDLE_BITMASK) == UART_MODULE_TXIDLE_LOW )
    {// TX idle low
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_UTXINV);
    }
    // Otherwise default to TX idle high



    /* ***** Configure Mode Settings ***** */

    // Select major mode
    switch( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) )
    {
    case UART_MAJOR_MODE_9BIT:
        // 9-bit mode
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL1);
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL0);

        // Select minor mode (bit format)
        if( (UART_GET_ATTR(module).mode_settings & UART_MINOR_MODE_BITMASK) == UART_MINOR_MODE_9BIT_9N2 )
        {// 9-bit mode, no parity, 2 stop bits
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_STSEL);
        }
        // Otherwise 9-bit mode, no parity, 1 stop bit (default)

        // Select minor mode (addressing)
        if( (UART_GET_ATTR(module).mode_settings & UART_MINOR_MODE_BITMASK) == UART_MINOR_MODE_9BIT_ADDR_MASK )
        {// 9-bit mode, 9th bit marks address byte, mask data according to local_addr_ array
            // Enable address character detection
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_ADDEN);

            // Allocate UART_DEF_LOCAL_ADDRESS_MAX number addresses
            ((uart_private_t *)module->private)->local_addr_ = malloc(sizeof(char)*UART_DEF_LOCAL_ADDR_SIZE);
            if( ((uart_private_t *)(module->private))->local_addr_ == NULL )
            {// Allocation failed
                // Return to an uninitialized state, free memory, and return an error code
                uart_cleanup(module);
                return UART_E_ALLOC;
            }
            
            ((uart_private_t *)module->private)->local_addr_length_ = 0;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MINOR_MODE_BITMASK) == UART_MINOR_MODE_9BIT_ADDR_PROM )
        {// 9-bit mode, 9th bit marks address byte, promiscuous mode
            // Enable address character detection
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_ADDEN);
        }// Otherwise 9-bit mode, treat all characters as data
            
        break;
    case UART_MAJOR_MODE_IRDA:
        // IrDA mode
        if( (UART_GET_ATTR(module).mode_settings & UART_MINOR_MODE_BITMASK) == UART_MINOR_MODE_IRDA_EXTERNAL )
        {// External IrDA encoder/decoder
            // Enable BCLKx pin
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UEN1);
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UEN0);
        }
        else
        {// Otherwise default to internal IrDA encoder/decoder
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_IREN);
            // Leave flow control as is
        }

        break;
    case UART_MAJOR_MODE_LIN:
        // LIN mode
        /**
         * @todo Fill in LIN mode code.
         */

        break;
    case UART_MAJOR_MODE_STD:
    default:
        // Standard (8-bit) mode (default)
        // Select minor mode
        switch( (UART_GET_ATTR(module).mode_settings & UART_MINOR_MODE_BITMASK) )
        {
        case UART_MINOR_MODE_STD_8N2:
            // Standard (8-bit) mode, no parity, 2 stop bits
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_STSEL);
            
            break;
        case UART_MINOR_MODE_STD_8O1:
            // Standard (8-bit) mode, odd parity, 1 stop bit
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL1);
            WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL0);
            
            break;
        case UART_MINOR_MODE_STD_8O2:
            // Standard (8-bit) mode, odd parity, 2 stop bits
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL1);
            WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL0);
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_STSEL);
            
            break;
        case UART_MINOR_MODE_STD_8E1:
            // Standard (8-bit) mode, even parity, 1 stop bit
            WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL1);
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL0);
            
            break;
        case UART_MINOR_MODE_STD_8E2:
            // Standard (8-bit) mode, even parity, 2 stop bits
            WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL1);
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_PDSEL0);
            WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_STSEL);
            
            break;
        case UART_MINOR_MODE_STD_8N1:
        default:
            // Standard (8-bit) mode, no parity, 1 stop bit (default)
            break;
        }

        break;
    }

    // Configure debug modes
    if( (UART_GET_ATTR(module).mode_settings & UART_DEBUG_MODE_BITMASK) == UART_DEBUG_MODE_LOOP )
    {// Configure loopback mode
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_LPBACK);
    }// Otherwise default to no debug aids on
    
    

    /* ***** Configure TX Buffer Settings ***** */

    // Configure TX buffer mode
    switch( (UART_GET_ATTR(module).tx_buffer_settings & UART_TX_BUFFER_MODE_BITMASK) )
    {
    case UART_TX_BUFFER_MODE_DMA:
        // Use DMA buffer for TX

        // Check for a valid DMA channel
        if( !dma_is_valid(tx_dma) )
        {// DMA channel is invalid
            uart_cleanup(module);
            return UART_E_INPUT;
        }

        // Copy DMA channel to private object
        ((uart_private_t *)module->private)->tx_dma_ = tx_dma;

        /**
         * @todo Set up TX DMA buffer.
         */
        
        break;
    case UART_TX_BUFFER_MODE_SOFT:
        // User software buffer for TX

        // Determine size of software buffer
        switch( (UART_GET_ATTR(module).tx_buffer_settings & UART_TX_BUFFER_SIZE_BITMASK) )
        {
        case UART_TX_BUFFER_SIZE_8:
            // Software buffer is 8 characters long
            buffer_size = 8;
            break;
        case UART_TX_BUFFER_SIZE_12:
            // Software buffer is 12 characters long
            buffer_size = 12;
            break;
        case UART_TX_BUFFER_SIZE_16:
            // Software buffer is 16 characters long
            buffer_size = 16;
            break;
        case UART_TX_BUFFER_SIZE_24:
            // Software buffer is 24 characters long
            buffer_size = 24;
            break;
        case UART_TX_BUFFER_SIZE_32:
            // Software buffer is 32 characters long
            buffer_size = 32;
            break;
        case UART_TX_BUFFER_SIZE_64:
            // Software buffer is 64 characters long
            buffer_size = 64;
            break;
        case UART_TX_BUFFER_SIZE_128:
            // Software buffer is 128 characters long
            buffer_size = 128;
            break;
        case UART_TX_BUFFER_SIZE_4:
        default:
            // Software buffer is 4 characters long
            buffer_size = 4;
            break;
        }

        // Check what character size to use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode, allocate a word for each character
            
            // Allocate TX buffer
            ((uart_private_t *)module->private)->tx_buffer_ \
                = malloc(sizeof(int)*buffer_size);
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            /**
             * @todo Fill in LIN code for buffer allocation.
             */
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            
            // Allocate TX buffer
            ((uart_private_t *)module->private)->tx_buffer_ \
                = malloc(sizeof(char)*buffer_size);
        }

        // Check for failed allocation
        if( ((uart_private_t *)module->private)->tx_buffer_ == NULL )
        {// Allocation failed
            uart_cleanup(module);
            return UART_E_ALLOC;
        }
        
        // Set read and write pointers
        ((uart_private_t *)module->private)->tx_head_ \
            = ((uart_private_t *)module->private)->tx_buffer_;
        ((uart_private_t *)module->private)->tx_tail_ \
            = ((uart_private_t *)module->private)->tx_buffer_;
                
        break;
    case UART_TX_BUFFER_MODE_HYBRID:
        // Use both DMA and software buffers

        // Check for a valid DMA channel
        if( !dma_is_valid(tx_dma) )
        {// DMA channel is invalid
            uart_cleanup(module);
            return UART_E_INPUT;
        }

        // Copy pointer to DMA channel to private object
        ((uart_private_t *)module->private)->tx_dma_ = tx_dma;

        /**
         * @todo Set up TX DMA buffer.
         */

        // Determine size of software buffer
        switch( (UART_GET_ATTR(module).tx_buffer_settings & UART_TX_BUFFER_SIZE_BITMASK) )
        {
        case UART_TX_BUFFER_SIZE_8:
            // Software buffer is 8 characters long
            buffer_size = 8;
            break;
        case UART_TX_BUFFER_SIZE_12:
            // Software buffer is 12 characters long
            buffer_size = 12;
            break;
        case UART_TX_BUFFER_SIZE_16:
            // Software buffer is 16 characters long
            buffer_size = 16;
            break;
        case UART_TX_BUFFER_SIZE_24:
            // Software buffer is 24 characters long
            buffer_size = 24;
            break;
        case UART_TX_BUFFER_SIZE_32:
            // Software buffer is 32 characters long
            buffer_size = 32;
            break;
        case UART_TX_BUFFER_SIZE_64:
            // Software buffer is 64 characters long
            buffer_size = 64;
            break;
        case UART_TX_BUFFER_SIZE_128:
            // Software buffer is 128 characters long
            buffer_size = 128;
            break;
        case UART_TX_BUFFER_SIZE_MATCH:
        default:
            // Software buffer tries to match DMA buffer length
            buffer_size = ((uart_private_t *)module->private)->tx_dma_->buffer_a_size;
            break;
        }

        // Check what character size to use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode, allocate a word for each character
            
            // Allocate TX buffer
            ((uart_private_t *)module->private)->tx_buffer_ \
                = malloc(sizeof(int)*buffer_size);
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            /**
             * @todo Fill in LIN code for buffer allocation.
             */
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            
            // Allocate TX buffer
            ((uart_private_t *)module->private)->tx_buffer_ \
                = malloc(sizeof(char)*buffer_size);
        }

        // Check for failed allocation
        if( ((uart_private_t *)module->private)->tx_buffer_ == NULL )
        {// Allocation failed
            uart_cleanup(module);
            return UART_E_ALLOC;
        }
        
        // Set read and write pointers
        ((uart_private_t *)module->private)->tx_head_ \
            = ((uart_private_t *)module->private)->tx_buffer_;
        ((uart_private_t *)module->private)->tx_tail_ \
            = ((uart_private_t *)module->private)->tx_buffer_;
        
        break;
    case UART_TX_BUFFER_MODE_HWONLY:
    default:
        // TX hardware FIFO only
        // Nothing to do
        break;
    }

    
    /* ***** Configure RX Buffer Settings ***** */
    
    // Configure RX buffer mode
    switch( (UART_GET_ATTR(module).rx_buffer_settings & UART_RX_BUFFER_MODE_BITMASK) )
    {
    case UART_RX_BUFFER_MODE_DMA:
        // Use DMA buffer for RX

        // Check for a valid DMA channel
        if( !dma_is_valid(rx_dma) )
        {// DMA channel is invalid
            uart_cleanup(module);
            return UART_E_INPUT;
        }

        // Copy DMA channel to private object
        ((uart_private_t *)module->private)->rx_dma_ = rx_dma;

        /**
         * @todo Set up RX DMA buffer.
         */
        
        break;
    case UART_RX_BUFFER_MODE_SOFT:
        // User software buffer for RX

        // Determine size of software buffer
        switch( (UART_GET_ATTR(module).rx_buffer_settings & UART_RX_BUFFER_SIZE_BITMASK) )
        {
        case UART_RX_BUFFER_SIZE_8:
            // Software buffer is 8 characters long
            buffer_size = 8;
            break;
        case UART_RX_BUFFER_SIZE_12:
            // Software buffer is 12 characters long
            buffer_size = 12;
            break;
        case UART_RX_BUFFER_SIZE_16:
            // Software buffer is 16 characters long
            buffer_size = 16;
            break;
        case UART_RX_BUFFER_SIZE_24:
            // Software buffer is 24 characters long
            buffer_size = 24;
            break;
        case UART_RX_BUFFER_SIZE_32:
            // Software buffer is 32 characters long
            buffer_size = 32;
            break;
        case UART_RX_BUFFER_SIZE_64:
            // Software buffer is 64 characters long
            buffer_size = 64;
            break;
        case UART_RX_BUFFER_SIZE_128:
            // Software buffer is 128 characters long
            buffer_size = 128;
            break;
        case UART_RX_BUFFER_SIZE_4:
        default:
            // Software buffer is 4 characters long
            buffer_size = 4;
            break;
        }

        // Check what character size to use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode, allocate a word for each character
            
            // Allocate RX buffer
            ((uart_private_t *)module->private)->rx_buffer_ \
                = malloc(sizeof(int)*buffer_size);
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            /**
             * @todo Fill in LIN code for buffer allocation.
             */
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            
            // Allocate RX buffer
            ((uart_private_t *)module->private)->rx_buffer_ \
                = malloc(sizeof(char)*buffer_size);
        }

        // Check for failed allocation
        if( ((uart_private_t *)module->private)->rx_buffer_ == NULL )
        {// Allocation failed
            uart_cleanup(module);
            return UART_E_ALLOC;
        }
        
        // Set read and write pointers
        ((uart_private_t *)module->private)->rx_head_ \
            = ((uart_private_t *)module->private)->rx_buffer_;
        ((uart_private_t *)module->private)->rx_tail_ \
            = ((uart_private_t *)module->private)->rx_buffer_;
                
        break;
    case UART_RX_BUFFER_MODE_HYBRID:
        // Use both DMA and software buffers

        // Check for a valid DMA channel
        if( !dma_is_valid(rx_dma) )
        {// DMA channel is invalid
            uart_cleanup(module);
            return UART_E_INPUT;
        }

        // Copy pointer to DMA channel to private object
        ((uart_private_t *)module->private)->rx_dma_ = rx_dma;

        /**
         * @todo Set up RX DMA buffer.
         */

        // Determine size of software buffer
        switch( (UART_GET_ATTR(module).rx_buffer_settings & UART_RX_BUFFER_SIZE_BITMASK) )
        {
        case UART_RX_BUFFER_SIZE_8:
            // Software buffer is 8 characters long
            buffer_size = 8;
            break;
        case UART_RX_BUFFER_SIZE_12:
            // Software buffer is 12 characters long
            buffer_size = 12;
            break;
        case UART_RX_BUFFER_SIZE_16:
            // Software buffer is 16 characters long
            buffer_size = 16;
            break;
        case UART_RX_BUFFER_SIZE_24:
            // Software buffer is 24 characters long
            buffer_size = 24;
            break;
        case UART_RX_BUFFER_SIZE_32:
            // Software buffer is 32 characters long
            buffer_size = 32;
            break;
        case UART_RX_BUFFER_SIZE_64:
            // Software buffer is 64 characters long
            buffer_size = 64;
            break;
        case UART_RX_BUFFER_SIZE_128:
            // Software buffer is 128 characters long
            buffer_size = 128;
            break;
        case UART_RX_BUFFER_SIZE_MATCH:
        default:
            // Software buffer tries to match DMA buffer length
            buffer_size = ((uart_private_t *)module->private)->rx_dma_->buffer_a_size;
            break;
        }

        // Check what character size to use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode, allocate a word for each character
            
            // Allocate TX buffer
            ((uart_private_t *)module->private)->rx_buffer_ \
                = malloc(sizeof(int)*buffer_size);
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            /**
             * @todo Fill in LIN code for buffer allocation.
             */
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            
            // Allocate RX buffer
            ((uart_private_t *)module->private)->rx_buffer_ \
                = malloc(sizeof(char)*buffer_size);
        }

        // Check for failed allocation
        if( ((uart_private_t *)module->private)->rx_buffer_ == NULL )
        {// Allocation failed
            uart_cleanup(module);
            return UART_E_ALLOC;
        }
        
        // Set read and write pointers
        ((uart_private_t *)module->private)->rx_head_ \
            = ((uart_private_t *)module->private)->rx_buffer_;
        ((uart_private_t *)module->private)->rx_tail_ \
            = ((uart_private_t *)module->private)->rx_buffer_;
        
        break;
    case UART_RX_BUFFER_MODE_HWONLY:
    default:
        // RX hardware FIFO only
        // Nothing to do
        break;
    }


    /* ***** Set Module Function Pointers ***** */

    // Determine TX buffer mode
    switch( (UART_GET_ATTR(module).tx_buffer_settings & UART_TX_BUFFER_MODE_BITMASK) )
    {
    case UART_TX_BUFFER_MODE_HWONLY:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_9bit_hwonly;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_lin_hwonly;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->write_ = &uart_private_write_8bit_hwonly;
        }

        ((uart_private_t *)module->private)->flush_tx_ = &uart_private_flush_tx_hwonly;
        ((uart_private_t *)module->private)->tx_isr_ = &uart_private_tx_isr_hwonly;

        break;
    case UART_TX_BUFFER_MODE_DMA:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_9bit_dma;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_lin_dma;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->write_ = &uart_private_write_8bit_dma;
        }

        ((uart_private_t *)module->private)->flush_tx_ = &uart_private_flush_tx_dma;
        ((uart_private_t *)module->private)->tx_isr_ = &uart_private_tx_isr_dma;

        break;
    case UART_TX_BUFFER_MODE_SOFT:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_9bit_soft;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_lin_soft;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->write_ = &uart_private_write_8bit_soft;
        }

        ((uart_private_t *)module->private)->flush_tx_ = &uart_private_flush_tx_soft;
        ((uart_private_t *)module->private)->tx_isr_ = &uart_private_tx_isr_soft;

        break;
    case UART_TX_BUFFER_MODE_HYBRID:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_9bit_hybrid;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->write_ = &uart_private_write_lin_hybrid;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->write_ = &uart_private_write_8bit_hybrid;
        }

        ((uart_private_t *)module->private)->flush_tx_ = &uart_private_flush_tx_hybrid;
        ((uart_private_t *)module->private)->tx_isr_ = &uart_private_tx_isr_hybrid;

        break;
    default:
        // Should never reach this point!
        uart_cleanup(module);
        return UART_E_ASSERT;
    }

    // Determine RX buffer mode
    switch( (UART_GET_ATTR(module).rx_buffer_settings & UART_RX_BUFFER_MODE_BITMASK) )
    {
    case UART_RX_BUFFER_MODE_HWONLY:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_9bit_hwonly;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_lin_hwonly;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->read_ = &uart_private_read_8bit_hwonly;
        }

        ((uart_private_t *)module->private)->flush_rx_ = &uart_private_flush_rx_hwonly;
        ((uart_private_t *)module->private)->rx_isr_ = &uart_private_rx_isr_hwonly;

        break;
    case UART_RX_BUFFER_MODE_DMA:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_9bit_dma;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_lin_dma;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->read_ = &uart_private_read_8bit_dma;
        }

        ((uart_private_t *)module->private)->flush_rx_ = &uart_private_flush_rx_dma;
        ((uart_private_t *)module->private)->rx_isr_ = &uart_private_rx_isr_dma;

        break;
    case UART_RX_BUFFER_MODE_SOFT:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_9bit_soft;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_lin_soft;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->read_ = &uart_private_read_8bit_soft;
        }

        ((uart_private_t *)module->private)->flush_rx_ = &uart_private_flush_rx_soft;
        ((uart_private_t *)module->private)->rx_isr_ = &uart_private_rx_isr_soft;

        break;
    case UART_RX_BUFFER_MODE_HYBRID:

        // Check what major mode is in use
        if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_9BIT )
        {// Using 9-bit mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_9bit_hybrid;
        }
        else if( (UART_GET_ATTR(module).mode_settings & UART_MAJOR_MODE_BITMASK) == UART_MAJOR_MODE_LIN )
        {// Using LIN mode
            ((uart_private_t *)module->private)->read_ = &uart_private_read_lin_hybrid;
        }
        else
        {// Default to using standard (8-bit) mode, allocate a byte for each character
            ((uart_private_t *)module->private)->read_ = &uart_private_read_8bit_hybrid;
        }

        ((uart_private_t *)module->private)->flush_rx_ = &uart_private_flush_rx_hybrid;
        ((uart_private_t *)module->private)->rx_isr_ = &uart_private_rx_isr_hybrid;

        break;
    default:
        // Should never reach this point!
        uart_cleanup(module);
        return UART_E_ASSERT;
    }

    return UART_E_NONE;
}

/**
 * @note This function makes use of uart_hw.h defined constants for BRG. Be sure to double check
 * that the Fcy you are using is able to adequetly generate the selected baudrate!
 *
 * @todo Set BRGH bit according to uart_hw.h.
 */
int uart_set_baudrate(uart_module_t *module,
                      uart_baudrate_t baudrate)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Invalid module
        return UART_E_MODULE;
    }

    // Cancel any autobaud in progress
    WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_ABAUD);

#if defined(UART_DEF_MANUAL_BAUDRATE) // Set BRGH and BRG manually through uart.def
    
#if defined(UART_DEF_SET_BRGH)
    // Set BRGH bit
    WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);
#else
    // Clear BRGH bit
    WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);
#endif

#if defined(UART_DEF_BRG)
    // Set BRG value
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_DEF_BRG;
#else // Can't set BRG value
#error "UART: Can't set baudrate manually, no BRG value defined!"
#endif
    
#else // Set BRGH and BRG according to uart_hw.h constants

    switch( baudrate )
    {
    case UART_BAUDRATE_1200:
        // Set baudrate to 1,200bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;
        
        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_1200;
        
        break;
    case UART_BAUDRATE_2400:
        // Set baudrate to 2,400bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;
        
        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_2400;
        
        break;
    case UART_BAUDRATE_4800:
        // Set baudrate to 4,800bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_4800;
        
        break;
    case UART_BAUDRATE_9600:
        // Set baudrate to 9,600bps
        ((uart_private_t *)module->private)->baudrate_ = UART_BAUDRATE_9600;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_9600;
        
        break;
    case UART_BAUDRATE_19200:
        // Set baudrate to 19,200bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_19200;
        
        break;
    case UART_BAUDRATE_38400:
        // Set baudrate to 38,400bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_38400;
        
        break;
    case UART_BAUDRATE_57600:
        // Set baudrate to 57,600bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_57600;
        
        break;
    case UART_BAUDRATE_115200:
        // Set baudrate to 115,200bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_115200;
        
        break;
    case UART_BAUDRATE_230400:
        // Set baudrate to 230,400bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_230400;
        
        break;
    case UART_BAUDRATE_460800:
        // Set baudrate to 460,800bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_460800;
        
        break;
    case UART_BAUDRATE_921600:
        // Set baudrate to 921,600bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_921600;
        
        break;
    case UART_BAUDRATE_1000000:
        // Set baudrate to 1,000,000bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_1000000;
        
        break;
    case UART_BAUDRATE_1843200:
        // Set baudrate to 1,843,200bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_1843200;
        
        break;
    case UART_BAUDRATE_2000000:
        // Set baudrate to 2,000,000bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_2000000;
        
        break;
    case UART_BAUDRATE_3686400:
        // Set baudrate to 3,686,400bps
        ((uart_private_t *)module->private)->baudrate_ = baudrate;

        // Set BRGH bit
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_BRGH);

        // Set UxBRG register to precomputed value
        *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG) = UART_HW_BRG_3686400;
        
        break;
    default:
        return UART_E_INPUT;

        break;
    }
#endif // defined(UART_DEF_MANUAL_BAUDRATE)

    return UART_E_NONE;
}

int uart_get_baudrate(uart_module_t *module)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    // Return current baudrate setting
    return ((uart_private_t *)module->private)->baudrate_;
}

int uart_autobaud(uart_module_t *module)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    // Start autobaud
    WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_ABAUD);

    // Set baudrate to UART_BAUDRATE_AUTO
    ((uart_private_t *)(module->private))->baudrate_ = UART_BAUDRATE_AUTO;

    return UART_E_NONE;
}

int uart_add_local_addr(uart_module_t *module,
                        char addr)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    /**
     * @todo Fill in uart_add_local_addr() code.
     */
    
    return UART_E_NONE;
}

int uart_remove_local_addr(uart_module_t *module,
                           char addr)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    /**
     * @todo Fill in uart_remove_local_addr() code.
     */

    return UART_E_NONE;
}

int uart_open(uart_module_t *module,
              uart_direction_t direction)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    // Check for valid open states
    // TX only is not allowed, so if UART_DIRECTION_TX is used RX must already be open.
    if( direction == UART_DIRECTION_TX \
        && (((uart_private_t *)(module->private))->open_state_ == UART_DIRECTION_NONE) )
    {// Invalid open state
        return UART_E_INPUT;
    }

    if( direction == UART_DIRECTION_RX || direction == UART_DIRECTION_TXRX )
    {// Set up RX functionality
        
        // Enable UART module (RX)
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UARTEN);

        // Check configuration
        switch( (UART_GET_ATTR(module).rx_buffer_settings & UART_RX_BUFFER_MODE_BITMASK) )
        {
        case UART_RX_BUFFER_MODE_HWONLY:
            // Set up interrupts
            break;
        case UART_RX_BUFFER_MODE_DMA:
            // Enable DMA channel
            // Set up interrupts
            break;
        case UART_RX_BUFFER_MODE_SOFT:
            // Set up interrupts
            break;
        case UART_RX_BUFFER_MODE_HYBRID:
            // Enable DMA channel
            // Set up interrupts
            break;
        default:
            // Should never reach this point!
            return UART_E_ASSERT;
        }
        
        // Update open_state_
        WRITE_MASK_SET(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_RX);
    }

    
    if( direction == UART_DIRECTION_TX || direction == UART_DIRECTION_TXRX )
    {// Set up TX functionality

        // Enable UART module (TX)
        WRITE_MASK_SET(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_UTXEN);

        // Check configuration
        switch( (UART_GET_ATTR(module).tx_buffer_settings & UART_TX_BUFFER_MODE_BITMASK) )
        {
        case UART_TX_BUFFER_MODE_HWONLY:
            // Enable interrupts
            break;
        case UART_TX_BUFFER_MODE_DMA:
            // Enable DMA channel
            // Enable interrupts
            break;
        case UART_TX_BUFFER_MODE_SOFT:
            // Enable interrupts
            break;
        case UART_TX_BUFFER_MODE_HYBRID:
            // Enable DMA channel
            // Enable interrupts
            break;
        default:
            // Should never reach this point!
            return UART_E_ASSERT;
        }
        
        // Update open_state_
        WRITE_MASK_SET(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_TX);
    }

    return UART_E_NONE;
}

int uart_write(uart_module_t *module,
               const void *buffer,
               unsigned int length)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    // Call correct write function and return result
    return ((uart_private_t *)module->private)->write_(module, buffer, length);
}

int uart_read(uart_module_t *module,
              void *buffer,
              unsigned int length)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    // Call correct read function and return result
    return ((uart_private_t *)module->private)->read_(module, buffer, length);
}

int uart_close(uart_module_t *module,
               uart_direction_t direction)
{
    // Check for valid module
    if( !uart_is_valid(module) )
    {// Module is invalid
        return UART_E_MODULE;
    }

    // Check for valid open states
    // TX only is not allowed, so if UART_DIRECTION_RX is used TX must not be open.
    if( direction == UART_DIRECTION_RX \
        && (((uart_private_t *)(module->private))->open_state_ == UART_DIRECTION_TXRX) )
    {// Invalid open state
        return UART_E_INPUT;
    }

    if( direction == UART_DIRECTION_RX || direction == UART_DIRECTION_TXRX )
    {// Disable RX functionality
        
        // Disable UART module (RX)
        WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE), UART_SFR_BITMASK_UARTEN);

        // Check configuration
        switch( (UART_GET_ATTR(module).rx_buffer_settings & UART_RX_BUFFER_MODE_BITMASK) )
        {
        case UART_RX_BUFFER_MODE_HWONLY:
            // Disable interrupts
            break;
        case UART_RX_BUFFER_MODE_DMA:
            // Disable DMA channel
            // Disable interrupts
            break;
        case UART_RX_BUFFER_MODE_SOFT:
            // Disable interrupts
            break;
        case UART_RX_BUFFER_MODE_HYBRID:
            // Disable DMA channel
            // Disable interrupts
            break;
        default:
            // Should never reach this point!
            return UART_E_ASSERT;
        }
        
        // Update open_state_
        WRITE_MASK_CLEAR(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_RX);
    }

    
    if( direction == UART_DIRECTION_TX || direction == UART_DIRECTION_TXRX )
    {// Disable TX functionality

        // Disable UART module (TX)
        WRITE_MASK_CLEAR(*(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA), UART_SFR_BITMASK_UTXEN);

        // Check configuration
        switch( (UART_GET_ATTR(module).tx_buffer_settings & UART_TX_BUFFER_MODE_BITMASK) )
        {
        case UART_TX_BUFFER_MODE_HWONLY:
            // Disable interrupts
            break;
        case UART_TX_BUFFER_MODE_DMA:
            // Disable DMA channel
            // Disable interrupts
            break;
        case UART_TX_BUFFER_MODE_SOFT:
            // Disable interrupts
            break;
        case UART_TX_BUFFER_MODE_HYBRID:
            // Disable DMA channel
            // Disable interrupts
            break;
        default:
            // Should never reach this point!
            return UART_E_ASSERT;
        }
        
        // Update open_state_
        WRITE_MASK_CLEAR(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_TX);
    }

    return UART_E_NONE;
}

void uart_cleanup(uart_module_t *module)
{

    // Close the module
    uart_close(module, UART_DIRECTION_TXRX);

    // Deal with DMA channels
    /**
     * @todo Deal with DMA channels.
     */

    // Free all allocated memory
    free( ((uart_private_t *)(module->private))->tx_buffer_ );
    free( ((uart_private_t *)(module->private))->rx_buffer_ );
    free( ((uart_private_t *)(module->private))->local_addr_ );
    free( module->private );
    module->private = NULL;
    
    // Set all SFRs to default values
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxMODE) = UART_SFR_DEFAULT_UxMODE;
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxSTA)  = UART_SFR_DEFAULT_UxSTA;
    *(UART_GET_BASE_ADDRESS(module) + UART_SFR_OFFSET_UxBRG)  = UART_SFR_DEFAULT_UxBRG;
}


inline bool uart_is_open(uart_module_t *module,
                         uart_direction_t direction)
{
    switch(direction)
    {
    case UART_DIRECTION_NONE:
        // Check if both TX and RX are closed
        // Return false if RX is enabled
        // Return true if RX is disabled (TX cannot be enabled without RX also enabled)
        return IS_MASK_CLEAR(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_BITMASK);

    case UART_DIRECTION_RX:
        // Check if RX is enabled (don't care about TX)
        // Return false if RX is disabled
        // Return true if RX is enabled
        return IS_MASK_SET(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_RX);
        
    case UART_DIRECTION_TX:
        // Check if TX is enabled (TX cannot be enabled without RX also enabled)
    case UART_DIRECTION_TXRX:
        // Check if both TX and RX are enabled
        // Return false if RX is disabled
        // Return true if RX and TX are enabled
        return IS_MASK_SET(((uart_private_t *)(module->private))->open_state_, UART_DIRECTION_TXRX);

    default:
        return false;
    }
}

inline bool uart_is_valid(uart_module_t *module)
{
    return ( module == NULL \
             || module->uart_number > (UART_HW_NUMBER_OF_MODULES-1) \
             || module->private == NULL ) ? false : true;
}

/**
 * @}
 */ // End of UARTModule group
