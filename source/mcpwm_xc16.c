/* -*- mode: c; tab-width: 4 -*- */
/**
 * @file mcpwm_module.c
 *
 * @brief Private data structures and function definitions for the MCPWM module.
 *
 * @details This file contains the private declarations and data as well as the function
 * definitions for the Motor Control PWM module. This code is specific to the dsPIC33F series of
 * 16-bit DSP microcontrollers. See the specific and family datasheets for more information on
 * a particular functionality provided by this module.
 *
 * All special function register addresses, offsets, and bitmasks are taken from the datasheets.
 *
 * @author Liam Bucci
 * @date 12/12/2012
 * @carlnumber FIRM-0004
 * @version 0.3.0
 */

// Include standard C libraries
#include <stdlib.h>

// Include Microchip peripheral libraries
#include <p33Fxxxx.h>

// Include board information
#include "../def/board.def"

// Include local libraries
#include "../include/bitops.h"

// Include public declarations
#include "../include/mcpwm_module.h"

/**
 * @defgroup MCPWMModulePrivate MCPWM Module Private
 *
 * @details This group contains all the private features of the MCPWM module code. It is meant to
 * document the internal parts of the MCPWM module. Code and variables which should not be seen by
 * an outside user.
 */

/**
 * @brief Offsets for all MCPWM registers.
 *
 * @details This enumeration defines all the special function register offsets (by word) for a
 * Motor Control PWM module with respect to the base_address pointer.
 *
 * @ingroup MCPWMModulePrivate
 */
enum mcpwm_offset_e
{
    MCPWM_OFFSET_PxTCON    = 0x0000, //!< The PxTCON special function register is defined as the base_address.
    MCPWM_OFFSET_PxTMR     = 0x0001,
    MCPWM_OFFSET_PxTPER    = 0x0002,
    MCPWM_OFFSET_PxSECMP   = 0x0003,
    MCPWM_OFFSET_PWMxCON1  = 0x0004,
    MCPWM_OFFSET_PWMxCON2  = 0x0005,
    MCPWM_OFFSET_PxDTCON1  = 0x0006,
    MCPWM_OFFSET_PxDTCON2  = 0x0007,
    MCPWM_OFFSET_PxFLTACON = 0x0008,
    MCPWM_OFFSET_PxFLTBCON = 0x0009,
    MCPWM_OFFSET_PxOVDCON  = 0x000A,
    MCPWM_OFFSET_PxDC1     = 0x000B,
    MCPWM_OFFSET_PxDC2     = 0x000C,
    MCPWM_OFFSET_PxDC3     = 0x000D,
    MCPWM_OFFSET_PxDC4     = 0x000E,
    MCPWM_OFFSET_PWMKEY    = 0x000F
};

/**
 * @brief Bitmasks for all MCPWM register bits.
 *
 * @details This enumeration defines all the bitmasks for the special function registers in a Motor Control
 * PWM module.
 *
 * @ingroup MCPWMModulePrivate
 */
enum mcpwm_bit_mask_e
{
    MCPWM_BITMASK_PTMOD   = 0x0003,
    MCPWM_BITMASK_PTMOD0  = 0x0001,
    MCPWM_BITMASK_PTMOD1  = 0x0002,
    
    MCPWM_BITMASK_PTCKPS  = 0x000C,
    MCPWM_BITMASK_PTCKPS0 = 0x0004,
    MCPWM_BITMASK_PTCKPS1 = 0x0008,

    MCPWM_BITMASK_PTOPS   = 0x00F0,
    MCPWM_BITMASK_PTOPS0  = 0x0010,
    MCPWM_BITMASK_PTOPS1  = 0x0020,
    MCPWM_BITMASK_PTOPS2  = 0x0040,
    MCPWM_BITMASK_PTOPS3  = 0x0080,

    MCPWM_BITMASK_PTSIDL  = 0x2000,

    MCPWM_BITMASK_PTEN    = 0x8000,

