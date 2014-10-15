/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file uart_xc16.h
 *
 * @brief This header defines and prototypes the public functions and data structures of a UART
 * module for the 16-bit series of PIC microcontrollers (e.g. dsPIC33F, PIC24.
 *
 * @author Liam Bucci
 * @date 6/6/2012
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

// Include guard
#ifndef _UART_XC16_H
#define _UART_XC16_H


/**
 * @defgroup UARTModule UART Module
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
#include <dma_channel.h>

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
enum uart_module_e
{
    // Default Module Settings
    UART_MODULE_DEFAULT            = 0x0000, /**< The default module settings */
    
    // CPU Idle settings
    UART_MODULE_CPUIDLE_BITMASK    = 0x0001, //!< Bitmask for CPU idle settings
    UART_MODULE_CPUIDLE_CONTINUE   = 0x0000, //!< Continue module operation during CPU idle @default
    UART_MODULE_CPUIDLE_STOP       = 0x0001, //!< Stop module operation during CPU idle

    // Wake-up setting
    UART_MODULE_WAKEUP_BITMASK     = 0x0002, //!< Bitmask for Wake-up settings
    UART_MODULE_WAKEUP_DIS         = 0x0000, //!< Disable wake-up on start bit during CPU sleep mode @default
    UART_MODULE_WAKEUP_EN          = 0x0002, //!< Enable wake-up on start bit during CPU sleep mode

    // Flow Control settings
    UART_MODULE_FLOWCNTL_BITMASK   = 0x0060, //!< Bitmask for Flow Control settings
    UART_MODULE_FLOWCNTL_NONE      = 0x0000, //!< No flow control @default
    UART_MODULE_FLOWCNTL_RTS       = 0x0020, //!< RTS pin only is enabled and used by hardware.
    UART_MODULE_FLOWCNTL_RTSCTS    = 0x0040, //!< RTS and CTS pins are enabled and used by hardware.

    // RTS Mode settings
    UART_MODULE_RTSMODE_BITMASK    = 0x0080, //!< Bitmask for RTS Mode settings
    UART_MODULE_RTSMODE_FLOW       = 0x0000, //!< RTS is in flow control mode @default
    UART_MODULE_RTSMODE_SIMPLEX    = 0x0080, //!< RTS is in simplex mode

    // Receive idle state setting (i.e. polarity inversion)
    UART_MODULE_RXIDLE_BITMASK     = 0x0100, /**< Bitmask for Receive idle state settings */
    UART_MODULE_RXIDLE_HIGH        = 0x0000, /**< Receive pin idle state is high @default */
    UART_MODULE_RXIDLE_LOW         = 0x0100, /**< Receive pin idle state is low */

    // Transmit idle state setting (i.e. polarity inversion)
    UART_MODULE_TXIDLE_BITMASK     = 0x0200, /**< Bitmask for Transmit idle state settings */
    UART_MODULE_TXIDLE_HIGH        = 0x0000, /**< Transmit pin idle state is high @default */
    UART_MODULE_TXIDLE_LOW         = 0x0200, /**< Transmit pin idle state is low */
};

/**
 * @todo Change IrDA to another mode so that it can be enabled in either 8-bit or 9-bit mode.
 */
enum uart_mode_e
{
    // Module default mode
    UART_MODE_DEFAULT              = 0x0000, /**< Default mode settings */
    
    // Module major modes
    UART_MAJOR_MODE_BITMASK        = 0x000F, /**< Bitmask for major modes */
    UART_MAJOR_MODE_STD            = 0x0000, /**< Standard (8-bit) data mode @default */
    UART_MAJOR_MODE_9BIT           = 0x0001, /**< 9-bit mode, reads/writes use words instead of
                                              * chars */
    UART_MAJOR_MODE_IRDA           = 0x0002, /**< IrDA mode
                                              * @todo Remove IrDA major mode and add a new IrDA
                                              * mode which sets up an IrDA encoder/decoder */
    UART_MAJOR_MODE_LIN            = 0x0003, /**< LIN mode */

