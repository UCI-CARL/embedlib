/* -*- mode: c; tab-width: 4 -*- */
/**
 * @file mcpwm_module.h
 *
 * @brief Public data structures and function declarations for the MCPWM module.
 *
 * @details This file contains the public declarations for the Motor Control PWM module. Any
 * publicly available functions and data structures are defined herein. The code in this file is
 * meant to be viewable and accessible to a user.
 *
 * This code is specific to the dsPIC33F series of 16-bit DSP microcontrollers. The code follows
 * the specifications in the related datasheets fairly tightly, so any question on specific
 * functionality should be directed toward the part specific and/or family datasheets.
 *
 * @author Liam Bucci
 * @date 12/12/2012
 * @carlnumber FIRM-0004
 * @version 0.3.0
 */

// Include guard
#ifndef _MCPWM_MODULE_H
#define _MCPWM_MODULE_H

/**
 * @defgroup MCPWMModulePublic MCPWM Module Public
 *
 * @details This group is meant to document all the public, user-accessible features of the MCPWM
 * module. Any data structure or function documented in this group is accessible by inclusion of
 * the mcpwm_module.h file.
 *
 * @{
 */

/**
 * @brief The attribute struct for a MCPWM module.
 *
 * @details This struct contains all the attribute data for a Motor Control PWM module. This data
 * is used to initialize the module and is meant to be fairly static within the lifetime of the
 * module. Any changes to this data should require a reinitialization of the module.
 */
typedef struct mcpwm_attr_s
{
    unsigned int clock_settings;
    unsigned int control_settings;
} mcpwm_attr_t;

/**
 * @brief A helper union for timebase values.
 *
 * @details This union is a helper for setting/getting time base related values. It allows for
 * easier access to the 15-bit value and 1-bit direction stored in timebase related registers as
 * well as giving access to the entire 16-bits as an unsigned int.
 */
typedef union mcpwm_timebase_u
{
    unsigned int uint;
    struct
    {
        unsigned int value :15;
        unsigned int direction :1;
    };
} mcpwm_timebase_t;

/**
 * @brief The Motor Control PWM module definition.
 *
 * @details This struct contains all the data relating to an instance of a Motor Control PWM
 * module. The base_address pointer should point to the PxTCON register of the hardware module
 * which should be controlled. The attr struct should hold the settings and configurations with
 * which the module was (or should be) initialized.
 */
typedef struct mcpwm_module_s
{
    volatile unsigned int *base_address;
    mcpwm_attr_t attr;
} mcpwm_module_t;

/**
 * @brief The possible return values of all MCPWM module functions.
 *
 * @details This enumeration defines the possible error and return messages of a Motor Control PWM
 * module function.
 */
enum mcpwm_error_e
{
    MCPWM_E_NONE    = 0,        //!< No error
    MCPWM_E_MODULE  = -1,       //!< Invalid module or base_address pointer
    MCPWM_E_INPUT   = -2,       //!< Invalid input pointer
    MCPWM_E_OUTPUT  = -3,       //!< Invalid output pointer
    MCPWM_E_IGNORE  = -4,       //!< Ignored due to incompatible or invalid settings
    MCPWM_E_UNKNOWN = 0x8000    //!< Unknown error
};

