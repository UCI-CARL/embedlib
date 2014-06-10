/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file dma_channel_xc16.h
 *
 * @brief This module contains code to control a single DMA channel on a dsPIC33F chip.
 *
 * It uses the
 * base address of the DMA control registers along with their defined offsets to set the various
 * registers for that particular channel.
 *
 * @date 12/3/2012
 * @author Liam Bucci
 * @carlnumber FIRM-0004
 * @version 0.3.0
 *
 */

// Include guard
#ifndef _DMA_CHANNEL_XC16_H
#define _DMA_CHANNEL_XC16_H

#include <stdbool.h>

/**
 * @defgroup DMAChannel DMA Channel
 *
 * @brief Code which controls and configures a single DMA channel.
 *
 * @details This module provides an interface for a single DMA channel. It allows the user to
 * configure and use the DMA channel with minimal hardware interfacing.
 *
 * If ping-pong mode is enabled the supplied buffer will be split in half and the lower half will
 * be used as the primary buffer (bufferA) and the upper half will be used as the secondary buffer
 * (bufferB). The buffer size must be an even number otherwise the block transfer size will be off
 * by one and the buffers will be mismatched.
 *
 * @{
 */

/**
 * @brief Stores the attributes and settings of a DMA channel.
 *
 * @details This struct stores the settings and configurations of a DMA channel. These settings are
 * what is used during initialization to configure the hardware DMA module. Once a DMA channel is
 * initialized these settings shouldn't be changed without reinitializing the channel.
 *
 * @public
 */
typedef struct dma_attr_s
{
    /**
     * @brief DMA channel configuration.
     *
     * @details These settings control how the DMA channel operates.
     */
    unsigned int config;

    /**
     * @brief The IRQ which triggers a DMA transfer.
     */
    dma_irq_t irq;

    /**
     * @brief The peripheral address to either read from or write to.
     */
    dma_peripheral_t peripheral_address;
} dma_attr_t;

/**
 * @brief A struct which represents a specific instance of a DMA channel.
 *
 * @details This structure holds the data which defines a particular DMA channel. There is a one-
 * to-one relationship between a #dma_channel_t data structure and a hardware DMA channel.
 *
 * The @em base_address should be set to the DMAxCON register address for the particular DMA
 * channel which will be described.
 *
 * @public
 */
typedef struct dma_channel_s
{
    /**
     * @brief The DMA channel number.
     *
     * @details This is the channel number from 0-7 of the DMA channel. This is used to help map
     * the correct special function registers and to return the correct data from global DMA
     * registers (e.g. DMACS0).
     *
     * This value should only be set once, during instantiation of the dma_channel_t object.
     */
    const unsigned int channel_number;

    /**
     * @brief A pointer to buffer A in DMA DPSRAM.
     *
     * @details This is a pointer to buffer A in DMA DPSRAM. This buffer is used when ping-pong
     * mode is disabled and as buffer A when ping-pong mode is disabled.
     *
     * This pointer may only be set once, during instantiation. It is assumed this value will
     * point to a statically allocated array in DMA DPSRAM. If the array is not in DPSRAM then DMA
     * transfers will not succeed.
     */
    volatile unsigned int * const buffer_a;

    /**
     * @brief The size of buffer A in DMA DPSRAM.
     *
     * @details This is the size of the array pointed to by @em buffer. This value may not extend
     * the array past the bounds of DPSRAM or else any DMA transfers to addresses outside the DMA
     * space will fail.
     *
     * This value should only be set once, during instantiation.
     */
    const unsigned int buffer_a_size;

    /**
     * @brief A pointer to buffer B in DMA DPSRAM.
     *
     * @details This is a pointer to buffer B in DMA DPSRAM which should be used for transfers.
     * This buffer should only be used if the module is configured to use Ping-Ping mode.
     *
     * This pointer may only be set once, during instantiation. It is assumed this value will
     * point to a statically allocated array in DMA DPSRAM. If the array is not in DPSRAM then DMA
     * transfers will not succeed. If buffer B is unused this value should be NULL.
     */
    volatile unsigned int * const buffer_b;

    /**
     * @brief The size of buffer B in DMA DPSRAM.
     *
     * @details This is the size of the array pointed to by @em buffer_b. This value may not extend
     * the array past the bounds of DPSRAM or else any DMA transfers to addresses outside the DMA
     * space will fail.
     *
     * This value should only be set once, during initialization, and should be the true size of
     * the statically allocated array in DMA DPSRAM. If buffer B is unused this value should be 0.
     */
    const unsigned int buffer_b_size;
    
    /**
     * @brief This holds private data specific to this channel.
     *
     * @details This pointer will point to a private structure which will be dynamically allocated
     * during initialization. For this reason, before either removal or reinitialization the
     * #dma_cleanup function should be called on the channel.
     */
    void *private;

} dma_channel_t;

