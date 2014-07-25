/* -*- mode: C; tab-width: 4; -*- */

/**
 * @file canbus_xc16.h
 *
 * @brief The CAN bus module controls the settings and input/output of the CAN
 * peripherals in the C30 compiler.
 *
 * @author Liam Bucci
 * @date 6/11/2012
 * @carlnumber FIRM-0009
 * @version 0.4.0
 */

// Include guard
#ifndef CANBUS_XC16_H_
#define CANBUS_XC16_H_

/**
 * @defgroup canbus CAN Bus Module
 *
 * @brief The CAN Bus Module controls all functionality dealing with the CAN bus hardware on the
 * PIC microcontroller. It can be used to configure the hardware, read and write, and set up
 * interrupts and other notifications.
 *
 * @details Nothing here.
 *
 * @{
 */

// Standard C include files
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Include other modules
#include <dma_channel.h>

#define CANBUS_TX_BUFFER_LENGTH 8
#define CANBUS_RX_BUFFER_LENGTH 24


/* ***** Public Enumerations ***** */

/**
 * @brief Contains the valid settings for the CAN module's attribute object.
 *
 * @details This enumeration defines all of the valid settings which may be used to construct a
 * CANBus's attribute object (@ref canbus_attr_s). The different groupings of settings represent
 * different settings within the attribute object. For example, the CANBUS_BIT_TIMING_PRE_x values
 * are used to set the @ref canbus_attr_s.bit_timing_pre value:
 *
 * @code
 * myattr.bit_timing_pre = CANBUS_BIT_TIMING_PRE_3TCY;
 * @endcode
 *
 * @see canbus_attr_s
 *
 * @public
 */
