/* -*- mode: C; tab-width: 4; -*- */

/**
 * @file pwm_xc16.c
 *
 * @brief This file contains the private implementations of the PWM module for the XC16 compiler.
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
 * @addtogroup pwm_xc16
 *
 * @private
 *
 * @{
 */

// Include standard C libraries
#include <stdlib.h>

// Include Microchip libraries
#include <xc.h>

// Include local libraries
#include <pwm.h>

enum pwm_sfr_offset_e
{
    PWM_SFR_OFFSET_PxTCON = 0x0000,
    PWM_SFR_OFFSET_PxTMR  = 0x0001,
    PWM_SFR_OFFSET_PxTPER = 0x0002,
    PWM_SFR_OFFSET_PxSECMP = 0x0003,
    PWM_SFR_OFFSET_PWMxCON1 = 0x0004,
    PWM_SFR_OFFSET_PWMxCON2 = 0x0005,
    PWM_SFR_OFFSET_PxDTCON1 = 0x0006,
    PWM_SFR_OFFSET_PxDTCON2 = 0x0007,
    PWM_SFR_OFFSET_PxFLTACON = 0x0008,
    PWM_SFR_OFFSET_PxFLTBCON = 0x0009,
    PWM_SFR_OFFSET_PxOVDCON = 0x000A,
    PWM_SFR_OFFSET_PxDC1 = 0x000B,
    PWM_SFR_OFFSET_PxDC2 = 0x000C,
    PWM_SFR_OFFSET_PxDC3 = 0x000D,
    PWM_SFR_OFFSET_PxDC4 = 0x000E
};

enum pwm_sfr_default_e
{
    PWM_SFR_DEFAULT_PxTCON    = 0x0000,
    PWM_SFR_DEFAULT_PxTMR     = 0x0000,
    PWM_SFR_DEFAULT_PxTPER    = 0x7FFF,
    PWM_SFR_DEFAULT_PxSECMP   = 0x0000,
    PWM_SFR_DEFAULT_PWMxCON1  = 0x0000, /**< The last 8 bits of this default mask the reset values
                                           of PENyH/L so that they remain the same value as their
                                           reset values (depends on PWMPIN). */
    PWM_SFR_DEFAULT_PWMxCON2  = 0x0000,
    PWM_SFR_DEFAULT_PxDTCON1  = 0x0000,
    PWM_SFR_DEFAULT_PxDTCON2  = 0x0000,
    PWM_SFR_DEFAULT_PxFLTACON = 0x000F, /**< The last 4 bits of this default depend on the hardware
                                           in use. They are set to 1's here for simplicity. */
    PWM_SFR_DEFAULT_PxFLTBCON = 0x000F, /**< The last 4 bits of this default depend on the hardware
                                           in use. They are set to 1's here for simplicity. */
    PWM_SFR_DEFAULT_PxOVDCON  = 0xFF00,
    PWM_SFR_DEFAULT_PxDC1     = 0x0000,
    PWM_SFR_DEFAULT_PxDC2     = 0x0000,
    PWM_SFR_DEFAULT_PxDC3     = 0x0000,
    PWM_SFR_DEFAULT_PxDC4     = 0x0000
};

/**
 * @brief This is the private object for a PWM module.
 *
 * @details Nothing here.
 *
 * @private
 */
struct pwm_private_s
{
    pwm_attr_t attr_;
};
typedef struct pwm_private_s pwm_private_t;


static int pwm_init(pwm_t *module,
                    pwm_attr_t *attr);
static int pwm_start(pwm_t *module);
static int pwm_stop(pwm_t *module);
static int pwm_enable_pin(pwm_t *module,
                          pwm_pin_t pin);
static int pwm_disable_pin(pwm_t *module,
                           pwm_pin_t pin);
static int pwm_write_dutycycle(pwm_t *module,
                               pwm_channel_t channel,
                               unsigned int dutycycle);
static unsigned int pwm_read_dutycycle(pwm_t *module,
                                       pwm_channel_t channel);
static int pwm_override_output(pwm_t *module,
                               pwm_pin_t pin,
                               pwm_output_t value);
static bool pwm_is_valid(pwm_t *module);
static bool pwm_is_running(pwm_t *module);
static int pwm_cleanup(pwm_t *module);


// Definition of global pwm variable
pwm_global_t pwm = {
    .init = pwm_init,
    .start = pwm_start,
    .stop = pwm_stop,
    .enable_pin = pwm_enable_pin,
    .disable_pin = pwm_disable_pin,
    .write_dutycycle = pwm_write_dutycycle,
    .read_dutycycle = pwm_read_dutycycle,
    .override_output = pwm_override_output,
    .is_valid = pwm_is_valid,
    .is_running = pwm_is_running,
    .cleanup = pwm_cleanup
};


static int pwm_init(pwm_t *module,
                    pwm_attr_t *attr)
{

}

static int pwm_start(pwm_t *module)
{

}

static int pwm_stop(pwm_t *module)
{

}

static int pwm_enable_pin(pwm_t *module,
                          pwm_pin_t pin)
{

}

static int pwm_disable_pin(pwm_t *module,
                           pwm_pin_t pin)
{

}

static int pwm_write_dutycycle(pwm_t *module,
                               pwm_channel_t channel,
                               unsigned int dutycycle)
{

}

static unsigned int pwm_read_dutycycle(pwm_t *module,
                                       pwm_channel_t channel)
{

}

static int pwm_override_output(pwm_t *module,
                               pwm_pin_t pin,
                               pwm_output_t value)
{

}

static bool pwm_is_valid(pwm_t *module)
{

}

static bool pwm_is_running(pwm_t *module)
{

}

static int pwm_cleanup(pwm_t *module)
{

}


/**
 * @}
 */ // End of group pwm_xc16