/**
 * @brief Return codes for all DMA channel functions
 *
 * @details These return codes define the function outcomes for all DMA channel functions.
 *
 * @public
 */
typedef enum dma_error_e
{
    DMA_E_NONE    = 0,      /**< No error, successful return */
    DMA_E_CHANNEL = -1,     /**< Invalid DMA channel */
    DMA_E_ALLOC   = -2,     /**< Error during dynamic memory allocation */
    DMA_E_INPUT   = -3,     /**< Invalid input to function */
    DMA_E_OUTPUT  = -4,     /**< Invalid output pointer */
    DMA_E_ASSERT  = 0xEFFE, /**< Assertion failed */
    DMA_E_UNKNOWN = 0xEFFF  /**< Unknown error */
} dma_error_t;

/**
 * @brief DMA channel configuration settings.
 *
 * @details This enumeration defines the various settings which may be used to configure how a DMA
 * channel functions. These settings are intentionally close to the hardware so that a DMA channel
 * may be controlled in a more flexible manner instead of being locked into a particular usage.
 *
 * The different settings are broken up by their names. The names each take the form:<br />
 * <tt>DMA_x_y</tt><br />
 * Where <tt>x</tt> is the group name and <tt>y</tt> is the setting for that group.
 *
 * All groups also have a bitmask for convenience. These have the form:<br />
 * <tt>DMA_BITMASK_x</tt><br />
 * Where <tt>x</tt> is the group name.
 *
 * Group Descriptions:<br />
 * - <tt>OPMODE</tt>: Operating mode determines whether the channel will continuously transfer data
 *   or if it will stop after one block transfer.
 * - <tt>PINGPONG</tt>: Ping-Pong mode jumps between the A and B buffers after each block transfer.
 *   See the #DMAModule details for more information on how Ping-Pong mode is handled.
 * - <tt>ADDRMODE</tt>: Addressing mode determines how the next piece of data is selected. If
 *   register indirect addressing is used then the channel may either increment the address after
 *   a transfer (post increment) or use the same address for the next transfer (no post increment).
 *   If peripheral indirect addressing is selected the peripheral provides the low bits of the
 *   address for a transfer. This requires that the buffer be memory aligned, so see the hardware
 *   data sheet for more information.
 * - <tt>NULLWRITE</tt>: Null write mode causes a null data write to the peripheral upon reading
 *   from the peripheral. This is useful for peripherals such as SPI. See the hardware data sheet
 *   for more information.
 * - <tt>DIR</tt>: The direction setting determines if transfers read from or write to the
 *   peripheral.
 * - <tt>DATASIZE</tt>: The data size sets the size of each transfer. Word means a full word will
 *   be read or written on each transfer, byte means only a single byte will be read or written.
 *   A block transfer will always make the same number of transfers, but the amount of data sent
 *   will change depending on data size.
 *
 * @see dma_attr_s
 * @public
 */
typedef enum dma_config_e
{
    // Operating Mode settings
    DMA_CONFIG_OPMODE_BITMASK            = 0x0001, /**< Bitmask for Operating Mode settings */
    DMA_CONFIG_OPMODE_CONTINUOUS         = 0x0000, /**< Continuous operation @default */
    DMA_CONFIG_OPMODE_ONESHOT            = 0x0001, /**< One-shot operation */

    // Ping-Pong Mode settings
    DMA_CONFIG_PINGPONG_BITMASK          = 0x0002, /**< Bitmask for Ping-Pong Mode settings */
    DMA_CONFIG_PINGPONG_DIS              = 0x0000, /**< Ping-Pong mode disabled @default */
    DMA_CONFIG_PINGPONG_EN               = 0x0002, /**< Ping-Pong mode enabled */

    // Addressing Mode settings
    DMA_CONFIG_ADDRMODE_BITMASK          = 0x000C, /**< Bitmask for Addressing Mode settings */
    DMA_CONFIG_ADDRMODE_REGIND_POSTINC   = 0x0000, /**< Register indirect w/ post-increment addressing
                                             * mode @default */
    DMA_CONFIG_ADDRMODE_REGIND_NOPOSTINC = 0x0004, /**< Register indirect w/o post-increment addressing
                                             * mode */
    DMA_CONFIG_ADDRMODE_PERIPHERAL_IND   = 0x0008, /**< Peripheral indirect addressing mode */

    // Null Write settings
    DMA_CONFIG_NULLWRITE_BITMASK         = 0x0010, /**< Bitmask for Null Write settings */
    DMA_CONFIG_NULLWRITE_DIS             = 0x0000, /**< Disable null write @default */
    DMA_CONFIG_NULLWRITE_EN              = 0x0010, /**< Enable null write */

    // Transfer Direction settings
    DMA_CONFIG_DIR_BITMASK               = 0x0040, /**< Bitmask for Transfer Direction settings */
    DMA_CONFIG_DIR_FROM_PERIPHERAL       = 0x0000, /**< Transfer data from peripheral to DMA buffer @default */
    DMA_CONFIG_DIR_TO_PERIPHERAL         = 0x0040, /**< Transfer data to peripheral from DMA buffer */

    // Data Size settings
    DMA_CONFIG_DATASIZE_BITMASK          = 0x0080, /**< Bitmask for Data Size settings */
    DMA_CONFIG_DATASIZE_WORD             = 0x0000, /**< Transfer a word at a time @default */
    DMA_CONFIG_DATASIZE_BYTE             = 0x0080  /**< Transfer a byte at a time */
} dma_config_t;