enum canbus_attr_e
{       
    // Baud Rate Prescaler (Fcan = Fcy)
    // TQ = N x Tcy
    // 6-bits
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_PRE_1TCY  = 0x0000, /**< Baud rate prescaler set to 1:1. @default */
    CANBUS_BIT_TIMING_PRE_2TCY  = 0x0001, /**< Baud rate prescaler set to 1:2. */
    CANBUS_BIT_TIMING_PRE_3TCY  = 0x0002, /**< Baud rate prescaler set to 1:3. */
    CANBUS_BIT_TIMING_PRE_4TCY  = 0x0003, /**< Baud rate prescaler set to 1:4. */
    CANBUS_BIT_TIMING_PRE_5TCY  = 0x0004, /**< Baud rate prescaler set to 1:5. */
    CANBUS_BIT_TIMING_PRE_6TCY  = 0x0005, /**< Baud rate prescaler set to 1:6. */
    CANBUS_BIT_TIMING_PRE_7TCY  = 0x0006, /**< Baud rate prescaler set to 1:7. */
    CANBUS_BIT_TIMING_PRE_8TCY  = 0x0007, /**< Baud rate prescaler set to 1:8. */
    CANBUS_BIT_TIMING_PRE_9TCY  = 0x0008, /**< Baud rate prescaler set to 1:9. */
    CANBUS_BIT_TIMING_PRE_10TCY = 0x0009, /**< Baud rate prescaler set to 1:10. */
    CANBUS_BIT_TIMING_PRE_11TCY = 0x000A, /**< Baud rate prescaler set to 1:11. */
    CANBUS_BIT_TIMING_PRE_12TCY = 0x000B, /**< Baud rate prescaler set to 1:12. */
    CANBUS_BIT_TIMING_PRE_13TCY = 0x000C, /**< Baud rate prescaler set to 1:13. */
    CANBUS_BIT_TIMING_PRE_14TCY = 0x000D, /**< Baud rate prescaler set to 1:14. */
    CANBUS_BIT_TIMING_PRE_15TCY = 0x000E, /**< Baud rate prescaler set to 1:15. */
    CANBUS_BIT_TIMING_PRE_16TCY = 0x000F, /**< Baud rate prescaler set to 1:16. */
    CANBUS_BIT_TIMING_PRE_17TCY = 0x0010, /**< Baud rate prescaler set to 1:17. */
    CANBUS_BIT_TIMING_PRE_18TCY = 0x0011, /**< Baud rate prescaler set to 1:18. */
    CANBUS_BIT_TIMING_PRE_19TCY = 0x0012, /**< Baud rate prescaler set to 1:19. */
    CANBUS_BIT_TIMING_PRE_20TCY = 0x0013, /**< Baud rate prescaler set to 1:20. */
    CANBUS_BIT_TIMING_PRE_21TCY = 0x0014, /**< Baud rate prescaler set to 1:21. */
    CANBUS_BIT_TIMING_PRE_22TCY = 0x0015, /**< Baud rate prescaler set to 1:22. */
    CANBUS_BIT_TIMING_PRE_23TCY = 0x0016, /**< Baud rate prescaler set to 1:23. */
    CANBUS_BIT_TIMING_PRE_24TCY = 0x0017, /**< Baud rate prescaler set to 1:24. */
    CANBUS_BIT_TIMING_PRE_25TCY = 0x0018, /**< Baud rate prescaler set to 1:25. */
    CANBUS_BIT_TIMING_PRE_26TCY = 0x0019, /**< Baud rate prescaler set to 1:26. */
    CANBUS_BIT_TIMING_PRE_27TCY = 0x001A, /**< Baud rate prescaler set to 1:27. */
    CANBUS_BIT_TIMING_PRE_28TCY = 0x001B, /**< Baud rate prescaler set to 1:28. */
    CANBUS_BIT_TIMING_PRE_29TCY = 0x001C, /**< Baud rate prescaler set to 1:29. */
    CANBUS_BIT_TIMING_PRE_30TCY = 0x001D, /**< Baud rate prescaler set to 1:30. */
    CANBUS_BIT_TIMING_PRE_31TCY = 0x001E, /**< Baud rate prescaler set to 1:31. */
    CANBUS_BIT_TIMING_PRE_32TCY = 0x001F, /**< Baud rate prescaler set to 1:32. */
    CANBUS_BIT_TIMING_PRE_33TCY = 0x0020, /**< Baud rate prescaler set to 1:33. */
    CANBUS_BIT_TIMING_PRE_34TCY = 0x0021, /**< Baud rate prescaler set to 1:34. */
    CANBUS_BIT_TIMING_PRE_35TCY = 0x0022, /**< Baud rate prescaler set to 1:35. */
    CANBUS_BIT_TIMING_PRE_36TCY = 0x0023, /**< Baud rate prescaler set to 1:36. */
    CANBUS_BIT_TIMING_PRE_37TCY = 0x0024, /**< Baud rate prescaler set to 1:37. */
    CANBUS_BIT_TIMING_PRE_38TCY = 0x0025, /**< Baud rate prescaler set to 1:38. */
    CANBUS_BIT_TIMING_PRE_39TCY = 0x0026, /**< Baud rate prescaler set to 1:39. */
    CANBUS_BIT_TIMING_PRE_40TCY = 0x0027, /**< Baud rate prescaler set to 1:40. */
    CANBUS_BIT_TIMING_PRE_41TCY = 0x0028, /**< Baud rate prescaler set to 1:41. */
    CANBUS_BIT_TIMING_PRE_42TCY = 0x0029, /**< Baud rate prescaler set to 1:42. */
    CANBUS_BIT_TIMING_PRE_43TCY = 0x002A, /**< Baud rate prescaler set to 1:43. */
    CANBUS_BIT_TIMING_PRE_44TCY = 0x002B, /**< Baud rate prescaler set to 1:44. */
    CANBUS_BIT_TIMING_PRE_45TCY = 0x002C, /**< Baud rate prescaler set to 1:45. */
    CANBUS_BIT_TIMING_PRE_46TCY = 0x002D, /**< Baud rate prescaler set to 1:46. */
    CANBUS_BIT_TIMING_PRE_47TCY = 0x002E, /**< Baud rate prescaler set to 1:47. */
    CANBUS_BIT_TIMING_PRE_48TCY = 0x002F, /**< Baud rate prescaler set to 1:48. */
    CANBUS_BIT_TIMING_PRE_49TCY = 0x0030, /**< Baud rate prescaler set to 1:49. */
    CANBUS_BIT_TIMING_PRE_50TCY = 0x0031, /**< Baud rate prescaler set to 1:50. */
    CANBUS_BIT_TIMING_PRE_51TCY = 0x0032, /**< Baud rate prescaler set to 1:51. */
    CANBUS_BIT_TIMING_PRE_52TCY = 0x0033, /**< Baud rate prescaler set to 1:52. */
    CANBUS_BIT_TIMING_PRE_53TCY = 0x0034, /**< Baud rate prescaler set to 1:53. */
    CANBUS_BIT_TIMING_PRE_54TCY = 0x0035, /**< Baud rate prescaler set to 1:54. */
    CANBUS_BIT_TIMING_PRE_55TCY = 0x0036, /**< Baud rate prescaler set to 1:55. */
    CANBUS_BIT_TIMING_PRE_56TCY = 0x0037, /**< Baud rate prescaler set to 1:56. */
    CANBUS_BIT_TIMING_PRE_57TCY = 0x0038, /**< Baud rate prescaler set to 1:57. */
    CANBUS_BIT_TIMING_PRE_58TCY = 0x0039, /**< Baud rate prescaler set to 1:58. */
    CANBUS_BIT_TIMING_PRE_59TCY = 0x003A, /**< Baud rate prescaler set to 1:59. */
    CANBUS_BIT_TIMING_PRE_60TCY = 0x003B, /**< Baud rate prescaler set to 1:60. */
    CANBUS_BIT_TIMING_PRE_61TCY = 0x003C, /**< Baud rate prescaler set to 1:61. */
    CANBUS_BIT_TIMING_PRE_62TCY = 0x003D, /**< Baud rate prescaler set to 1:62. */
    CANBUS_BIT_TIMING_PRE_63TCY = 0x003E, /**< Baud rate prescaler set to 1:63. */
    CANBUS_BIT_TIMING_PRE_64TCY = 0x003F, /**< Baud rate prescaler set to 1:64. */
    
    // Sync Jump Width
    // 2-bits
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_SYNC_JUMP_1TQ = 0x0000, /**< Synchronization jump width set to 1TQ.
                                                 @default */
    CANBUS_BIT_TIMING_SYNC_JUMP_2TQ = 0x0001, /**< Synchronization jump width set to 2TQ. */
    CANBUS_BIT_TIMING_SYNC_JUMP_3TQ = 0x0002, /**< Synchronization jump width set to 3TQ. */
    CANBUS_BIT_TIMING_SYNC_JUMP_4TQ = 0x0003, /**< Synchronization jump width set to 4TQ. */

