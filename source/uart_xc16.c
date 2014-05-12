/* -*- mode: C; tab-width: 4; -*- */
/**
 * @file uart_module.c
 *
 * @brief This file defines all the functions of a UART module for the dsPIC33F microcontroller.
 *
 * @author     Liam Bucci
 * @date       6/6/2012
 * @carlnumber FIRM-0004
 * @version    0.3.0
 *
 * @todo Move pin assignments to main.
 */

/**
 * @addtogroup UARTModule
 *
 * @{
 */

// Include standard C library files
#include <stdlib.h>

// Include Microchip Peripheral Library files
#include <p33Fxxxx.h>
#include <pps.h>
#include <dma.h>

// Include board information
#include "../def/board.def"

// Include helper code
#include "../include/dma_channel.h"
#include "../include/cbuffer.h"

// Include module declaration
#include "../include/uart_module.h"


/* Private Data Declaration */
/**
 * @brief The private data structure for a module.
 *
 * @details This structure stores data which should only be accessed and modified by module code.
 *
 * @private
 */
struct uart_private_s
{
    cbuffer_t *rx_buffer; /**< A pointer to the software RX buffer */
    cbuffer_t *tx_buffer; /**< A pointer to the software TX buffer */
    
    uart_direction_e open_state; /**< Whether the module is open and which directions are operating */
};

/* Private Enumerations */
/**
 * This enumeration defines the offsets for the UART module special function registers.
 *
 * @private
 */
enum uart_offsets_e
{
    UART_OFFSET_UxMODE  = 0x0000,
    UART_OFFSET_UxSTA   = 0x0001,
    UART_OFFSET_UxTXREG = 0x0002,
    UART_OFFSET_UxRXREG = 0x0003,
    UART_OFFSET_UxBRG   = 0x0004
};

/**
 * This enumeration defines the UART bit offsets.
 *
 * @private
 */
enum uart_bitmasks_e
{
    //UxMODE
    UART_BITMASK_STSEL    = 0x0001,
    UART_BITMASK_PDSEL0   = 0x0002,
    UART_BITMASK_PDSEL1   = 0x0004,
    UART_BITMASK_BRGH     = 0x0008,
    UART_BITMASK_URXINV   = 0x0010,
    UART_BITMASK_ABAUD    = 0x0020,
    UART_BITMASK_LPBACK   = 0x0040,
    UART_BITMASK_WAKE     = 0x0080,
    UART_BITMASK_UEN0     = 0x0100,
    UART_BITMASK_UEN1     = 0x0200,
    UART_BITMASK_RTSMD    = 0x0800,
    UART_BITMASK_IREN     = 0x1000,
    UART_BITMASK_USIDL    = 0x2000,
    UART_BITMASK_UARTEN   = 0x8000,
    
    //UxSTA
    UART_BITMASK_URXDA    = 0x0001,
    UART_BITMASK_OERR     = 0x0002,
    UART_BITMASK_FERR     = 0x0004,
    UART_BITMASK_PERR     = 0x0008,
    UART_BITMASK_RIDLE    = 0x0010,
    UART_BITMASK_ADDEN    = 0x0020,
    UART_BITMASK_URXISEL0 = 0x0040,
    UART_BITMASK_URXISEL1 = 0x0080,
    UART_BITMASK_TRMT     = 0x0100,
    UART_BITMASK_UTXBF    = 0x0200,
    UART_BITMASK_UTXEN    = 0x0400,
    UART_BITMASK_UTXBRK   = 0x0800,
    UART_BITMASK_UTXISEL0 = 0x2000,
    UART_BITMASK_UTXINV   = 0x4000,
    UART_BITMASK_UTXISEL1 = 0x8000
};

/**
 * @brief Default values for all UART module registers
 *
 * @details This enumeration defines the default values for all special function registers of a
 * UART module.
 *
 * @private
 */
enum uart_defaults_e
{
    UART_DEFAULT_UxMODE = 0x0000, /**< The default value for the UxMODE SFR */
    UART_DEFAULT_UxSTA  = 0x0110, /**< The default value for the UxSTA SFR */
    UART_DEFAULT_UxBRG  = 0x0000  /**< The default value for the UxBRG SFR */
};