/**
 * @brief Defines the IRQs which may start a DMA transfer.
 *
 * @details This enumeration defines the valid IRQ numbers which may start a DMA transfer. Only one
 * IRQ may be used per DMA channel.
 *
 * @see dma_attr_s
 * @public
 */
typedef enum dma_irq_e
{
    DMA_IRQ_INT0    = 0x0000, /**< INT0 will cause a transfer @default */
    DMA_IRQ_IC1     = 0x0001, /**< IC1 will cause a transfer */
    DMA_IRQ_OC1     = 0x0002, /**< OC1 will cause a transfer */
    DMA_IRQ_IC2     = 0x0005, /**< IC2 will cause a transfer */
    DMA_IRQ_OC2     = 0x0006, /**< OC2 will cause a transfer */
    DMA_IRQ_TMR2    = 0x0007, /**< TMR2 will cause a transfer */
    DMA_IRQ_TMR3    = 0x0008, /**< TMR3 will cause a transfer */
    DMA_IRQ_SPI1    = 0x000A, /**< SPI1 will cause a transfer */
    DMA_IRQ_UART1RX = 0x000B, /**< UART1RX will cause a transfer */
    DMA_IRQ_UART1TX = 0x000C, /**< UART1TX will cause a transfer */
    DMA_IRQ_ADC1    = 0x000D, /**< ADC1 will cause a transfer */
    DMA_IRQ_ADC2    = 0x0015, /**< ADC2 will cause a transfer */
    DMA_IRQ_UART2RX = 0x001E, /**< UART2RX will cause a transfer */
    DMA_IRQ_UART2TX = 0x001F, /**< UART2TX will cause a transfer */
    DMA_IRQ_SPI2    = 0x0021, /**< SPI2 will cause a transfer */
    DMA_IRQ_ECAN1RX = 0x0022, /**< ECAN1RX will cause a transfer */
    DMA_IRQ_ECAN2RX = 0x0037, /**< ECAN2RX will cause a transfer */
    DMA_IRQ_DCI     = 0x003C, /**< DCI will cause a transfer */
    DMA_IRQ_ECAN1TX = 0x0046, /**< ECAN1TX will cause a transfer */
    DMA_IRQ_ECAN2TX = 0x0047  /**< ECAN2TX will cause a transfer */
} dma_irq_t;

/**
 * @brief Defines the valid peripheral addresses for a DMA transfer.
 *
 * @details This enumeration defines the valid peripheral addresses which may be read from or
 * written to during a DMA transfer.
 *
 * The default value for the peripheral address is NULL (0x0000), so if this is not set (or set
 * to an invalid value) then DMA transfers will fail.
 *
 * @see dma_attr_s
 * @public
 */