    // Propagation Segment
    // 3-bits
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_PROP_SEG_1TQ = 0x0000, /**< Propagation segment set to 1TQ. @default */
    CANBUS_BIT_TIMING_PROP_SEG_2TQ = 0x0001, /**< Propagation segment set to 2TQ. */
    CANBUS_BIT_TIMING_PROP_SEG_3TQ = 0x0002, /**< Propagation segment set to 3TQ. */
    CANBUS_BIT_TIMING_PROP_SEG_4TQ = 0x0003, /**< Propagation segment set to 4TQ. */
    CANBUS_BIT_TIMING_PROP_SEG_5TQ = 0x0004, /**< Propagation segment set to 5TQ. */
    CANBUS_BIT_TIMING_PROP_SEG_6TQ = 0x0005, /**< Propagation segment set to 6TQ. */
    CANBUS_BIT_TIMING_PROP_SEG_7TQ = 0x0006, /**< Propagation segment set to 7TQ. */
    CANBUS_BIT_TIMING_PROP_SEG_8TQ = 0x0007, /**< Propagation segment set to 8TQ. */

    // Phase Segment 1
    // 3-bits
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_PHASE_SEG1_1TQ = 0x0000, /**< Phase segment 1 set to 1TQ. @default */
    CANBUS_BIT_TIMING_PHASE_SEG1_2TQ = 0x0001, /**< Phase segment 1 set to 2TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG1_3TQ = 0x0002, /**< Phase segment 1 set to 3TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG1_4TQ = 0x0003, /**< Phase segment 1 set to 4TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG1_5TQ = 0x0004, /**< Phase segment 1 set to 5TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG1_6TQ = 0x0005, /**< Phase segment 1 set to 6TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG1_7TQ = 0x0006, /**< Phase segment 1 set to 7TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG1_8TQ = 0x0007, /**< Phase segment 1 set to 8TQ. */

    // Phase Segment 2
    // 3-bits
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_PHASE_SEG2_1TQ = 0x0000, /**< Phase segment 2 set to 1TQ. @default */
    CANBUS_BIT_TIMING_PHASE_SEG2_2TQ = 0x0001, /**< Phase segment 2 set to 2TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG2_3TQ = 0x0002, /**< Phase segment 2 set to 3TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG2_4TQ = 0x0003, /**< Phase segment 2 set to 4TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG2_5TQ = 0x0004, /**< Phase segment 2 set to 5TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG2_6TQ = 0x0005, /**< Phase segment 2 set to 6TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG2_7TQ = 0x0006, /**< Phase segment 2 set to 7TQ. */
    CANBUS_BIT_TIMING_PHASE_SEG2_8TQ = 0x0007, /**< Phase segment 2 set to 8TQ. */

    // Phase Segment 2 Programmable
    // 1-bit
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_PHASE_SEG2_PROG_DIS = 0x0000, /**< Phase segment 2 is programmable.
                                                       @default */
    CANBUS_BIT_TIMING_PHASE_SEG2_PROG_EN  = 0x0001, /**< Phase segment 2 is the greater of phase
                                                       segment 1 or information processing time
                                                       (IPT). */
    
    // Number of Samples
    // 1-bit
    // These values are directly related to hardware values, DO NOT CHANGE!
    CANBUS_BIT_TIMING_SAMPLE_SINGLE = 0x0000, /**< Sample once at the sample point. @default */
    CANBUS_BIT_TIMING_SAMPLE_TRIPLE = 0x0001, /**< Sample three times at the sample point and use
                                                 the mode value. */
    
    // Wake-up on CAN filter hit
    // 1-bit
    CANBUS_MODULE_WAKEUP_BITMASK = 0x0001, /**< Bitmask for wake-up on CAN filter hit setting. */
    CANBUS_MODULE_WAKEUP_DIS     = 0x0000, /**< Disable wake-up on CAN filter hit. @default */
    CANBUS_MODULE_WAKEUP_EN      = 0x0001, /**< Enable wake-up on CAN filter hit. */

    // CPU idle mode operation
    // 1-bit
    CANBUS_MODULE_CPUIDLE_CONTINUE = 0x0000, /**< Continue CAN module during CPU idle mode.
                                                @default */
    CANBUS_MODULE_CPUIDLE_STOP     = 0x0001, /**< Stop CAN module during CPU idle mode. */

    // Message timestamping
    // 1-bit
    CANBUS_MODULE_TIMESTAMP_DIS = 0x0000, /**< Disable timestamping on message receipt.
                                             @default */
    CANBUS_MODULE_TIMESTAMP_EN  = 0x0001, /**< Enable timestamping on message receipt. Triggers
                                             Input Capture 2 (must be set up to capture). */