/**
 * @brief The clock settings for a MCPWM module.
 *
 * @details This enumeration defines the valid clock settings for a Motor Control PWM module. The
 * settings are meant to be bitwise OR'd together to form the clock_settings variable in the attr
 * struct of an mcpwm_module_t. Only one value may be used per type of setting, so for example only
 * one Time Base Mode setting may be used. If more than one is used unexpected results could occur.
 *
 *
 * Setting                  Bit(s)
 * ----------------------------------------------
 * Time Base Mode           0b0000 0000 0000 00xx
 *   Free Running           0b0000 0000 0000 0000
 *   Single Event           0b0000 0000 0000 0001
 *   Continuous             0b0000 0000 0000 0010
 *   Continuous w/ Dbl Int  0b0000 0000 0000 0011
 *
 * Time Base Prescale       0b0000 0000 0000 xx00
 *   1:1 Prescale           0b0000 0000 0000 0000
 *   1:4 Prescale           0b0000 0000 0000 0100
 *   1:16 Prescale          0b0000 0000 0000 1000
 *   1:64 Prescale          0b0000 0000 0000 1100
 *
 * Time Base Postscale      0b0000 0000 xxxx 0000
 *   1:1 Postscale          0b0000 0000 0000 0000
 *   1:2 Postscale          0b0000 0000 0001 0000
 *   1:3 Postscale          0b0000 0000 0010 0000
 *   1:4 Postscale          0b0000 0000 0011 0000
 *   1:5 Postscale          0b0000 0000 0100 0000
 *   1:6 Postscale          0b0000 0000 0101 0000
 *   1:7 Postscale          0b0000 0000 0110 0000
 *   1:8 Postscale          0b0000 0000 0111 0000
 *   1:9 Postscale          0b0000 0000 1000 0000
 *   1:10 Postscale         0b0000 0000 1001 0000
 *   1:11 Postscale         0b0000 0000 1010 0000
 *   1:12 Postscale         0b0000 0000 1011 0000
 *   1:13 Postscale         0b0000 0000 1100 0000
 *   1:14 Postscale         0b0000 0000 1101 0000
 *   1:15 Postscale         0b0000 0000 1110 0000
 *   1:16 Postscale         0b0000 0000 1111 0000
 *
 * Special Event Postscale  0b0000 xxxx 0000 0000
 *   1:1 Postscale          0b0000 0000 0000 0000
 *   1:2 Postscale          0b0000 0001 0000 0000
 *   1:3 Postscale          0b0000 0010 0000 0000
 *   1:4 Postscale          0b0000 0011 0000 0000
 *   1:5 Postscale          0b0000 0100 0000 0000
 *   1:6 Postscale          0b0000 0101 0000 0000
 *   1:7 Postscale          0b0000 0110 0000 0000
 *   1:8 Postscale          0b0000 0111 0000 0000
 *   1:9 Postscale          0b0000 1000 0000 0000
 *   1:10 Postscale         0b0000 1001 0000 0000
 *   1:11 Postscale         0b0000 1010 0000 0000
 *   1:12 Postscale         0b0000 1011 0000 0000
 *   1:13 Postscale         0b0000 1100 0000 0000
 *   1:14 Postscale         0b0000 1101 0000 0000
 *   1:15 Postscale         0b0000 1110 0000 0000
 *   1:16 Postscale         0b0000 1111 0000 0000
 *
 * Dead Time A Prescale     0b00xx 0000 0000 0000
 *   1:1 Prescale           0b0000 0000 0000 0000
 *   1:2 Prescale           0b0001 0000 0000 0000
 *   1:4 Prescale           0b0010 0000 0000 0000
 *   1:8 Prescale           0b0011 0000 0000 0000
 *
 * Dead Time B Prescale     0bxx00 0000 0000 0000
 *   1:1 Prescale           0b0000 0000 0000 0000
 *   1:2 Prescale           0b0100 0000 0000 0000
 *   1:4 Prescale           0b1000 0000 0000 0000
 *   1:8 Prescale           0b1100 0000 0000 0000
 */
enum mcpwm_clock_settings_e
{
    //! Time Base Modes
    MCPWM_MASK_TIMEBASE_MODE         = 0x0003,
    MCPWM_TIMEBASE_FREE_RUNNING      = 0x0000,
    MCPWM_TIMEBASE_SINGLE_EVENT      = 0x0001,
    MCPWM_TIMEBASE_CONTINUOUS        = 0x0002,
    MCPWM_TIMEBASE_CONTINUOUS_DBLINT = 0x0003,
    
    //! @par Time Base Prescale Ratios
    MCPWM_MASK_TIMEBASE_PRESCALE     = 0x000C,
    MCPWM_TIMEBASE_PRESCALE_1        = 0x0000,
    MCPWM_TIMEBASE_PRESCALE_4        = 0x0004,
    MCPWM_TIMEBASE_PRESCALE_16       = 0x0008,
    MCPWM_TIMEBASE_PRESCALE_64       = 0x000C,