    // Module minor modes
    UART_MINOR_MODE_BITMASK        = 0x00F0, /**< Bitmask for minor modes */
    
    // Standard (8-bit) bit settings
    UART_MINOR_MODE_STD_8N1        = 0x0000, /**< 8 data bits, no parity, 1 stop bit @default */
    UART_MINOR_MODE_STD_8N2        = 0x0010, /**< 8 data bits, no parity, 2 stop bits */
    UART_MINOR_MODE_STD_8O1        = 0x0020, /**< 8 data bits, odd parity, 1 stop bit */
    UART_MINOR_MODE_STD_8O2        = 0x0030, /**< 8 data bits, odd parity, 2 stop bits */
    UART_MINOR_MODE_STD_8E1        = 0x0040, /**< 8 data bits, even parity, 1 stop bit */
    UART_MINOR_MODE_STD_8E2        = 0x0050, /**< 8 data bits, even parity, 2 stop bits */

    // 9-bit bit format
    UART_MINOR_MODE_9BIT_9N1       = 0x0000, /**< 9 data bits, no parity, 1 stop bit @default */
    UART_MINOR_MODE_9BIT_9N2       = 0x0010, /**< 9 data bits, no parity, 2 stop bits */

    // 9-bit addressing
    UART_MINOR_MODE_9BIT_DATA      = 0x0000, /**< Treat bytes as data @default */
    UART_MINOR_MODE_9BIT_ADDR_MASK = 0x0020, /**< Mask bytes not bound for this device */
    UART_MINOR_MODE_9BIT_ADDR_PROM = 0x0040, /**< Listen to all bytes, regardless of address */

    // IrDA encoder/decoder settings
    UART_MINOR_MODE_IRDA_INTERNAL  = 0x0000, /**< Use the internal IrDA encoder and decoder @default */
    UART_MINOR_MODE_IRDA_EXTERNAL  = 0x0010, /**< Use an external IrDA encoder and decoder */