    // FIFO start buffer
    // 5-bits
    // Note: Buffer 8 is the default start, so it has a value of 0x0000.
    // Note: FIFO may not be disabled
    CANBUS_FIFO_START_B8   = 0x0000, /**< Select buffer 8 as FIFO start. @default */
    CANBUS_FIFO_START_B0   = 0x0001, /**< Select buffer 0 as FIFO start. */
    CANBUS_FIFO_START_B1   = 0x0002, /**< Select buffer 1 as FIFO start. */
    CANBUS_FIFO_START_B2   = 0x0003, /**< Select buffer 2 as FIFO start. */
    CANBUS_FIFO_START_B3   = 0x0004, /**< Select buffer 3 as FIFO start. */
    CANBUS_FIFO_START_B4   = 0x0005, /**< Select buffer 4 as FIFO start. */
    CANBUS_FIFO_START_B5   = 0x0006, /**< Select buffer 5 as FIFO start. */
    CANBUS_FIFO_START_B6   = 0x0007, /**< Select buffer 6 as FIFO start. */
    CANBUS_FIFO_START_B7   = 0x0008, /**< Select buffer 7 as FIFO start. */
    CANBUS_FIFO_START_B9   = 0x0009, /**< Select buffer 9 as FIFO start. */
    CANBUS_FIFO_START_B10  = 0x000A, /**< Select buffer 10 as FIFO start. */
    CANBUS_FIFO_START_B11  = 0x000B, /**< Select buffer 11 as FIFO start. */
    CANBUS_FIFO_START_B12  = 0x000C, /**< Select buffer 12 as FIFO start. */
    CANBUS_FIFO_START_B13  = 0x000D, /**< Select buffer 13 as FIFO start. */
    CANBUS_FIFO_START_B14  = 0x000E, /**< Select buffer 14 as FIFO start. */
    CANBUS_FIFO_START_B15  = 0x000F, /**< Select buffer 15 as FIFO start. */
    CANBUS_FIFO_START_B16  = 0x0010, /**< Select buffer 16 as FIFO start. */
    CANBUS_FIFO_START_B17  = 0x0011, /**< Select buffer 17 as FIFO start. */
    CANBUS_FIFO_START_B18  = 0x0012, /**< Select buffer 18 as FIFO start. */
    CANBUS_FIFO_START_B19  = 0x0013, /**< Select buffer 19 as FIFO start. */
    CANBUS_FIFO_START_B20  = 0x0014, /**< Select buffer 20 as FIFO start. */
    CANBUS_FIFO_START_B21  = 0x0015, /**< Select buffer 21 as FIFO start. */
    CANBUS_FIFO_START_B22  = 0x0016, /**< Select buffer 22 as FIFO start. */
    CANBUS_FIFO_START_B23  = 0x0017, /**< Select buffer 23 as FIFO start. */
    CANBUS_FIFO_START_B24  = 0x0018, /**< Select buffer 24 as FIFO start. */
    CANBUS_FIFO_START_B25  = 0x0019, /**< Select buffer 25 as FIFO start. */
    CANBUS_FIFO_START_B26  = 0x001A, /**< Select buffer 26 as FIFO start. */
    CANBUS_FIFO_START_B27  = 0x001B, /**< Select buffer 27 as FIFO start. */
    CANBUS_FIFO_START_B28  = 0x001C, /**< Select buffer 28 as FIFO start. */
    CANBUS_FIFO_START_B29  = 0x001D, /**< Select buffer 29 as FIFO start. */
    CANBUS_FIFO_START_B30  = 0x001E, /**< Select buffer 30 as FIFO start. */
    CANBUS_FIFO_START_B31  = 0x001F, /**< Select buffer 31 as FIFO start. */

    // FIFO length
    // 3-bits
    // Note: Length 24 is the default, so it has a value of 0x0000.
    // Note: FIFO may not be disabled.
    CANBUS_FIFO_LENGTH_24 = 0x0000, /**< FIFO is 24 buffers long. @default */
    CANBUS_FIFO_LENGTH_4  = 0x0001, /**< FIFO is 4 buffers long. */
    CANBUS_FIFO_LENGTH_6  = 0x0002, /**< FIFO is 6 buffers long. */
    CANBUS_FIFO_LENGTH_8  = 0x0003, /**< FIFO is 8 buffers long. */
    CANBUS_FIFO_LENGTH_12 = 0x0004, /**< FIFO is 12 buffers long. */
    CANBUS_FIFO_LENGTH_16 = 0x0005, /**< FIFO is 16 buffers long. */
    CANBUS_FIFO_LENGTH_32 = 0x0006, /**< FIFO is 32 buffers long. */
};


