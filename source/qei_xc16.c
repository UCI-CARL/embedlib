/* -*- mode: c; tab-width: 4; -*- */

/**
 * @file qei_module.c
 *
 * @brief This file contains the private declarations and function definitions for a Quadrature Encoder
 * Interface (QEI) module.
 *
 * @author     Liam Bucci
 * @date       12/18/2012
 * @carlnumber FIRM-0004
 * @version    0.3.0
 */

/* Standard C Libraries */
#include <stdlib.h>

/* Microchip Libraries */
#include <p33Fxxxx.h>

/* Module Include */
#include "../include/qei_module.h"


/**
 * @defgroup QEIModulePrivate QEI Module Private
 *
 * @details This group contains all the private features of the QEI module code. It is meant to
 * document the internal parts of the QEI module. Code and variables which should not be seen by
 * an outside user.
 */

/* Private Enumerations */

/**
 * @brief Offsets all QEI registers.
 *
 * @details This enumeration defines the offsets (by word) of each register in a QEI module with
 * respect to the base_address pointer.
 *
 * @ingroup QEIModulePrivate
 */
enum qei_offset_e
{
    QEI_OFFSET_QEIxCON  = 0x0000, //!< Offset of QEIxCON register (base address)
    QEI_OFFSET_DFLTxCON = 0x0001, //!< Offset of DFLTxCON register
    QEI_OFFSET_POSxCNT  = 0x0002, //!< Offset of POSxCNT register
    QEI_OFFSET_MAXxCNT  = 0x0003  //!< Offset of MAXxCNT register
};

/**
 * @brief Bitmasks for all QEI register bits.
 *
 * @details This enumeration defines the bitmasks for each bit/set of bits in the QEI module
 * registers.
 *
 * @ingroup QEIModulePrivate
 */
enum qei_bitmask_e
{
    QEI_BITMASK_UDSRC   = 0x0001, //!< Bitmask for UDSRC bit
    QEI_BITMASK_TQCS    = 0x0002, //!< Bitmask for TQCS bit
    QEI_BITMASK_POSRES  = 0x0004, //!< Bitmask for POSRES bit
    QEI_BITMASK_TQCKPS  = 0x0018, //!< Bitmask for TQCKPS bits
    QEI_BITMASK_TQCKPS0 = 0x0008, //!< Bitmask for TQCKPS0 bit
    QEI_BITMASK_TQCKPS1 = 0x0010, //!< Bitmask for TQCKPS1 bit
    QEI_BITMASK_TQGATE  = 0x0020, //!< Bitmask for TQGATE bit
    QEI_BITMASK_PCDOUT  = 0x0040, //!< Bitmask for PCDOUT bit
    QEI_BITMASK_SWPAB   = 0x0080, //!< Bitmask for SWPAB bit
    QEI_BITMASK_QEIM    = 0x0700, //!< Bitmask for QEIM bits
    QEI_BITMASK_QEIM0   = 0x0100, //!< Bitmask for QEIM0 bit
    QEI_BITMASK_QEIM1   = 0x0200, //!< Bitmask for QEIM1 bit
    QEI_BITMASK_QEIM2   = 0x0400, //!< Bitmask for QEIM2 bit
    QEI_BITMASK_UPDN    = 0x0800, //!< Bitmask for UPDN bit
    QEI_BITMASK_INDEX   = 0x1000, //!< Bitmask for INDEX bit
    QEI_BITMASK_QEISIDL = 0x2000, //!< Bitmask for QEISIDL bit
    QEI_BITMASK_CNTERR  = 0x8000, //!< Bitmask for CNTERR bit

    QEI_BITMASK_QECK    = 0x0070, //!< Bitmask for QECK bits
    QEI_BITMASK_QECK0   = 0x0010, //!< Bitmask for QECK0 bit
    QEI_BITMASK_QECK1   = 0x0020, //!< Bitmask for QECK1 bit
    QEI_BITMASK_QECK2   = 0x0040, //!< Bitmask for QECK2 bit
    QEI_BITMASK_QEOUT   = 0x0080, //!< Bitmask for QEOUT bit
    QEI_BITMASK_CEID    = 0x0100, //!< Bitmask for CEID bit
    QEI_BITMASK_IMV     = 0x0600, //!< Bitmask for IMV bits
    QEI_BITMASK_IMV0    = 0x0200, //!< Bitmask for IMV0 bit
    QEI_BITMASK_IMV1    = 0x0400  //!< Bitmask for IMV1 bit
};