typedef enum dma_peripheral_e
{
    DMA_PERIPHERAL_IC1BUF    = 0x0140, /**< IC1BUF peripheral address used for transfers */
    DMA_PERIPHERAL_IC2BUF    = 0x0144, /**< IC2BUF peripheral address used for transfers */
    DMA_PERIPHERAL_OC1R      = 0x0182, /**< OC1R peripheral address used for transfers */
    DMA_PERIPHERAL_OC1RS     = 0x0180, /**< OC1RS peripheral address used for transfers */
    DMA_PERIPHERAL_OC2R      = 0x0188, /**< OC2R peripheral address used for transfers */
    DMA_PERIPHERAL_OC2RS     = 0x0186, /**< OC2RS peripheral address used for transfers */
    DMA_PERIPHERAL_SPI1BUF   = 0x0248, /**< SPI1BUF peripheral address used for transfers */
    DMA_PERIPHERAL_SPI2BUF   = 0x0268, /**< SPI2BUF peripheral address used for transfers */
    DMA_PERIPHERAL_U1RXREG   = 0x0226, /**< U1RXREG peripheral address used for transfers */
    DMA_PERIPHERAL_U1TXREG   = 0x0224, /**< U1TXREG peripheral address used for transfers */
    DMA_PERIPHERAL_U2RXREG   = 0x0236, /**< U2RXREG peripheral address used for transfers */
    DMA_PERIPHERAL_U2TXREG   = 0x0234, /**< U2TXREG peripheral address used for transfers */
    DMA_PERIPHERAL_C1RXD     = 0x0440, /**< C1RXD peripheral address used for transfers */
    DMA_PERIPHERAL_C1TXD     = 0x0442, /**< C1TXD peripheral address used for transfers */
    DMA_PERIPHERAL_C2RXD     = 0x0540, /**< C2RXD peripheral address used for transfers */
    DMA_PERIPHERAL_C2TXD     = 0x0542, /**< C2TXD peripheral address used for transfers */
    DMA_PERIPHERAL_DCIRXBUF0 = 0x0290, /**< DCIRXBUF0 peripheral address used for transfers */
    DMA_PERIPHERAL_DCITXBUF0 = 0x0298, /**< DCITXBUF0 peripheral address used for transfers */
    DMA_PERIPHERAL_ADC1BUF0  = 0x0300, /**< ADC1BUF0 peripheral address used for transfers */
    DMA_PERIPHERAL_ADC2BUF0  = 0x0340  /**< ADC2BUF0 peripheral address used for transfers */
} dma_peripheral_t;

/**
 * @brief Options for when the DMA channel should interrupt.
 *
 * @details This enumeration provides the valid options for when a DMA channel will interrupt
 * during a transfer. It can be at half or full block transfers or the interrupt can be disabled.
 *
 * @see dma_interrupt_on
 * @public
 */
typedef enum dma_interrupt_on_e
{
    DMA_INTERRUPT_ON_FULL = 0x0000, /**< Interrupt on full block transfer. @default */
    DMA_INTERRUPT_ON_HALF = 0x0001, /**< Interrupt on half block transfer. */
} dma_interrupt_on_t;

/**
 * @brief The possible ping-pong buffer statuses.
 *
 * @details This enumeration provides the two possible ping-pong buffer states: buffer A selected,
 * or buffer B selected.
 *
 * @see dma_pingpong_status
 * @public
 */
typedef enum dma_pingpong_status_e
{
    DMA_PINGPONG_BUFFER_A = 0x0000, /**< Buffer A selected */
    DMA_PINGPONG_BUFFER_B = 0x0001 /**< Buffer B selected */
} dma_pingpong_status_t;


/**
 * @brief Initializes the given DMA channel using the specified attributes.
 *
 * @details The initialization function for a DMA channel will use the given attributes in the
 * #dma_attr_s struct to configure the DMA channel. The attribute data is stored in the private
 * data structure of the DMA channel and may not be modified once initialized.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to initialize.
 * @param[in]  attr
 *             A pointer to a dma_attr_s struct storing the attributes to use to configure the DMA
 *             channel.
 * @returns An integer value representing the outcome of the initialization. A negative number
 * means failure, a zero means success. The type of error is coded by the #dma_error_e enumeration.
 *
 * @see dma_attr_s
 * @see dma_error_e
 *
 * @public
 */
int dma_init(dma_channel_t *dma_channel,
             dma_attr_t *attr);

/**
 * @brief Enables the DMA channel for transfers.
 *
 * @details This function enables the DMA channel so that it will start accepting transfer
 * requests.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to enable.
 * @returns An integer value representing the outcome of the function. The values are coded by the
 * #dma_error_e enumeration.
 *
 * @public
 */
int dma_enable(dma_channel_t *dma_channel);

/**
 * @brief Disables the given DMA channel.
 *
 * @details Disables the given DMA channel.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to disable.
 * @returns An integer value representing the outcome of the function. The values are coded by the
 * #dma_error_e enumeration.
 *
 * @public
 */
int dma_disable(dma_channel_t *dma_channel);

/**
 * @brief Sets when a DMA interrupt should occur for the given channel.
 *
 * @details Sets whether a DMA interrupt should occur on half or full block transfers.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to work on.
 * @param[in]  int_on
 *             A value specifying when the DMA channel should interrupt.
 * @returns An integer value representing the outcome of the function. The values are coded by the
 * #dma_error_e enumeration.
 *
 * @public
 */