    // Module debug mode
    UART_DEBUG_MODE_BITMASK        = 0x8000, /**< Bitmask for debug modes */
    UART_DEBUG_MODE_OFF            = 0x0000, /**< No debug aids @default */
    UART_DEBUG_MODE_LOOP           = 0x8000  /**< Loop all data back internally (No pin assignment
                                              * necessary) */
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
typedef enum uart_tx_buffer_e
{
    // TX buffer mode
    UART_TX_BUFFER_MODE_BITMASK = 0x000F, /**< Bitmask for TX buffer mode settings */
    UART_TX_BUFFER_MODE_HWONLY  = 0x0000, /**< TX uses hardware FIFO buffer only @default */
    UART_TX_BUFFER_MODE_DMA     = 0x0001, /**< TX uses a DMA buffer */
    UART_TX_BUFFER_MODE_SOFT    = 0x0002, /**< TX uses a software buffer */
    UART_TX_BUFFER_MODE_HYBRID  = 0x0003, /**< TX uses a combination DMA and software buffer */

    // TX software buffer size
    UART_TX_BUFFER_SIZE_BITMASK = 0x00F0, /**< Bitmask for TX buffer size */
    UART_TX_BUFFER_SIZE_MATCH   = 0x0000, /**< Try to match DMA buffer size (hybrid only) @default */
    UART_TX_BUFFER_SIZE_4       = 0x0000, /**< 4 byte software buffer (software only) @default */
    UART_TX_BUFFER_SIZE_8       = 0x0010, /**< 8 byte software buffer */
    UART_TX_BUFFER_SIZE_12      = 0x0020, /**< 12 byte software buffer */
    UART_TX_BUFFER_SIZE_16      = 0x0030, /**< 16 byte software buffer */
    UART_TX_BUFFER_SIZE_24      = 0x0040, /**< 24 byte software buffer */
    UART_TX_BUFFER_SIZE_32      = 0x0050, /**< 32 byte software buffer */
    UART_TX_BUFFER_SIZE_64      = 0x0060, /**< 64 byte software buffer */
    UART_TX_BUFFER_SIZE_128     = 0x0070, /**< 128 byte software buffer */
} uart_tx_buffer_t;

typedef enum uart_rx_buffer_e
{
    // RX buffer mode
    UART_RX_BUFFER_MODE_BITMASK = 0x000F, /**< Bitmask for RX buffer mode settings */
    UART_RX_BUFFER_MODE_HWONLY  = 0x0000, /**< RX uses hardware FIFO buffer only @default */
    UART_RX_BUFFER_MODE_DMA     = 0x0001, /**< RX uses a DMA buffer */
    UART_RX_BUFFER_MODE_SOFT    = 0x0002, /**< RX uses a software buffer */
    UART_RX_BUFFER_MODE_HYBRID  = 0x0003, /**< RX uses a combination DMA and software buffer */

    // RX software buffer size
    UART_RX_BUFFER_SIZE_BITMASK = 0x00F0, /**< Bitmask for RX buffer size */
    UART_RX_BUFFER_SIZE_MATCH   = 0x0000, /**< Try to match DMA buffer size (hybrid only) @default */
    UART_RX_BUFFER_SIZE_4       = 0x0000, /**< 4 byte software buffer (software only) @default */
    UART_RX_BUFFER_SIZE_8       = 0x0010, /**< 8 byte software buffer */
    UART_RX_BUFFER_SIZE_12      = 0x0020, /**< 12 byte software buffer */
    UART_RX_BUFFER_SIZE_16      = 0x0030, /**< 16 byte software buffer */
    UART_RX_BUFFER_SIZE_24      = 0x0040, /**< 24 byte software buffer */
    UART_RX_BUFFER_SIZE_32      = 0x0050, /**< 32 byte software buffer */
    UART_RX_BUFFER_SIZE_64      = 0x0060, /**< 64 byte software buffer */
    UART_RX_BUFFER_SIZE_128     = 0x0070, /**< 128 byte software buffer */
} uart_rx_buffer_t;
    

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
typedef enum uart_baudrate_e
{
    UART_BAUDRATE_UNKNOWN = 0x0000, /**< The baud rate value is either manually set or has not been
                                       set yet. In either case the value is unknown. @default */
    UART_BAUDRATE_1200    = 0x0001, /**< Module baud rate is 1200bps */
    UART_BAUDRATE_2400    = 0x0002, /**< Module baud rate is 2400bps */
    UART_BAUDRATE_4800    = 0x0003, /**< Module baud rate is 4800bps */
    UART_BAUDRATE_9600    = 0x0004, /**< Module baud rate is 9600bps */
    UART_BAUDRATE_19200   = 0x0005, /**< Module baud rate is 19200bps */
    UART_BAUDRATE_38400   = 0x0006, /**< Module baud rate is 38400bps */
    UART_BAUDRATE_57600   = 0x0007, /**< Module baud rate is 57600bps */
    UART_BAUDRATE_115200  = 0x0008, /**< Module baud rate is 115200bps */
    UART_BAUDRATE_230400  = 0x0009, /**< Module baud rate is 230400bps */
    UART_BAUDRATE_460800  = 0x000A, /**< Module baud rate is 460800bps */
    UART_BAUDRATE_921600  = 0x000B, /**< Module baud rate is 921600bps */
    UART_BAUDRATE_1000000 = 0x000C, /**< Module baud rate is 1000000bps */
    UART_BAUDRATE_1843200 = 0x000D, /**< Module baud rate is 1843200bps */
    UART_BAUDRATE_2000000 = 0x000E, /**< Module baud rate is 2000000bps */
    UART_BAUDRATE_3686400 = 0x000F, /**< Module baud rate is 3686400bps */
    UART_BAUDRATE_AUTO    = 0x0010  /**< Module baud rate is determined automatically by reception
                                       of a sync character (0x55). This setting should not be used
                                       to initialize the module. Instead call the provided autobaud
                                       function. This setting is provided only as a read value. */
    
} uart_baudrate_t;

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
    UART_E_NONE     = 0,      /**< Success, no errors */
    UART_E_MODULE   = -1,     /**< The module was invalid */
    UART_E_CONFIG   = -2,     /**< Conflicting or invalid settings/configuration */ 
    UART_E_INPUT    = -3,     /**< The input pointer or data was invalid */
    UART_E_OUTPUT   = -4,     /**< The output pointer was invalid */
    UART_E_DMA      = -5,     /**< A problem occurred with DMA */
    UART_E_SOFTBUF  = -6,     /**< A problem occurred with the software buffer */
    UART_E_ALLOC    = -7,     /**< A problem occurred during dynamic memory allocation */
    UART_E_CLOSED   = -8,     /**< The module is closed and cannot be read from/written to */
    UART_E_ASSERT   = 0x8001, /**< An assertion failed */
    UART_E_UNKNOWN  = 0x8000  /**< Unknown error occurred */
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
typedef enum uart_direction_e
{
    UART_DIRECTION_BITMASK = 0x0003, /**< Bitmask for direction settings */
    UART_DIRECTION_NONE    = 0x0000, /**< No direction, also means module is 'closed' */
    UART_DIRECTION_TX      = 0x0001, /**< Transmit */
    UART_DIRECTION_RX      = 0x0002, /**< Receive */
    UART_DIRECTION_TXRX    = 0x0003  /**< Transmit and Receive */
} uart_direction_t;


/* ***** Attribute Declaration ***** */

/**
 * @brief The attribute struct for a UART module.
 *
 * @details This struct holds the attributes (or settings) for a particular instance of a UART
 * module. The settings stored within will be used during module initialization.
 *
 * The settings stored in this struct are meant to be static over the life of the module. In
 * other words, the module should be reinitialized if any of these settings are changed.
 *
 * To set the values just logically OR together settings from the corresponding enumeration. See
 * that enumeration documentation for which settings may be used together.
 *
 * @public
 */
typedef struct uart_attr_s
{
    
    /**
     * @brief General module function settings.
     *
     * @details The #module_settings variable stores settings relating to basic functionality of
     * the module, such as idle states, flow control, etc.
     *
     * @see uart_module_e
     */
    int module_settings;

    /**
     * @brief Operation mode settings for the module.
     *
     * @details The #mode_settings variable stores how the module will operate (e.g. 8/9-bit data,
     * LIN, etc.). This is broken down into major and minor modes (along with a few
     * other miscellaneous modes).
     *
     * @see uart_mode_e
     */
    int mode_settings;

    /**
     * @brief Transmit buffer specific settings.
     *
     * @details The #tx_buffer_settings variable stores settings relating to the transmission of
     * data. Settings such as TX buffer mode and buffer size.
     *
     * @see uart_tx_buffer_e
     */
    int tx_buffer_settings;

    /**
     * @brief Receive buffer specific settings.
     *
     * @details The #rx_buffer_settings variable stores settings relating to the reception of data.
     * Settings such as RX buffer mode and buffer size.
     *
     * @see uart_rx_buffer_e
     */
    int rx_buffer_settings;

    
} uart_attr_t;


/* ***** Module Definition ***** */

// Forward declaration of uart_module_t for use in uart_module_s definition
struct uart_module_s;
typedef struct uart_module_s uart_module_t;

/**
 * @brief The UART module data structure definition.
 *
 * @details This structure is a representation of an actual hardware UART module, it contains all
 * the settings and data which are used to control and use that particular UART module. It also
 * contains some software settings which are used to configure how the uart_module code interacts
 * with the hardware UART.
 * 
 * @public
 */
struct uart_module_s
{
    /**
     * @brief The UART module number.
     *
     * @details This is the hardware module number of the UART module. This is used to help map
     * the correct special function registers.
     *
     * This value should only be set once, during instantiation of the uart_module_t object.
     */
    const unsigned int uart_number;
    