/**
 * @brief This enumeration defines the default values for all the registers in a QEI module.
 *
 * @note Any change to these default values will change assumptions made in the qei_init()
 * function.
 *
 * @ingroup QEIModulePrivate
 */
enum qei_default_e
{
    QEI_DEFAULT_QEIxCON  = 0x0000, //!< Default value for QEIxCON register
    QEI_DEFAULT_DFLTxCON = 0x0000, //!< Default value for DFLTxCON register
    QEI_DEFAULT_POSxCNT  = 0x0000, //!< Default value for POSxCNT register
    QEI_DEFAULT_MAXxCNT  = 0xFFFF  //!< Default value for MAXxCNT register
};


/**
 * @details The initialization function for a QEI module sets all registers to their default values
 * as defined in the qei_default_e enumeration. The bit values in each register are then modified
 * according to the specified module's attr struct. Bit values are only changed if they differ from
 * the default values. If the default values are changed at any time this function should be
 * changed as well to make up for the new values.
 */
int qei_init(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Set all registers to default values
    *(module->base_address + QEI_OFFSET_QEIxCON) = QEI_DEFAULT_QEIxCON;
    *(module->base_address + QEI_OFFSET_DFLTxCON) = QEI_DEFAULT_DFLTxCON;
    *(module->base_address + QEI_OFFSET_POSxCNT) = QEI_DEFAULT_POSxCNT;
    *(module->base_address + QEI_OFFSET_MAXxCNT) = QEI_DEFAULT_MAXxCNT;

    // Set the UDSRC bit
    if( module->attr.control_settings & QEI_TIMER_UDSRC_QEB )
    {// Set UDSRC bit to QEBx pin control
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_UDSRC;
    }

    // Set the TQCS bit
    if( module->attr.control_settings & QEI_TIMER_SRC_QEA )
    {// Set TQCS to external clock from QEAx pin
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_TQCS;
    }

    // Set TQCKPS bit
    switch(module->attr.control_settings&QEI_MASK_TIMER_PRE)
    {
    case QEI_TIMER_PRE_8:
        // Prescale 1:8
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_TQCKPS0;
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_TQCKPS1);
        break;
    case QEI_TIMER_PRE_64:
        // Prescale 1:64
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_TQCKPS0);
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_TQCKPS1;
        break;
    case QEI_TIMER_PRE_256:
        // Prescale 1:256
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_TQCKPS;
        break;
    default:
        // Prescale 1:1
        break;
    }

    // Set TQGATE bit
    if( module->attr.control_settings & QEI_TIMER_GATE )
    {// Enable gating
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_TQGATE;
    }

    // Set POSRES bit
    if( module->attr.control_settings & QEI_DECODER_INDEX_RST )
    {// Index pulse resets counter
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_POSRES;
    }

    // Set PCDOUT bit
    if( module->attr.control_settings & QEI_DECODER_UPDN_OUTEN )
    {// Enable direction output on UPDN pin
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_PCDOUT;
    }

    // Set SWPAB bit
    if( module->attr.control_settings & QEI_DECODER_SWAP_AB )
    {// Swap A and B input signals
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_SWPAB;
    }

    // Set QEISIDL bit
    if( module->attr.control_settings & QEI_STOP_IN_IDLE )
    {// Stop QEI module in idle
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_QEISIDL;
    }

    // Set CEID bit
    if( module->attr.control_settings & QEI_CNTERR_INT_DIS )
    {// Disable count error interrupts
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_CEID;
    }

    // Set QECK bits
    switch( module->attr.control_settings & QEI_MASK_FILTER_DIV )
    {
    case QEI_FILTER_DIV_2:
        // Filter divider 1:2
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK0;
        break;
    case QEI_FILTER_DIV_4:
        // Filter divider 1:4
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK1;
        break;
    case QEI_FILTER_DIV_16:
        // Filter divider 1:16
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK1 | QEI_BITMASK_QECK0;
        break;
    case QEI_FILTER_DIV_32:
        // Filter divider 1:32
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK2;
        break;
    case QEI_FILTER_DIV_64:
        // Filter divider 1:64
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK2 | QEI_BITMASK_QECK0;
        break;
    case QEI_FILTER_DIV_128:
        // Filter divider 1:128
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK2 | QEI_BITMASK_QECK1;
        break;
    case QEI_FILTER_DIV_256:
        // Filter divider 1:256
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QECK2 | QEI_BITMASK_QECK1 | QEI_BITMASK_QECK0;
        break;
    default:
        // Filter divider 1:1
        break;
    }

    // Set QEOUT bit
    if( module->attr.control_settings & QEI_FILTER_EN )
    {// Enable digital filter
        *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_QEOUT;
    }

    return QEI_E_NONE;
}