    //! Time Base Postscale Ratios
    MCPWM_MASK_TIMEBASE_POSTSCALE    = 0x00F0,
    MCPWM_TIMEBASE_POSTSCALE_1       = 0x0000,
    MCPWM_TIMEBASE_POSTSCALE_2       = 0x0010,
    MCPWM_TIMEBASE_POSTSCALE_3       = 0x0020,
    MCPWM_TIMEBASE_POSTSCALE_4       = 0x0030,
    MCPWM_TIMEBASE_POSTSCALE_5       = 0x0040,
    MCPWM_TIMEBASE_POSTSCALE_6       = 0x0050,
    MCPWM_TIMEBASE_POSTSCALE_7       = 0x0060,
    MCPWM_TIMEBASE_POSTSCALE_8       = 0x0070,
    MCPWM_TIMEBASE_POSTSCALE_9       = 0x0080,
    MCPWM_TIMEBASE_POSTSCALE_10      = 0x0090,
    MCPWM_TIMEBASE_POSTSCALE_11      = 0x00A0,
    MCPWM_TIMEBASE_POSTSCALE_12      = 0x00B0,
    MCPWM_TIMEBASE_POSTSCALE_13      = 0x00C0,
    MCPWM_TIMEBASE_POSTSCALE_14      = 0x00D0,
    MCPWM_TIMEBASE_POSTSCALE_15      = 0x00E0,
    MCPWM_TIMEBASE_POSTSCALE_16      = 0x00F0,

    //! Special Event Postscale Ratios
    MCPWM_MASK_SPEVT_POSTSCALE       = 0x0F00,
    MCPWM_SPEVT_POSTSCALE_1          = 0x0000,
    MCPWM_SPEVT_POSTSCALE_2          = 0x0100,
    MCPWM_SPEVT_POSTSCALE_3          = 0x0200,
    MCPWM_SPEVT_POSTSCALE_4          = 0x0300,
    MCPWM_SPEVT_POSTSCALE_5          = 0x0400,
    MCPWM_SPEVT_POSTSCALE_6          = 0x0500,
    MCPWM_SPEVT_POSTSCALE_7          = 0x0600,
    MCPWM_SPEVT_POSTSCALE_8          = 0x0700,
    MCPWM_SPEVT_POSTSCALE_9          = 0x0800,
    MCPWM_SPEVT_POSTSCALE_10         = 0x0900,
    MCPWM_SPEVT_POSTSCALE_11         = 0x0A00,
    MCPWM_SPEVT_POSTSCALE_12         = 0x0B00,
    MCPWM_SPEVT_POSTSCALE_13         = 0x0C00,
    MCPWM_SPEVT_POSTSCALE_14         = 0x0D00,
    MCPWM_SPEVT_POSTSCALE_15         = 0x0E00,
    MCPWM_SPEVT_POSTSCALE_16         = 0x0F00,

    //! Dead Time Unit A Prescale Ratios
    MCPWM_MASK_DEADTIMEA_PRESCALE    = 0x3000,
    MCPWM_DEADTIMEA_PRESCALE_1       = 0x0000,
    MCPWM_DEADTIMEA_PRESCALE_2       = 0x1000,
    MCPWM_DEADTIMEA_PRESCALE_4       = 0x2000,
    MCPWM_DEADTIMEA_PRESCALE_8       = 0x3000,

    //! Dead Time Unit B Prescale Ratios
    MCPWM_MASK_DEADTIMEB_PRESCALE    = 0xC000,
    MCPWM_DEADTIMEB_PRESCALE_1       = 0x0000,
    MCPWM_DEADTIMEB_PRESCALE_2       = 0x4000,
    MCPWM_DEADTIMEB_PRESCALE_4       = 0x8000,
    MCPWM_DEADTIMEB_PRESCALE_8       = 0xC000,
};