    /**
     * These function pointers are used as callback functions for when characters are available for
     * reading (RX) or room becomes available in the transmit software buffer (TX). See
     * #uart_tx_rx_settings_e for more information on specifying when the callback functions are
     * invoked.
     *
     * @note These functions may be invoked during an ISR, so ensure that they are short and are
     * appropriate for an interrupt service routine. Ideally they should just schedule another
     * function to do any more reads or writes to the module. They may not call uart_write(),
     * uart_read(), or uart_flush().
     */
    void (* const tx_callback)(uart_module_t *module); /**< The callback function invoked when space in
                                                          the TX buffer is available. */
    void (* const rx_callback)(uart_module_t *module); /**< The callback function invoked when characters
                                                          are available in the RX buffer */
    
    /**
     * This pointer holds private data for the module. This pointer should not be modified by any
     * code outside of the module. See the #uart_private_s struct for more information.
     */
    void *private;

};


/* ***** Public Function Declarations ***** */

/**
 * @brief Initializes the hardware and DMA channels and allocates memory for buffers.
 *
 * @details This function uses the #uart_attr_s struct contained in the provided module to
 * initialize the underlying hardware, initialize the assigned DMA channels, and allocate memory
 * for the buffers if needed.
 *
 * Once this function is called on a module it is ready for opening with the #uart_open() function.
 *
 * @param[in]  module
 *             The module to initialize.
 * @param[in]  attr
 *             The attribute struct to use to configure the module.
 * @param[in]  tx_dma
 *             The DMA channel which will be used for transmitting if DMA or hybrid buffer modes
 *             are chosen.
 * @param[in]  rx_dma
 *             The DMA channel which will be used for receiving if DMA or hybrid buffer modes are
 *             chosen.
 * @returns A #uart_error_e value.
 *
 * @todo Change to variable argument and allow dma channels and callback functions to be optional.
 * @todo Return a pointer to the module instead of having the user pre-allocate it. This can
 * include a DEF option to statically or dynamically allocate the space. Statically allocated
 * modules would require the user to allocate the variable, then pass in a pointer.
 *
 * @see uart_attr_s
 * @public
 */
int uart_init(uart_module_t *module,
              uart_attr_t *attr,
              dma_channel_t *tx_dma,
              dma_channel_t *rx_dma);

// Cancels any autobaud (can be called asynchronously)
int uart_set_baudrate(uart_module_t *module,
                      uart_baudrate_t baudrate);

int uart_get_baudrate(uart_module_t *module);

// Baudrate will not actually be set until the sync character is received (must be the next
// character received).
int uart_autobaud(uart_module_t *module);

int uart_add_local_addr(uart_module_t *module,
                        char addr);

int uart_remove_local_addr(uart_module_t *module,
                           char addr);


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
              uart_direction_t direction);

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
               const void *buffer,
               unsigned int length);