/** Module Function Prototypes */
int uart_init(uart_module_t *module)
{
    dma_attr_s dma_attr;
    int retval = UART_E_NONE;
    
    // Check for a valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        // Return an error
        return UART_E_MODULE;
    }

    // Allocate a new private struct
    if( module->private = malloc(sizeof(struct uart_private_s)) == NULL )
    {// Allocation failed
        return UART_E_ALLOC;
    }

    // Set all SFRs to default values
    *(module->base_address + UART_OFFSET_UxMODE) = UART_DEFAULT_UxMODE;
    *(module->base_address + UART_OFFSET_UxSTA)  = UART_DEFAULT_UxSTA;
    *(module->base_address + UART_OFFSET_UxBRG)  = UART_DEFAULT_UxBRG;

    // ---- Configure control settings ----
    // Configure Loopback settings
    if( module->attr.control_settings & UART_BITMASK_LOOPBACK )
    {// Enable loopback
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_LPBACK;
    }
    else
    {// Disable loopback
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_LPBACK);
    }

    // Configure IrDA settings
    if( module->attr.control_settings & UART_IRDA_INT )
    {// Enable internal IrDA encoding/decoding
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_IREN;
    }
    else if( module->attr.control_settings & UART_IRDA_EXT )
    {// Enable external IrDA encoding/decoding
        
    }
    else
    {// Disable IrDA encoding/decoding
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_IREN);
    }

    // Configure CPU Idle settings
    if( module->attr.control_settings & UART_CPUIDLE_STOP )
    {// Stop module operation during CPU idle
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_USIDL;
    }
    else
    {// Continue module operation during CPU idle
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_USIDL);
    }

    // Configure Wake-up settings
    if( module->attr.control_settings & UART_WAKEUP_EN )
    {// Enable wake-up on start bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_WAKE;
    }
    else
    {// Disable wake-up on start bit
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_WAKE);
    }

    // Configure Flow Control settings
    if( (module->attr.control_settings & UART_BITMASK_FLOWCNTL) == UART_FLOWCNTL_RTS )
    {// RTS pin only
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_UEN0;
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_UEN1);
    }
    else if( (module->attr.control_settings & UART_BITMASK_FLOWCNTL) == UART_FLOWCNTL_RTSCTS )
    {// RTS and CTS pins
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_UEN0);
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_UEN1;
    }
    else
    {// No flow control
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_UEN1 | UART_BITMASK_UEN0);
    }

    // Configure RTS Mode settings
    if( module->attr.control_settings & UART_RTSMODE_SIMPLEX )
    {// Simplex mode
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_RTSMD;
    }
    else
    {// Flow Control mode
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_RTSMD);
    }

    // Configure RX Idle state settings
    if( module->attr.control_settings & UART_RXIDLE_LOW )
    {// RX idle low
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_URXINV;
    }
    else
    {// RX idle high
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_URXINV);
    }

    // Configure TX Idle state settings
    if( module->attr.control_settings & UART_TXIDLE_LOW )
    {// TX idle low
        *(module->base_address + UART_OFFSET_UxSTA) |= UART_BITMASK_UTXINV;
    }
    else
    {// TX idle high
        *(module->base_address + UART_OFFSET_UxSTA) &= ~(UART_BITMASK_UTXINV);
    }

    // Configure Address Character Detect settings
    if( module->attr.control_settings & UART_ADDRDETECT_EN )
    {// Enable address character detection
        *(module->base_address + UART_OFFSET_UxSTA) |= UART_BITMASK_ADDEN;
    }
    else
    {// Disable address character detection
        *(module->base_address + UART_OFFSET_UxSTA) &= ~(UART_BITMASK_ADDEN);
    }
        
    // Configure Data Format settings
    switch( module->attr.control & UART_BITMASK_DATAFMT )
    {
    case UART_DATAFMT_9N1:
        *(module->base_address + UART_OFFSET_UxMODE) |= (UART_BITMASK_PDSEL1 | UART_BITMASK_PDSEL0);
        break;
    case UART_DATAFMT_8E1:
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_PDSEL0;
        break;
    case UART_DATAFMT_8O1:
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_PDSEL1;
        break;
    case UART_DATAFMT_8N2:
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_STSEL;
        break;
    case UART_DATAFMT_9N2:
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_PDSEL1 | UART_BITMASK_PDSEL0 | UART_BITMASK_STSEL;
        break;
    case UART_DATAFMT_8E2:
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_PDSEL0 | UART_BITMASK_STSEL;
        break;
    case UART_DATAFMT_8O2:
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_PDSEL1 | UART_BITMASK_STSEL;
        break;

        // Some trickery here to use 8N1 as default and still return an error code.
    default:
        retval = UART_E_SETTINGS;
    case UART_DATAFMT_8N1:
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_PDSEL1 | UART_BITMASK_PDSEL0 | UART_BITMASK_STSEL);
        break;
    }   

    // Set baud rate