/**
 * @brief Contains the valid modes that the CANBus object can take on. The module can only take on
 * a single mode at any one time.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_mode_e
{
    CANBUS_MODE_DISABLE = 0x0000,
    CANBUS_MODE_NORMAL  = 0x0001,
    CANBUS_MODE_LISTEN  = 0x0002,
    
    CANBUS_MODE_DEBUG_LOOPBACK  = 0x0003,
    CANBUS_MODE_DEBUG_LISTENALL = 0x0004
};
typedef enum canbus_mode_e canbus_mode_t;

/**
 * @brief Constants defining the valid masks in the CAN bus hardware.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_mask_e
{
    CANBUS_MASK_M0   = 0x0001,
    CANBUS_MASK_M1   = 0x0002,
    CANBUS_MASK_M2   = 0x0003,
    CANBUS_MASK_NONE = 0x0010,
    CANBUS_MASK_ALL  = 0x0011
};
typedef enum canbus_mask_e canbus_mask_t;

/**
 * @brief Constants defining the various valid filters in the CAN bus hardware.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_filter_e
{
    CANBUS_FILTER_F0   = 0x0000, /**< Select filter 0. */
    CANBUS_FILTER_F1   = 0x0001, /**< Select filter 1. */
    CANBUS_FILTER_F2   = 0x0002, /**< Select filter 2. */
    CANBUS_FILTER_F3   = 0x0003, /**< Select filter 3. */
    CANBUS_FILTER_F4   = 0x0004, /**< Select filter 4. */
    CANBUS_FILTER_F5   = 0x0005, /**< Select filter 5. */
    CANBUS_FILTER_F6   = 0x0006, /**< Select filter 6. */
    CANBUS_FILTER_F7   = 0x0007, /**< Select filter 7. */
    CANBUS_FILTER_F8   = 0x0008, /**< Select filter 8. */
    CANBUS_FILTER_F9   = 0x0009, /**< Select filter 9. */
    CANBUS_FILTER_F10  = 0x000A, /**< Select filter 10. */
    CANBUS_FILTER_F11  = 0x000B, /**< Select filter 11. */
    CANBUS_FILTER_F12  = 0x000C, /**< Select filter 12. */
    CANBUS_FILTER_F13  = 0x000D, /**< Select filter 13. */
    CANBUS_FILTER_F14  = 0x000E, /**< Select filter 14. */
    CANBUS_FILTER_F15  = 0x000F, /**< Select filter 15. */
    CANBUS_FILTER_NONE = 0x0010, /**< Select no filters. Null option. */
    CANBUS_FILTER_ALL  = 0x0011  /**< Select all filters. */
};
typedef enum canbus_filter_e canbus_filter_t;

/**
 * @brief Constants defining the various valid buffers in the CAN bus hardware.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_buffer_e
{
    // These values are directly related to hardware values, DO NOT CHANGE!!
    // The values are expected to be the same as the buffer numbers.
    CANBUS_BUFFER_B0   = 0x0000, /**< Select buffer 0. */
    CANBUS_BUFFER_B1   = 0x0001, /**< Select buffer 1. */
    CANBUS_BUFFER_B2   = 0x0002, /**< Select buffer 2. */
    CANBUS_BUFFER_B3   = 0x0003, /**< Select buffer 3. */
    CANBUS_BUFFER_B4   = 0x0004, /**< Select buffer 4. */
    CANBUS_BUFFER_B5   = 0x0005, /**< Select buffer 5. */
    CANBUS_BUFFER_B6   = 0x0006, /**< Select buffer 6. */
    CANBUS_BUFFER_B7   = 0x0007, /**< Select buffer 7. */
    CANBUS_BUFFER_B8   = 0x0008, /**< Select buffer 8. */
    CANBUS_BUFFER_B9   = 0x0009, /**< Select buffer 9. */
    CANBUS_BUFFER_B10  = 0x000A, /**< Select buffer 10. */
    CANBUS_BUFFER_B11  = 0x000B, /**< Select buffer 11. */
    CANBUS_BUFFER_B12  = 0x000C, /**< Select buffer 12. */
    CANBUS_BUFFER_B13  = 0x000D, /**< Select buffer 13. */
    CANBUS_BUFFER_B14  = 0x000E, /**< Select buffer 14. */
    CANBUS_BUFFER_B15  = 0x000F, /**< Select buffer 15. */
    CANBUS_BUFFER_B16  = 0x0010, /**< Select buffer 16. */
    CANBUS_BUFFER_B17  = 0x0011, /**< Select buffer 17. */
    CANBUS_BUFFER_B18  = 0x0012, /**< Select buffer 18. */
    CANBUS_BUFFER_B19  = 0x0013, /**< Select buffer 19. */
    CANBUS_BUFFER_B20  = 0x0014, /**< Select buffer 20. */
    CANBUS_BUFFER_B21  = 0x0015, /**< Select buffer 21. */
    CANBUS_BUFFER_B22  = 0x0016, /**< Select buffer 22. */
    CANBUS_BUFFER_B23  = 0x0017, /**< Select buffer 23. */
    CANBUS_BUFFER_B24  = 0x0018, /**< Select buffer 24. */
    CANBUS_BUFFER_B25  = 0x0019, /**< Select buffer 25. */
    CANBUS_BUFFER_B26  = 0x001A, /**< Select buffer 26. */
    CANBUS_BUFFER_B27  = 0x001B, /**< Select buffer 27. */
    CANBUS_BUFFER_B28  = 0x001C, /**< Select buffer 28. */
    CANBUS_BUFFER_B29  = 0x001D, /**< Select buffer 29. */
    CANBUS_BUFFER_B30  = 0x001E, /**< Select buffer 30. */
    CANBUS_BUFFER_B31  = 0x001F, /**< Select buffer 31. */
    
    CANBUS_BUFFER_FIFO = 0x0020, /**< Select FIFO buffer. */
    CANBUS_BUFFER_NONE = 0x0021, /**< Select no buffers. Null option. */
    CANBUS_BUFFER_ALL  = 0x0022  /**< Select all buffers. */
};
typedef enum canbus_buffer_e canbus_buffer_t;

/**
 * @brief Constants defining the valid priorities for messages being transmitted.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_priority_e
{
    // These values are directly related to hardware value, DO NOT CHANGE!!
    CANBUS_PRIORITY_LOWEST  = 0x0000, /**< Lowest priority message. */
    CANBUS_PRIORITY_LOW     = 0x0001, /**< Low priority message. */
    CANBUS_PRIORITY_HIGH    = 0x0002, /**< High priority message. */
    CANBUS_PRIORITY_HIGHEST = 0x0003  /**< Highest priority message. */
};
typedef enum canbus_priority_e canbus_priority_t;