    MCPWM_BITMASK_PTMR    = 0x7FFF,
    MCPWM_BITMASK_PTDIR   = 0x8000,

    MCPWM_BITMASK_PTPER   = 0x7FFF,

    MCPWM_BITMASK_SEVTCMP = 0x7FFF,
    MCPWM_BITMASK_SEVTDIR = 0x8000,

    MCPWM_BITMASK_PEN1L   = 0x0001,
    MCPWM_BITMASK_PEN2L   = 0x0002,
    MCPWM_BITMASK_PEN3L   = 0x0004,
    MCPWM_BITMASK_PEN4L   = 0x0008,
    MCPWM_BITMASK_PEN1H   = 0x0010,
    MCPWM_BITMASK_PEN2H   = 0x0020,
    MCPWM_BITMASK_PEN3H   = 0x0040,
    MCPWM_BITMASK_PEN4H   = 0x0080,

    MCPWM_BITMASK_PMOD1   = 0x0100,
    MCPWM_BITMASK_PMOD2   = 0x0200,
    MCPWM_BITMASK_PMOD3   = 0x0400,
    MCPWM_BITMASK_PMOD4   = 0x0800,

    MCPWM_BITMASK_UDIS    = 0x0001,
    MCPWM_BITMASK_OSYNC   = 0x0002,
    MCPWM_BITMASK_IUE     = 0x0004,

    MCPWM_BITMASK_SEVOPS  = 0x0F00,
    MCPWM_BITMASK_SEVOPS0 = 0x0100,
    MCPWM_BITMASK_SEVOPS1 = 0x0200,
    MCPWM_BITMASK_SEVOPS2 = 0x0400,
    MCPWM_BITMASK_SEVOPS3 = 0x0800,

    MCPWM_BITMASK_DTA     = 0x003F,
    MCPWM_BITMASK_DTAPS   = 0x00C0,
    MCPWM_BITMASK_DTAPS0  = 0x0040,
    MCPWM_BITMASK_DTAPS1  = 0x0080,

    MCPWM_BITMASK_DTB     = 0x3F00,
    MCPWM_BITMASK_DTBPS   = 0xC000,
    MCPWM_BITMASK_DTBPS0  = 0x4000,
    MCPWM_BITMASK_DTBPS1  = 0x8000,

    MCPWM_BITMASK_DTS1I   = 0x0001,
    MCPWM_BITMASK_DTS1A   = 0x0002,
    MCPWM_BITMASK_DTS2I   = 0x0004,
    MCPWM_BITMASK_DTS2A   = 0x0008,
    MCPWM_BITMASK_DTS3I   = 0x0010,
    MCPWM_BITMASK_DTS3A   = 0x0020,
    MCPWM_BITMASK_DTS4I   = 0x0040,
    MCPWM_BITMASK_DTS4A   = 0x0080,

    MCPWM_BITMASK_FAEN1   = 0x0001,
    MCPWM_BITMASK_FAEN2   = 0x0002,
    MCPWM_BITMASK_FAEN3   = 0x0004,
    MCPWM_BITMASK_FAEN4   = 0x0008,

    MCPWM_BITMASK_FLTAM   = 0x0080,

    MCPWM_BITMASK_FAOV1L  = 0x0100,
    MCPWM_BITMASK_FAOV1H  = 0x0200,
    MCPWM_BITMASK_FAOV2L  = 0x0400,
    MCPWM_BITMASK_FAOV2H  = 0x0800,
    MCPWM_BITMASK_FAOV3L  = 0x1000,
    MCPWM_BITMASK_FAOV3H  = 0x2000,
    MCPWM_BITMASK_FAOV4L  = 0x4000,
    MCPWM_BITMASK_FAOV4H  = 0x8000,

    MCPWM_BITMASK_FBEN1   = 0x0001,
    MCPWM_BITMASK_FBEN2   = 0x0002,
    MCPWM_BITMASK_FBEN3   = 0x0004,
    MCPWM_BITMASK_FBEN4   = 0x0008,

    MCPWM_BITMASK_FLTBM   = 0x0080,