#if defined(UART_BRGH) && defined(UART_BRG) // Set BRGH and BRG manually
#if UART_BRGH >= 1 // Set BRGH
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;
#else // Clear BRGH
        *(module->base_address + UART_OFFSET_UxMODE) &= ~(UART_BITMASK_BRGH);
#end
        *(module->base_address + UART_OFFSET_UxBRG) = UART_BRG;
#else // Set BRGH and BRG according to baud_rate
    switch(module->attr.baud_rate)
    {
    case UART_BAUDRATE_1200:
#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 8332;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 4165;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 2082;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 1665;
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 1040;
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 832;
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
    case UART_BAUDRATE_2400:

#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 4165;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 2082;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 1040;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 832;
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 519;
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 415;
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    case UART_BAUDRATE_4800:
        #if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 2082;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 1040;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 519;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 415;
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 259;
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 207;
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    case UART_BAUDRATE_19200:
#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 519;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 259;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 129;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 103;
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 64;
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 51;
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    case UART_BAUDRATE_38400:
#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 259;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 129;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 64;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 51;
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 31;

        #warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 25;
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    case UART_BAUDRATE_57600:
#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 172;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 85;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 42;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 33;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 20;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 16;
        
#warning "UART: Selected baud rate will have >1% error!"
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    case UART_BAUDRATE_115200:
#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 85;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 42;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 20;
        
#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 16;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 9;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 7;

#warning "UART: Selected baud rate will have >1% error!"
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    case UART_BAUDRATE_230400:
      #if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 42;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 20;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 9;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 7;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 4;

#warning "UART: Selected baud rate will have >1% error!"
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 3;

#warning "UART: Selected baud rate will have >1% error!"
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif  
        break;
    default:
        retval = UART_E_SETTINGS;
    case UART_BAUDRATE_9600:
        // Default baud rate is 9600bps
#if FCY == 40000000 // Fcy is 40MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 1040;
#elif FCY == 20000000 // Fcy is 20MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 519;
#elif FCY == 10000000 // Fcy is 10MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 259;
#elif FCY == 8000000 // Fcy is 8MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 207;
#elif FCY == 5000000 // Fcy = 5MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 129;
#elif FCY == 4000000 // Fcy = 4MHz
        // Set BRGH bit
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_BRGH;

        // Set UxBRG register to precomputed value
        *(module->base_address + UART_OFFSET_BRG) = 103;
#else
#error "UART: Unknown FCY value. Can't set baud rate!"
#endif
        break;
    }
