/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file dma_channel.h
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
#ifndef _DMA_CHANNEL_H
#define _DMA_CHANNEL_H

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
struct dma_attr_s
{
    /**
     * @brief DMA channel control settings.
     *
     * @details These settings control how the DMA channel operates.
     */
    unsigned int control_settings;

    /**
     * @brief The IRQ which triggers a DMA transfer.
     */
    unsigned int irq;

    /**
     * @brief The peripheral address to either read from or write to.
     */
    volatile unsigned int peripheral_address;
};

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
     * @brief The base hardware address of the DMA channel.
     *
     * @details This is the address from which all SFRs are referenced. It should be set to the
     * address of DMAxCON for the particular DMA channel which is to be described.
     *
     * This value should only be set once, during instantiation.
     */
    volatile unsigned int * const base_address;

    /**
     * @brief A pointer to a buffer in DMA DPSRAM.
     *
     * @details This is a pointer to the buffer in DMA DPSRAM which should be used for transfers.
     * Any transfers to or from a peripheral will use this buffer.
     *
     * This value should only be set once, during instantiation. It is assumed this value will
     * point to a statically allocated array in DMA DPSRAM. If the array is not in DPSRAM then DMA
     * transfers will not succeed.
     */
    volatile unsigned int * const buffer;

    /**
     * @brief The size of the buffer in DMA DPSRAM.
     *
     * @details This is the size of the array pointed to by @em buffer. This value may not extend
     * the array past the bounds of DPSRAM or else any DMA transfers to addresses outside the DMA
     * space will fail.
     *
     * This value should only be set once, during instantiation.
     */
    const unsigned int buffer_size;

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
enum dma_error_e
{
    DMA_E_NONE    = 0,     /**< No error, successful return */
    DMA_E_CHANNEL = -1,    /**< Invalid DMA channel */
    DMA_E_ALLOC   = -2,    /**< Error during dynamic memory allocation */
    DMA_E_INPUT   = -3,    /**< Invalid input to function */
    DMA_E_OUTPUT  = -4,    /**< Invalid output pointer */
    DMA_E_UNKNOWN = 0xFFFF /**< Unknown error */
};

/**
 * @brief DMA channel control settings.
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
 * - <tt>PINGPONG</tt>: Ping-Pong mode splits the DMA buffer in half and jumps between the two new
 *   buffers after each block transfer. See the #DMAModule details for more information on how
 *   Ping-Pong mode is handled.
 * - <tt>ADDRMODE</tt>: Addressing mode determines how the next piece of data is selected. If
 *   register indirect addressing is used then the channel may either increment the address after
 *   a transfer (post increment) or use the same address for the next transfer (no post increment).
 *   If peripheral indirect addressing is selected the peripheral provides the low bits of the
 *   address for a transfer. This requires that the buffer be memory aligned, so see the hardware
 *   data sheet for more information.
 * - <tt>NULLWRITE</tt>: Null write mode causes a null data write to the peripheral upon reading
 *   from the peripheral. This is useful for peripherals such as SPI. See the hardware data sheet
 *   for more information.
 * - <tt>INT</tt>: Block transfer interrupts can be set for either half data transfer or all data
 *   transfer. If half data transfer is selected the DMA interrupt will occur after half a block is
 *   transfered (and not after a full block). If all data transfer is selected the DMA interrupt
 *   will occur after a whole block is transfered.
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
enum dma_control_settings_e
{
    // Operating Mode settings
    DMA_BITMASK_OPMODE          = 0x0001, /**< Bitmask for Operating Mode settings */
    DMA_OPMODE_CONTINUOUS       = 0x0000, /**< Continuous operation @default */
    DMA_OPMODE_ONESHOT          = 0x0001, /**< One-shot operation */

    // Ping-Pong Mode settings
    DMA_BITMASK_PINGPONG        = 0x0002, /**< Bitmask for Ping-Pong Mode settings */
    DMA_PINGPONG_DIS            = 0x0000, /**< Ping-Pong mode disabled @default */
    DMA_PINGPONG_EN             = 0x0002, /**< Ping-Pong mode enabled */

    // Addressing Mode settings
    DMA_BITMASK_ADDRMODE        = 0x000C, /**< Bitmask for Addressing Mode settings */
    DMA_ADDRMODE_REGISTER_IND   = 0x0000, /**< Register indirect addressing mode @default */
    DMA_ADDRMODE_POSTINC_EN     = 0x0000, /**< Enable Post-increment mode (Register indirect
                                           * addressing mode only) @default */
    DMA_ADDRMODE_POSTINC_DIS    = 0x0004, /**< Disable Post-increment mode (Register indirect
                                           * addressing mode only) */
    DMA_ADDRMODE_PERIPHERAL_IND = 0x0008, /**< Peripheral indirect addressing mode (will ignore any
                                           * Post-increment settings) */

    // Null Write settings
    DMA_BITMASK_NULLWRITE       = 0x0010, /**< Bitmask for Null Write settings */
    DMA_NULLWRITE_DIS           = 0x0000, /**< Disable null write @default */
    DMA_NULLWRITE_EN            = 0x0010, /**< Enable null write */

    // Block Transfer Interrupt settings
    DMA_BITMASK_INT             = 0x0020, /**< Bitmask for Block Transfer Interrupt settings */
    DMA_INT_ALL                 = 0x0000, /**< Interrupt when all data has been moved @default */
    DMA_INT_HALF                = 0x0020, /**< Interrupt when half data has been moved */

    // Transfer Direction settings
    DMA_BITMASK_DIR             = 0x0040, /**< Bitmask for Transfer Direction settings */
    DMA_DIR_FROMPERIPHERAL      = 0x0000, /**< Transfer data from peripheral to DMA buffer @default */
    DMA_DIR_TOPERIPHERAL        = 0x0040, /**< Transfer data to peripheral from DMA buffer */

    // Data Size settings
    DMA_BITMASK_DATASIZE        = 0x0080, /**< Bitmask for Data Size settings */
    DMA_DATASIZE_WORD           = 0x0000, /**< Transfer a word at a time @default */
    DMA_DATASIZE_BYTE           = 0x0080  /**< Transfer a byte at a time */
};