/**
 * This enumeration defines all the valid control settings for a Motor Control PWM module. These
 * values should be used to set the control_settings variable in the mcpwm_attr_s struct.
 *
 * Setting                  Bit(s)
 * ----------------------------------------------
 * I/O Pin Pair Mode        0b0000 0000 0000 xxxx
 *   Pair 1 Complementary   0b0000 0000 0000 0000
 *   Pair 1 Independent     0b0000 0000 0000 0001
 *   Pair 2 Complementary   0b0000 0000 0000 0000
 *   Pair 2 Independent     0b0000 0000 0000 0010
 *   Pair 3 Complementary   0b0000 0000 0000 0000
 *   Pair 3 Independent     0b0000 0000 0000 0100
 *   Pair 4 Complementary   0b0000 0000 0000 0000
 *   Pair 4 Independent     0b0000 0000 0000 1000
 *
 * Immediate Update         0b0000 0000 0001 0000
 *
 * Output Override Sync     0b0000 0000 0010 0000
 *
 * Fault A Mode             0b0000 0000 0x00 0000
 *   Latch                  0b0000 0000 0000 0000
 *   Cycle-by-Cycle         0b0000 0000 0100 0000
 *
 * Fault B Mode             0b0000 0000 x000 0000
 *   Latch                  0b0000 0000 0000 0000
 *   Cycle-by-Cycle         0b0000 0000 1000 0000
 *
 * Stop in Idle             0b0000 0001 0000 0000
 */
enum mcpwm_control_settings_e
{
    MCPWM_PINMODE_P1_COMP = 0x0000,
    MCPWM_PINMODE_P1_IND  = 0x0001,
    MCPWM_PINMODE_P2_COMP = 0x0000,
    MCPWM_PINMODE_P2_IND  = 0x0002,
    MCPWM_PINMODE_P3_COMP = 0x0000,
    MCPWM_PINMODE_P3_IND  = 0x0004,
    MCPWM_PINMODE_P4_COMP = 0x0000,
    MCPWM_PINMODE_P4_IND  = 0x0008,

    MCPWM_IMMEDIATE       = 0x0010,

    MCPWM_OOSYNC          = 0x0020,

    MCPWM_FLTA_LATCH      = 0x0000,
    MCPWM_FLTA_CYCLE      = 0x0040,

    MCPWM_FLTB_LATCH      = 0x0000,
    MCPWM_FLTB_CYCLE      = 0x0080,
    
    MCPWM_STOP_IN_IDLE    = 0x0100
};

/**
 * This enumeration defines the values which can be used in time base counting direction bits.
 */
enum mcpwm_timebase_dir_e
{
    MCPWM_TIMEBASE_UP   = 0x0000,
    MCPWM_TIMEBASE_DOWN = 0x0001
};

/**
 * This enumeration defines values which specify valid pin pairs in a Motor Control PWM module.
 */
enum mcpwm_pin_pair_e
{
    MCPWM_P1 = 0x0000,
    MCPWM_P2 = 0x0001,
    MCPWM_P3 = 0x0002,
    MCPWM_P4 = 0x0003
};

/**
 * This enumeration defines the values used to enable/disable pins using the mcpwm_enable_pins()
 * and mcpwm_disable_pins() functions. These values must be bitwise OR independent.
 */
enum mcpwm_pin_e
{
    MCPWM_P1L = 0x0001,
    MCPWM_P2L = 0x0002,
    MCPWM_P3L = 0x0004,
    MCPWM_P4L = 0x0008,
    MCPWM_P1H = 0x0010,
    MCPWM_P2H = 0x0020,
    MCPWM_P3H = 0x0040,
    MCPWM_P4H = 0x0080
};

/**
 * This enumeration defines the values used to select a dead time unit for each pin pair. The
 * values should be used in the mcpwm_deadtime_select() function.
 */
enum mcpwm_deadtime_signal_dir_e
{
    MCPWM_DEADTIME_GOING_INACTIVE = 0x0000,
    MCPWM_DEADTIME_GOING_ACTIVE   = 0x0001
};

/**
 * This enumeration defines the valid dead time units for a Motor Control PWM module. The values
 * should be used in the mcpwm_deadtime_select() function.
 */
enum mcpwm_deadtime_unit_e
{
    MCPWM_DEADTIME_UNITA = 0x0000,
    MCPWM_DEADTIME_UNITB = 0x0001
};

/**
 * This enumeration defines the valid fault inputs for a Motor Control PWM module.
 */
enum mcpwm_fault_input_e
{
    MCPWM_FAULTA = 0x0000,
    MCPWM_FAULTB = 0x0001
};

/**
 * This enumeration defines all valid override values for a Motor Control PWM module.
 */
enum mcpwm_override_value_e
{
    MCPWM_OVERRIDE_INACTIVE = 0x0000,
    MCPWM_OVERRIDE_ACTIVE   = 0x0001
};

/**
 * This enumeration defines the valid duty cycle registers for a Motor Control PWM module.
 */
