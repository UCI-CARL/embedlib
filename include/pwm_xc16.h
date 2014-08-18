/* -*- mode: C; tab-width: 4; -*- */

/**
 * @file pwm_xc16.h
 *
 * @brief This file contains the public interfaces of the PWM module for the XC16 compiler.
 *
 * @details Nothing here.
 *
 * @date 2014/08/08
 * @author Liam Bucci
 * @carlnumber FIRM-0009
 * @version 0.4.0
 *
 * @private
 */

/**
 * @defgroup pwm_xc16 "PWM (XC16)"
 *
 * @public
 *
 * @{
 */

#ifndef PWM_XC16_H_
#define PWM_XC16_H_

#include <stdbool.h>
/**
 * @brief Constants defining the valid PWM module pins.
 *
 * @details Nothing here.
 *
 * @public
 */
enum pwm_pin_e
{
    PWM_PIN_NONE = 0x0000,
    PWM_PIN_P1L  = 0x0001,
    PWM_PIN_P1H  = 0x0002,
    PWM_PIN_P2L  = 0x0003,
    PWM_PIN_P2H  = 0x0004,
    PWM_PIN_P3L  = 0x0005,
    PWM_PIN_P3H  = 0x0006,
    PWM_PIN_P4L  = 0x0007,
    PWM_PIN_P4H  = 0x0008,
    PWM_PIN_ALL  = 0x000F
};
typedef enum pwm_pin_e pwm_pin_t;

/**
 * @brief Constants defining the valid PWM module channels.
 *
 * @details Nothing here.
 *
 * @public
 */
enum pwm_channel_e
{
    PWM_CHANNEL_NONE = 0x0000,
    PWM_CHANNEL_P1   = 0x0001,
    PWM_CHANNEL_P2   = 0x0002,
    PWM_CHANNEL_P3   = 0x0003,
    PWM_CHANNEL_P4   = 0x0004,
    PWM_CHANNEL_ALL  = 0x000F
};
typedef enum pwm_channel_e pwm_channel_t;

/**
 * @brief Constants defining the valid PWM module outputs.
 *
 * @details Nothing here.
 *
 * @public
 */
enum pwm_output_e
{
    PWM_OUTPUT_OVERRIDE_INACTIVE = 0x0000,
    PWM_OUTPUT_OVERRIDE_ACTIVE   = 0x0001,
    PWM_OUTPUT_OVERRIDE_DISABLE  = 0x000F
};
typedef enum pwm_output_e pwm_output_t;

/**
 * @brief Constants defining the valid time base settings for the attribute object.
 *
 * @details Nothing here.
 *
 * @public
 */
enum pwm_attr_time_base_e
{
    PWM_ATTR_TIME_BASE_PRESCALER_1TCY = 0x0000,
    PWM_ATTR_TIME_BASE_PRESCALER_4TCY = 0x0001,
    PWM_ATTR_TIME_BASE_PRESCALER_16TCY = 0x0002,
    PWM_ATTR_TIME_BASE_PRESCALER_64TCY = 0x0003,

    PWM_ATTR_TIME_BASE_POSTSCALER_1  = 0x0000,
    PWM_ATTR_TIME_BASE_POSTSCALER_2  = 0x0001,
    PWM_ATTR_TIME_BASE_POSTSCALER_3  = 0x0002,
    PWM_ATTR_TIME_BASE_POSTSCALER_4  = 0x0003,
    PWM_ATTR_TIME_BASE_POSTSCALER_5  = 0x0004,
    PWM_ATTR_TIME_BASE_POSTSCALER_6  = 0x0005,
    PWM_ATTR_TIME_BASE_POSTSCALER_7  = 0x0006,
    PWM_ATTR_TIME_BASE_POSTSCALER_8  = 0x0007,
    PWM_ATTR_TIME_BASE_POSTSCALER_9  = 0x0008,
    PWM_ATTR_TIME_BASE_POSTSCALER_10 = 0x0009,
    PWM_ATTR_TIME_BASE_POSTSCALER_11 = 0x000A,
    PWM_ATTR_TIME_BASE_POSTSCALER_12 = 0x000B,
    PWM_ATTR_TIME_BASE_POSTSCALER_13 = 0x000C,
    PWM_ATTR_TIME_BASE_POSTSCALER_14 = 0x000D,
    PWM_ATTR_TIME_BASE_POSTSCALER_15 = 0x000E,
    PWM_ATTR_TIME_BASE_POSTSCALER_16 = 0x000F
};