/**
 * @brief Constants defining the directions in which a buffer may open.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_direction_e
{
    // These values are directly related to hardware values, DO NOT CHANGE!!
    CANBUS_DIRECTION_RX   = 0x0000, /**< Receive. */
    CANBUS_DIRECTION_TX   = 0x0001, /**< Transmit. */
};
typedef enum canbus_direction_e canbus_direction_t;

/**
 * @brief Constants defining the different events which may trigger the @ref notify() function.
 *
 * @details Nothing here.
 *
 * @public
 */
enum canbus_notice_e
{
    CANBUS_NOTICE_NONE             = 0x0000,
    CANBUS_NOTICE_TX_SUCCESS       = 0x0001,
    CANBUS_NOTICE_RX_SUCCESS       = 0x0002,
    CANBUS_NOTICE_FIFO_ALMOST_FULL = 0x0004,
    CANBUS_NOTICE_OVERFLOW         = 0x0008,
    CANBUS_NOTICE_ERROR            = 0x0010
};
typedef enum canbus_notice_e canbus_notice_t;

/**
 * @brief Contains the possible error values from CANBus functions.
 *
 * @details All error values are negative numbers.
 *
 * @public
 */
typedef enum canbus_error_e
{
    CANBUS_E_NONE   = 0,  /**< No error, success. */
    CANBUS_E_AGAIN  = -1, /**< Resource currently in use, try again later. */
    CANBUS_E_OBJECT = -2, /**< Canbus object was invalid or not usable. */
    CANBUS_E_ALLOC  = -3, /**< Allocation of dynamic memory failed. */
    CANBUS_E_WRITE  = -4, /**< Writing to the buffer failed. */
    CANBUS_E_READ   = -5, /**< Reading from the buffer failed. */
    
    CANBUS_E_ASSERT  = 0x8001, /**< Assertion failed! This is generally an unrecoverable error. */
    CANBUS_E_UNKNOWN = 0x8000  /**< Unknown error! This is generally an unrecoverable error. */
} canbus_error_t;
    


/* ***** Public Data Structures ***** */

/**
 * @brief The header object stores the frame header of a CAN bus message.
 *
 * @details The header object stores the ID information from a CAN bus message. This includes both
 * the standard ID (SID) and the extended ID (EID) as well as the IDE bit to determine if the
 * message should send the SID or both the SID and the EID. The RTR bit determines if the message
 * should request a remote frame.
 *
 * Remote transmit request (RTR) bit:
 * 0 - Don't send remote transmit request or the received message was not an RTR.
 * 1 - Send remote transmit request or the received message was an RTR.
 *
 * Extended ID enable (IDE) bit:
 * 0 - Send only standard ID (SID) or received message was a standard message.
 * 1 - Send both standard ID (SID) and extended ID (EID) or received message was an extended
 * message.
 *
 * Autotransmit (auto_tx) bit (ignore in received messages):
 * 0 - Send as soon as possible.
 * 1 - Send the message only when a matching RTR message is received.
 *
 * @public
 */
struct canbus_header_s
{
    // Disabled since, currently, a filter may not point at a TX buffer.
    //int auto_tx :1; /**< Automatically transmit the message when an RTR message is received. */
    
    int sid :11; /**< The standard ID of a message (11-bits). */
    int rtr :1;  /**< The remote transmit request (RTR) bit. */
    int ide :1;  /**< The extended ID enable bit. */
    union
    {
        int eid :18; /**< The extend ID of a message (18-bits). */
        struct
        {
            int eid_l :16;
            int eid_h :2;
        };
    };
};
typedef struct canbus_header_s canbus_header_t;

/**
 * @brief The message object stores an entire CAN bus message, including header.
 *
 * @details Nothing here.
 *
 * @public
 */
struct canbus_message_s
{
    canbus_header_t header; /**< The header of the message, containing message ID and flags. */
    unsigned char filter;   /**< The filter that enabled this message to be received. Only relevant
                               for received messages, ignored for transmitted messages. */
    unsigned char dlc;      /**< Data length code (DLC), the number of data bytes in the message. */
    unsigned char data[8];  /**< Data stored in this message (up to 8 bytes). */
};
typedef struct canbus_message_s canbus_message_t;


/* ***** Attribute Object Definition ***** */

/**
 * @brief The attribute object stores the immutable settings of the module. It is passed in at
 * initialization and the settings may not be changed without reinitializing the module.
 *
 * @details The attribute object is a storage container for hardware settings that may not be
 * changed while the module is running (i.e. initialized). The values may only be changed by
 * reinitializing the module.
 *
 * The bitfields within the attribute object can be set using the defined enumerations. Each
 * enumeration has a group of valid settings which correspond to each bitfield within the
 * attribute object.
 *
 * Once the attribute object is set up, it should be passed to the @ref canbus_init function along
 * with an uninitialized @ref canbus_t object.
 *
 * @note A completely zeroed attribute object is guaranteed to be valid and set up a default CAN bus
 * object.
 *
 * @public
 */
struct canbus_attr_s
{
    union
    {
        int bit_timing :19;
        struct
        {
            int bit_timing_pre             :6;
            int bit_timing_sync_jump       :2;
            int bit_timing_prop_seg        :3;
            int bit_timing_phase_seg1      :3;
            int bit_timing_phase_seg2      :3;
            int bit_timing_phase_seg2_prog :1;
            int bit_timing_sample          :1;
        };
    };

