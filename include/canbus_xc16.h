/* -*- mode: C; tab-width: 4; -*- */
/** @file {canbus_module.h} **************************************************
 * @brief The CAN bus module controls the settings and input/output of the CAN
 * peripherals in the C30 compiler.
 *
 * @author {Liam Bucci}
 * @date {6/11/2012}
 * @project {Carl Platform}
 * @subproject {Motor Controller}
 * @version {0.2.0}
 *
 ****************************************************************************/

#ifndef _CANBUS_MODULE_H
#define _CANBUS_MODULE_H

// Standard C include files
#include <stdint.h>
#include <stdbool.h>

// CarlLib include files
//#include <CarlMessage>


#define CANBUS_TX_BUFFER_LENGTH 8
#define CANBUS_RX_BUFFER_LENGTH 24


/** Module Public Enumerations
 * These enumerations define valid hardware settings.
 */
enum bit_timing_e
{
    // Baud Rate Prescaler (Fcan = Fcy)
    // TQ = N x Tcy
    // 6-bits starting at bit 0
    CAN_BAUD_PRE_1TCY  = 0x00000000,
    CAN_BAUD_PRE_2TCY  = 0x00000001,
    CAN_BAUD_PRE_3TCY  = 0x00000002,
    CAN_BAUD_PRE_4TCY  = 0x00000003,
    CAN_BAUD_PRE_5TCY  = 0x00000004,
    CAN_BAUD_PRE_6TCY  = 0x00000005,
    CAN_BAUD_PRE_7TCY  = 0x00000006,
    CAN_BAUD_PRE_8TCY  = 0x00000007,
    CAN_BAUD_PRE_9TCY  = 0x00000008,
    CAN_BAUD_PRE_10TCY = 0x00000009,
    CAN_BAUD_PRE_11TCY = 0x0000000A,
    CAN_BAUD_PRE_12TCY = 0x0000000B,
    CAN_BAUD_PRE_13TCY = 0x0000000C,
    CAN_BAUD_PRE_14TCY = 0x0000000D,
    CAN_BAUD_PRE_15TCY = 0x0000000E,
    CAN_BAUD_PRE_16TCY = 0x0000000F,
    CAN_BAUD_PRE_17TCY = 0x00000010,
    CAN_BAUD_PRE_18TCY = 0x00000011,
    CAN_BAUD_PRE_19TCY = 0x00000012,
    CAN_BAUD_PRE_20TCY = 0x00000013,
    CAN_BAUD_PRE_21TCY = 0x00000014,
    CAN_BAUD_PRE_22TCY = 0x00000015,
    CAN_BAUD_PRE_23TCY = 0x00000016,
    CAN_BAUD_PRE_24TCY = 0x00000017,
    CAN_BAUD_PRE_25TCY = 0x00000018,
    CAN_BAUD_PRE_26TCY = 0x00000019,
    CAN_BAUD_PRE_27TCY = 0x0000001A,
    CAN_BAUD_PRE_28TCY = 0x0000001B,
    CAN_BAUD_PRE_29TCY = 0x0000001C,
    CAN_BAUD_PRE_30TCY = 0x0000001D,
    CAN_BAUD_PRE_31TCY = 0x0000001E,
    CAN_BAUD_PRE_32TCY = 0x0000001F,
    CAN_BAUD_PRE_33TCY = 0x00000020,
    CAN_BAUD_PRE_34TCY = 0x00000021,
    CAN_BAUD_PRE_35TCY = 0x00000022,
    CAN_BAUD_PRE_36TCY = 0x00000023,
    CAN_BAUD_PRE_37TCY = 0x00000024,
    CAN_BAUD_PRE_38TCY = 0x00000025,
    CAN_BAUD_PRE_39TCY = 0x00000026,
    CAN_BAUD_PRE_40TCY = 0x00000027,
    CAN_BAUD_PRE_41TCY = 0x00000028,
    CAN_BAUD_PRE_42TCY = 0x00000029,
    CAN_BAUD_PRE_43TCY = 0x0000002A,
    CAN_BAUD_PRE_44TCY = 0x0000002B,
    CAN_BAUD_PRE_45TCY = 0x0000002C,
    CAN_BAUD_PRE_46TCY = 0x0000002D,
    CAN_BAUD_PRE_47TCY = 0x0000002E,
    CAN_BAUD_PRE_48TCY = 0x0000002F,
    CAN_BAUD_PRE_49TCY = 0x00000030,
    CAN_BAUD_PRE_50TCY = 0x00000031,
    CAN_BAUD_PRE_51TCY = 0x00000032,
    CAN_BAUD_PRE_52TCY = 0x00000033,
    CAN_BAUD_PRE_53TCY = 0x00000034,
    CAN_BAUD_PRE_54TCY = 0x00000035,
    CAN_BAUD_PRE_55TCY = 0x00000036,
    CAN_BAUD_PRE_56TCY = 0x00000037,
    CAN_BAUD_PRE_57TCY = 0x00000038,
    CAN_BAUD_PRE_58TCY = 0x00000039,
    CAN_BAUD_PRE_59TCY = 0x0000003A,
    CAN_BAUD_PRE_60TCY = 0x0000003B,
    CAN_BAUD_PRE_61TCY = 0x0000003C,
    CAN_BAUD_PRE_62TCY = 0x0000003D,
    CAN_BAUD_PRE_63TCY = 0x0000003E,
    CAN_BAUD_PRE_64TCY = 0x0000003F,
    
    // Sync Jump Width
    // 2-bits starting at bit 6
    SYNC_JUMP_1TQ  = 0x00000000,
    SYNC_JUMP_2TQ  = 0x00000040,
    SYNC_JUMP_3TQ  = 0x00000080,
    SYNC_JUMP_4TQ  = 0x000000C0,

