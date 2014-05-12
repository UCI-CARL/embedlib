/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file uart_module.h
 *
 * @brief This header defines and prototypes the public functions and data structures of a UART
 * module for the dsPIC33FJ series of microcontrollers.
 *
 * @author     Liam Bucci
 * @date       6/6/2012
 * @carlnumber FIRM-0004
 */

// Include guard
#ifndef _UART_MODULE_H
#define _UART_MODULE_H

/**
 * @defgroup UARTModule UART Module
 *
 * @author  Liam Bucci
 * @date    1/4/2012
 * @version 0.1.0
 *
 * @brief Documents all the features of the UART module for the dsPIC33F family of
 * microcontrollers.
 *
 * @details The typical usage of this module is to use the provided enumerations to change the
 * settings stored within the attribute struct. Set the DMA channel pointers to valid,
 * uninitialized channels. Then initialize the module using the uart_init function.
 *
 * Once the module is initialized it can be enabled and bytes may be sent or received. If no DMA
 * channels have been allocated to it, the module will attempt to send and receive bytes using the
 * in-built hardware buffers, however, it is preferable to assign DMA channels to the module.
 *
 * Each module also has two optional software buffers (one for TX, one for RX). The RX buffer will
 * act as a backup buffer to any supplied DMA buffer or as a primary in the case of no DMA channel.
 * The TX buffer will be used to accumulate bytes before sending a block of characters through DMA.
 * If no DMA channel is assigned then TX blocks will be the size of the hardware buffers (4 bytes).
 * The software buffers are dynamically allocated and will try to match any assigned DMA channels
 * or the values in the tx_rx_settings variable of the attr struct.
 *
 * If an initialized module is no longer needed make sure to clean it up using #uart_cleanup. When
 * a module is initialized it dynamically allocates memory, and that memory is freed during the
 * clean up funcion.
 *
 * @note Any assigned DMA channels must already contain valid pointers to the DMA RAM buffer, but
 * must not be initialized. The uart_init function will initialize and set up the DMA channel.
 *
 * @{
 */

// Include DMA channel functionality
#include "../include/dma_channel.h"

//! @todo Move this to a .def file.
#define UART_TX_BUFFER_SIZE 16
#define UART_RX_BUFFER_SIZE 32

/* Attribute Declaration */
/**
 * @brief The attribute struct for a UART module.
 *
 * @details This struct holds the attributes (or settings) for a particular instance of a UART
 * module. The settings stored within will be used during module initialization.
 *
 * The settings stored in this struct are meant to be static over the life of the module. In
 * other words, the module should be reinitialized if any of these settings are changed.
 *
 * @public
 */
struct uart_attr_s
{
    /**
     * @brief General module function settings.
     *
     * @details The control_settings variable stores settings relating to basic functionality of
     * the module, such as idle states, flow control, etc.
     *
     * @see uart_control_settings_e
     */
    int control_settings;

    /**
     * @brief Transmit and receive specific settings.
     *
     * @details The tx_rx_settings variable stores settings relating to the transmission and
     * reception of data. Settings such as character format (i.e. number of data, parity, and stop
     * bits), buffer sizes, etc.
     *
     * @see uart_tx_rx_settings_e
     */
    int tx_rx_settings;

    /**
     * @brief The baud rate used by the module for transmission and reception of characters.
     *
     * @details The baud_rate variable stores a coded value representing the baud rate of the
     * module.
     *
     * @see uart_baud_rate_e
     */
    int baud_rate;
};


/* Module Declaration */
/**
 * @brief The UART module data structure definition.
 *
 * @details This structure is a representation of an actual hardware UART module, it contains all
 * the settings and data which are used to control and use that particular UART module. It also
 * contains some soft settings which are used to configure how the uart_module code interacts with
 * the hardware UART.
 *
 * @see uart_attr_s
 * @see uart_private_s
 * @public
 */