/**
 * @brief Reads in up to #length characters from the given module.
 *
 * @details This function will try to read in up to #length characters from the given module
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
              void *buffer,
              unsigned int length);

/**
 * @brief Flushes any data in either the TX or RX buffer.
 *
 * @details The flush function tries to force any data in the buffers to be sent or made available
 * for reading. This is not guaranteed to work, however, depending on the state of the hardware.
 *
 * @param[in]  module
 *             The UART module to work on.
 * @param[in]  direction
 *             The direction to try to flush (TX, RX, or TXRX).
 * @return A value corresponding to one of #uart_error_e.
 * @private
 */
int uart_flush(uart_module_t *module,
               uart_direction_t direction);

/**
 * @brief The TX interrupt service routine (ISR) for a UART module.
 *
 * @details This function should be inserted into the appropriate ISR for the given UART module.
 */


// Blocking?
int uart_close(uart_module_t *module,
               uart_direction_t direction);

// Must be atomic!
void uart_cleanup(uart_module_t *module);

/**
 * @brief Check if the given module is open in the given direction.
 *
 * @details This function will return true if the given module is open in the given direction(s)
 * and false if the module is not open in the specified direction(s).
 *
 * @note This function is not safe! It does not check for a valid module before dereferencing
 * the module pointer. Be sure to check the module using the #uart_is_valid function first.
 *
 * @public
 */
inline bool uart_is_open(uart_module_t *module,
                         uart_direction_t direction);
inline bool uart_is_valid(uart_module_t *module);

/**
 * @}
 */ // End of UARTModulePublic group

// End include guard
#endif // _UART_MODULE_H