/**
 * @brief Constants defining the valid module settings for the attribute object.
 *
 * @details Nothing here.
 *
 * @public
 */
enum pwm_attr_module_e
{
    PWM_ATTR_MODULE_CONTINUE_IN_IDLE_EN  = 0x0000,
    PWM_ATTR_MODULE_CONTINUE_IN_IDLE_DIS = 0x0001,

    PWM_ATTR_MODULE_IMMEDIATE_UPDATE_DIS = 0x0000,
    PWM_ATTR_MODULE_IMMEDIATE_UPDATE_EN  = 0x0001,

    PWM_ATTR_MODULE_SYNC_OVERRIDE_DIS = 0x0000,
    PWM_ATTR_MODULE_SYNC_OVERRIDE_EN  = 0x0001
};

/**
 * @brief Constants defining the valid errors that can be returned by module functions.
 *
 * @details Nothing here.
 *
 * @public
 */
enum pwm_error_e
{
    PWM_E_NONE = 0,
    PWM_E_MODULE = -1,
    PWM_E_INPUT = -2,
    PWM_E_ALLOC = -3,
    PWM_E_UNSUPPORTED = -4,

    PWM_E_ASSERT = 0x8000
};
typedef enum pwm_error_e pwm_error_t;

/**
 * @brief The attribute object contains all the static settings for a PWM module.
 *
 * @details Nothing here.
 *
 * @public
 */
struct pwm_attr_s
{
    /**
     * @brief Time base settings.
     *
     * @details Nothing here.
     */
    struct
    {
        unsigned int prescaler  :2;
        unsigned int postscaler :4;
        unsigned int period :15;
    } time_base;

    /**
     * @brief Module settings.
     *
     * @details Nothing here.
     */
    struct
    {
        unsigned int continue_in_idle :1;
        unsigned int immediate_update :1;
        unsigned int sync_override    :1;
    } module;
};
typedef struct pwm_attr_s pwm_attr_t;

// Predeclaration of pwm module struct
struct pwm_s;
typedef struct pwm_s pwm_t;

/**
 * @brief The module object contains the definition of a specific module.
 *
 * @details Once initialized, this object represents a particular PWM hardware module. This object
 * may be passed to any @em pwm function and that function will work on the given module.
 *
 * @public
 */
struct pwm_s
{
    const unsigned int module_number; // Must be 1 or 2

    void (* notify)(pwm_t *module);

    void *private;
};

/**
 * @brief This global object is used as a type of PWM namespace. It contains all of the public
 * functions of the PWM module.
 *
 * @details Nothing here.
 *
 * @public
 */
struct pwm_global_s
{
    int (* const init)(pwm_t *module, pwm_attr_t *attr);
    int (* const start)(pwm_t *module);
    int (* const stop)(pwm_t *module);
    int (* const enable_pin)(pwm_t *module, pwm_pin_t pin);
    int (* const disable_pin)(pwm_t *module, pwm_pin_t pin);
    int (* const write_dutycycle)(pwm_t *module, pwm_channel_t channel, unsigned int dutycycle);
    unsigned int (* const read_dutycycle)(pwm_t *module, pwm_channel_t channel);
    int (* const override_output)(pwm_t *module, pwm_pin_t pin, pwm_output_t value);
    bool (* const is_valid)(pwm_t *module);
    bool (* const is_running)(pwm_t *module);
    int (* const cleanup)(pwm_t *module);
};
typedef struct pwm_global_s pwm_global_t;

extern pwm_global_t pwm;


#endif // PWM_XC16_H_

/**
 * @}
 */ // End of group pwm_xc16
