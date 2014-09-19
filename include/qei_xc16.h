/* -*- mode: c; tab-width: 4; -*- */

/**
 * @file qei_module.h
 *
 * @brief This file contains the public declarations and definitions for a Quadrature Encoder
 * Interface (QEI) module.
 *
 * @author     Liam Bucci
 * @date       12/18/2012
 * @carlnumber FIRM-0004
 * @version    0.3.0
 */


// Include guard
#ifndef _QEI_MODULE_H
#define _QEI_MODULE_H


/**
 * @defgroup QEIModulePublic QEI Module Public
 *
 * @details This group contains all the public features of the QEI module code. It is meant to
 * document the parts of the QEI module which are available to the user.
 *
 * @{
 */


/* Attribute Declaration */
/**
 * @brief The attribute struct for a Quadrature Encoder Interface module.
 *
 * @details This struct contains the various settings for a QEI module. It only contains settings that would
 * typically be initialized once and remain in place through out the life of the module. Other,
 * more dynamic, changes should be made using the various functions provided with the module.
 */
struct qei_attr_s
{
    unsigned int control_settings; //!< The settings which control the general function of the QEI module.
};

/* Module Declaration */
/**
 * @brief The Quadrature Encoder Interface module definition.
 *
 * @details This struct defines the QEI module. It contains a base address which is a pointer to the base
 * register of the specific module controlled by a particular instance. The struct also contains
 * the initialization attributes for the module.
 */
typedef struct qei_module_s
{
    volatile unsigned int *base_address; //!< Address which defines the base register of the specific QEI module. Should point to a QEIxCON register type.
    struct qei_attr_s attr;              //!< Attribute struct which holds the initialization values for this QEI module.
} qei_module_t;

/* Public Enumeration Definitions */

/**
 * @brief Possible error and return values.
 *
 * @details This enumeration defines all the possible values which may be returned from a function
 * in the QEI module code. Each value describes a different outcome or error.
 */
enum qei_error_e
{
    QEI_E_NONE   = 0,  //!< No error has occurred, a successful return value.
    QEI_E_MODULE = -1, //!< The module was invalid (either module or base_address were NULL). No changes were made.
    QEI_E_IGNORE = -2, //!< There was a conflicting setting that didn't allow the function to continue, all changes were ignored.
    QEI_E_INPUT  = -3, //!< The input to the function was invalid or unknown, no changes were made.
    QEI_E_OUTPUT = -4  //!< The output was invalid (e.g. NULL output pointer), no changes were made.
};

/**
 * @brief All valid settings for a QEI module.
 *
 * @details This enumeration defines all the valid values which may be assigned to the
 * @em control_settings variable in the attr struct of a QEI module. See the individual values'
 * documentation for descriptions of what each setting does. When a module is initialized using
 * #qei_init these values are used to configure the module.
 */
enum qei_control_settings_e
{
    QEI_TIMER_UDSRC_UPDN    = 0x0000, //!< UPDN bit controls counter direction in timer mode @default
    QEI_TIMER_UDSRC_QEB     = 0x0001, //!< QEBx pin controls counter direction in timer mode
    
    QEI_TIMER_SRC_TCY       = 0x0000, //!< Tcy selected as timer clock source @default
    QEI_TIMER_SRC_QEA       = 0x0002, //!< QEAx selected as timer clock source

    QEI_MASK_TIMER_PRE      = 0x000C, //!< Mask for timer prescale settings
    QEI_TIMER_PRE_1         = 0x0000, //!< Timer prescale 1:1 @default
    QEI_TIMER_PRE_8         = 0x0004, //!< Timer prescale 1:8
    QEI_TIMER_PRE_64        = 0x0008, //!< Timer prescale 1:64
    QEI_TIMER_PRE_256       = 0x000C, //!< Timer prescale 1:256

    QEI_TIMER_NOGATE        = 0x0000, //!< Disable gating in timer mode @default
    QEI_TIMER_GATE          = 0x0010, //!< Enable gating in timer mode

    QEI_DECODER_INDEX_NORST = 0x0000, //!< Don't reset counter on index pulse @default
    QEI_DECODER_INDEX_RST   = 0x0020, //!< Reset counter on index pulse

    QEI_DECODER_UPDN_OUTDIS = 0x0000, //!< Disable direction output on UPDN pin @default
    QEI_DECODER_UPDN_OUTEN  = 0x0040, //!< Enable direction output on UPDN pin

    QEI_DECODER_NOSWAP_AB   = 0x0000, //!< Don't swap A and B input signals @default
    QEI_DECODER_SWAP_AB     = 0x0080, //!< Swap A and B input signals