typedef struct uart_module_s
{
    /**
     * The base address of the hardware UART module which is controlled by this uart_module. This
     * should be set to the UxMODE special function register address of the specific UART module.
     */
    volatile unsigned int * const base_address;

    /**
     * This struct holds the attributes (i.e. settings) for this module.
     */
    struct uart_attr_s attr;

    /**
     * This pointer is the assigned DMA channel for transmission. If it is valid (i.e. not
     * NULL) it will be used. Otherwise only the software and hardware buffers will be used.
     */
    dma_channel_t *tx_dma;
    /**
     * This pointer is the assigned DMA channel for reception. If it is valid (i.e. not
     * NULL) it will be used. Otherwise only the software and hardware buffers will be used.
     */
    dma_channel_t *rx_dma;

    /**
     * These function pointers are used as callback functions for when characters are available for
     * reading (RX) or room becomes available in the transmit software buffer (TX). See
     * #uart_tx_rx_settings_e for more information on specifying when the callback functions are
     * invoked.
     *
     * @note These functions may be invoked during an ISR, so ensure that they are short and are
     * appropriate for an interrupt service routine. Ideally they should just schedule another
     * function to do any more reads or writes to the module.
     */
    void (*tx_callback)(uart_module_t *module); /**< The callback function invoked when space in
                                                 * the TX buffer is available. */
    void (*rx_callback)(uart_module_t *module); /**< The callback function invoked when characters
                                                 * are available in the RX buffer */

    /**
     * This pointer holds private data for the module. This pointer should not be modified by any
     * code outside of the module. See the #uart_private_s struct for more information.
     */
    void *private;

} uart_module_t;


/* Public Enumerations Definitions */

/**
 * @brief Defines the valid control settings for a UART module.
 *
 * @details The values of this enumeration are designed to be bitwise OR'd together to form a valid
 * @em control_settings value for the #uart_attr_s struct. The @em control_settings variable
 * will be used during module initialization.
 *
 * The different settings are broken up by their names. The names each take the form: <br />
 * <tt>UART_x_y</tt><br />
 * Where <tt>x</tt> is the group name and <tt>y</tt> is the setting for that group.
 *
 * All of the groups also have a bitmask for convenience. These have the form:<br />
 * <tt>UART_BITMASK_x</tt><br />
 *
 * Where <tt>x</tt> is the group name.
 *
 * Group Descriptions:<br />
 * - <tt>LOOPBACK</tt>: Debug feature connects TX and RX internally.
 * - <tt>IRDA</tt>: IrDA can be configured to use the internal encoder/decoder or an external one.
 * - <tt>CPUIDLE</tt>: Determines if module continues operation during CPU idle mode.
 * - <tt>WAKEUP</tt>: Determines if module will wake on a start bit when CPU is in sleep mode.
 * - <tt>FLOWCNTL</tt>: Specifies which pins should be used for hardware flow control.
 * - <tt>RTSMODE</tt>: If the RTS pin is enable for flow control this setting specifies what type
 *   of flow control it uses.
 * - <tt>RXIDLE</tt>: Specifies the idle state of the RX pin.
 * - <tt>TXIDLE</tt>: Specifies the idle state of the TX pin.
 * - <tt>ADDRDETECT</tt>: Specifies whether the module should detect address characters. An address
 *   character is defined as bit 8 = 1, so if 9 bit data reception is not enabled this setting does
 *   nothing.
 *
 * @see uart_attr_s
 * @public
 */
enum uart_control_settings_e
{
    // Loopback settings
    UART_BITMASK_LOOPBACK   = 0x0001, //!< Bitmask for Loopback settings
    UART_LOOPBACK_DIS       = 0x0000, //!< Disable loopback @default
    UART_LOOPBACK_EN        = 0x0001, //!< Enable loopback

    // IrDA settings
    UART_BITMASK_IRDA       = 0x0006, //!< Bitmask for IrDA settings
    UART_IRDA_DIS           = 0x0000, //!< Disable IrDA encoding/decoding @default
    UART_IRDA_INT           = 0x0002, //!< Enable internal IrDA encoding/decoding
    UART_IRDA_EXT           = 0x0004, //!< Enable external IrDA encoding/decoding