int dma_set_interrupt_on(dma_channel_t *dma_channel,
                         dma_interrupt_on_t int_on);
/**
 * @brief Gets the dma_interrupt_on_t value of the given DMA channel.
 *
 * @details Returns the dma_interrupt_on_t value which determines whether a DMA interrupt should
 * occur on half or full block transfers.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to work on.
 * @returns The dma_interrupt_on_t value of the given DMA channel or a dma_error_t error code.
 *
 * @public
 */
int dma_get_interrupt_on(dma_channel_t *dma_channel);

/**
 * @brief Get the ping-pong buffer status.
 *
 * @details Get which buffer is currently selected. This is only used in ping-pong mode, however,
 * if the function is called with ping-pong mode disabled it will always return buffer A.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to work on.
 * @returns A dma_pingpong_status_t value coding for the currently selected buffer (A or B) or a
 * dma_error_t error code.
 *
 * @see dma_pingpong_status_t
 * @public
 */
int dma_pingpong_status(dma_channel_t *dma_channel);

/**
 * @brief Forces a DMA transfer.
 *
 * @details This function forces the specified DMA channel to start a transfer.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel on which to force start a transfer.
 * @returns An integer value representing the outcome of the function. The values are coded by the
 * #dma_error_e enumeration.
 *
 * @public
 */
int dma_force(dma_channel_t *dma_channel);

/**
 * @brief Get the status of the force bit.
 *
 * @details Returns the status of the force bit in the given DMA channel.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to work on.
 * @returns The boolean value of the force bit of the given DMA channel.
 *
 * @public
 */
bool dma_is_force(dma_channel_t *dma_channel);

/**
 * @brief Sets the block transfer size for the specified DMA channel.
 *
 * @details This function specifies the block transfer size for the specified DMA channel. By
 * default the block size is the buffer size, however, this may be changed at any time without
 * reinitializing the channel. Block size changes should be made with care, however, as they can
 * cause buffer overruns if done while a transfer is on-going. The safest method is to disable the
 * channel, change the block size, then re-enable the channel and start a new transfer.
 *
 * The maximum block size is 1024 bytes or 512 words (depends on the data size), however, the block
 * size may not be larger than the current buffer size.
 *
 * @note The block size is the number of data transfers per block, which equates to @em count+1.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel of which to change the block size.
 * @param[in]  block_size
 *             The size in bytes or words of a block transfer.
 * @returns An integer value representing the outcome of the function. Zero for success, a negative
 * code for failure (see #dma_error_e).
 *
 * @public
 */
int dma_set_block_size(dma_channel_t *dma_channel,
                       unsigned int block_size);

/**
 * @brief Returns the current block size for the specified DMA channel.
 *
 * @details This function returns the current block size for the specified DMA channel. The block
 * size may be anywhere from 0-1024 in size and is equivalent to @em count+1.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel from which to read the block size.
 * @returns An integer value which, if positive, is the block size and if negative, is an error
 * code from #dma_error_e.
 *
 * @public
 */
int dma_get_block_size(dma_channel_t *dma_channel);

/**
 * @brief Outputs the attribute structure stored within a DMA channel.
 *
 * @details This function copies the #dma_attr_s struct stored in the private structure of the DMA
 * channel to the struct pointed to by @em attr.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel from which to get the attributes.
 * @param[out] attr
 *             A pointer to a #dma_attr_s struct to which to copy the internal attribute struct.
 * @returns An integer value which is a return code from #dma_error_e.
 *
 * @public
 */
int dma_get_attr(dma_channel_t *dma_channel,
                 dma_attr_t *attr);

/**
 * @brief Cleans up a DMA channel for deletion or reinitialization.
 *
 * @details This function frees any private, allocated memory, disables the DMA channel, and
 * resets it to default settings. This function should be called before reinitializing any DMA
 * channel.
 *
 * @param[in]  dma_channel
 *             A pointer to the channel to clean up.
 * @returns An integer value return code from #dma_error_e. If zero, the channel was cleaned up
 * successfully and may be deleted or reinitialized. If negative an error occurred.
 *
 * @public
 */
int dma_cleanup(dma_channel_t *dma_channel);


/**
 * @brief Returns whether the DMA channel is valid.
 *
 * @details A simple inline function which will determine if the given DMA channel is valid.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel to test for validity.
 * @returns A boolean value which is true if the channel is valid, false otherwise.
 *
 * @public
 */
inline bool dma_is_valid(dma_channel_t *dma_channel);


/**
 * @}
 */ // End of DMAChannel group

// End of include guard
#endif //_DMA_CHANNEL_XC16_H

