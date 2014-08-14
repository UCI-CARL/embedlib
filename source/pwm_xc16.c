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

#define PWM_BASE_ADDRESS(module) ( ((pwm_private_t *)((module)->private))->base_address_ )

#define PWM_UNLOCK(module) { (*(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMKEY)) = 0xABCD; \
        (*(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMKEY)) = 0x4321; }

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
    PWM_SFR_DEFAULT_PWMxCON1  = 0x0F00, /**< The last 8 bits of this default mask the reset values
                                           of PENyH/L so that they remain the same value as their
                                           reset values (depends on PWMPIN). @note All pairs are
                                           set to independent output mode (opposite of default). */
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

struct pwm_pxtcon_bits_s
{
    unsigned int ptmod :2;
    unsigned int ptckps :2;
    unsigned int ptops :4;
    unsigned int :5;
    unsigned int ptsidl :1;
    unsigned int :1;
    unsigned int pten :1;
};
typedef struct pwm_pxtcon_bits_s pwm_pxtcon_bits_t;

struct pwm_pxtmr_bits_s
{
    unsigned int ptmr :15;
    unsigned int ptdir :1;
};
typedef struct pwm_pxtmr_bits_s pwm_pxtmr_bits_t;

struct pwm_pxtper_bits_s
{
    unsigned int ptper :15;
};
typedef struct pwm_pxtper_bits_s pwm_pxtper_bits_t;

struct pwm_pxsecmp_bits_s
{
    unsigned int sevtcmp :15;
    unsigned int sevtdir :1;
};
typedef struct pwm_pxsecmp_bits_s pwm_pxsecmp_bits_t;

struct pwm_pwmxcon1_bits_s
{
    unsigned int pen1l :1;
    unsigned int pen2l :1;
    unsigned int pen3l :1;
    unsigned int pen4l :1;
    unsigned int pen1h :1;
    unsigned int pen2h :1;
    unsigned int pen3h :1;
    unsigned int pen4h :1;
    unsigned int pmod1 :1;
    unsigned int pmod2 :1;
    unsigned int pmod3 :1;
    unsigned int pmod4 :1;
};
typedef struct pwm_pwmxcon1_bits_s pwm_pwmxcon1_bits_t;

struct pwm_pwmxcon2_bits_s
{
    unsigned int udis :1;
    unsigned int osync :1;
    unsigned int iue :1;
    unsigned int :5;
    unsigned int sevops :4;
};
typedef struct pwm_pwmxcon2_bits_s pwm_pwmxcon2_bits_t;

struct pwm_pxdtcon1_bits_s
{
    unsigned int dta :6;
    unsigned int dtaps :2;
    unsigned int dtb :6;
    unsigned int dtbps :2;
};
typedef struct pwm_pxdtcon1_bits_s pwm_pxdtcon1_bits_t;

struct pwm_pxdtcon2_bits_s
{
    unsigned int dts1i :1;
    unsigned int dts1a :1;
    unsigned int dts2i :1;
    unsigned int dts2a :1;
    unsigned int dts3i :1;
    unsigned int dts3a :1;
    unsigned int dts4i :1;
    unsigned int dts4a :1;
};
typedef struct pwm_pxdtcon2_bits_s pwm_pxdtcon2_bits_t;

struct pwm_pxfltacon_bits_s
{
    unsigned int faen1 :1;
    unsigned int faen2 :1;
    unsigned int faen3 :1;
    unsigned int faen4 :1;
    unsigned int :3;
    unsigned int fltam :1;
    unsigned int faov1l :1;
    unsigned int faov1h :1;
    unsigned int faov2l :1;
    unsigned int faov2h :1;
    unsigned int faov3l :1;
    unsigned int faov3h :1;
    unsigned int faov4l :1;
    unsigned int faov4h :1;
};
typedef struct pwm_pxfltacon_bits_s pwm_pxfltacon_bits_t;

struct pwm_pxfltbcon_bits_s
{
    unsigned int fben1 :1;
    unsigned int fben2 :1;
    unsigned int fben3 :1;
    unsigned int fben4 :1;
    unsigned int :3;
    unsigned int fltbm :1;
    unsigned int fbov1l :1;
    unsigned int fbov1h :1;
    unsigned int fbov2l :1;
    unsigned int fbov2h :1;
    unsigned int fbov3l :1;
    unsigned int fbov3h :1;
    unsigned int fbov4l :1;
    unsigned int fbov4h :1;
};
typedef struct pwm_pxfltbcon_bits_s pwm_pxfltbcon_bits_t;