    // CPU Idle settings
    UART_BITMASK_CPUIDLE    = 0x0008, //!< Bitmask for CPU idle settings
    UART_CPUIDLE_CONTINUE   = 0x0000, //!< Continue module operation during CPU idle @default
    UART_CPUIDLE_STOP       = 0x0008, //!< Stop module operation during CPU idle

    // Wake-up setting
    UART_BITMASK_WAKEUP     = 0x0010, //!< Bitmask for Wake-up settings
    UART_WAKEUP_DIS         = 0x0000, //!< Disable wake-up on start bit during CPU sleep mode @default
    UART_WAKEUP_EN          = 0x0010, //!< Enable wake-up on start bit during CPU sleep mode

    // Flow Control settings
    UART_BITMASK_FLOWCNTL   = 0x0060, //!< Bitmask for Flow Control settings
    UART_FLOWCNTL_NONE      = 0x0000, //!< No flow control @default
    UART_FLOWCNTL_RTS       = 0x0020, //!< RTS pin only is enabled and used by hardware.
    UART_FLOWCNTL_RTSCTS    = 0x0040, //!< RTS and CTS pins are enabled and used by hardware.

    // RTS Mode settings
    UART_BITMASK_RTSMODE    = 0x0080, //!< Bitmask for RTS Mode settings
    UART_RTSMODE_FLOW       = 0x0000, //!< RTS is in flow control mode @default
    UART_RTSMODE_SIMPLEX    = 0x0080, //!< RTS is in simplex mode

    // Receive idle state setting (i.e. polarity inversion)
    UART_BITMASK_RXIDLE     = 0x0100, /**< Bitmask for Receive idle state settings */
    UART_RXIDLE_HIGH        = 0x0000, /**< Receive pin idle state is high @default */
    UART_RXIDLE_LOW         = 0x0100, /**< Receive pin idle state is low */

    // Transmit idle state setting (i.e. polarity inversion)
    UART_BITMASK_TXIDLE     = 0x0200, /**< Bitmask for Transmit idle state settings */
    UART_TXIDLE_HIGH        = 0x0000, /**< Transmit pin idle state is high @default */
    UART_TXIDLE_LOW         = 0x0200, /**< Transmit pin idle state is low */

    // Address Character Detect settings
    UART_BITMASK_ADDRDETECT = 0x0400, /**< Bitmask for Address character detect settings */
    UART_ADDRDETECT_DIS     = 0x0000, /**< Disable address character detection @default */
    UART_ADDRDETECT_EN      = 0x0400  /**< Enable address character detection */

    // ---- Any DMA settings should go here ----
};


/**
 * @brief Defines valid TX/RX settings for a uart module.
 *
 * @details The values of this enumeration are designed to be bitwise OR'd together to form a valid
 * @em tx_rx_settings value for the #uart_attr_s struct. The @em tx_rx_settings variable will be
 * used during module initialization.
 *
 * The different settings are broken up by their names. The names each take the form: <br />
 * <tt>UART_x_y</tt><br />
 * Where <tt>x</tt> is the group name and <tt>y</tt> is the setting for that group.
 *
 * All of the groups also have a bitmask for convenience. These have the form:<br />
 * <tt>UART_BITMASK_x</tt><br />
 *
 * Where <tt>x</tt> is the group name.
 *
 * Group Descriptions:<br />
 * - <tt>DATAFMT</tt>: Specifies the number of data and stop bits and the type, if any, of parity.
 * - <tt>RXINT</tt>: Specifies when an interrupt should occur for reception of a character.
 * - <tt>TXINT</tt>: Specifies when an interrupt should occur for transmission of a character.
 * - <tt>RXBUF</tt>: Specifies the RX software buffer size. If set to DMA then the module will try
 *   to match the size of the assigned DMA buffer. If it cannot match the DMA buffer size or no DMA
 *   channel is assigned it will fall back to a 4 character buffer.
 * - <tt>TXBUF</tt>: Specifies the TX software buffer size. If set to DMA then the module will try
 *   to match the size of the assigned DMA buffer. If it cannot match the DMA buffer size or no DMA
 *   channel is assigned it will fall back to a 4 character buffer.
 *
 * @see uart_attr_s
 * @public
 */