    MCPWM_BITMASK_FBOV1L  = 0x0100,
    MCPWM_BITMASK_FBOV1H  = 0x0200,
    MCPWM_BITMASK_FBOV2L  = 0x0400,
    MCPWM_BITMASK_FBOV2H  = 0x0800,
    MCPWM_BITMASK_FBOV3L  = 0x1000,
    MCPWM_BITMASK_FBOV3H  = 0x2000,
    MCPWM_BITMASK_FBOV4L  = 0x4000,
    MCPWM_BITMASK_FBOV4H  = 0x8000,

    MCPWM_BITMASK_POUT1L  = 0x0001,
    MCPWM_BITMASK_POUT1H  = 0x0002,
    MCPWM_BITMASK_POUT2L  = 0x0004,
    MCPWM_BITMASK_POUT2H  = 0x0008,
    MCPWM_BITMASK_POUT3L  = 0x0010,
    MCPWM_BITMASK_POUT3H  = 0x0020,
    MCPWM_BITMASK_POUT4L  = 0x0040,
    MCPWM_BITMASK_POUT4H  = 0x0080,

    MCPWM_BITMASK_POVD1L  = 0x0100,
    MCPWM_BITMASK_POVD1H  = 0x0200,
    MCPWM_BITMASK_POVD2L  = 0x0400,
    MCPWM_BITMASK_POVD2H  = 0x0800,
    MCPWM_BITMASK_POVD3L  = 0x1000,
    MCPWM_BITMASK_POVD3H  = 0x2000,
    MCPWM_BITMASK_POVD4L  = 0x4000,
    MCPWM_BITMASK_POVD4H  = 0x8000
};

/**
 * @brief The default values of all registers in a MCPWM module.
 *
 * @details This enumeration defines all the default values of the registers that make up a MCPWM
 * module. Some of the default values are dependent on configuration settings, specifically the
 * PWMxCON1, PxFLTACON, and PxFLTBCON registers. Be sure to read the documentation regarding these
 * registers.
 *
 * @ingroup MCPWMModulePrivate
 */
enum mcpwm_default_e
{
    MCPWM_DEFAULT_PxTCON            = 0x0000, //!< Default value of PxTCON
    MCPWM_DEFAULT_PxTMR             = 0x0000, //!< Default value of PxTMR
    MCPWM_DEFAULT_PxTPER            = 0x0000, //!< Default value of PxTPER
    MCPWM_DEFAULT_PxSECMP           = 0x0000, //!< Default value of PxSECMP
    MCPWM_DEFAULT_PWMxCON1          = 0x00FF, //!< Default value of PWMxCON1 when PWMPIN=0
    MCPWM_DEFAULT_PWMxCON1_PWMPIN   = 0x0000, //!< Default value of PWMxCON1 when PWMPIN=1
    MCPWM_DEFAULT_PWMxCON2          = 0x0000, //!< Default value of PWMxCON2
    MCPWM_DEFAULT_PxDTCON1          = 0x0000, //!< Default value of PxDTCON1
    MCPWM_DEFAULT_PxDTCON2          = 0x0000, //!< Default value of PxDTCON2
    MCPWM_DEFAULT_PxFLTACON         = 0x0000, //!< Default value of PxFLTACON when PWMLOCK is not present
    MCPWM_DEFAULT_PxFLTACON_PWMLOCK = 0x000F, //!< Default value of PxFLTACON when PWMLOCK is present
    MCPWM_DEFAULT_PxFLTBCON         = 0x0000, //!< Default value of PxFLTBCON when PWMLOCK is not present
    MCPWM_DEFAULT_PxFLTBCON_PWMLOCK = 0x000F, //!< Default value of PxFLTBCON when PWMLOCK is present
    MCPWM_DEFAULT_PxOVDCON          = 0xFF00, //!< Default value of PxOVDCON
    MCPWM_DEFAULT_PxDC1             = 0x0000, //!< Default value of PxDC1
    MCPWM_DEFAULT_PxDC2             = 0x0000, //!< Default value of PxDC2
    MCPWM_DEFAULT_PxDC3             = 0x0000, //!< Default value of PxDC3
    MCPWM_DEFAULT_PxDC4             = 0x0000, //!< Default value of PxDC4
    MCPWM_DEFAULT_PWMKEY            = 0x0000  //!< Default value of PWMKEY
};