struct pwm_pxovdcon_bits_s
{
    unsigned int pout1l :1;
    unsigned int pout1h :1;
    unsigned int pout2l :1;
    unsigned int pout2h :1;
    unsigned int pout3l :1;
    unsigned int pout3h :1;
    unsigned int pout4l :1;
    unsigned int pout4h :1;
    unsigned int povd1l :1;
    unsigned int povd1h :1;
    unsigned int povd2l :1;
    unsigned int povd2h :1;
    unsigned int povd3l :1;
    unsigned int povd3h :1;
    unsigned int povd4l :1;
    unsigned int povd4h :1;
};
typedef struct pwm_pxovdcon_bits_s pwm_pxovdcon_bits_t;


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
    volatile unsigned int base_address_;
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
    // Check for valid input
    if( module == NULL )
    {// Invalid module pointer
        return PWM_E_MODULE;
    }

    if( module->private != NULL )
    {// Module is already initialized
        return PWM_E_MODULE;
    }

    if( attr == NULL )
    {// Invalid attribute object
        return PWM_E_INPUT;
    }

    // Allocate private object
    module->private = calloc(sizeof(pwm_private_t), 1);
    if( module->private == NULL )
    {// Allocation failed
        return PWM_E_ALLOC;
    }

    // Copy attribute struct to private object
    ((pwm_private_t *)(module->private))->attr_ = *attr;
    
    if( module->module_number == 1 )
    {// MCPWM1
        // Set base address
        ((pwm_private_t *)(module->private))->base_address_ = &P1TCON;
    }
#if PWM_HW_MAX_MODULE_NUM == 2
    else if( module->module_number == 2 )
    {// MCPWM2
        // Set base address
        ((pwm_private_t *)(module->private))->base_address_ = &P2TCON;
    }
#endif
    else
    {// Invalid module number
        pwm.cleanup(module);
        return PWM_E_MODULE;
    }

    // Set default SFRs
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON) = PWM_SFR_DEFAULT_PxTCON;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTMR) = PWM_SFR_DEFAULT_PxTMR;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTPER) = PWM_SFR_DEFAULT_PxTPER;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxSECMP) = PWM_SFR_DEFAULT_PxSECMP;
    PWM_UNLOCK(module);
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1) = PWM_SFR_DEFAULT_PWMxCON1;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON2) = PWM_SFR_DEFAULT_PWMxCON2;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDTCON1) = PWM_SFR_DEFAULT_PxDTCON1;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDTCON2) = PWM_SFR_DEFAULT_PxDTCON2;
    PWM_UNLOCK(module);
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxFLTACON) = PWM_SFR_DEFAULT_PxFLTACON;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) = PWM_SFR_DEFAULT_PxOVDCON;
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1) = PWM_SFR_DEFAULT_PxDC1;
    // Only MCPWM1 has these SFRs
    if( module->module_number == 1 )
    {
        PWM_UNLOCK(module);
        *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxFLTBCON) = PWM_SFR_DEFAULT_PxFLTBCON;
        *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC2) = PWM_SFR_DEFAULT_PxDC2;
        *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC3) = PWM_SFR_DEFAULT_PxDC3;
        *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC4) = PWM_SFR_DEFAULT_PxDC4;
    }

    // Set time base settings
    // Set free running mode
    ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptmod = 0b00;
    // Set prescaler
    switch( ((pwm_private_t *)(module->private))->attr_.time_base.prescaler )
    {
    case PWM_ATTR_TIME_BASE_PRESCALER_1TCY:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptckps = 0b00;
        break;
    case PWM_ATTR_TIME_BASE_PRESCALER_4TCY:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptckps = 0b01;
        break;
    case PWM_ATTR_TIME_BASE_PRESCALER_16TCY:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptckps = 0b10;
        break;
    case PWM_ATTR_TIME_BASE_PRESCALER_64TCY:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptckps = 0b11;
        break;
    default:
        // Assertion failed, no other values should be possible!
        pwm.cleanup(module);
        return PWM_E_ASSERT;
    }

    // Set postscaler
    switch( ((pwm_private_t *)(module->private))->attr_.time_base.postscaler )
    {
    case PWM_ATTR_TIME_BASE_POSTSCALER_1:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0000;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_2:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0001;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_3:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0010;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_4:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0011;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_5:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0100;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_6:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0101;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_7:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0110;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_8:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b0111;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_9:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1000;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_10:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1001;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_11:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1010;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_12:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1011;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_13:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1100;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_14:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1101;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_15:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1110;
        break;
    case PWM_ATTR_TIME_BASE_POSTSCALER_16:
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptops = 0b1111;
        break;
    default:
        // Assertion failed, no other values should be possible!
        pwm.cleanup(module);
        return PWM_E_ASSERT;
    }

    // Set period
    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTPER) \
        = ((pwm_private_t *)(module->private))->attr_.time_base.period;

    // Set module settings
    // Set continue in idle
    if( ((pwm_private_t *)(module->private))->attr_.module.continue_in_idle \
        == PWM_ATTR_MODULE_CONTINUE_IN_IDLE_EN )
    {
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptsidl = 0;
    }
    else
    {
        ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->ptsidl = 1;
    }

    // Set immediate update
    if( ((pwm_private_t *)(module->private))->attr_.module.immediate_update \
        == PWM_ATTR_MODULE_IMMEDIATE_UPDATE_DIS )
    {
        ((pwm_pwmxcon2_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON2))->iue = 0;
    }
    else
    {
        ((pwm_pwmxcon2_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON2))->iue = 1;
    }

    // Set sync override
    if( ((pwm_private_t *)(module->private))->attr_.module.sync_override \
        == PWM_ATTR_MODULE_SYNC_OVERRIDE_DIS )
    {
        ((pwm_pwmxcon2_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON2))->osync = 1;
    }
    else
    {
        ((pwm_pwmxcon2_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON2))->osync = 0;
    }
    
    return PWM_E_NONE;
}