    QEI_RUN_IN_IDLE         = 0x0000, //!< Continue running QEI module in idle @default
    QEI_STOP_IN_IDLE        = 0x0100, //!< Stop QEI module in idle

    QEI_CNTERR_INT_EN       = 0x0000, //!< Enable interrupts due to count errors @default
    QEI_CNTERR_INT_DIS      = 0x0200, //!< Disable interrupts due to count errors

    QEI_MASK_FILTER_DIV     = 0x7000, //!< Mask for filter divider settings
    QEI_FILTER_DIV_1        = 0x0000, //!< Filter divider 1:1 @default
    QEI_FILTER_DIV_2        = 0x1000, //!< Filter divider 1:2
    QEI_FILTER_DIV_4        = 0x2000, //!< Filter divider 1:4
    QEI_FILTER_DIV_16       = 0x3000, //!< Filter divider 1:16
    QEI_FILTER_DIV_32       = 0x4000, //!< Filter divider 1:32
    QEI_FILTER_DIV_64       = 0x5000, //!< Filter divider 1:64
    QEI_FILTER_DIV_128      = 0x6000, //!< Filter divider 1:128
    QEI_FILTER_DIV_256      = 0x7000, //!< Filter divider 1:256

    QEI_FILTER_DIS          = 0x0000, //!< Use unfiltered signals @default
    QEI_FILTER_EN           = 0x8000  //!< Use filtered signals
};

/**
 * @brief All valid module modes.
 *
 * @details This enumeration defines the valid modes for a QEI module. These are the values which
 * may be passed to #qei_start.
 */
enum qei_mode_e
{
    QEI_MODE_OFF      = 0x0000, //!< Module is off, the counter is not running
    QEI_MODE_TIMER    = 0x0001, //!< Module is in Timer mode
    QEI_MODE_x2_INDEX = 0x0002, //!< Module is in x2 Quadrature Count Mode with Index Reset
    QEI_MODE_x2_MATCH = 0x0003, //!< Module is in x2 Quadrature Count Mode with Match Reset
    QEI_MODE_x4_INDEX = 0x0004, //!< Module is in x4 Quadrature Count Mode with Index Reset
    QEI_MODE_x4_MATCH = 0x0005  //!< Module is in x4 Quadrature Count Mode with Match Reset
};

/**
 * @brief Valid count directions.
 *
 * @details This enumeration defines the values representing the counting directions for POSxCNT.
 */
enum qei_direction_e
{
    QEI_DOWN = 0x0000, //!< Counting down
    QEI_UP   = 0x0001  //!< Counting up
};

/**
 * @brief Valid input signals.
 *
 * @details This enumeration defines the valid input signals (or phase inputs) for a QEI module.
 * These values are used mainly in the #qei_set_index_match function.
 */
enum qei_input_signal_e
{
    QEI_INPUT_A = 0x0000, //!< Phase A input signal
    QEI_INPUT_B = 0x0001  //!< Phase B input signal
};

/* Public Function Declarations */

/**
 * @brief This function initializes the given QEI module using the attr struct contained within. It
 * also sets all registers to their default values (as specified in the datasheet as reset values).
 *
 * @param[in]  module
 *             A pointer to the module to initialize.
 *
 * @return A zero if initialization was successful, a negative number otherwise which corresponds
 * to the type of error which occurred.
 *
 * @see qei_control_settings_e
 * @see qei_error_e
 */
int qei_init(qei_module_t *module);

/**
 * @brief This function sets the required values of the input signal(s) for an index match reset.
 * If the module is in x4 mode with the index enabled then both A and B input signal values must be
 * defined in two calls to this function. If in x2 mode, then only one call must be made. Any calls
 * after that will reset the match value.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  input_signal
 *             The input signal for which to change the match value (i.e. A or B).
 *
 * @param[in]  value
 *             The value to match for the provided input signal. Should be either 0 or 1. Any other
 *             values will be truncated.
 *
 * @return A zero if the change was successful, a negative number otherwise which corresponds
 * to the type of error which occurred.
 *
 * @see qei_error_e
 */
int qei_set_index_match(qei_module_t *module,
                        enum qei_input_signal_e input_signal,
                        unsigned int value);

/**
 * @brief This function starts the QEI module counting. It starts the module up in the provided
 * mode, which may be any of the qei_mode_e values.
 *
 * @param[in]  module
 *             A pointer to the module to start.
 *
 * @param[in]  mode
 *             A value representing the mode to start the module in.
 *
 * @return A zero if the module was started successfully, a negative number otherwise which
 * corresponds to the type of error that occurred.
 *
 * @see qei_mode_e
 * @see qei_error_e
 */
int qei_start(qei_module_t *module,
              enum qei_mode_e mode);