enum uart_tx_rx_settings_e
{
    // Data Format settings (i.e. data, parity, and stop bits)
    UART_BITMASK_DATAFMT    = 0x0007, /**< Bitmask for data format settings */
    UART_DATAFMT_8N1        = 0x0000, /**< 8 Data bits, 1 Stop bit, No parity @default */
    UART_DATAFMT_9N1        = 0x0001, /**< 9 Data bits, 1 Stop bit, No parity */
    UART_DATAFMT_8E1        = 0x0002, /**< 8 Data bits, 1 Stop bit, Even parity */
    UART_DATAFMT_8O1        = 0x0003, /**< 8 Data bits, 1 Stop bit, Odd parity */
    UART_DATAFMT_8N2        = 0x0004, /**< 8 Data bits, 2 Stop bits, No parity */
    UART_DATAFMT_9N2        = 0x0005, /**< 9 Data bits, 2 Stop bits, No parity */
    UART_DATAFMT_8E2        = 0x0006, /**< 8 Data bits, 2 Stop bits, Even parity */
    UART_DATAFMT_8O2        = 0x0007, /**< 8 Data bits, 2 Stop bits, Odd parity */

    // RX interrupt settings
    UART_BITMASK_RXINT      = 0x0060, /**< Bitmask for RX interrupt settings */
    UART_RXINT_CHAR         = 0x0000, /**< Interrupt on every received character @default */
    UART_RXINT_3            = 0x0020, /**< Interrupt when RX buffer is 3/4 full (i.e. 3 characters) */
    UART_RXINT_FULL         = 0x0040, /**< Interrupt when RX buffer is full */

    // TX interrupt settings
    UART_BITMASK_TXINT      = 0x0180, /**< Bitmask for TX interrupt settings */
    UART_TXINT_CHAR         = 0x0000, /**< Interrupt whenever a character is sent to the shift
                                       * register @default */
    UART_TXINT_BUFFER       = 0x0080, /**< Interrupt when last character is sent to shift register
                                       * and buffer is empty */
    UART_TXINT_COMPLETE     = 0x0100, /**< Interrupt when all characters have been transmitted and
                                       * buffer is empty */

    // RX Software Buffer Size settings
    UART_BITMASK_RXBUF      = 0x0E00, /**< Bitmask for RX Software Buffer Size settings */
    UART_RXBUF_NONE         = 0x0000, /**< Don't use a software buffer @default */
    UART_RXBUF_4            = 0x0200, /**< Use a 4 character RX software buffer */
    UART_RXBUF_8            = 0x0400, /**< Use an 8 character RX software buffer */
    UART_RXBUF_12           = 0x0600, /**< Use a 12 character RX software buffer */
    UART_RXBUF_16           = 0x0800, /**< Use a 16 character RX software buffer */
    UART_RXBUF_24           = 0x0A00, /**< Use a 24 character RX software buffer */
    UART_RXBUF_32           = 0x0C00, /**< Use a 32 character RX software buffer */
    UART_RXBUF_DMA          = 0x0E00, /**< Try to match the RX software buffer to the size of the
                                       * DMA buffer */
    
    // TX Software Buffer Size settings
    UART_BITMASK_TXBUF      = 0x7000, /**< Bitmask for TX Software Buffer Size settings */
    UART_TXBUF_NONE         = 0x0000, /**< Don't use a software buffer @default */
    UART_TXBUF_4            = 0x1000, /**< Use a 4 character TX software buffer */
    UART_TXBUF_8            = 0x2000, /**< Use an 8 character TX software buffer */
    UART_TXBUF_12           = 0x3000, /**< Use a 12 character TX software buffer */
    UART_TXBUF_16           = 0x4000, /**< Use a 16 character TX software buffer */
    UART_TXBUF_24           = 0x5000, /**< Use a 24 character TX software buffer */
    UART_TXBUF_32           = 0x6000, /**< Use a 32 character TX software buffer */
    UART_TXBUF_DMA          = 0x7000  /**< Try to match the TX software buffer to the size of the
                                       * DMA buffer */

};