enum mcpwm_duty_cycle_register_e
{
    MCPWM_DUTY_CYCLE_1 = 0x0000,
    MCPWM_DUTY_CYCLE_2 = 0x0001,
    MCPWM_DUTY_CYCLE_3 = 0x0002,
    MCPWM_DUTY_CYCLE_4 = 0x0003
};

/**
 * This function initializes the given Motor Control PWM module using the mcpwm_attr_s struct
 * contained in the module. It only sets the values given in attr, it doesn't start the time base
 * clock or set any values which can be modified with other module functions.
 *
 * @param[in]  module
 *             A pointer to the module to be initialized.
 *
 * @return If initialization was successful a zero is returned otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_attr_s
 * @see mcpwm_clock_settings_e
 * @see mcpwm_control_settings_e
 * @see mcpwm_error_e
 */
int mcpwm_init(mcpwm_module_t *module);

/**
 * This function sets the time base counter register of the given module to the value stored in
 * timebase. The direction cannot be set and so is ignored.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  timebase
 *             The new time base counter value to set.
 *
 * @return If the change was successful a zero is returned otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_timebase_s
 * @see mcpwm_error_e
 */
int mcpwm_set_timebase(mcpwm_module_t *module,
                       mcpwm_timebase_t timebase);

/**
 * This function gets the time base counter register value including the direction of count for the
 * given module. The value and direction are stored in the pointer timebase.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @param[out] timebase
 *             A pointer to the mcpwm_timebase_s struct in which to store the time base counter
 *             value and direction.
 *
 * @return If the read was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_timebase_s
 * @see mcpwm_error_e
 */
int mcpwm_get_timebase(mcpwm_module_t *module,
                       mcpwm_timebase_t *timebase);

/**
 * This function sets the period register of the given module. There is no direction bit in this
 * register, so it is ignored in the given mcpwm_timebase_s struct.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  period
 *             The new period value to set.
 *
 * @return If the write was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_timebase_s
 * @see mcpwm_error_e
 */
int mcpwm_set_period(mcpwm_module_t *module,
                     mcpwm_timebase_t period);

/**
 * This function gets the current value stored in the period register of the given module. There
 * is no direction bit in this register, so it should always read zero in the returned
 * mcpwm_timebase_s struct.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @param[out] period
 *             A pointer to the mcpwm_timebase_s struct in which to write the period register value.
 *
 * @return If the read was successful a zero is returned, otherwise a negative number corresponding
 * to the type of error that occurred.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_timebase_s
 * @see mcpwm_error_e
 */
int mcpwm_get_period(mcpwm_module_t *module,
                     mcpwm_timebase_t *period);

/**
 * This function sets the special event compare register of the given module.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  spevt
 *             The new value and direction to store in the special event compare register.
 *
 * @return If the write was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_timebase_s
 * @see mcpwm_error_e
 */
int mcpwm_set_spevt(mcpwm_module_t *module,
                    mcpwm_timebase_t spevt);

/**
 * This function gets the value currently in the special event compare register of the given
 * module.
 *
 * @param[in]  module
 *             A pointer to the module to read.
 *
 * @param[out] spevt
 *             A pointer to a mcpwm_timebase_s struct in which to store the value and direction of
 *             the special event compare register.
 *
 * @return If the read was successful a zero is returned, otherwise a negative number corresponding
 * to the type of error that occurred.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_timebase_s
 * @see mcpwm_error_e
 */
int mcpwm_get_spevt(mcpwm_module_t *module,
                    mcpwm_timebase_t *spevt);

/**
 * This function locks the given module's duty cycle and period registers from being automatically
 * updated. This function is incompatible with the MCPWM_IMMEDIATE setting.
 *
 * @param[in]  module
 *             A pointer to the module to lock.
 *
 * @return If the module was successfully locked a zero is returned, otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_error_e
 */
int mcpwm_update_lock(mcpwm_module_t *module);

/**
 * This function unlocks the given module's duty cycle and period registers so that they will
 * automatically update. This function is incompatible with the MCPWM_IMMEDIATE setting.
 *
 * @param[in]  module
 *             A pointer to the module to unlock.
 *
 * @return If the module was successfully unlocked a zero is returned, otherwise a negative number
 * corresponding to the type of error that occurred.
 *
 * @see mcpwm_error_e
 */