    // Propagation Segment
    // 3-bits starting at bit 8
    PROP_SEG_1TQ   = 0x00000000,
    PROP_SEG_2TQ   = 0x00000100,
    PROP_SEG_3TQ   = 0x00000200,
    PROP_SEG_4TQ   = 0x00000300,
    PROP_SEG_5TQ   = 0x00000400,
    PROP_SEG_6TQ   = 0x00000500,
    PROP_SEG_7TQ   = 0x00000600,
    PROP_SEG_8TQ   = 0x00000700,

    // Phase Segment 1
    // 3-bits starting at bit 11
    PHASE_SEG1_1TQ = 0x00000000,
    PHASE_SEG1_2TQ = 0x00000800,
    PHASE_SEG1_3TQ = 0x00001000,
    PHASE_SEG1_4TQ = 0x00001800,
    PHASE_SEG1_5TQ = 0x00002000,
    PHASE_SEG1_6TQ = 0x00002800,
    PHASE_SEG1_7TQ = 0x00003000,
    PHASE_SEG1_8TQ = 0x00003800,

    // Phase Segment 2
    // 3-bits starting at bit 14
    PHASE_SEG2_1TQ = 0x00000000,
    PHASE_SEG2_2TQ = 0x00004000,
    PHASE_SEG2_3TQ = 0x00008000,
    PHASE_SEG2_4TQ = 0x0000C000,
    PHASE_SEG2_5TQ = 0x00010000,
    PHASE_SEG2_6TQ = 0x00014000,
    PHASE_SEG2_7TQ = 0x00018000,
    PHASE_SEG2_8TQ = 0x0001C000,

    // Phase Segment 2 Programmable
    // 1-bit starting at bit 17
    DISABLE_PHASE_SEG2_PROG = 0x00000000,
    ENABLE_PHASE_SEG2_PROG  = 0x00020000,
    
    // Number of Samples
    // 1-bit starting at bit 18
    SINGLE_SAMPLE = 0x00000000,
    TRIPLE_SAMPLE = 0x00040000,
    
    // Wake-up on CAN filter hit
    // 1-bit starting at bit 19
    DISABLE_WAKE = 0x00000000,
    ENABLE_WAKE  = 0x00080000
};

enum bit_timing_mask_e
{
    CAN_BAUD_PRE_MASK   = 0x0000003F,
    CAN_SYNC_JUMP_MASK  = 0x000000C0,
    CAN_PROP_SEG_MASK   = 0x00000700,
    CAN_PHASE_SEG1_MASK = 0x00003800,
    CAN_PHASE_SEG2_MASK = 0x0001C000
};

enum open_mode_e
{
    CAN_MODE_NORMAL    = 0x0000,
    CAN_MODE_DISABLE   = 0x0001,
    CAN_MODE_LOOP      = 0x0002,
    CAN_MODE_LISTEN    = 0x0003,
    CAN_MODE_CONFIG    = 0x0004,
    CAN_MODE_LISTENALL = 0x0007,
    CAN_READ           = 0x0008,
    CAN_WRITE          = 0x0010
};


/** Module Public Data Structures
 * These data structures are used to hold specific data formats used in the
 * CAN bus module.
 */



/** Module Struct Definition
 * This structure is the actual module, it contains all the settings, data,
 * and functions of the module. It must include a run_period variable, an
 * init function, and a run function.
 */
typedef struct canbus_module_s
{
    /** Module Settings
     * These settings control how the module functions. The variables are
     * public and may be changed by outside functions. However, do so only
     * if you know what you are doing! Otherwise use the accessor functions.
     */
    unsigned int run_period;
    volatile unsigned int *can_base_address;
    volatile unsigned int *tx_dma_base_address;
    volatile unsigned int *rx_dma_base_address;
    uint32_t bit_timing;
    unsigned int open_mode;
    

    /** Module Data
     * These variables keep track of the state of the hardware controlled by
     * this module. They can also keep track of the internal state of the
     * module, but they are public and may be accessed by outside functions.
     * However, do not directly change these variables unless you know what
     * you are doing. Use the accessors/mutators to modify the module.
     */
    volatile unsigned int *can_buffer;
    volatile uint8_t tx_locks;
    volatile uint8_t tx_ready;
    volatile bool rx_lock;
    

    /** Module Functions
     * These are the public functions of the module, other modules (or the
     * kernel) may call them.
     */
    int (*init)(struct canbus_module_s *);
    int (*run)(struct canbus_module_s *);
    int (*open)(struct canbus_module_s *, unsigned int);
    int (*write)(struct canbus_module_s *, const carlmessage_t *);
    int (*read)(struct canbus_module_s *, carlmessage_t *);
    int (*close)(struct canbus_module_s *);
    int (*flush_tx)(struct canbus_module_s *);
    int (*flush_rx)(struct canbus_module_s *);
    int (*set_bit_timing)(struct canbus_module_s *, uint32_t);
    int (*set_mask)(struct canbus_module_s *, unsigned int, carlmessage_header_t);
    int (*set_filter)(struct canbus_module_s *, unsigned int, carlmessage_header_t, unsigned int);
    int (*enable_filter)(struct canbus_module_s *, unsigned int);
    int (*disable_filter)(struct canbus_module_s *, unsigned int);
} canbus_module_t;


/** Module Constructor
 * This function is used to construct a new, default initialized
 * canbus_module. It will return a pointer to a malloc'd canbus_module_t. Be
 * sure to free it after you're done using it!
 */
canbus_module_t * new_canbus_module(void);

/** Module Destructor
 * This function is used to clean up a canbus_module_t. It will free any
 * memory that has been malloc'd and disable the hardware.
 */
void delete_canbus_module(canbus_module_t *module);


#endif // _CANBUS_MODULE_H