/**
 * @brief Defines the valid baud rates for a UART module.
 *
 * @details A single enumeration value should be stored in the @em baud_rate variable of the
 * #uart_attr_s struct before initialization to set the baud rate of the module. This baud rate
 * determines the rate at which bits are sent and received in asynchronous communications.
 *
 * @see uart_attr_s
 * @public
 */
enum uart_baud_rate_e
{
    UART_BAUDRATE_9600   = 0x0000, /**< Module baud rate is 9600bps @default */
    UART_BAUDRATE_1200   = 0x0001, /**< Module baud rate is 1200bps */
    UART_BAUDRATE_2400   = 0x0002, /**< Module baud rate is 2400bps */
    UART_BAUDRATE_4800   = 0x0003, /**< Module baud rate is 4800bps */
    UART_BAUDRATE_19200  = 0x0004, /**< Module baud rate is 19200bps */
    UART_BAUDRATE_38400  = 0x0005, /**< Module baud rate is 38400bps */
    UART_BAUDRATE_57600  = 0x0006, /**< Module baud rate is 57600bps */
    UART_BAUDRATE_115200 = 0x0007, /**< Module baud rate is 115200bps */
    UART_BAUDRATE_230400 = 0x0008  /**< Module baud rate is 230400bps */
};

/**
 * @brief Defines the possible return values and error codes for all UART module functions.
 *
 * @details The values defined in this enumeration describe the various return states and error
 * cases for all the functions in the UART module.
 *
 * @public
 */
enum uart_error_e
{
    UART_E_NONE     = 0,  /**< Success, no errors */
    UART_E_MODULE   = -1, /**< The module was invalid */
    UART_E_SETTINGS = -2, /**< Conflicting or invalid settings */ 
    UART_E_INPUT    = -3, /**< The input pointer or data was invalid */
    UART_E_OUTPUT   = -4, /**< The output pointer was invalid */
    UART_E_DMA      = -5, /**< A problem occurred with DMA */
    UART_E_SOFTBUF  = -6, /**< A problem occurred with the software buffer */
    UART_E_ALLOC    = -7  /**< A problem occurred during dynamic memory allocation */
};

/**
 * @brief Defines the valid directions of communication.
 *
 * @details The values defined in this enumeration are used to describe the different possible
 * directions of communication. This is used in functions such as #uart_open, #uart_close, and
 * #uart_flush.
 *
 * @see uart_open
 * @see uart_close
 * @see uart_flush
 * @public
 */
enum uart_direction_e
{
    UART_NONE = 0x0000, /**< No direction, also means module is 'closed' */
    UART_TX   = 0x0001, /**< Transmit */
    UART_RX   = 0x0002, /**< Receive */
    UART_TXRX = 0x0003  /**< Transmit and Receive */
};


/* Public Functions */

/**
 * @brief Initializes the hardware and DMA channels and allocates memory for buffers.
 *
 * @details This function uses the #uart_attr_s struct contained in the provided module to
 * initialize the underlying hardware, initialize the assigned DMA channels, and allocate memory
 * for the buffers if needed.
 *
 * Once this function is called on a module it is ready for opening with the #uart_open function.
 *
 * @param[in]  module
 *             The module to initialize.
 *
 * @returns A #uart_error_e value.
 *
 * @see uart_attr_s
 * @public
 */
int uart_init(uart_module_t *module);

/**
 * @brief Opens the module for reading, writing, or both.
 *
 * @details Once a module is initialized it may be opened for reading and/or writing. If it is
 * opened for writing (TX) then it will allow calls to the #uart_write function and characters will
 * start being transmitted. If the module is opened for reading (RX) then calls to #uart_read will
 * be allowed and received characters will be accepted.
 *
 * @param[in]  module
 *             The module to open.
 * @param[in]  direction
 *             The direction to open the module, i.e. TX, RX, or TXRX.
 *
 * @see uart_write
 * @see uart_read
 * @public
 */