static int pwm_start(pwm_t *module)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return PWM_E_MODULE;
    }

    ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->pten = 1;

    return PWM_E_NONE;
}

static int pwm_stop(pwm_t *module)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return PWM_E_MODULE;
    }

    ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->pten = 0;
    
    return PWM_E_NONE;
}

static int pwm_enable_pin(pwm_t *module,
                          pwm_pin_t pin)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return PWM_E_MODULE;
    }

    // Handle null case of pin NONE
    if( pin == PWM_PIN_NONE )
    {// Do nothing and return
        return PWM_E_NONE;
    }

    // Make sure pin exists for specified module
    // Check for case of ALL first so it takes precedence over module number
    if( pin == PWM_PIN_ALL )
    {// Enable all pins
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1l = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1h = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen2l = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen2h = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen3l = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen3h = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen4l = 1;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen4h = 1;
    }
    else if( pin == PWM_PIN_P1L )
    {
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1l = 1;
    }
    else if( pin == PWM_PIN_P1H )
    {
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1h = 1;
    }
    else if( module->module_number == 1 )
    {// Only MCPWM1 has 4 channels
        if( pin == PWM_PIN_P2L )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen2l = 1;
        }
        else if( pin == PWM_PIN_P2H )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen2h = 1;
        }
        else if( pin == PWM_PIN_P3L )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen3l = 1;
        }
        else if( pin == PWM_PIN_P3H )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen3h = 1;
        }
        else if( pin == PWM_PIN_P4L )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen4l = 1;
        }
        else if( pin == PWM_PIN_P4H )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen4h = 1;
        }
        else
        {// Invalid pin number
            return PWN_E_INPUT;
        }
    }
    else
    {// Invalid input
        return PWM_E_INPUT;
    }

    return PWN_E_NONE;
}

static int pwm_disable_pin(pwm_t *module,
                           pwm_pin_t pin)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return PWM_E_MODULE;
    }

    // Handle null case of pin NONE
    if( pin == PWM_PIN_NONE )
    {// Do nothing and return
        return PWM_E_NONE;
    }

    // Make sure pin exists for specified module
    // Check for case of ALL first so it takes precedence over module number
    if( pin == PWM_PIN_ALL )
    {// Enable all pins
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1l = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1h = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen2l = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen2h = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen3l = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen3h = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen4l = 0;
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen4h = 0;
    }
    else if( pin == PWM_PIN_P1L )
    {
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1l = 0;
    }
    else if( pin == PWM_PIN_P1H )
    {
        PWM_UNLOCK(module);
        ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1))->pen1h = 0;
    }
    else if( module->module_number == 1 )
    {// Only MCPWM1 has 4 channels
        if( pin == PWM_PIN_P2L )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen2l = 0;
        }
        else if( pin == PWM_PIN_P2H )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen2h = 0;
        }
        else if( pin == PWM_PIN_P3L )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen3l = 0;
        }
        else if( pin == PWM_PIN_P3H )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen3h = 0;
        }
        else if( pin == PWM_PIN_P4L )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen4l = 0;
        }
        else if( pin == PWM_PIN_P4H )
        {
            PWM_UNLOCK(module);
            ((pwm_pwmxcon1_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1)) \
                ->pen4h = 0;
        }
        else
        {// Invalid pin number
            return PWN_E_INPUT;
        }
    }
    else
    {// Invalid input
        return PWM_E_INPUT;
    }

    return PWN_E_NONE;
}