/**
 * @details No details.
 */
int mcpwm_init(mcpwm_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    // Initialize all settings to default (0x0000)
    *(module->base_address + MCPWM_OFFSET_PxTCON) = 0x0000;
    *(module->base_address + MCPWM_OFFSET_PWMxCON1) &= ~(MCPWM_BITMASK_PMOD1 | MCPWM_BITMASK_PMOD2 | MCPWM_BITMASK_PMOD3 | MCPWM_BITMASK_PMOD4);
    *(module->base_address + MCPWM_OFFSET_PWMxCON2) &= ~(MCPWM_BITMASK_SEVOPS | MCPWM_BITMASK_IUE | MCPWM_BITMASK_OSYNC);
    *(module->base_address + MCPWM_OFFSET_PxDTCON1) &= ~(MCPWM_BITMASK_DTAPS | MCPWM_BITMASK_DTBPS);

    switch(module->attr.clock_settings & MCPWM_MASK_TIMEBASE_MODE)
    {
    case MCPWM_TIMEBASE_FREE_RUNNING:
        // Set up time base in free running mode
        *(module->base_address + MCPWM_OFFSET_PxTCON) &= ~(MCPWM_BITMASK_PTMOD);
        break;
        //! @todo Add all modes
    default:
        // Unknown time base mode
        return MCPWM_E_INPUT;
    }

    switch(module->attr.clock_settings & MCPWM_MASK_TIMEBASE_PRESCALE)
    {
    case MCPWM_TIMEBASE_PRESCALE_1:
        // Time base prescale 1:1
        *(module->base_address + MCPWM_OFFSET_PxTCON) &= ~(MCPWM_BITMASK_PTCKPS);
        break;
    default:
        // Unknown time base input prescale
        return MCPWM_E_INPUT;
    }
    
    switch(module->attr.clock_settings & MCPWM_MASK_TIMEBASE_POSTSCALE)
    {
    case MCPWM_TIMEBASE_POSTSCALE_1:
        // Time base postscale 1:1
        *(module->base_address + MCPWM_OFFSET_PxTCON) &= ~(MCPWM_BITMASK_PTOPS);
        break;
    default:
        // Unknown time base output postscale
        return MCPWM_E_INPUT;
    }




    if(module->attr.control_settings & MCPWM_PINMODE_P1_IND)
    {// Set pin pair 1 to independent
        *(module->base_address + MCPWM_OFFSET_PWMxCON1) |= MCPWM_BITMASK_PMOD1;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_timebase(mcpwm_module_t *module,
                       mcpwm_timebase_t timebase)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    *(module->base_address + MCPWM_OFFSET_PxTMR) = timebase.value;
    
    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_get_timebase(mcpwm_module_t *module,
                       mcpwm_timebase_t *timebase)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    // Check for valid output 
    
    timebase->uint = *(module->base_address + MCPWM_OFFSET_PxTMR);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_period(mcpwm_module_t *module,
                     mcpwm_timebase_t period)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    *(module->base_address + MCPWM_OFFSET_PxTPER) = period.value;

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_get_period(mcpwm_module_t *module,
                     mcpwm_timebase_t *period)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    period->uint = *(module->base_address + MCPWM_OFFSET_PxTPER);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_spevt(mcpwm_module_t *module,
                    mcpwm_timebase_t spevt)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    *(module->base_address + MCPWM_OFFSET_PxSECMP) = spevt.uint;

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_get_spevt(mcpwm_module_t *module,
                    mcpwm_timebase_t *spevt)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    spevt->uint = *(module->base_address + MCPWM_OFFSET_PxSECMP);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_update_lock(mcpwm_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    // Check if MCPWM_IMMEDIATE is set
    if( *(module->base_address + MCPWM_OFFSET_PWMxCON2) & MCPWM_BITMASK_IUE )
    {// MCPWM_IMMEDIATE is set, ignore and return error
        return MCPWM_E_IGNORE;
    }

    // Set update lock bit
    *(module->base_address + MCPWM_OFFSET_PWMxCON2) |= MCPWM_BITMASK_UDIS;

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_update_unlock(mcpwm_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }
    
    // Check if MCPWM_IMMEDIATE is set
    if( *(module->base_address + MCPWM_OFFSET_PWMxCON2) & MCPWM_BITMASK_IUE )
    {// MCPWM_IMMEDIATE is set, ignore and return error
        return MCPWM_E_IGNORE;
    }

    // Set update lock bit
    *(module->base_address + MCPWM_OFFSET_PWMxCON2) &= ~(MCPWM_BITMASK_UDIS);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_enable_pins(mcpwm_module_t *module,
                      unsigned int pins)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    *(module->base_address + MCPWM_OFFSET_PWMxCON1) = BITS_INSERT(*(module->base_address + MCPWM_OFFSET_PWMxCON1),0x00FF,pins);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_disable_pins(mcpwm_module_t *module,
                       unsigned int pins)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    *(module->base_address + MCPWM_OFFSET_PWMxCON1) = ~(pins&0x00FF);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_deadtime(mcpwm_module_t *module,
                       enum mcpwm_deadtime_unit_e unit,
                       unsigned int value)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( unit == MCPWM_DEADTIME_UNITA )
    {// Set DTA to supplied value
        *(module->base_address + MCPWM_OFFSET_PxDTCON1) = BITS_INSERT(*(module->base_address + MCPWM_OFFSET_PxDTCON1),MCPWM_BITMASK_DTA,value);
    }
    else if( unit == MCPWM_DEADTIME_UNITB )
    {// Set DTB to supplied value
        *(module->base_address + MCPWM_OFFSET_PxDTCON1) = BITS_INSERT(*(module->base_address + MCPWM_OFFSET_PxDTCON1),MCPWM_BITMASK_DTB,value);
    }
    else
    {// Unknown unit
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_get_deadtime(mcpwm_module_t *module,
                       enum mcpwm_deadtime_unit_e unit)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( unit == MCPWM_DEADTIME_UNITA )
    {// Get DTA
        return (*(module->base_address + MCPWM_OFFSET_PxDTCON1) & MCPWM_BITMASK_DTA);
    }
    else if( unit == MCPWM_DEADTIME_UNITB )
    {// Get DTB
        return (*(module->base_address + MCPWM_OFFSET_PxDTCON1) & MCPWM_BITMASK_DTB)>>8;
    }
    else
    {// Unknown unit
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_deadtime_select(mcpwm_module_t *module,
                          enum mcpwm_pin_pair_e pin_pair,
                          enum mcpwm_deadtime_signal_dir_e signal_dir,
                          enum mcpwm_deadtime_unit_e unit)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( pin_pair == MCPWM_P1 )
    {// Pin Pair 1
        if( signal_dir == MCPWM_DEADTIME_GOING_INACTIVE )
        {// Going inactive
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS1I);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS1I;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else if( signal_dir == MCPWM_DEADTIME_GOING_ACTIVE )
        {// Going active
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS1A);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS1A;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else
        {// Unknown signal_dir
            return MCPWM_E_INPUT;
        }
    }
    else if( pin_pair == MCPWM_P2 )
    {// Pin Pair 2
        if( signal_dir == MCPWM_DEADTIME_GOING_INACTIVE )
        {// Going inactive
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS2I);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS2I;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else if( signal_dir == MCPWM_DEADTIME_GOING_ACTIVE )
        {// Going active
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS2A);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS2A;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else
        {// Unknown signal_dir
            return MCPWM_E_INPUT;
        }
    }
    else if( pin_pair == MCPWM_P3 )
    {// Pin Pair 1
        if( signal_dir == MCPWM_DEADTIME_GOING_INACTIVE )
        {// Going inactive
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS3I);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS3I;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else if( signal_dir == MCPWM_DEADTIME_GOING_ACTIVE )
        {// Going active
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS3A);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS3A;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else
        {// Unknown signal_dir
            return MCPWM_E_INPUT;
        }
    }
    else if( pin_pair == MCPWM_P4 )
    {// Pin Pair 1
        if( signal_dir == MCPWM_DEADTIME_GOING_INACTIVE )
        {// Going inactive
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS4I);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS4I;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else if( signal_dir == MCPWM_DEADTIME_GOING_ACTIVE )
        {// Going active
            if( unit == MCPWM_DEADTIME_UNITA )
            {// Select unit A
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) &= ~(MCPWM_BITMASK_DTS4A);
            }
            else if( unit == MCPWM_DEADTIME_UNITB )
            {// Select unit B
                *(module->base_address + MCPWM_OFFSET_PxDTCON2) |= MCPWM_BITMASK_DTS4A;
            }
            else
            {// Unknown unit
                return MCPWM_E_INPUT;
            }
        }
        else
        {// Unknown signal_dir
            return MCPWM_E_INPUT;
        }
    }
    else
    {// Unknown pin pair
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_fault_override(mcpwm_module_t *module,
                             enum mcpwm_fault_input_e fault_input,
                             enum mcpwm_pin_e pin,
                             enum mcpwm_override_value_e override_value)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( fault_input == MCPWM_FAULTA )
    {// Fault A input
        if( pin == MCPWM_P1L )
        {// Pin 1L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV1L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV1L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P1H )
        {// Pin 1H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV1H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV1H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P2L )
        {// Pin 2L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV2L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV2L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P2H )
        {// Pin 2H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV2H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV2H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P3L )
        {// Pin 3L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV3L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV3L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P3H )
        {// Pin 3H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV3H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV3H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P4L )
        {// Pin 4L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV4L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV4L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P4H )
        {// Pin 4H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAOV4H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAOV4H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else
        {// Unknown pin
            return MCPWM_E_INPUT;
        }
    }
    else if( fault_input == MCPWM_FAULTB )
    {// Fault B input
        if( pin == MCPWM_P1L )
        {// Pin 1L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV1L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV1L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P1H )
        {// Pin 1H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV1H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV1H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P2L )
        {// Pin 2L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV2L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV2L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P2H )
        {// Pin 2H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV2H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV2H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P3L )
        {// Pin 3L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV3L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV3L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P3H )
        {// Pin 3H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV3H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV3H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P4L )
        {// Pin 4L
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV4L);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV4L;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else if( pin == MCPWM_P4H )
        {// Pin 4H
            if( override_value == MCPWM_OVERRIDE_INACTIVE )
            {// Override inactive
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBOV4H);
            }
            else if( override_value == MCPWM_OVERRIDE_ACTIVE )
            {// Override active
                *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBOV4H;
            }
            else
            {// Unknown override value
                return MCPWM_E_INPUT;
            }
        }
        else
        {// Unknown pin
            return MCPWM_E_INPUT;
        }
    }
    else
    {// Unknown fault input
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_enable_fault(mcpwm_module_t *module,
                       enum mcpwm_fault_input_e fault_input,
                       enum mcpwm_pin_pair_e pin_pair)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( fault_input == MCPWM_FAULTA )
    {// Fault A input
        switch(pin_pair)
        {
        case MCPWM_P1:
            // Pin Pair 1
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAEN1;
            break;
        case MCPWM_P2:
            // Pin Pair 2
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAEN2;
            break;
        case MCPWM_P3:
            // Pin Pair 3
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAEN3;
            break;
        case MCPWM_P4:
            // Pin Pair 4
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) |= MCPWM_BITMASK_FAEN4;
            break;
        default:
            // Unknown pin pair
            return MCPWM_E_INPUT;
        }
    }
    else if( fault_input == MCPWM_FAULTB )
    {// Fault B input
        switch(pin_pair)
        {
        case MCPWM_P1:
            // Pin Pair 1
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBEN1;
            break;
        case MCPWM_P2:
            // Pin Pair 2
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBEN2;
            break;
        case MCPWM_P3:
            // Pin Pair 3
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBEN3;
            break;
        case MCPWM_P4:
            // Pin Pair 4
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) |= MCPWM_BITMASK_FBEN4;
            break;
        default:
            // Unknown pin pair
            return MCPWM_E_INPUT;
        }
    }
    else
    {// Unknown fault input
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_disable_fault(mcpwm_module_t *module,
                        enum mcpwm_fault_input_e fault_input,
                        enum mcpwm_pin_pair_e pin_pair)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( fault_input == MCPWM_FAULTA )
    {// Fault A input
        switch(pin_pair)
        {
        case MCPWM_P1:
            // Pin Pair 1
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAEN1);
            break;
        case MCPWM_P2:
            // Pin Pair 2
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAEN2);
            break;
        case MCPWM_P3:
            // Pin Pair 3
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAEN3);
            break;
        case MCPWM_P4:
            // Pin Pair 4
            *(module->base_address + MCPWM_OFFSET_PxFLTACON) &= ~(MCPWM_BITMASK_FAEN4);
            break;
        default:
            // Unknown pin pair
            return MCPWM_E_INPUT;
        }
    }
    else if( fault_input == MCPWM_FAULTB )
    {// Fault B input
        switch(pin_pair)
        {
        case MCPWM_P1:
            // Pin Pair 1
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBEN1);
            break;
        case MCPWM_P2:
            // Pin Pair 2
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBEN2);
            break;
        case MCPWM_P3:
            // Pin Pair 3
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBEN3);
            break;
        case MCPWM_P4:
            // Pin Pair 4
            *(module->base_address + MCPWM_OFFSET_PxFLTBCON) &= ~(MCPWM_BITMASK_FBEN4);
            break;
        default:
            // Unknown pin pair
            return MCPWM_E_INPUT;
        }
    }
    else
    {// Unknown fault input
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_output_override(mcpwm_module_t *module,
                              enum mcpwm_pin_e pin,
                              enum mcpwm_override_value_e override_value)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    switch(pin)
    {
    case MCPWM_P1L:
        // Pin 1L
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT1L);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT1L;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P1H:
        // Pin 1H
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT1H);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT1H;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P2L:
        // Pin 2L
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT2L);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT2L;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P2H:
        // Pin 2H
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT2H);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT2H;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P3L:
        // Pin 3L
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT3L);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT3L;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P3H:
        // Pin 3H
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT3H);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT3H;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P4L:
        // Pin 4L
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT4L);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT4L;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    case MCPWM_P4H:
        // Pin 4H
        if( override_value == MCPWM_OVERRIDE_INACTIVE )
        {// Override inactive
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POUT4H);
        }
        else if( override_value == MCPWM_OVERRIDE_ACTIVE )
        {// Override active
            *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POUT4H;
        }
        else
        {// Unknown override value
            return MCPWM_E_INPUT;
        }
        break;
    default:
        // Unknown pin
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_enable_output_override(mcpwm_module_t *module,
                                 int pins)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( pins & MCPWM_P1L )
    {// Enable output override on pin 1L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD1L);
    }

    if( pins & MCPWM_P1H )
    {// Enable output override on pin 1H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD1H);
    }

    if( pins & MCPWM_P2L )
    {// Enable output override on pin 2L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD2L);
    }

    if( pins & MCPWM_P2H )
    {// Enable output override on pin 2H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD2H);
    }

    if( pins & MCPWM_P3L )
    {// Enable output override on pin 3L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD3L);
    }

    if( pins & MCPWM_P3H )
    {// Enable output override on pin 3H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD3H);
    }

    if( pins & MCPWM_P4L )
    {// Enable output override on pin 4L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD4L);
    }

    if( pins & MCPWM_P4H )
    {// Enable output override on pin 4H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) &= ~(MCPWM_BITMASK_POVD4H);
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_disable_output_override(mcpwm_module_t *module,
                                  int pins)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    if( pins & MCPWM_P1L )
    {// Disable output override on pin 1L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD1L;
    }

    if( pins & MCPWM_P1H )
    {// Disable output override on pin 1H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD1H;
    }

    if( pins & MCPWM_P2L )
    {// Disable output override on pin 2L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD2L;
    }

    if( pins & MCPWM_P2H )
    {// Disable output override on pin 2H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD2H;
    }

    if( pins & MCPWM_P3L )
    {// Disable output override on pin 3L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD3L;
    }

    if( pins & MCPWM_P3H )
    {// Disable output override on pin 3H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD3H;
    }

    if( pins & MCPWM_P4L )
    {// Disable output override on pin 4L
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD4L;
    }

    if( pins & MCPWM_P4H )
    {// Disable output override on pin 4H
        *(module->base_address + MCPWM_OFFSET_PxOVDCON) |= MCPWM_BITMASK_POVD4H;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_start(mcpwm_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    // Enable time base
    *(module->base_address + MCPWM_OFFSET_PxTCON) |= MCPWM_BITMASK_PTEN;

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_stop(mcpwm_module_t *module)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    // Disable time base
    *(module->base_address + MCPWM_OFFSET_PxTCON) &= ~(MCPWM_BITMASK_PTEN);

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_set_duty_cycle(mcpwm_module_t *module,
                         enum mcpwm_duty_cycle_register_e duty_cycle_register,
                         unsigned int duty_cycle)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    switch(duty_cycle_register)
    {
    case MCPWM_DUTY_CYCLE_1:
        // Set duty cycle register 1
        *(module->base_address + MCPWM_OFFSET_PxDC1) = duty_cycle;
        break;
    case MCPWM_DUTY_CYCLE_2:
        // Set duty cycle register 2
        *(module->base_address + MCPWM_OFFSET_PxDC2) = duty_cycle;
        break;
    case MCPWM_DUTY_CYCLE_3:
        // Set duty cycle register 3
        *(module->base_address + MCPWM_OFFSET_PxDC3) = duty_cycle;
        break;
    case MCPWM_DUTY_CYCLE_4:
        // Set duty cycle register 4
        *(module->base_address + MCPWM_OFFSET_PxDC4) = duty_cycle;
        break;
    default:
        // Unknown duty cycle register
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}

/**
 * @details No details.
 */
int mcpwm_get_duty_cycle(mcpwm_module_t *module,
                         enum mcpwm_duty_cycle_register_e duty_cycle_register,
                         unsigned int *duty_cycle)
{
    // Check for valid module
    if( module == NULL || module->base_address == NULL )
    {// Invalid module
        return MCPWM_E_MODULE;
    }

    // Check for valid output pointer
    if( duty_cycle == NULL )
    {// Invalid output pointer
        return MCPWM_E_OUTPUT;
    }

    switch(duty_cycle_register)
    {
    case MCPWM_DUTY_CYCLE_1:
        // Set duty cycle register 1
        *(duty_cycle) = *(module->base_address + MCPWM_OFFSET_PxDC1);
        break;
    case MCPWM_DUTY_CYCLE_2:
        // Set duty cycle register 2
        *(duty_cycle) = *(module->base_address + MCPWM_OFFSET_PxDC2);
        break;
    case MCPWM_DUTY_CYCLE_3:
        // Set duty cycle register 3
        *(duty_cycle) = *(module->base_address + MCPWM_OFFSET_PxDC3);
        break;
    case MCPWM_DUTY_CYCLE_4:
        // Set duty cycle register 4
        *(duty_cycle) = *(module->base_address + MCPWM_OFFSET_PxDC4);
        break;
    default:
        // Unknown duty cycle register
        return MCPWM_E_INPUT;
    }

    return MCPWM_E_NONE;
}