int mcpwm_update_unlock(mcpwm_module_t *module);

/**
 * This function enables the given pins of the given module for PWM output.
 *
 * @param[in]  module
 *             A pointer to the module to enable.
 *
 * @param[in]  pins
 *             A value corresponding to the bitwise ORing of the pins to be enabled.
 *
 * @return If the change was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_pin_e
 * @see mcpwm_error_e
 */
int mcpwm_enable_pins(mcpwm_module_t *module,
                      unsigned int pins);

/**
 * This function disables the given pins of the given module. The pins become general purpose I/O.
 *
 * @param[in]  module
 *             A pointer to the module to disable.
 *
 * @param[in]  pins
 *             A value corresponding to the bitwise ORing of the pins to be enabled.
 *
 * @return If the change was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_pin_e
 * @see mcpwm_error_e
 */
int mcpwm_disable_pins(mcpwm_module_t *module,
                       unsigned int pins);

/**
 * This function sets the dead time value of the given dead time unit for the given module. The
 * value is 6-bits wide, anything over that will be truncated.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  unit
 *             A value corresponding to the dead time unit to change.
 *
 * @param[in]  value
 *             The 6-bit value to write to the register.
 *
 * @return If the change was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_deadtime_unit_e
 * @see mcpwm_error_e
 */
int mcpwm_set_deadtime(mcpwm_module_t *module,
                       enum mcpwm_deadtime_unit_e unit,
                       unsigned int value);

/**
 * This function gets the value stored in the dead time value register of the dead time unit A for
 * the given module.
 *
 * @param[in]  module
 *             A pointer to the module to read.
 *
 * @param[in]  unit
 *             A value corresponding to the dead time unit to change.
 *
 * @return If the read was successful the 6-bit value is returned, if not a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_error_e
 */
int mcpwm_get_deadtime(mcpwm_module_t *module,
                       enum mcpwm_deadtime_unit_e unit);

/**
 * This function selects the dead time unit to use for the given pin pair and signal direction in
 * the given module.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  pin_pair
 *             A value corresponding to the pin pair to change.
 *
 * @param[in]  signal_dir
 *             A value corresponding to the signal direction to change.
 *
 * @param[in]  unit
 *             A value corresponding to the dead time unit to select for the given pin pair and
 *             signal direction.
 *
 * @return If the change was successful a zero is returned, otherwise a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_pin_pair_e
 * @see mcpwm_deadtime_signal_dir_e
 * @see mcpwm_deadtime_unit_e
 * @see mcpwm_error_e
 */
int mcpwm_deadtime_select(mcpwm_module_t *module,
                          enum mcpwm_pin_pair_e pin_pair,
                          enum mcpwm_deadtime_signal_dir_e signal_dir,
                          enum mcpwm_deadtime_unit_e unit);

/**
 * This function sets the value of the fault override for the given fault input and pin in the
 * given module. When the specified fault input pin is brought low and the respective pin pair
 * has that fault input enabled the output pins will be driven to the value specified by
 * override_value. Each pin may have a different value.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  fault_input
 *             The fault input to change.
 *
 * @param[in]  pin
 *             The pin to change.
 *
 * @param[in]  override_value
 *             The value to drive on the pin during fault.
 *
 * @return If the change was successful a zero is returned, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_fault_input_e
 * @see mcpwm_pin_e
 * @see mcpwm_override_value_e
 * @see mcpwm_enable_fault()
 * @see mcpwm_disable_fault()
 * @see mcpwm_error_e
 */
int mcpwm_set_fault_override(mcpwm_module_t *module,
                             enum mcpwm_fault_input_e fault_input,
                             enum mcpwm_pin_e pin,
                             enum mcpwm_override_value_e override_value);

/**
 * This function enables the specified fault input for the given pin pair. If a fault input is
 * enabled for a pin pair, when the fault input is brought low it will drive that pin pair to the
 * value specified with mcpwm_set_fault_override().
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  fault_input
 *             The fault input to enable for the given pin pair.
 *
 * @param[in]  pin_pair
 *             The pin pair to drive from the given fault input.
 *
 * @return If the change is successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_fault_input_e
 * @see mcpwm_pin_pair_e
 * @see mcpwm_error_e
 */