static int pwm_write_dutycycle(pwm_t *module,
                               pwm_channel_t channel,
                               unsigned int dutycycle)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return PWM_E_MODULE;
    }

    // Handle the special case of NONE
    if( channel == PWM_CHANNEL_NONE )
    {// Do nothing and exit
        return PWM_E_NONE;
    }

    // Handle MCPWM1 and MCPWM2 differently
    if( module->module_number == 1 )
    {// MCPWM1
        // Check for valid input and set the correct duty cycle SFR
        if( channel == PWM_CHANNEL_P1 )
        {// Channel 1
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1) = dutycycle;
        }
        else if( channel == PWM_CHANNEL_P2 )
        {// Channel 2
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC2) = dutycycle;
        }
        else if( channel == PWM_CHANNEL_P3 )
        {// Channel 3
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC3) = dutycycle;
        }
        else if( channel == PWM_CHANNEL_P4 )
        {// Channel 4
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC4) = dutycycle;
        }
        else if( channel == PWM_CHANNEL_ALL )
        {// All channels
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1) = dutycycle;
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC2) = dutycycle;
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC3) = dutycycle;
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC4) = dutycycle;
        }
        else
        {// Channel shouldn't have any other values
            // Invalid channel number
            return PWM_E_INPUT;
        }
    }
    else if( module->module_number == 2 )
    {// MCPWM2
        // Check for valid input and set the correct duty cycle SFR
        if( channel == PWM_CHANNEL_P1 )
        {// Channel 1
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1) = dutycycle;
        }
        else if( channel == PWM_CHANNEL_ALL )
        {// All channels
            *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1) = dutycycle;
        }
        else
        {// Any other channel is invalid
            // Invalid channel number
            return PWM_E_INPUT;
        }
    }
    else
    {
        // No other module numbers are valid, this should have been caught by is_valid()
        return PWM_E_ASSERT;
    }

    return PWM_E_NONE;
}

static unsigned int pwm_read_dutycycle(pwm_t *module,
                                       pwm_channel_t channel)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        // Return the default value
        return PWM_SFR_DEFAULT_PxDC1;
    }

    // Handle the special case of NONE
    if( channel == PWM_CHANNEL_NONE )
    {// Return the default value
        return PWM_SFR_DEFAULT_PxDC1;
    }

    // Handle the special case of ALL
    if( channel == PWM_CHANNEL_ALL )
    {// Return the default value
        return PWM_SFR_DEFAULT_PxDC1;
    }

    // Handle MCPWM1 and MCPWM2 differently
    if( module->module_number == 1 )
    {// MCPWM1
        // Check for valid input and set the correct duty cycle SFR
        if( channel == PWM_CHANNEL_P1 )
        {// Channel 1
            return *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1);
        }
        else if( channel == PWM_CHANNEL_P2 )
        {// Channel 2
            return *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC2);
        }
        else if( channel == PWM_CHANNEL_P3 )
        {// Channel 3
            return *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC3);
        }
        else if( channel == PWM_CHANNEL_P4 )
        {// Channel 4
            return *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC4);
        }
        else
        {// Channel shouldn't have any other values
            // Invalid channel number
            return PWM_SFR_DEFAULT_PxDC1;
        }
    }
    else if( module->module_number == 2 )
    {// MCPWM2
        // Check for valid input and set the correct duty cycle SFR
        if( channel == PWM_CHANNEL_P1 )
        {// Channel 1
            return *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1);
        }
        else if( channel == PWM_CHANNEL_ALL )
        {// All channels
            return *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1);
        }
        else
        {// Any other channel is invalid
            // Invalid channel number
            return PWM_SFR_DEFAULT_PxDC1;
        }
    }
    else
    {
        // No other module numbers are valid, this should have been caught by is_valid()
        return PWM_SFR_DEFAULT_PxDC1;
    }
}