/**
 * @details The value of the IMV bits is dependent on the mode the module is in. If the module is
 * in x4 Quadrature Count Mode with Index Reset then IMV0 corresponds to the required state of
 * input signal A for a match and IMV1 corresponds to the required state of input signal B for a
 * match. If the module is in x2 Quadrature Count Mode with Index Reset then IMV1 selects the input
 * signal to use in the match and IMV0 corresponds to the value required for a match on the
 * selected input signal. If the module is not in either of these modes then ignore the
 * modifications.
 *
 * The parameter @em value is assumed to be a boolean value. Any value other than zero is
 * translated as 1.
 */
int qei_set_index_match(qei_module_t *module,
                        enum qei_input_signal_e input_signal,
                        unsigned int value)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Check which mode the module is in
    if( (*(module->base_address + QEI_OFFSET_QEIxCON) & QEI_BITMASK_QEIM) == (QEI_BITMASK_QEIM2 | QEI_BITMASK_QEIM1) )
    {// x4 Quadrature Count Mode with Index Reset
        // Check which input signal to modify
        if( input_signal == QEI_INPUT_A )
        {// Change input signal A value
            if( value )
            {// Set IMV0
                *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_IMV0;
            }
            else
            {// Clear IMV0
                *(module->base_address + QEI_OFFSET_DFLTxCON) &= QEI_BITMASK_IMV0;
            }
        }
        else if( input_signal == QEI_INPUT_B )
        {// Change input signal B value
            if( value )
            {// Set IMV1
                *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_IMV1;
            }
            else
            {// Clear IMV1
                *(module->base_address + QEI_OFFSET_DFLTxCON) &= QEI_BITMASK_IMV1;
            }
        }
        else
        {// Unknown input signal
            return QEI_E_INPUT;
        }
    }
    else if( (*(module->base_address + QEI_OFFSET_QEIxCON) & QEI_BITMASK_QEIM) == QEI_BITMASK_QEIM2 )
    {// x2 Quadrature Count Mode with Index Reset
        // Check which input signal to use
        if( input_signal == QEI_INPUT_A )
        {// Use input signal A
            // Clear IMV1
            *(module->base_address + QEI_OFFSET_DFLTxCON) &= ~(QEI_BITMASK_IMV1);
        }
        else if( input_signal == QEI_INPUT_B )
        {// Use input signal B
            // Set IMV1
            *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_IMV1;
        }
        else
        {// Unknown input signal
            return QEI_E_INPUT;
        }

        // Set the value of the specified input
        if( value )
        {// Set IMV0
            *(module->base_address + QEI_OFFSET_DFLTxCON) |= QEI_BITMASK_IMV0;
        }
        else
        {// Clear IMV0
            *(module->base_address + QEI_OFFSET_DFLTxCON) &= ~(QEI_BITMASK_IMV0);
        }
    }
    else
    {// Unsupported mode, ignore modifications
        return QEI_E_IGNORE;
    }

    return QEI_E_NONE;
}

/**
 * @details This function first stops the module, so if it is currently running some counts could
 * be missed. Best practice is to manually stop the module before starting it in a new mode.
 *
 * Before stopping the module the function will make sure the requested mode is valid.
 */