#endif // End set BRGH and BRG according to baud_rate.

    // Set up software RX buffer if applicable
    switch( module->attr.tx_rx_settings & UART_BITMASK_RXBUF )
    {
    case UART_RXBUF_4:
        // Use a 4 character software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(4);
        break;
    case UART_RXBUF_8:
        // Use an 8 character software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(8);
        break;
    case UART_RXBUF_12:
        // Use a 12 character software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(12);
        break;
    case UART_RXBUF_16:
        // Use a 16 character software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(16);
        break;
    case UART_RXBUF_24:
        // Use a 24 character software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(24);
        break;
    case UART_RXBUF_32:
        // Use a 32 character software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(32);
        break;
    case UART_RXBUF_DMA:
        // Try to match software buffer to DMA buffer

        // Check if rx_dma is valid
        if( !dma_isvalid(module->rx_dma) )
        {// Channel is invalid
            // Return an error
            retval = UART_E_SETTINGS;
            // Use default setting of no software buffer
            ((struct uart_private_s *)module->private)->rx_buffer = NULL;
        }
        else
        {
            // Allocate a new cbuffer the same length as half the DMA buffer (since we'll be using ping-pong)
            ((struct uart_private_s *)module->private)->rx_buffer = new_cbuffer(dma_get_bufferA_size(module->rx_dma));
        }
        break;

        // Some trickery here to use UART_RXBUF_NONE as default and still return an error code
    default:
        retval = UART_E_SETTINGS;
    case UART_RXBUF_NONE:
        // Don't use a software RX buffer
        ((struct uart_private_s *)module->private)->rx_buffer = NULL;
        break;
    }

    
    // Set up software TX buffer if applicable
    switch( module->attr.tx_rx_settings & UART_BITMASK_TXBUF )
    {
    case UART_TXBUF_4:
        // Use a 4 character software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(4);
        break;
    case UART_TXBUF_8:
        // Use an 8 character software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(8);
        break;
    case UART_TXBUF_12:
        // Use a 12 character software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(12);
        break;
    case UART_TXBUF_16:
        // Use a 16 character software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(16);
        break;
    case UART_TXBUF_24:
        // Use a 24 character software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(24);
        break;
    case UART_TXBUF_32:
        // Use a 32 character software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(32);
        break;
    case UART_TXBUF_DMA:
        // Try to match software buffer to DMA buffer

        // Check if tx_dma is valid
        if( !dma_isvalid(module->tx_dma) )
        {// Channel is invalid
            // Return an error
            retval = UART_E_SETTINGS;
            // Use default setting of no software buffer
            ((struct uart_private_s *)module->private)->tx_buffer = NULL;
        }
        else
        {
            // Allocate a new cbuffer the same length as the DMA buffer
            ((struct uart_private_s *)module->private)->tx_buffer = new_cbuffer(dma_get_bufferA_size(module->tx_dma));
        }
        break;

        // Some trickery here to use UART_TXBUF_NONE as default and still return an error code
    default:
        retval = UART_E_SETTINGS;
    case UART_TXBUF_NONE:
        // Don't use a software TX buffer
        ((struct uart_private_s *)module->private)->tx_buffer = NULL;
        break;
    }

    // Configure RX DMA channel and software buffer if applicable
    if( module->rx_dma != NULL && module->rx_dma->base_address != NULL && module->rx_dma->primary_buffer != NULL )
    {// DMA channel is valid
        // Check if software buffer is enabled
        if( ((struct uart_private_s *)module->private)->rx_buffer != NULL )
        {// RX software buffer is valid
            // Configure DMA channel to respond to UART RX
            dma_attr.control = DMA_OPMODE_CONTINUOUS | DMA_PINGPONG_EN | DMA_ADDRMODE_REGISTER_IND | DMA_ADDRMODE_POSTINC_EN | DMA_INT_ALL | DMA_DIR_FROMPERIPHERAL | DMA_DATASIZE_WORD;
            //! @todo Find a way to generalize DMA IRQ to both UART modules.
            dma_attr.irq = DMA_IRQ_UART1RX;
            dma_attr.peripheral_address = (volatile unsigned int *)(module->base_address + UART_OFFSET_UxRXREG);

            // Initialize RX DMA channel
            dma_init(module->rx_dma, &dma_attr);
            
            //! @todo Enable DMA interrupt.
    
    // Configure interrupts (must have UART module ISRs in place!)
    // If DMA is enabled then the UART module ISRs should go in the corresponding DMA channel ISR.
    // If no DMA is used then the UART module ISRs should go in the UxTX/UxRX ISRs.
    if( module->rx_dma != NULL && module->rx_dma->base_address != NULL && module->rx_dma->primary_buffer != NULL )
    {// DMA channel is valid
        // If software buffer
        // Configure RX interrupts for every character
        

    // Initialize PPS
    //! PPS initialization must take place AFTER uart hardware is set up.
    //! Otherwise the first byte becomes garbled!
    PPSUnLock;
    PPSOutput(PPS_U1TX,PPS_RP10);
    PPSInput(PPS_U1RX,PPS_RP11);
    PPSLock;
}

int uart_open(uart_module_t *module,
              uart_direction_e direction)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {
        return UART_E_MODULE;
    }

    // Enable appropriate directions
    if( direction == UART_TX || direction == UART_TXRX )
    {
        // Enable UART and TX
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_UARTEN;
        *(module->base_address + UART_OFFSET_UxSTA) |= UART_BITMASK_UTXEN;

        // Enable DMA if applicable
        if( module->tx_dma != NULL && module->tx_dma->base_address != NULL && module->tx_dma->primary_buffer != NULL )
        {// DMA channel is valid
            if( dma_enable(module->tx_dma) != DMA_E_NONE )
            {// An error occurred
                // handle errors!
                return UART_E_DMA;
            }
        }
    }

    if( direction == UART_RX || direction == UART_TXRX )
    {
        // Enable UART
        *(module->base_address + UART_OFFSET_UxMODE) |= UART_BITMASK_UARTEN;
}

int uart_write(uart_module_t *module,
               const char *buf,
               unsigned int length)
{
    unsigned int bytes_written = 0;

    // Check for valid module and buffer
    if( module == NULL || module->tx_dma == NULL )
    {
        return -1;
    }

    // Check if transmission is currently in progress
    if( !(*(module->base_address + UART_OFFSET_STA) & UART_BITMASK_TRMT) )
    {// Transfer in progress
        return -2;
    }

    
    for( bytes_written = 0; bytes_written < length && bytes_written < module->tx_dma->primary_buffer_length; ++bytes_written )
    {
        *((unsigned char *)(module->tx_dma->primary_buffer) + bytes_written) = *(buf + bytes_written);
    }

    // Set DMA channel count
    dma_set_count(module->tx_dma, bytes_written-1);
    
    // Force a DMA transfer
    dma_force(module->tx_dma);

    return bytes_written;
}


int uart_read(struct uart_module_s *module,
              char *buf,
              unsigned int length)
{
    //! @TODO Change rx_buffer to FIFO, currently the function will miss bytes if max_bytes is smaller than the amount of bytes in the buffer!
    unsigned int bytes_read = 0;
    volatile unsigned int *read_ptr;
    char *write_ptr;

    if( module == NULL )
    {
        return -1;
    }
    
    //! @TODO Make function atomic!
/*
    if( max_bytes == 0 )
    {// If out_array is zero length return the number of bytes available in the buffer.
        return module->rx_buffer_length;
    }

    // Set read_ptr to start of rx_buffer
    read_ptr = &(module->rx_buffer[0]);

    // Initialize write_ptr to first byte of out_array,
    // Loop while write_ptr is less than the 1 + the highest index of out_array,
    // Increment read_ptr and write_ptr each iteration
    for( write_ptr=&(out_array[0]); write_ptr<&(out_array[max_bytes]); ++read_ptr, ++write_ptr )
    {
        if( read_ptr == &(module->rx_buffer[module->rx_buffer_length]) )
        {// End of rx_buffer, no more bytes to read
            module->rx_buffer_length = 0;
            uart_flush_rx(module); // Flush any residual bytes from UART buffer
            if( module->rx_buffer_length == 0 )
            {// No more bytes to read
                return bytes_read;
            }
            read_ptr = &(module->rx_buffer[0]); // Reset read_ptr to beginning of rx_buffer
        }

        *write_ptr = *read_ptr;
        ++bytes_read;
    }
*/
    return bytes_read;
}


int uart_flush(uart_module_t *module,
               uart_direction_e direction)
{
    if( module == NULL )
    {
        return -1;
    }
    /*
    if( U1STAbits.TRMT == 1 )
    {// Shift register is empty, no transmission in progress.

        if( module->tx_buffer.cursor != module->tx_buffer.head )
        {// There are characters in the buffer
            memcpy(module->tx_dma->primary_buffer, module->tx_buffer.head, module->tx_buffer.cursor - module->tx_buffer.head);
            dma_set_count(module->tx_dma, module->tx_buffer.cursor - module->tx_buffer.head);
            dma_enable(module->tx_dma);
            dma_force(module->tx_dma);
            module->tx_buffer.cursor = module->tx_buffer.head;
        }

        return 0;
    }
    */
    return -1;
}

int uart_close(uart_module_t *module,
               uart_direction_e direction)
{

}

int uart_cleanup(uart_module_t *module)
{

}

/*
int uart_flush_rx(struct uart_module_s *module)
{
    if( module == NULL )
    {
        return -1;
    }
    
    while( U1STAbits.URXDA == 1 )
    {
        if( module->rx_buffer_length >= UART_RX_BUFFER_LENGTH )
        {// RX buffer is full
            return -1;
        }
        
        module->rx_buffer[module->rx_buffer_length] = U1RXREG;
        ++module->rx_buffer_length;
    }

    return 0;
}
*/

/**
 * @}
 */ // End of UARTModule group