    union
    {
        int module :3;
        struct
        {
            int module_wakeup    :1;
            int module_cpuidle   :1;
            int module_timestamp :1;
        };
    };

    union
    {
        int fifo :8;
        struct
        {
            int fifo_start :5;
            int fifo_length :3;
        };
    };

    union
    {
        int buffer_dir :8;
        struct
        {
            int b0_dir :1;
            int b1_dir :1;
            int b2_dir :1;
            int b3_dir :1;
            int b4_dir :1;
            int b5_dir :1;
            int b6_dir :1;
            int b7_dir :1;
        };
    };
};
typedef struct canbus_attr_s canbus_attr_t;

/* ***** Canbus Object Declaration ***** */

// Forward declaration of canbus_s and canbus_t for use in canbus_s declaration
struct canbus_s;
typedef struct canbus_s canbus_t;

/**
 * @brief The actual CAN bus object.
 *
 * @details  This structure is the actual CAN bus object, it contains all the settings and data of
 * the module.
 *
 * The @ref canbus_number value and the notify function should be set at instantiation as they are
 * const values. The canbus_number is used to map the object to the correct hardware registers and
 * should not be modified once set.
 *
 * @public
 */
struct canbus_s
{
    /* ***** Public Variables ***** */
    
    /**
     * @brief The CANBus module number.
     *
     * @details This is the hardware module number of the CANBus module. This is used to help map
     * the correct special function registers.
     *
     * This value should only be set once, during instantiation of the canbus_t object.
     *
     * @public
     */
    const unsigned int module_number;

    /**
     * @brief A callback function that is called at particular defined events.
     *
     * @details This is the callback function that will be invoked when any of the events occur
     * that are set using @ref notify_on. This function must be set by the user at the time of
     * initialization of the canbus object and may not be changed 
     *
     * @public
     */
    void (* const notify)(canbus_t *canbus,
                          canbus_notice_t notice);

    /* ***** Private Variables ***** */
    
    /**
     * @brief The private storage variable of the canbus object.
     *
     * @details This is a pointer to the private storage variable of the canbus object. It should
     * not be modified by the user.
     *
     * @public
     */
    void *private;
};
typedef struct canbus_s canbus_t;

/**
 * @brief A storage container for the public function pointers of a CAN bus object.
 *
 * @details The public functions of the object are referenced using the function pointers contained
 * in the canbus_global_t object. The standard use is to call the functions using the global
 * @ref canbus variable which has all function pointers assigned to the correct values.
 *
 * @code
 * canbus.init(&c1, &c1_attr, &dma_ch0, &dma_ch1);
 * @endcode
 *
 * @public
 */
struct canbus_global_s
{
    /* ***** Public Functions (Not User Modifiable) ***** */

    /**
     * @brief The initialization function for a CANBus object.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const init)(canbus_t *object,
                       canbus_attr_t *attr,
                       unsigned int tx_dma_channel,
                       unsigned int rx_dma_channel,
                       volatile unsigned int *dma_buffer,
                       unsigned int dma_buffer_length);
    
    /**
     * @brief Set the mode of the CAN bus hardware.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const set_mode)(canbus_t *object,
                           canbus_mode_t mode);
    /**
     * @brief Set when the notify function should be called.
     *
     * @details This function sets what events should evoke calling the @ref notify callback
     * function. The notification parameter can be any set of @ref canbus_notice_t values logically
     * OR'd together. Any included notice values will trigger the @ref notify callback when the
     * associated event occurs.
     *
     * @param[in]  canbus       The canbus_t object to modify.
     * @param[in]  notification Any group of canbus_notice_t values logically OR'd together.
     * @return A @ref canbus_error_t value.
     *
     * @public
     */
    int (* const notify_on)(canbus_t *object,
                            int notification);
    
    /**
     * @brief Set the value of a mask in the CAN bus module.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const set_mask)(canbus_t *object,
                           canbus_mask_t mask_num,
                           canbus_header_t *mask_value);
    
    /**
     * @brief Assign a mask to a filter in the CAN bus module.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const assign_mask)(canbus_t *object,
                              canbus_mask_t mask_num,
                              canbus_filter_t filter_num);

    /**
     * @brief Set the value of a filter in the CAN bus module.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const set_filter)(canbus_t *object,
                             canbus_filter_t filter_num,
                             canbus_header_t *filter_value);

    /**
     * @brief Connect a filter and a buffer, so when a message hits the filter it will be stored in
     * the given buffer.
     *
     * @details Nothing here.
     *
     * @note The default buffer for a received message is B0, however, if a buffer is opened as a
     * transmit buffer and tries to receive a message it could lead to unknown behavior. The only
     * exception to this is if the message is marked as 'auto_tx', in which case a received RTR
     * message matching a filter pointing to that buffer will cause the message to be sent.
     *
     * @public
     */
    int (* const connect)(canbus_t *object,
                          canbus_filter_t filter_num,
                          canbus_buffer_t buffer_num);