int mcpwm_enable_fault(mcpwm_module_t *module,
                       enum mcpwm_fault_input_e fault_input,
                       enum mcpwm_pin_pair_e pin_pair);

/**
 * This function disables the specified fault input for the given pin pair. If the fault input pin is
 * brought low it will no longer drive the given pin pair to the values specified with
 * mcpwm_set_fault_override().
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  fault_input
 *             The fault input to disable for the given pin pair.
 *
 * @param[in]  pin_pair
 *             The pin pair to disable from the given fault input.
 *
 * @return If the change was successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_fault_input_e
 * @see mcpwm_pin_pair_e
 * @see mcpwm_error_e
 */
int mcpwm_disable_fault(mcpwm_module_t *module,
                        enum mcpwm_fault_input_e fault_input,
                        enum mcpwm_pin_pair_e pin_pair);

/**
 * This function sets the output override value for the given pin in the given module. When the
 * output override is enabled for that pin the value given in override_value will be driven on
 * the pin.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  pin
 *             The pin for which to set the output override value.
 *
 * @param[in]  override_value
 *             The value to drive on the pin when the output override for that pin is enabled.
 *
 * @return If the change is successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_pin_e
 * @see mcpwm_override_value_e
 * @see mcpwm_error_e
 */
int mcpwm_set_output_override(mcpwm_module_t *module,
                              enum mcpwm_pin_e pin,
                              enum mcpwm_override_value_e override_value);

/**
 * This function enables the output override for the given pins in the given module. When enabled
 * the value given in mcpwm_set_output_override() is driven on the given pins.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  pins
 *             The bitwise OR of mcpwm_pin_e values for the pins to enable output override on.
 *
 * @return If the change is successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_pin_e
 * @see mcpwm_error_e
 */
int mcpwm_enable_output_override(mcpwm_module_t *module,
                                 int pins);

/**
 * This function disables the output override for the given pins in the given module. When disabled
 * the pins will return to normal function dependent on the other settings of the module, e.g. fault
 * input, etc.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  pins
 *             The bitwise OR of mcpwm_pin_e values for the pins to disable output override on.
 *
 * @return If the change is successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_pin_e
 * @see mcpwm_error_e
 */
int mcpwm_disable_output_override(mcpwm_module_t *module,
                                  int pins);

/**
 * This function starts the time base counter of the given module.
 *
 * @param[in]  module
 *             A pointer to the module for which to start the time base running.
 *
 * @return If the start was successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_error_e
 */
int mcpwm_start(mcpwm_module_t *module);

/**
 * This function stops the time base counter of the given module.
 *
 * @param[in]  module
 *             A pointer to the module for which to stop the time base running.
 *
 * @return If the stop was successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_clock_settings_e
 * @see mcpwm_error_e
 */
int mcpwm_stop(mcpwm_module_t *module);

/**
 * This function sets the given duty cycle register of the given module to the value supplied in
 * duty_cycle.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  duty_cycle_register
 *             The duty cycle register to set.
 *
 * @param[in]  duty_cycle
 *             The value to write to the duty cycle register.
 *
 * @return If the write is successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_duty_cycle_register_e
 * @see mcpwm_update_lock()
 * @see mcpwm_update_unlock()
 * @see mcpwm_error_e
 */
int mcpwm_set_duty_cycle(mcpwm_module_t *module,
                         enum mcpwm_duty_cycle_register_e duty_cycle_register,
                         unsigned int duty_cycle);

/**
 * This function gets the value stored in the given duty cycle register of the given module.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  duty_cycle_register
 *             The register from which to read the duty cycle value.
 *
 * @param[out] duty_cycle
 *             The value of the duty cycle register will be stored in the variable pointed to by
 *             this pointer.
 *
 * @return If the read is successful return a zero, otherwise return a negative number
 * corresponding to the type of error.
 *
 * @see mcpwm_duty_cycle_register_e
 * @see mcpwm_error_e
 */
int mcpwm_get_duty_cycle(mcpwm_module_t *module,
                         enum mcpwm_duty_cycle_register_e duty_cycle_register,
                         unsigned int *duty_cycle);

/**
 * @}
 */ // End of MCPWMModulePublic group

#endif //_MCPWM_MODULE_H