static int pwm_override_output(pwm_t *module,
                               pwm_pin_t pin,
                               pwm_output_t value)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return PWM_E_MODULE;
    }

    // Handle special case of pin NONE
    if( pin == PWM_PIN_NONE )
    {// Do nothing and exit
        return PWM_E_NONE;
    }

    // Handle MCPWM1 and MCPWM2 differently
    if( module->module_number == 1 )
    {// MCPWM1
        // Set specified pin(s) to specified output
        if( pin == PWM_PIN_ALL )
        {// Set all pins to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for all pins
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0xFF00;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override all pins to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) = 0x00FF;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override all pins to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) = 0x0000;
            }
            else
            {// Unknown value given
                return PWN_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P1L )
        {// Set pin P1L to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P1L
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x0100;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P1L to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<0 & ~(1<<8);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P1L to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<0) & ~(1<<8);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P1H )
        {// Set pin P1H to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P1H
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x0200;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P1H to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<1 & ~(1<<9);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P1H to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<1) & ~(1<<9);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P2L )
        {// Set pin P2L to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P2L
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x0400;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P2L to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<2 & ~(1<<10);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P2L to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<2) & ~(1<<10);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P2H )
        {// Set pin P2H to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P2H
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x0800;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P2H to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<3 & ~(1<<11);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P2H to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<3) & ~(1<<11);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P3L )
        {// Set pin P3L to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P3L
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x1000;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P3L to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<4 & ~(1<<12);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P3L to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<4) & ~(1<<12);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P3H )
        {// Set pin P3H to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P3H
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x2000;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P3H to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<5 & ~(1<<13);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P3H to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<5) & ~(1<<13);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P4L )
        {// Set pin P4L to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P4L
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x4000;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P4L to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<6 & ~(1<<14);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P4L to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<6) & ~(1<<14);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else if( pin == PWM_PIN_P4H )
        {// Set pin P4H to value
            if( value == PWM_OUTPUT_OVERRIDE_DISABLE )
            {// Disable override for pin P4H
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) |= 0x8000;
            }
            else if( value == PWM_OUTPUT_OVERRIDE_ACTIVE )
            {// Override pin P4H to active
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) | 1<<7 & ~(1<<15);
            }
            else if( value == PWM_OUTPUT_OVERRIDE_INACTIVE )
            {// Override pin P4H to inactive
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) \
                    = *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) & ~(1<<7) & ~(1<<15);
            }
            else
            {// Unknown value given
                return PWM_E_INPUT;
            }
        }
        else
        {// Unknown pin given
            return PWM_E_INPUT;
        }

        return PWM_E_NONE;
}

static bool pwm_is_valid(pwm_t *module)
{
    return !(module == NULL \
             || module->module_number == 0 \
             || module->module_number > 2 \
             || module->private == NULL);
}

static bool pwm_is_running(pwm_t *module)
{
    // Check for a valid module
    if( !pwm.is_valid(module) )
    {// Invalid module
        return false;
    }

    if( ((pwm_pxtcon_bits_t *)(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON))->pten == 1 )
    {// Time base is running
        return true;
    }
    else
    {// Time base is stopped
        return false;
    }
}

static int pwm_cleanup(pwm_t *module)
{
    // Check if module pointer is valid
    if( module != NULL )
    {// Assume module pointer is valid
        // Check if private pointer is valid
        if( module->private != NULL )
        {// Assume private pointer is valid
            // Check if the module number is valid (required by order of init function)
            if( module->module_number == 1 || module->module_number == 2 )
            {// Module number is valid
                // Reset all SFRs to default values
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTCON) = PWM_SFR_DEFAULT_PxTCON;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTMR) = PWM_SFR_DEFAULT_PxTMR;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxTPER) = PWM_SFR_DEFAULT_PxTPER;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxSECMP) = PWM_SFR_DEFAULT_PxSECMP;
                PWM_UNLOCK(module);
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON1) = PWM_SFR_DEFAULT_PWMxCON1;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PWMxCON2) = PWM_SFR_DEFAULT_PWMxCON2;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDTCON1) = PWM_SFR_DEFAULT_PxDTCON1;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDTCON2) = PWM_SFR_DEFAULT_PxDTCON2;
                PWM_UNLOCK(module);
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxFLTACON) = PWM_SFR_DEFAULT_PxFLTACON;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxOVDCON) = PWM_SFR_DEFAULT_PxOVDCON;
                *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC1) = PWM_SFR_DEFAULT_PxDC1;
                // Only MCPWM1 has these SFRs
                if( module->module_number == 1 )
                {
                    PWM_UNLOCK(module);
                    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxFLTBCON) = PWM_SFR_DEFAULT_PxFLTBCON;
                    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC2) = PWM_SFR_DEFAULT_PxDC2;
                    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC3) = PWM_SFR_DEFAULT_PxDC3;
                    *(PWM_BASE_ADDRESS(module) + PWM_SFR_OFFSET_PxDC4) = PWM_SFR_DEFAULT_PxDC4;
                }
            }

            // Free private object memory
            free(module->private);
        }

        // Free module memory
        free(module);
    }

    return PWM_E_NONE;
}


/**
 * @}
 */ // End of group pwm_xc16