    /**
     * @brief Disconnect a filter and a buffer.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const disconnect)(canbus_t *object,
                             canbus_filter_t filter_num);

    /**
     * @brief Write a message to the CAN bus using the given buffer and priority level.
     *
     * @details Write the given message out to the CAN bus by placing it in the specified buffer
     * with the specified priority. The message will be sent out when the bus becomes available and
     * there is no other higher priority message waiting to be sent.
     *
     * Priority works by first comparing the priority level given here, then if two messages have
     * the same priority level the message in the lower buffer wins. So a message in B0 will be sent
     * before a message in B1 if they both have the same priority.
     *
     * A message may only be written to a buffer if that buffer has been opened (@ref open()) as a
     * TX buffer.
     *
     * @param[in]  canbus     The canbus_t object to which to write.
     * @param[in]  buffer_num The buffer to which to write the message.
     * @param[in]  message    A pointer to the message to write.
     * @param[in]  priority   The transmit priority of the message.
     * @return A @ref canbus_error_t value.
     *
     * @public
     */
    int (* const write)(canbus_t *object,
                        canbus_buffer_t buffer_num,
                        const canbus_message_t *message,
                        canbus_priority_t priority);

    /**
     * @brief Abort any current transmission attempts in the specified buffer.
     *
     * @details If there is a current message waiting for transmission in the specified buffer
     * abort the transmission. If there is no current message don't do anything.
     *
     * @param[in]  canbus     The canbus_t object to work on.
     * @param[in]  buffer_num The buffer from which to abort any transmissions.
     * @return A <cc>1</cc> if a message was successfully aborted, a <cc>0</cc> if there was no
     * message in the buffer. If there was an error then a @ref canbus_error_t value.
     *
     * @public
     */
    int (* const abort_write)(canbus_t *object,
                              canbus_buffer_t buffer_num);
    
    /**
     * @brief Read a message from the specified buffer.
     *
     * @details Read the next available message from the given buffer. If the buffer is marked TX
     * an error code will be returned. If the read is successful the message is written to the given
     * canbus_message_t pointer and a <cc>1</cc> is returned. If no message is available a
     * <cc>0</cc> is returned and the supplied message buffer is unaffected.
     *
     * A buffer may only receive messages if it meets three criteria:
     * 1. A filter buffer pointer points at it.
     * 2. The filter is enabled.
     * 3. The buffer has been opened as RX.
     *
     * @param[in]  canbus      The canbus object from which to read.
     * @param[in]  buffer_num  The buffer from which to read.
     * @param[out] message     The message pointer to which to write the new message.
     * @returns A <cc>1</cc> if a message was read into the message pointer, a <cc>0</cc> if no
     * message was available for reading, or a canbus_error_t value if an error occurred.
     *
     * @public
     */
    int (* const read)(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message);

    /**
     * @brief The same as @ref read(), but doesn't remove the message from the buffer after reading.
     *
     * @details Peek() works the same as @ref read(), however, it does not actually remove the
     * message from the buffer, so the message will remain in the buffer until an actual @ref read()
     * call.
     *
     * @param[in]  canbus      The canbus object from which to read.
     * @param[in]  buffer_num  The buffer from which to read.
     * @param[out] message     A pointer to the message object into which to write the new message.
     * @return A <cc>1</cc> if a message was read into the message pointer, a <cc>0</cc> if no
     * message was available for reading, or a @ref canbus_error_t value if an error occurred.
     *
     * @public
     */
    int (* const peek)(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message);

    /**
     * @brief Check if the specified buffer is empty.
     *
     * @details Check if the specified buffer is empty.
     *
     * @param[in]  canbus     The canbus_t object to check.
     * @param[in]  buffer_num The buffer to check.
     * @return If the buffer is empty, <cc>true</cc>; otherwise <cc>false</cc>.
     *
     * @public
     */
    bool (* const is_empty)(canbus_t *object,
                            canbus_buffer_t buffer_num);

    /**
     * @brief Check if a buffer exists in DMA RAM.
     *
     * @details Nothing here.
     *
     * @public
     */
    bool (* const buffer_exists)(canbus_t *object,
                                 canbus_buffer_t buffer_num);
    
    /**
     * @brief Check if a CAN bus object is valid.
     *
     * @details Nothing here.
     *
     * @public
     */
    bool (* const is_valid)(canbus_t *object);

    /**
     * @brief Get the buffer's direction.
     *
     * @details Nothing here.
     *
     * @public
     */
    int (* const get_direction)(canbus_t *object,
                                canbus_buffer_t buffer_num);

    /**
     * @brief Free any dynamically allocated memory and shutdown the hardware module.
     *
     * @details Nothing here.
     *
     * @public
     */
    void (* const clean_up)(canbus_t *object);

    /* ***** Interrupt Service Routine (ISR) ***** */

    /**
     * @brief The interrupt service routine (ISR) for the canbus object.
     *
     * @details The ISR function for the canbus object. This function should be called from within
     * the actual vectored ISR for the specific CAN bus module. In other words, within the ISR for
     * the C1 hardware module the user should insert a call to this function and pass it the
     * @ref canbus_t object which corresponds to C1.
     *
     * @note This function should not be called outside of the ISR.
     *
     * @param[in]  canbus The canbus_t object to work on.
     * @return None.
     *
     * @public
     */
    void (* const isr)(canbus_t *object);
    
};

/* ***** Declare Global Canbus Object ***** */
extern canbus_global_t canbus;

/**
 * @}
 */ // End canbus group

#endif // CANBUS_MODULE_H_