int uart_open(uart_module_t *module,
              uart_direction_e direction);

/**
 * @brief Send the given characters out of the specified UART module.
 *
 * @details This function operates differently depending on how the buffers and DMA channels were
 * initialized. In general, however, it operates on blocks of characters. So the software buffer,
 * if it is used, will not flush to the DMA buffer until it is either full or has enough characters
 * to fill the DMA buffer. Similarly, the DMA buffer will not be flushed to the hardware (i.e. a
 * transmission) until the DMA buffer is full. At any time the user may call the #uart_flush
 * function to forcibly flush all characters from both buffers and force a transmission if
 * possible.
 *
 * Below is a more detailed description of how the function operates depending on the configuration
 * of the DMA buffer and the software buffer:
 *
 * <b>TX DMA Enabled and Software Buffer Enabled</b><br />
 * The function will try to write as many characters as possible to the software buffer until
 * either of two conditions is met:
 * - The software buffer is full.
 * - The software buffer has enough characters to fill the DMA buffer.
 * At that point the software buffer will be flushed into the DMA buffer if possible and writing to
 * the software buffer will continue. If there is a DMA transfer in progress then the function will
 * try to keep writing to the software buffer until it is full. If the software buffer is full and
 * there is a DMA transfer in progress the function will exit and return the number of characters
 * successfully written.
 *
 * <b>TX DMA Enabled and Software Buffer Disabled</b><br />
 * If there is no DMA transfer in progress the function will try to write to the DMA buffer until
 * it is full or all characters have been written. If the DMA buffer fills then a DMA transfer will
 * begin and the function will return the number of characters written. Otherwise, the transfer
 * will wait until a full DMA buffer is available or the #uart_flush function is called.
 *
 * <b> TX DMA Disabled and Software Buffer Enabled</b><br />
 * The function will try to write to the software buffer until it is either full or has enough
 * characters to fill the hardware FIFO buffer (4 characters). If there is no transmission in
 * progress then the software buffer will be flushed to the hardware FIFO buffer and a transmission
 * will begin. Otherwise the function will continue to write to the software buffer until it is
 * full. If a transmission is in progress and the software buffer is full then the function will
 * return the number of characters written.
 *
 * <b> TX DMA Disabled and Software Buffer Disabled</b><br />
 * The function will attempt to write to the hardware FIFO buffer directly. If there are no more
 * characters available in the buffer then the function will return the number of characters
 * written.
 *
 * @note Accesses to the software buffer are atomic.
 *
 * @param[in]  module
 *             The module to which to write.
 * @param[in]  buf
 *             A pointer to the array of characters to write to the module.
 * @param[in]  length
 *             The length of the array of characters to write to the module.
 *
 * @returns The number of characters successfully written to the module.
 *
 * @see uart_flush
 * @public
 */
int uart_write(uart_module_t *module,
               const char *buf,
               unsigned int length);

/**
 * @brief Reads in up to @em length characters from the given module.
 *
 * @details This function will try to read in up to @em length characters from the given module
 * into the given array.
 *
 * @note Accesses to the software buffer are atomic.
 *
 * @param[in]  module
 *             The module from which to read.
 * @param[in]  buf
 *             A pointer to the array to which to write characters.
 * @param[in]  length
 *             The maximum number of characters that may be written to the array @em buf.
 *
 * @returns The number of characters successfully read from the module.
 *
 * @see uart_flush
 * @public
 */
int uart_read(uart_module_t *module,
              char *buf,
              unsigned int length);
int uart_flush(uart_module_t *module,
               uart_direction_e direction);
int uart_close(uart_module_t *module,
               uart_direction_e direction);
int uart_cleanup(uart_module_t *module);
void uart_tx_isr(uart_module_t *module);
void uart_rx_isr(uart_module_t *module);
void uart_dma_isr(uart_module_t *module);

/**
 * @}
 */ // End of UARTModulePublic group

// End include guard
#endif // _UART_MODULE_H