int qei_start(qei_module_t *module,
              enum qei_mode_e mode)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }
    
    // Set new mode
    switch( mode )
    {
    case QEI_MODE_OFF:
        // Stop module
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);
        break;
    case QEI_MODE_TIMER:
        // Stop module first
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);
        // Set timer mode
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_QEIM0;
        break;
    case QEI_MODE_x2_INDEX:
        // Stop module first
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);
        // Set x2 Quadrature Count Mode with Index Reset
        *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_QEIM2;
        break;
    case QEI_MODE_x2_MATCH:
        // Stop module first
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);
        // Set x2 Quadrature Count Mode with Match Reset
        *(module->base_address + QEI_OFFSET_QEIxCON) |= (QEI_BITMASK_QEIM2 | QEI_BITMASK_QEIM0);
        break;
    case QEI_MODE_x4_INDEX:
        // Stop module first
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);
        // Set x4 Quadrature Count Mode with Index Reset
        *(module->base_address + QEI_OFFSET_QEIxCON) |= (QEI_BITMASK_QEIM2 | QEI_BITMASK_QEIM1);
        break;
    case QEI_MODE_x4_MATCH:
        // Stop module first
        *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);
        // Set x4 Quadrature Count Mode with Match Reset
        *(module->base_address + QEI_OFFSET_QEIxCON) |= (QEI_BITMASK_QEIM2 | QEI_BITMASK_QEIM1 | QEI_BITMASK_QEIM0);
        break;
    default:
        // Unknown mode
        return QEI_E_INPUT;
    }

    return QEI_E_NONE;
}

/**
 * @details None.
 */
int qei_stop(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Stop module
    *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_QEIM);

    return QEI_E_NONE;
}

/**
 * @details The UPDN bit is only writeable when the QEI module is in Timer mode. Otherwise the
 * change is ignored.
 *
 * The parameter @em updn is assumed to be a boolean value. Any value other than zero is translated
 * as 1.
 */
int qei_set_updn(qei_module_t *module,
                 enum qei_direction_e updn)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Check for Timer mode
    if( (*(module->base_address + QEI_OFFSET_QEIxCON) & QEI_BITMASK_QEIM) == QEI_BITMASK_QEIM0 )
    {// Module is in Timer mode
        if( updn )
        {// Set UPDN bit
            *(module->base_address + QEI_OFFSET_QEIxCON) |= QEI_BITMASK_UPDN;
        }
        else
        {// Clear UPDN bit
            *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_UPDN);
        }
    }
    else
    {// UPDN bit is not writable
        return QEI_E_IGNORE;
    }

    return QEI_E_NONE;
}

/**
 * @details None.
 */
int qei_get_updn(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Return the boolean value of UPDN bit
    return !!(*(module->base_address + QEI_OFFSET_QEIxCON) & QEI_BITMASK_UPDN );
}

/**
 * @details None.
 */
int qei_get_index(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Return the boolean value of INDEX bit
    return !!(*(module->base_address + QEI_OFFSET_QEIxCON) & QEI_BITMASK_INDEX);
}

/**
 * @details None.
 */
int qei_clr_cnterr(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Clear CNTERR bit
    *(module->base_address + QEI_OFFSET_QEIxCON) &= ~(QEI_BITMASK_CNTERR);

    return QEI_E_NONE;
}

/**
 * @details None.
 */
int qei_get_cnterr(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Return the boolean value of CNTERR bit
    return !!(*(module->base_address + QEI_OFFSET_QEIxCON) & QEI_BITMASK_CNTERR);
}

/**
 * @details None.
 */
int qei_set_poscnt(qei_module_t *module,
                   unsigned int poscnt)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Set value of POSxCNT register
    *(module->base_address + QEI_OFFSET_POSxCNT) = poscnt;

    return QEI_E_NONE;
}

/**
 * @details Since this function returns an unsigned int it can't return a negative value as an
 * error type. The only possible error is an invalid module, in which case it will return 0.
 */
unsigned int qei_get_poscnt(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return 0;
    }

    // Return value of POSxCNT
    return *(module->base_address + QEI_OFFSET_POSxCNT);
}

/**
 * @details None.
 */
int qei_set_maxcnt(qei_module_t *module,
                   unsigned int maxcnt)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return QEI_E_MODULE;
    }

    // Set value of MAXxCNT register
    *(module->base_address + QEI_OFFSET_MAXxCNT) = maxcnt;

    return QEI_E_NONE;
}

/**
 * @details Since this function returns an unsigned int it can't return a negative value as an
 * error type. The only possible error is an invalid module, in which case it will return 0.
 */
unsigned int qei_get_maxcnt(qei_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return 0;
    }

    // Return value of MAXxCNT register
    return *(module->base_address + QEI_OFFSET_MAXxCNT);
}

/**
 * @}
 */ // End of QEIModulePrivate group