/**
 * @brief Defines the IRQs which may start a DMA transfer.
 *
 * @details This enumeration defines the valid IRQ numbers which may start a DMA transfer. Only one
 * IRQ may be used per DMA channel.
 *
 * @see dma_attr_s
 * @public
 */
enum dma_irq_e
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
};

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
enum dma_peripheral_address_e
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
};



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
             struct dma_attr_s *attr);

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
 * @brief Outputs a pointer to the first word/byte of the primary DMA channel buffer.
 *
 * @details This function outputs a pointer to the first word (or byte) of the primary buffer for
 * the specified DMA channel. This value will always point to the same address used during
 * initialization, even if ping-pong mode is enabled. This is because the primary buffer will
 * always be start with the first word (or byte) of the initialization buffer.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel for which to get the buffer pointer.
 * @param[out] buffer
 *             A pointer to a pointer which stores the address of the primary buffer for the
 *             specified DMA channel.
 * @returns An integer value which, if negative, codes for a #dma_error_e enumeration value. If the
 * value is positive it represents the size of the primary DMA buffer.
 *
 * @public
 */
int dma_get_bufferA(dma_channel_t *dma_channel,
                    volatile unsigned int **buffer);

/**
 * @brief Returns the size of the primary buffer in the specified DMA channel.
 *
 * @details This function will return the size of the primary buffer in the specified DMA channel.
 * If ping-pong mode is enabled then this size will be different than the @em buffer_size used to
 * initialize the DMA channel. To be specific it will be half the size.
 *
 * This is the same value returned by #dma_get_bufferA, so if both a pointer and size are needed
 * that function is a more efficient way to go about it (particularly if ping-pong mode is
 * enabled).
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel from which to get the buffer size.
 * @returns An integer value which, if positive or zero, is the size of the current DMA buffer. If
 * the return value is negative the function call failed and the value represents a #dma_error_e
 * return code.
 *
 * @public
 */
int dma_get_bufferA_size(dma_channel_t *dma_channel);

/**
 * @brief Outputs a pointer to the first word/byte of the secondary DMA channel buffer.
 *
 * @details This function outputs a pointer to the first word (or byte) of the secondary buffer for
 * the specified DMA channel if ping-pong mode is enabled. However, if ping-pong mode is not
 * enabled then a NULL pointer is output and a zero size is returned.
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel for which to get the buffer pointer.
 * @param[out] buffer
 *             A pointer to a pointer which stores the address of the secondary buffer for the
 *             specified DMA channel.
 * @returns An integer value which, if negative, codes for a #dma_error_e enumeration value. If the
 * value is positive or zero it represents the size of the secondary DMA buffer.
 *
 * @public
 */
int dma_get_bufferB(dma_channel_t *dma_channel, volatile unsigned int **buffer);

/**
 * @brief Returns the size of the secondary buffer in the specified DMA channel.
 *
 * @details This function will return the size of the secondary buffer in the specified DMA
 * channel. If ping-pong mode is enabled then this size will be different than the @em buffer_size
 * used to initialize the DMA channel. To be specific it will be half the size.
 *
 * This is the same value returned by #dma_get_bufferB, so if both a pointer and size are needed
 * that function is a more efficient way to go about it (particularly if ping-pong mode is
 * enabled).
 *
 * @param[in]  dma_channel
 *             A pointer to the DMA channel from which to get the buffer size.
 * @returns An integer value which, if positive, is the size of the secondary DMA buffer. If the
 * return value is negative the function call failed and the value represents a #dma_error_e
 * return code.
 *
 * @public
 */
int dma_get_bufferB_size(dma_channel_t *dma_channel);

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
int dma_get_attr(dma_channel_t * dma_channel,
                 struct dma_attr_s *attr);

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
inline bool dma_isvalid(dma_channel_t *dma_channel)
{
    if( dma_channel == NULL || dma_channel->base_address == NULL || dma_channel->buffer == NULL || dma_channel->buffer_size == 0 )
    {// Invalid
        return false;
    }
    else
    {// Assume valid
        return true;
    }
}

/**
 * @}
 */ // End of DMAChannel group

// End of include guard
#endif //_DMA_CHANNEL_H