/**
 * @brief This function stops the QEI module from counting. The POSxCNT and MAXxCNT registers are
 * left intact.
 *
 * @param[in]  module
 *             A pointer to the module to stop.
 *
 * @return A zero if stopping the module was successful, a negative number otherwise which
 * corresponds to the type of error which occurred.
 *
 * @see qei_error_e
 */
int qei_stop(qei_module_t *module);

/**
 * @brief This function sets the value of the UPDN bit which determines the direction of count of
 * the POSxCNT register. This function is only valid if the module is in timer mode.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  updn
 *             The value to write to the UPDN bit of QEIxCON. Must be a qei_direction_e value. Any
 *             other values will be truncated.
 *
 * @return A zero if the write was successful, a negative number otherwise which corresponds to the
 * type of error which occurred.
 *
 * @see qei_direction_e
 * @see qei_error_e
 */
int qei_set_updn(qei_module_t *module,
                 unsigned int updn);

/**
 * @brief This function gets the value of the UPDN bit in the QEIxCON register. This value
 * corresponds to the current direction of count of the module.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @return A qei_direction_e value which will always be positive or zero, or a negative number in
 * the case of an error. The negative number will correspond to the type of error which occurred.
 *
 * @see qei_direction_e
 * @see qei_error_e
 */
int qei_get_updn(qei_module_t *module);

/**
 * @brief This function gets the current value of the index pin.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @return The value of the index pin, where 0 is low and 1 is high. If the return value is
 * negative an error occurred and the value will correspond to the type of error which occurred.
 *
 * @see qei_error_e
 */
int qei_get_index(qei_module_t *module);

/**
 * @brief This function clears the CNTERR bit of the QEIxCON register. This should be done after a
 * position counting error has occurred (e.g. during index reset mode the max count was
 * overflowed).
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @return A zero if the change was successful, a negative number corresponding to the type of
 * error otherwise.
 *
 * @see qei_error_e
 *
 * @note The CNTERR bit is only applicable when the QEI mode is using index to reset the counter.
 */
int qei_clr_cnterr(qei_module_t *module);

/**
 * @brief This function returns whether or not there has been a counting error. A counting error
 * can occur if, for example, MAXxCNT + 1 has been reach during index reset mode. If an error has
 * occurred it must be manually cleared using qei_clr_cnterr() before another error can be
 * generated.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @return Returns the status of the CNTERR bit in the QEIxCON register. No error has occurred if
 * zero is returned, an error has occurred if a one is returned. If a negative number is returned
 * the read was unsuccessful and the negative number corresponds to the type of error that
 * occurred.
 *
 * @see qei_error_e
 *
 * @note The CNTERR bit is only applicable when the QEI mode is using index to reset the counter.
 */
int qei_get_cnterr(qei_module_t *module);

/**
 * @brief This function sets the POSxCNT register of the specified QEI module to @em poscnt. This
 * function is not atomic. If the counter is running the value may change while this function is
 * executing.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  poscnt
 *             The new value to write to POSxCNT.
 *
 * @return A zero if the write was successful. A negative number if the write failed, the number
 * corresponds to the type of error that occurred.
 *
 * @see qei_error_e
 */
int qei_set_poscnt(qei_module_t *module,
                   unsigned int poscnt);

/**
 * @brief This function gets the current value of the POSxCNT from the specified QEI module. This
 * function is not atomic. If the counter is running the value may change while this function is
 * executing.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @return The value stored in the POSxCNT register of the specified module. If the module is
 * invalid then zero is returned.
 */
unsigned int qei_get_poscnt(qei_module_t *module);

/**
 * @brief This function sets the value of the MAXxCNT register of the specified module to the value
 * given by @em maxcnt. This function is not atomic, so if the counter is running it could pass the
 * new value of MAXxCNT before it is written.
 *
 * @param[in]  module
 *             A pointer to the module to change.
 *
 * @param[in]  maxcnt
 *             The new value to write to MAXxCNT in the specified module.
 *
 * @return A zero if the write was successful, a negative number corresponding to the type of error
 * otherwise.
 *
 * @see qei_error_e
 */
int qei_set_maxcnt(qei_module_t *module,
                   unsigned int maxcnt);

/**
 * @brief This function gets the current value of the MAXxCNT register of the specified module.
 *
 * @param[in]  module
 *             A pointer to the module to read from.
 *
 * @return The current value of the MAXxCNT register in the specified module. If the module is
 * invalid a zero is returned.
 */
unsigned int qei_get_maxcnt(qei_module_t *module);


/**
 * @}
 */ // End of QEIModulePublic group

// End include guard
#endif //_QEI_MODULE_H

