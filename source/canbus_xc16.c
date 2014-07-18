/* -*- mode: C; tab-width: 4; -*- */
/** @file {canbus_module.c} **************************************************
 * @brief This file contains all the function definitions and private
 * variables for the CAN bus module.
 *
 * @author {Liam Bucci}
 * @date {6/11/2012}
 * @project {Carl Platform}
 * @subproject {Motor Controller}
 * @version {0.2.0}
 *
 ****************************************************************************/

// Standard C include files
#include <stdlib.h>
#include <stdbool.h>

// Microchip peripheral libraries
#include <xc.h>
#include <pps.h>
#include <dma_channel.h>

// Include board information
#include <board.def>

// Include local library code
#include <bitops.h>
#include <dma_channel.h>

// Include user definitions
#include <canbus.def>

// Canbus include files
#include <canbus_hw.h>
#include <canbus.h>


/* ***** Preprocessor Macros ***** */

#define CANBUS_BASE_ADDRESS(object) ( ((canbus_private_t *)((object)->private))->base_address_ )

#define CANBUS_ATTR(object) ( ((canbus_private_t *)((object)->private))->attr_ )


/* ***** Private Enumerations ***** */

enum canbus_sfr_offsets_e
{
    CANBUS_SFR_OFFSET_CiCTRL1    = 0x0000,
    CANBUS_SFR_OFFSET_CiCTRL2    = 0x0001,
    CANBUS_SFR_OFFSET_CiVEC      = 0x0002,
    CANBUS_SFR_OFFSET_CiFCTRL    = 0x0003,
    CANBUS_SFR_OFFSET_CiFIFO     = 0x0004,
    CANBUS_SFR_OFFSET_CiINTF     = 0x0005,
    CANBUS_SFR_OFFSET_CiINTE     = 0x0006,
    CANBUS_SFR_OFFSET_CiEC       = 0x0007,
    CANBUS_SFR_OFFSET_CiCFG1     = 0x0008,
    CANBUS_SFR_OFFSET_CiCFG2     = 0x0009,
    CANBUS_SFR_OFFSET_CiFEN1     = 0x000A,
    CANBUS_SFR_OFFSET_CiFMSKSEL1 = 0x000B,
    CANBUS_SFR_OFFSET_CiFMSKSEL2 = 0x000C,

    CANBUS_SFR_OFFSET_CiRXFUL1   = 0x0010,
    CANBUS_SFR_OFFSET_CiRXFUL2   = 0x0011,
    CANBUS_SFR_OFFSET_CiRXOVF1   = 0x0014,
    CANBUS_SFR_OFFSET_CiRXOVF2   = 0x0015,
    CANBUS_SFR_OFFSET_CiTR01CON  = 0x0018,
    CANBUS_SFR_OFFSET_CiTR23CON  = 0x0019,
    CANBUS_SFR_OFFSET_CiTR45CON  = 0x001A,
    CANBUS_SFR_OFFSET_CiTR67CON  = 0x001B,
    CANBUS_SFR_OFFSET_CiRXD      = 0x0020,
    CANBUS_SFR_OFFSET_CiTXD      = 0x0021,

    CANBUS_SFR_OFFSET_CiBUFPNT1  = 0x0010,
    CANBUS_SFR_OFFSET_CiBUFPNT2  = 0x0011,
    CANBUS_SFR_OFFSET_CiBUFPNT3  = 0x0012,
    CANBUS_SFR_OFFSET_CiBUFPNT4  = 0x0013,
    CANBUS_SFR_OFFSET_CiRXM0SID  = 0x0018,
    CANBUS_SFR_OFFSET_CiRXM0EID  = 0x0019,
    CANBUS_SFR_OFFSET_CiRXM1SID  = 0x001A,
    CANBUS_SFR_OFFSET_CiRXM1EID  = 0x001B,
    CANBUS_SFR_OFFSET_CiRXM2SID  = 0x001C,
    CANBUS_SFR_OFFSET_CiRXM2EID  = 0x001D,
    CANBUS_SFR_OFFSET_CiRXF0SID  = 0x0020,
    CANBUS_SFR_OFFSET_CiRXF0EID  = 0x0021,
    CANBUS_SFR_OFFSET_CiRXF1SID  = 0x0022,
    CANBUS_SFR_OFFSET_CiRXF1EID  = 0x0023,
    CANBUS_SFR_OFFSET_CiRXF2SID  = 0x0024,
    CANBUS_SFR_OFFSET_CiRXF2EID  = 0x0025,
    CANBUS_SFR_OFFSET_CiRXF3SID  = 0x0026,
    CANBUS_SFR_OFFSET_CiRXF3EID  = 0x0027,
    CANBUS_SFR_OFFSET_CiRXF4SID  = 0x0028,
    CANBUS_SFR_OFFSET_CiRXF4EID  = 0x0029,
    CANBUS_SFR_OFFSET_CiRXF5SID  = 0x002A,
    CANBUS_SFR_OFFSET_CiRXF5EID  = 0x002B,
    CANBUS_SFR_OFFSET_CiRXF6SID  = 0x002C,
    CANBUS_SFR_OFFSET_CiRXF6EID  = 0x002D,
    CANBUS_SFR_OFFSET_CiRXF7SID  = 0x002E,
    CANBUS_SFR_OFFSET_CiRXF7EID  = 0x002F,
    CANBUS_SFR_OFFSET_CiRXF8SID  = 0x0030,
    CANBUS_SFR_OFFSET_CiRXF8EID  = 0x0031,
    CANBUS_SFR_OFFSET_CiRXF9SID  = 0x0032,
    CANBUS_SFR_OFFSET_CiRXF9EID  = 0x0033,
    CANBUS_SFR_OFFSET_CiRXF10SID = 0x0034,
    CANBUS_SFR_OFFSET_CiRXF10EID = 0x0035,
    CANBUS_SFR_OFFSET_CiRXF11SID = 0x0036,
    CANBUS_SFR_OFFSET_CiRXF11EID = 0x0037,
    CANBUS_SFR_OFFSET_CiRXF12SID = 0x0038,
    CANBUS_SFR_OFFSET_CiRXF12EID = 0x0039,
    CANBUS_SFR_OFFSET_CiRXF13SID = 0x003A,
    CANBUS_SFR_OFFSET_CiRXF13EID = 0x003B,
    CANBUS_SFR_OFFSET_CiRXF14SID = 0x003C,
    CANBUS_SFR_OFFSET_CiRXF14EID = 0x003D,
    CANBUS_SFR_OFFSET_CiRXF15SID = 0x003E,
    CANBUS_SFR_OFFSET_CiRXF15EID = 0x003F
};

enum canbus_sfr_bitmasks_e
{
    // CiCFG1
    CANBUS_SFR_BITMASK_BRP = 0x003F,
    CANBUS_SFR_BITMASK_SJW = 0x00C0,

    // CiCFG2
    CANBUS_SFR_BITMASK_PRSEG = 0x0007,
    CANBUS_SFR_BITMASK_SEG1PH = 0x0038,
    CANBUS_SFR_BITMASK_SAM = 0x0040,
    CANBUS_SFR_BITMASK_SEG2PHTS = 0x0080,
    CANBUS_SFR_BITMASK_SEG2PH = 0x0700,
    CANBUS_SFR_BITMASK_WAKFIL = 0x4000,

    // CiFEN1
    CANBUS_SFR_BITMASK_FLTEN0 = 0x0001,
    CANBUS_SFR_BITMASK_FLTEN1 = 0x0002,
    CANBUS_SFR_BITMASK_FLTEN2 = 0x0004,
    CANBUS_SFR_BITMASK_FLTEN3 = 0x0008,
    CANBUS_SFR_BITMASK_FLTEN4 = 0x0010,
    CANBUS_SFR_BITMASK_FLTEN5 = 0x0020,
    CANBUS_SFR_BITMASK_FLTEN6 = 0x0040,
    CANBUS_SFR_BITMASK_FLTEN7 = 0x0080,
    CANBUS_SFR_BITMASK_FLTEN8 = 0x0100,
    CANBUS_SFR_BITMASK_FLTEN9 = 0x0200,
    CANBUS_SFR_BITMASK_FLTEN10 = 0x0400,
    CANBUS_SFR_BITMASK_FLTEN11 = 0x0800,
    CANBUS_SFR_BITMASK_FLTEN12 = 0x1000,
    CANBUS_SFR_BITMASK_FLTEN13 = 0x2000,
    CANBUS_SFR_BITMASK_FLTEN14 = 0x4000,
    CANBUS_SFR_BITMASK_FLTEN15 = 0x8000,

    // CiRXFnSID/CiRXMnSID
    CANBUS_SFR_BITMASK_EID16 = 0x0001,
    CANBUS_SFR_BITMASK_EID17 = 0x0002,
    CANBUS_SFR_BITMASK_EID16_17 = 0x0003,
    CANBUS_SFR_BITMASK_EXIDE = 0x0008,
    CANBUS_SFR_BITMASK_MIDE = 0x0008,
    CANBUS_SFR_BITMASK_SID0 = 0x0020,
    CANBUS_SFR_BITMASK_SID1 = 0x0040,
    CANBUS_SFR_BITMASK_SID2 = 0x0080,
    CANBUS_SFR_BITMASK_SID3 = 0x0100,
    CANBUS_SFR_BITMASK_SID4 = 0x0200,
    CANBUS_SFR_BITMASK_SID5 = 0x0400,
    CANBUS_SFR_BITMASK_SID6 = 0x0800,
    CANBUS_SFR_BITMASK_SID7 = 0x1000,
    CANBUS_SFR_BITMASK_SID8 = 0x2000,
    CANBUS_SFR_BITMASK_SID9 = 0x4000,
    CANBUS_SFR_BITMASK_SID10 = 0x8000,
    CANBUS_SFR_BITMASK_SID0_10 = 0xFFE0,
    
    // CiRXFnEID/CiRXMnEID
    CANBUS_SFR_BITMASK_EID0 = 0x0001,
    CANBUS_SFR_BITMASK_EID0 = 0x0002,
    CANBUS_SFR_BITMASK_EID0 = 0x0004,
    CANBUS_SFR_BITMASK_EID0 = 0x0008,
    CANBUS_SFR_BITMASK_EID0 = 0x0010,
    CANBUS_SFR_BITMASK_EID0 = 0x0020,
    CANBUS_SFR_BITMASK_EID0 = 0x0040,
    CANBUS_SFR_BITMASK_EID0 = 0x0080,
    CANBUS_SFR_BITMASK_EID0 = 0x0100,
    CANBUS_SFR_BITMASK_EID0 = 0x0200,
    CANBUS_SFR_BITMASK_EID0 = 0x0400,
    CANBUS_SFR_BITMASK_EID0 = 0x0800,
    CANBUS_SFR_BITMASK_EID0 = 0x1000,
    CANBUS_SFR_BITMASK_EID0 = 0x2000,
    CANBUS_SFR_BITMASK_EID0 = 0x4000,
    CANBUS_SFR_BITMASK_EID0 = 0x8000,

    // CiFMSKSEL1
    CANBUS_SFR_BITMASK_F0MSK = 0x0003,
    CANBUS_SFR_BITMASK_F1MSK = 0x000C,
    CANBUS_SFR_BITMASK_F2MSK = 0x0030,
    CANBUS_SFR_BITMASK_F3MSK = 0x00C0,
    CANBUS_SFR_BITMASK_F4MSK = 0x0300,
    CANBUS_SFR_BITMASK_F5MSK = 0x0C00,
    CANBUS_SFR_BITMASK_F6MSK = 0x3000,
    CANBUS_SFR_BITMASK_F7MSK = 0xC000,

    // CiFMSKSEL2
    CANBUS_SFR_BITMASK_F8MSK = 0x0003,
    CANBUS_SFR_BITMASK_F9MSK = 0x000C,
    CANBUS_SFR_BITMASK_F10MSK = 0x0030,
    CANBUS_SFR_BITMASK_F11MSK = 0x00C0,
    CANBUS_SFR_BITMASK_F12MSK = 0x0300,
    CANBUS_SFR_BITMASK_F13MSK = 0x0C00,
    CANBUS_SFR_BITMASK_F14MSK = 0x3000,
    CANBUS_SFR_BITMASK_F15MSK = 0xC000,

    // CiBUFPNT1
    CANBUS_SFR_BITMASK_F0BP = 0x000F,
    CANBUS_SFR_BITMASK_F1BP = 0x00F0,
    CANBUS_SFR_BITMASK_F2BP = 0x0F00,
    CANBUS_SFR_BITMASK_F3BP = 0xF000,

    // CiBUFPNT2
    CANBUS_SFR_BITMASK_F4BP = 0x000F,
    CANBUS_SFR_BITMASK_F5BP = 0x00F0,
    CANBUS_SFR_BITMASK_F6BP = 0x0F00,
    CANBUS_SFR_BITMASK_F7BP = 0xF000,

    // CiBUFPNT3
    CANBUS_SFR_BITMASK_F8BP = 0x000F,
    CANBUS_SFR_BITMASK_F9BP = 0x00F0,
    CANBUS_SFR_BITMASK_F10BP = 0x0F00,
    CANBUS_SFR_BITMASK_F11BP = 0xF000,

    // CiBUFPNT4
    CANBUS_SFR_BITMASK_F12BP = 0x000F,
    CANBUS_SFR_BITMASK_F13BP = 0x00F0,
    CANBUS_SFR_BITMASK_F14BP = 0x0F00,
    CANBUS_SFR_BITMASK_F15BP = 0xF000,

    // CiRXFUL1
    CANBUS_SFR_BITMASK_RXFUL0 = 0x0001,
    CANBUS_SFR_BITMASK_RXFUL1 = 0x0002,
    CANBUS_SFR_BITMASK_RXFUL2 = 0x0004,
    CANBUS_SFR_BITMASK_RXFUL3 = 0x0008,
    CANBUS_SFR_BITMASK_RXFUL4 = 0x0010,
    CANBUS_SFR_BITMASK_RXFUL5 = 0x0020,
    CANBUS_SFR_BITMASK_RXFUL6 = 0x0040,
    CANBUS_SFR_BITMASK_RXFUL7 = 0x0080,
    CANBUS_SFR_BITMASK_RXFUL8 = 0x0100,
    CANBUS_SFR_BITMASK_RXFUL9 = 0x0200,
    CANBUS_SFR_BITMASK_RXFUL10 = 0x0400,
    CANBUS_SFR_BITMASK_RXFUL11 = 0x0800,
    CANBUS_SFR_BITMASK_RXFUL12 = 0x1000,
    CANBUS_SFR_BITMASK_RXFUL13 = 0x2000,
    CANBUS_SFR_BITMASK_RXFUL14 = 0x4000,
    CANBUS_SFR_BITMASK_RXFUL15 = 0x8000,

    // CiRXFUL2
    CANBUS_SFR_BITMASK_RXFUL16 = 0x0001,
    CANBUS_SFR_BITMASK_RXFUL17 = 0x0002,
    CANBUS_SFR_BITMASK_RXFUL18 = 0x0004,
    CANBUS_SFR_BITMASK_RXFUL19 = 0x0008,
    CANBUS_SFR_BITMASK_RXFUL20 = 0x0010,
    CANBUS_SFR_BITMASK_RXFUL21 = 0x0020,
    CANBUS_SFR_BITMASK_RXFUL22 = 0x0040,
    CANBUS_SFR_BITMASK_RXFUL23 = 0x0080,
    CANBUS_SFR_BITMASK_RXFUL24 = 0x0100,
    CANBUS_SFR_BITMASK_RXFUL25 = 0x0200,
    CANBUS_SFR_BITMASK_RXFUL26 = 0x0400,
    CANBUS_SFR_BITMASK_RXFUL27 = 0x0800,
    CANBUS_SFR_BITMASK_RXFUL28 = 0x1000,
    CANBUS_SFR_BITMASK_RXFUL29 = 0x2000,
    CANBUS_SFR_BITMASK_RXFUL30 = 0x4000,
    CANBUS_SFR_BITMASK_RXFUL31 = 0x8000,

    // CiRXOVF1
    CANBUS_SFR_BITMASK_RXOVF0 = 0x0001,
    CANBUS_SFR_BITMASK_RXOVF1 = 0x0002,
    CANBUS_SFR_BITMASK_RXOVF2 = 0x0004,
    CANBUS_SFR_BITMASK_RXOVF3 = 0x0008,
    CANBUS_SFR_BITMASK_RXOVF4 = 0x0010,
    CANBUS_SFR_BITMASK_RXOVF5 = 0x0020,
    CANBUS_SFR_BITMASK_RXOVF6 = 0x0040,
    CANBUS_SFR_BITMASK_RXOVF7 = 0x0080,
    CANBUS_SFR_BITMASK_RXOVF8 = 0x0100,
    CANBUS_SFR_BITMASK_RXOVF9 = 0x0200,
    CANBUS_SFR_BITMASK_RXOVF10 = 0x0400,
    CANBUS_SFR_BITMASK_RXOVF11 = 0x0800,
    CANBUS_SFR_BITMASK_RXOVF12 = 0x1000,
    CANBUS_SFR_BITMASK_RXOVF13 = 0x2000,
    CANBUS_SFR_BITMASK_RXOVF14 = 0x4000,
    CANBUS_SFR_BITMASK_RXOVF15 = 0x8000,

    // CiRXOVF2
    CANBUS_SFR_BITMASK_RXOVF16 = 0x0001,
    CANBUS_SFR_BITMASK_RXOVF17 = 0x0002,
    CANBUS_SFR_BITMASK_RXOVF18 = 0x0004,
    CANBUS_SFR_BITMASK_RXOVF19 = 0x0008,
    CANBUS_SFR_BITMASK_RXOVF20 = 0x0010,
    CANBUS_SFR_BITMASK_RXOVF21 = 0x0020,
    CANBUS_SFR_BITMASK_RXOVF22 = 0x0040,
    CANBUS_SFR_BITMASK_RXOVF23 = 0x0080,
    CANBUS_SFR_BITMASK_RXOVF24 = 0x0100,
    CANBUS_SFR_BITMASK_RXOVF25 = 0x0200,
    CANBUS_SFR_BITMASK_RXOVF26 = 0x0400,
    CANBUS_SFR_BITMASK_RXOVF27 = 0x0800,
    CANBUS_SFR_BITMASK_RXOVF28 = 0x1000,
    CANBUS_SFR_BITMASK_RXOVF29 = 0x2000,
    CANBUS_SFR_BITMASK_RXOVF30 = 0x4000,
    CANBUS_SFR_BITMASK_RXOVF31 = 0x8000,

    // CiFCTRL
    CANBUS_SFR_BITMASK_FSA = 0x001F,
    CANBUS_SFR_BITMASK_DMABS = 0xE000,

    // CiFIFO
    CANBUS_SFR_BITMASK_FNRB = 0x003F,
    CANBUS_SFR_BITMASK_FBP = 0x3F00,

    // CiINTF
    CANBUS_SFR_BITMASK_TBIF = 0x0001,
    CANBUS_SFR_BITMASK_RBIF = 0x0002,
    CANBUS_SFR_BITMASK_RBOVIF = 0x0004,
    CANBUS_SFR_BITMASK_FIFOIF = 0x0008,
    CANBUS_SFR_BITMASK_ERRIF = 0x0020,
    CANBUS_SFR_BITMASK_WAKIF = 0x0040,
    CANBUS_SFR_BITMASK_IVRIF = 0x0080,
    CANBUS_SFR_BITMASK_EWARN = 0x0100,
    CANBUS_SFR_BITMASK_RXWAR = 0x0200,
    CANBUS_SFR_BITMASK_TXWAR = 0x0400,
    CANBUS_SFR_BITMASK_RXBP = 0x0800,
    CANBUS_SFR_BITMASK_TXBP = 0x1000,
    CANBUS_SFR_BITMASK_TXBO = 0x2000,

    // CiINTE
    CANBUS_SFR_BITMASK_TBIE = 0x0001,
    CANBUS_SFR_BITMASK_RBIE = 0x0002,
    CANBUS_SFR_BITMASK_RBOVIE = 0x0004,
    CANBUS_SFR_BITMASK_FIFOIE = 0x0008,
    CANBUS_SFR_BITMASK_ERRIE = 0x0020,
    CANBUS_SFR_BITMASK_WAKIE = 0x0040,
    CANBUS_SFR_BITMASK_IVRIE = 0x0080,

    // CiVEC
    CANBUS_SFR_BITMASK_ICODE = 0x007F,
    CANBUS_SFR_BITMASK_FILHIT = 0x1F00,

    // CiCTRL1
    CANBUS_SFR_BITMASK_WIN = 0x0001,
    CANBUS_SFR_BITMASK_CANCAP = 0x0008,
    CANBUS_SFR_BITMASK_OPMODE = 0x00E0,
    CANBUS_SFR_BITMASK_REQOP = 0x0700,
    CANBUS_SFR_BITMASK_ABAT = 0x1000,
    CANBUS_SFR_BITMASK_CSIDL = 0x2000,

    // ...
};

enum canbus_sfr_defaults_e
{
    CANBUS_SFR_DEFAULT_CiCTRL1    = 0x0480,
    CANBUS_SFR_DEFAULT_CiCTRL2    = 0x0000,
    CANBUS_SFR_DEFAULT_CiVEC      = 0x0040,
    CANBUS_SFR_DEFAULT_CiFCTRL    = 0x0000,
    CANBUS_SFR_DEFAULT_CiFIFO     = 0x0000,
    CANBUS_SFR_DEFAULT_CiINTF     = 0x0000,
    CANBUS_SFR_DEFAULT_CiINTE     = 0x0000,
    CANBUS_SFR_DEFAULT_CiEC       = 0x0000,
    CANBUS_SFR_DEFAULT_CiCFG1     = 0x0000,
    CANBUS_SFR_DEFAULT_CiCFG2     = 0x0000,
    CANBUS_SFR_DEFAULT_CiFEN1     = 0x0000,
    CANBUS_SFR_DEFAULT_CiFMSKSEL1 = 0x0000,
    CANBUS_SFR_DEFAULT_CiFMSKSEL2 = 0x0000,

    CANBUS_SFR_DEFAULT_CiRXFUL1   = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXFUL2   = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXOVF1   = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXOVF2   = 0x0000,
    CANBUS_SFR_DEFAULT_CiTR01CON  = 0x0000,
    CANBUS_SFR_DEFAULT_CiTR23CON  = 0x0000,
    CANBUS_SFR_DEFAULT_CiTR45CON  = 0x0000,
    CANBUS_SFR_DEFAULT_CiTR67CON  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXD      = 0x0000,
    CANBUS_SFR_DEFAULT_CiTXD      = 0x0000,

    CANBUS_SFR_DEFAULT_CiBUFPNT1  = 0x0000,
    CANBUS_SFR_DEFAULT_CiBUFPNT2  = 0x0000,
    CANBUS_SFR_DEFAULT_CiBUFPNT3  = 0x0000,
    CANBUS_SFR_DEFAULT_CiBUFPNT4  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXM0SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXM0EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXM1SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXM1EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXM2SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXM2EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF0SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF0EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF1SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF1EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF2SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF2EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF3SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF3EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF4SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF4EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF5SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF5EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF6SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF6EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF7SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF7EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF8SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF8EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF9SID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF9EID  = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF10SID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF10EID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF11SID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF11EID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF12SID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF12EID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF13SID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF13EID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF14SID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF14EID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF15SID = 0x0000,
    CANBUS_SFR_DEFAULT_CiRXF15EID = 0x0000
};

enum canbus_opmode_e
{
    CANBUS_OPMODE_NORMAL = 0b000,
    CANBUS_OPMODE_DISABLE = 0b001,
    CANBUS_OPMODE_LOOPBACK = 0b010,
    CANBUS_OPMODE_LISTENONLY = 0b011,
    CANBUS_OPMODE_CONFIGURATION = 0b100,
    CANBUS_OPMODE_LISTENALL = 0b111
};

struct canbus_cictrl1_bits_s
{
    int win :1;
    int :2;
    int cancap :1;
    int :1;
    int opmode :3;
    int reqop :3;
    int :1;
    int abat :1;
    int csidl :1;
    int :2;
};
typedef struct canbus_cictrl1_bits_s canbus_cictrl1_bits_t;

struct canbus_cictrl2_bits_s
{
    int dncnt :5;
    int :11;
};
typedef struct canbus_cictrl2_bits_s canbus_cictrl2_bits_t;

struct canbus_civec_bits_s
{
    int icode :7;
    int :1;
    int filhit :5;
    int :3;
};
typedef struct canbus_civec_bits_s canbus_civec_bits_t;

struct canbus_cifctrl_bits_s
{
    int fsa :5;
    int :8;
    int dmabs :3;
};
typedef struct canbus_cifctrl_bits_s canbus_cifctrl_bits_t;

struct canbus_cififo_bits_s
{
    int fnrb :6;
    int :2;
    int fbp :6;
    int :2;
};
typedef struct canbus_cififo_bits_s canbus_cififo_bits_t;

struct canbus_ciintf_bits_s
{
    int tbif :1;
    int rbif :1;
    int rbovif :1;
    int fifoif :1;
    int :1;
    int errif :1;
    int wakif :1;
    int ivrif :1;
    int ewarn :1;
    int rxwar :1;
    int txwar :1;
    int rxbp :1;
    int txbp :1;
    int txbo :1;
    int :2;
};
typedef struct canbus_ciintf_bits_s canbus_ciintf_bits_t;

struct canbus_ciinte_bits_s
{
    int tbie :1;
    int rbie :1;
    int rbovie :1;
    int fifoie :1;
    int :1;
    int errie :1;
    int wakie :1;
    int ivrie :1;
    int :8;
};
typedef struct canbus_ciinte_bits_s canbus_ciinte_bits_t;

struct canbus_ciec_bits_s
{
    int rerrcnt :8;
    int terrcnt :8;
};
typedef struct canbus_ciec_bits_s canbus_ciec_bits_t;

struct canbus_cicfg1_bits_s
{
    int brp :6;
    int sjw :2;
    int :8;
};
typedef struct canbus_cicfg1_bits_s canbus_cicfg1_bits_t;

struct canbus_cicfg2_bits_s
{
    int prseg :3;
    int seg1ph :3;
    int sam :1;
    int seg2phts :1;
    int seg2ph :3;
    int :3;
    int wakfil :1;
    int :1;
};
typedef struct canbus_cicfg2_bits_s canbus_cicfg2_bits_t;

struct canbus_cifen1_bits_s
{
    int flten0 :1;
    int flten1 :1;
    int flten2 :1;
    int flten3 :1;
    int flten4 :1;
    int flten5 :1;
    int flten6 :1;
    int flten7 :1;
    int flten8 :1;
    int flten9 :1;
    int flten10 :1;
    int flten11 :1;
    int flten12 :1;
    int flten13 :1;
    int flten14 :1;
    int flten15 :1;
};
typedef struct canbus_cifen1_bits_s canbus_cifen1_bits_t;

struct canbus_cifmsksel1_bits_s
{
    int f0msk :2;
    int f1msk :2;
    int f2msk :2;
    int f3msk :2;
    int f4msk :2;
    int f5msk :2;
    int f6msk :2;
    int f7msk :2;
};
typedef struct canbus_cifmsksel1_bits_s canbus_cifmsksel1_bits_t;

struct canbus_cifmsksel2_bits_s
{
    int f8msk :2;
    int f9msk :2;
    int f10msk :2;
    int f11msk :2;
    int f12msk :2;
    int f13msk :2;
    int f14msk :2;
    int f15msk :2;
};
typedef struct canbus_cifmsksel2_bits_s canbus_cifmsksel2_bits_t;

struct canbus_cirxful1_bits_s
{
    int rxful0 :1;
    int rxful1 :1;
    int rxful2 :1;
    int rxful3 :1;
    int rxful4 :1;
    int rxful5 :1;
    int rxful6 :1;
    int rxful7 :1;
    int rxful8 :1;
    int rxful9 :1;
    int rxful10 :1;
    int rxful11 :1;
    int rxful12 :1;
    int rxful13 :1;
    int rxful14 :1;
    int rxful15 :1;
};
typedef struct canbus_cirxful1_bits_s canbus_cirxful1_bits_t;

struct canbus_cirxful2_bits_s
{
    int rxful16 :1;
    int rxful17 :1;
    int rxful18 :1;
    int rxful19 :1;
    int rxful20 :1;
    int rxful21 :1;
    int rxful22 :1;
    int rxful23 :1;
    int rxful24 :1;
    int rxful25 :1;
    int rxful26 :1;
    int rxful27 :1;
    int rxful28 :1;
    int rxful29 :1;
    int rxful30 :1;
    int rxful31 :1;
};
typedef struct canbus_cirxful2_bits_s canbus_cirxful2_bits_t;

struct canbus_cirxovf1_bits_s
{
    int rxovf0 :1;
    int rxovf1 :1;
    int rxovf2 :1;
    int rxovf3 :1;
    int rxovf4 :1;
    int rxovf5 :1;
    int rxovf6 :1;
    int rxovf7 :1;
    int rxovf8 :1;
    int rxovf9 :1;
    int rxovf10 :1;
    int rxovf11 :1;
    int rxovf12 :1;
    int rxovf13 :1;
    int rxovf14 :1;
    int rxovf15 :1;
};
typedef struct canbus_cirxovf1_bits_s canbus_cirxovf1_bits_t;

struct canbus_cirxovf2_bits_s
{
    int rxovf16 :1;
    int rxovf17 :1;
    int rxovf18 :1;
    int rxovf19 :1;
    int rxovf20 :1;
    int rxovf21 :1;
    int rxovf22 :1;
    int rxovf23 :1;
    int rxovf24 :1;
    int rxovf25 :1;
    int rxovf26 :1;
    int rxovf27 :1;
    int rxovf28 :1;
    int rxovf29 :1;
    int rxovf30 :1;
    int rxovf31 :1;
};
typedef struct canbus_cirxovf2_bits_s canbus_cirxovf2_bits_t;

struct canbus_citrmncon_bits_s
{
    int txmpri :2;
    int rtrenm :1;
    int txreqm :1;
    int txerrm :1;
    int txlarbm :1;
    int txabtm :1;
    int txenm :1;

    int txnpri :2;
    int rtrenn :1;
    int txreqn :1;
    int txerrn :1;
    int txlarbn :1;
    int txabtn :1;
    int txenn :1;;
};
typedef struct canbus_citrmncon_bits_s canbus_citrmncon_bits_t;

struct canbus_cibufpnt_bits_s
{
    int fabp :4;
    int fbbp :4;
    int fcbp :4;
    int fdbp :4;
};
typedef struct canbus_cibufpnt_bits_s canbus_cibufpnt_bits_t;

union canbus_cirxmnsid_bits_u
{
    struct
    {
        int eid16 :1;
        int eid17 :1;
        int :1;
        int mide :1;
        int :1;
        int sid0 :1;
        int sid1 :1;
        int sid2 :1;
        int sid3 :1;
        int sid4 :1;
        int sid5 :1;
        int sid6 :1;
        int sid7 :1;
        int sid8 :1;
        int sid9 :1;
        int sid10 :1;
    };
    struct
    {
        int eid :2;
        int :3;
        int sid :11;
    };
};
typedef union canbus_cirxmnsid_bits_u canbus_cirxmnsid_bits_t;

struct canbus_cirxmneid_bits_s
{
    int eid0 :1;
    int eid1 :1;
    int eid2 :1;
    int eid3 :1;
    int eid4 :1;
    int eid5 :1;
    int eid6 :1;
    int eid7 :1;
    int eid8 :1;
    int eid9 :1;
    int eid10 :1;
    int eid11 :1;
    int eid12 :1;
    int eid13 :1;
    int eid14 :1;
    int eid15 :1;
};
typedef struct canbus_cirxmneid_bits_s canbus_cirxmneid_bits_t;

union canbus_cirxfnsid_bits_u
{
    struct
    {
        int eid16 :1;
        int eid17 :1;
        int :1;
        int exide :1;
        int :1;
        int sid0 :1;
        int sid1 :1;
        int sid2 :1;
        int sid3 :1;
        int sid4 :1;
        int sid5 :1;
        int sid6 :1;
        int sid7 :1;
        int sid8 :1;
        int sid9 :1;
        int sid10 :1;
    };
    struct
    {
        int eid_h :2;
        int :3;
        int sid :11;
    };
};
typedef union canbus_cirxfnsid_bits_u canbus_cirxfnsid_bits_t;

union canbus_cirxfneid_bits_u
{
    struct
    {
        int eid0 :1;
        int eid1 :1;
        int eid2 :1;
        int eid3 :1;
        int eid4 :1;
        int eid5 :1;
        int eid6 :1;
        int eid7 :1;
        int eid8 :1;
        int eid9 :1;
        int eid10 :1;
        int eid11 :1;
        int eid12 :1;
        int eid13 :1;
        int eid14 :1;
        int eid15 :1;
    };
    struct
    {
        int eid_l :16;
    };
};
typedef struct canbus_cirxfneid_bits_s canbus_cirxfneid_bits_t;
    


/* ***** Canbus Private Object ***** */

struct canbus_private_s
{
    canbus_attr_t attr_;
    volatile unsigned int *base_address_;
    dma_channel_t *tx_dma_;
    dma_channel_t *rx_dma_;
    unsigned int enabled_filters_;
    
};
typedef struct canbus_private canbus_private_t;


/* ***** Private Function Prototypes ***** */

static int canbus_init(canbus_t *object,
                       canbus_attr_t *attr,
                       unsigned int tx_dma_channel,
                       unsigned int rx_dma_channel,
                       volatile unsigned int *dma_buffer,
                       unsigned int dma_buffer_length);
static int canbus_set_mode(canbus_t *object,
                           canbus_mode_t mode);
static int canbus_notify_on(canbus_t *object,
                            int notification);
static int canbus_set_mask(canbus_t *object,
                           canbus_mask_t mask_num,
                           canbus_header_t *mask_value);
static int canbus_assign_mask(canbus_t *object,
                              canbus_mask_t mask_num,
                              canbus_filter_t filter_num);
static int canbus_set_filter(canbus_t *object,
                             canbus_filter_t filter_num,
                             canbus_header_t *filter_value);
static int canbus_set_buffer_pointer(canbus_t *object,
                                     canbus_filter_t filter_num,
                                     canbus_buffer_t buffer_num);
static int canbus_enable_filter(canbus_t *object,
                                canbus_filter_t filter_num);
static int canbus_disable_filter(canbus_t *object,
                                 canbus_filter_t filter_num);
static int canbus_open(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_direction_t direction);
static int canbus_write(canbus_t *object,
                        canbus_buffer_t buffer_num,
                        const canbus_message_t *message,
                        canbus_priority_t priority);
static int canbus_read(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message);
static int canbus_peek(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message);
static bool canbus_is_empty(canbus_t *object,
                            canbus_buffer_t buffer_num);
static bool canbus_is_open(canbus_t *object,
                           canbus_buffer_t buffer_num);
static bool canbus_is_valid(canbus_t *object);
static int canbus_close(canbus_t *object,
                        canbus_buffer_t buffer_num);
static void canbus_clean_up(canbus_t *object);
static void canbus_isr(canbus_t *object);


/* ***** Define Global Canbus Object ***** */

/**
 * @brief The global canbus object which is used as a namespace to call all public functions.
 *
 * @private
 */
canbus_global_t canbus = {
    .init = canbus_init,
    .set_mode = canbus_set_mode,
    .set_mask = canbus_set_mask,
    .assign_mask = canbus_assign_mask,
    .set_filter = canbus_set_filter,
    .set_buffer_pointer = canbus_set_buffer_pointer,
    .enable_filter = canbus_enable_filter,
    .disable_filter = canbus_disable_filter,
    .open = canbus_open,
    .write = canbus_write,
    .abort_write = canbus_abort_write,
    .read = canbus_read,
    .peek = canbus_peek,
    .is_empty = canbus_is_empty,
    .is_open = canbus_is_open,
    .is_valid = canbus_is_valid,
    .close = canbus_close,
    .clean_up = canbus_clean_up,
    .irq = canbus_irq
};

/* ***** Private Function Definitions ***** */

/**
 * @brief The initialization function for a CANBus object.
 *
 * @details Nothing here.
 *
 * @private
 */
static int canbus_init(canbus_t *object,
                       canbus_attr_t *attr,
                       unsigned int tx_dma_channel,
                       unsigned int rx_dma_channel,
                       volatile unsigned int *dma_buffer,
                       unsigned int dma_buffer_length)
{
    dma_attr_t tx_dma_attr;
    dma_attr_t rx_dma_attr;
    unsigned int last_fifo_buffer;
    
    // Check for a valid canbus pointer and module number
    if( object == NULL \
        || object->module_number == 0 \
        || object->module_number > CANBUS_HW_NUMBER_OF_MODULES )
    {// Invalid canbus pointer or module number
        return CANBUS_E_OBJECT;
    }

    // Check for valid attr pointer
    if( attr == NULL )
    {// Invalid attr pointer
        return CANBUS_E_INPUT;
    }

    // Allocate a new private struct (initialize all values to zero)
    // Any errors past this point must call clean_up() before returning!
    object->private = calloc(1,sizeof(canbus_private_t));
    if( object->private == NULL )
    {// Allocation failed
        return CANBUS_E_ALLOC;
    }

#if (CANBUS_HW_NUMBER_OF_MODULES < 1) || (CANBUS_HW_NUMBER_OF_MODULES > 2)
#error "CANBUS(__file__,__line__): CANBUS_HW_NUMBER_OF_MODULES is out of bounds. Expected a number \
between 1 and 2."
#endif
    switch( object->module_number )
    {
#if CANBUS_HW_NUMBER_OF_MODULES >= 1
    case 1:
        CANBUS_BASE_ADDRESS(object) = CANBUS_HW_BASE_ADDRESS_C1;
        break;
#endif
#if CANBUS_HW_NUMBER_OF_MODULES >= 2
    case 2:
        CANBUS_BASE_ADDRESS(object) = CANBUS_HW_BASE_ADDRESS_C2;
        break;
#endif
    default:
        // Invalid module_number
        // Should never happen!
        // Assertion failed!
        canbus.clean_up(object);
        return CANBUS_E_ASSERT;
    }

    // Save the attr object to the private data object
    ((canbus_private_t *)object->private)->attr_ = *attr;

    // Set configuration mode
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
        = CANBUS_OPMODE_CONFIGURATION;

    // Wait for configuration mode
    while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
           ->opmode != CANBUS_OPMODE_CONFIGURATION )
    {
    }
    
    // Set all SFRs to default values
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1) = CANBUS_SFR_DEFAULT_CiCTRL1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL2) = CANBUS_SFR_DEFAULT_CiCTRL2;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiVEC) = CANBUS_SFR_DEFAULT_CiVEC;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL) = CANBUS_SFR_DEFAULT_CiFCTRL;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFIFO) = CANBUS_SFR_DEFAULT_CiFIFO;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiINTF) = CANBUS_SFR_DEFAULT_CiINTF;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiINTE) = CANBUS_SFR_DEFAULT_CiINTE;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiEC) = CANBUS_SFR_DEFAULT_CiEC;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1) = CANBUS_SFR_DEFAULT_CiCFG1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2) = CANBUS_SFR_DEFAULT_CiCFG2;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1) = CANBUS_SFR_DEFAULT_CiFEN1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFMSKSEL1) = CANBUS_SFR_DEFAULT_CiFMSKSEL1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFMSKSEL2) = CANBUS_SFR_DEFAULT_CiFMSKSEL2;

    // Set buffer window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

    // Set buffer SFRs to default values
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL1) = CANBUS_SFR_DEFAULT_CiRXFUL1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL2) = CANBUS_SFR_DEFAULT_CiRXFUL2;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXOVF1) = CANBUS_SFR_DEFAULT_CiRXOVF1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXOVF2) = CANBUS_SFR_DEFAULT_CiRXOVF2;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON) = CANBUS_SFR_DEFAULT_CiTR01CON;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON) = CANBUS_SFR_DEFAULT_CiTR23CON;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON) = CANBUS_SFR_DEFAULT_CiTR45CON;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON) = CANBUS_SFR_DEFAULT_CiTR67CON;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXD) = CANBUS_SFR_DEFAULT_CiRXD;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTXD) = CANBUS_SFR_DEFAULT_CiTXD;

    // Set filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    // Set filter SFRs to default values
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1) = CANBUS_SFR_DEFAULT_CiBUFPNT1;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2) = CANBUS_SFR_DEFAULT_CiBUFPNT2;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3) = CANBUS_SFR_DEFAULT_CiBUFPNT3;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4) = CANBUS_SFR_DEFAULT_CiBUFPNT4;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0SID) = CANBUS_SFR_DEFAULT_CiRXM0SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0EID) = CANBUS_SFR_DEFAULT_CiRXM0EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID) = CANBUS_SFR_DEFAULT_CiRXM1SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1EID) = CANBUS_SFR_DEFAULT_CiRXM1EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID) = CANBUS_SFR_DEFAULT_CiRXM2SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2EID) = CANBUS_SFR_DEFAULT_CiRXM2EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID) = CANBUS_SFR_DEFAULT_CiRXF0SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0EID) = CANBUS_SFR_DEFAULT_CiRXF0EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID) = CANBUS_SFR_DEFAULT_CiRXF1SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1EID) = CANBUS_SFR_DEFAULT_CiRXF1EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID) = CANBUS_SFR_DEFAULT_CiRXF2SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2EID) = CANBUS_SFR_DEFAULT_CiRXF2EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID) = CANBUS_SFR_DEFAULT_CiRXF3SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3EID) = CANBUS_SFR_DEFAULT_CiRXF3EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID) = CANBUS_SFR_DEFAULT_CiRXF4SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4EID) = CANBUS_SFR_DEFAULT_CiRXF4EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID) = CANBUS_SFR_DEFAULT_CiRXF5SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5EID) = CANBUS_SFR_DEFAULT_CiRXF5EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID) = CANBUS_SFR_DEFAULT_CiRXF6SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6EID) = CANBUS_SFR_DEFAULT_CiRXF6EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID) = CANBUS_SFR_DEFAULT_CiRXF7SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7EID) = CANBUS_SFR_DEFAULT_CiRXF7EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID) = CANBUS_SFR_DEFAULT_CiRXF8SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8EID) = CANBUS_SFR_DEFAULT_CiRXF8EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID) = CANBUS_SFR_DEFAULT_CiRXF9SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9EID) = CANBUS_SFR_DEFAULT_CiRXF9EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID) = CANBUS_SFR_DEFAULT_CiRXF10SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10EID) = CANBUS_SFR_DEFAULT_CiRXF10EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID) = CANBUS_SFR_DEFAULT_CiRXF11SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11EID) = CANBUS_SFR_DEFAULT_CiRXF11EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID) = CANBUS_SFR_DEFAULT_CiRXF12SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12EID) = CANBUS_SFR_DEFAULT_CiRXF12EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID) = CANBUS_SFR_DEFAULT_CiRXF13SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13EID) = CANBUS_SFR_DEFAULT_CiRXF13EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID) = CANBUS_SFR_DEFAULT_CiRXF14SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14EID) = CANBUS_SFR_DEFAULT_CiRXF14EID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID) = CANBUS_SFR_DEFAULT_CiRXF15SID;
    *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15EID) = CANBUS_SFR_DEFAULT_CiRXF15EID;

    // Set hardware bit timing
    ((canbus_cicfg1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1))->brp \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing_pre;
    ((canbus_cicfg1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1))->sjw \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing_sync_jump;
    ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2))->prseg \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing_prop_seg;
    ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2))->seg1ph \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing_phase_seg1;
    if( ((canbus_cicfg2_bits_t *)(object->private))->attr_.bit_timing_phase_seg2_prog \
        == CANBUS_BIT_TIMING_PHASE_SEG2_PROG_EN )
    {
        ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2)) \
            ->seg2phts = CANBUS_BIT_TIMING_PHASE_SEG2_PROG_EN;
        ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2)) \
            ->seg2ph = ((canbus_private_t *)(object->private))->attr_.bit_timing_phase_seg2_prog;
    }
    ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2))->sam \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing_sample;

    // Set module configuration
    if( ((canbus_private_t *)(object->private))->attr_.module_wakeup == CANBUS_MODULE_WAKEUP_EN )
    {
        ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1)) \
            ->wakfil = 1;
    }
    if( ((canbus_private_t *)(object->private))->attr_.module_cpuidle \
        == CANBUS_MODULE_CPUIDLE_STOP )
    {
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
            ->csidl = 1;
    }
    if( ((canbus_private_t *)(object->private))->attr_.module_timestamp \
        == CANBUS_MODULE_TIMESTAMP_EN )
    {
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
            ->cancap = 1;
    }

    // Allocate TX DMA channel
    ((canbus_private_t *)(object->private))->tx_dma_ = calloc(1, sizeof(dma_channel_t));
    // Check if allocation failed
    if( ((canbus_private_t *)(object->private))->tx_dma_ == NULL )
    {// Allocation failed
        canbus.clean_up(object);
        return CANBUS_E_ALLOC;
    }

    // Allocate RX DMA channel
    ((canbus_private_t *)(object->private))->rx_dma_ = calloc(1, sizeof(dma_channel_t));
    // Check if allocation failed
    if( ((canbus_private_t *)(object->private))->rx_dma_ == NULL )
    {// Allocation failed
        canbus.clean_up(object);
        return CANBUS_E_ALLOC;
    }

    // Set DMA channel numbers
    ((canbus_private_t *)(object->private))->tx_dma_->channel_number = tx_dma_channel;
    ((canbus_private_t *)(object->private))->rx_dma_->channel_number = rx_dma_channel;

    // Set DMA buffer pointers and lengths
    ((canbus_private_t *)(object->private))->tx_dma_->buffer_a = dma_buffer;
    ((canbus_private_t *)(object->private))->tx_dma_->buffer_a_length = dma_buffer_length;
    ((canbus_private_t *)(object->private))->rx_dma_->buffer_a = dma_buffer;
    ((canbus_private_t *)(object->private))->rx_dma_->buffer_a_length = dma_buffer_length;
    
    // Set up DMA attributes
    tx_dma_attr.config = DMA_CONFIG_OPMODE_CONTINUOUS     \
        | DMA_CONFIG_PINGPONG_DIS                         \
        | DMA_CONFIG_ADDRMODE_PERIPHERAL_IND              \
        | DMA_CONFIG_NULLWRITE_DIS                        \
        | DMA_CONFIG_DIR_TO_PERIPHERAL                    \
        | DMA_CONFIG_DATASIZE_WORD;
    
    switch( object->module_number )
    {
#if CANBUS_HW_NUMBER_OF_MODULES >= 1
    case 1:
        tx_dma_attr.irq = DMA_IRQ_ECAN1TX;
        tx_dma_attr.peripheral_address = DMA_PERIPHERAL_C1TXD;
        rx_dma_attr.irq = DMA_IRQ_ECAN1RX;
        rx_dma_attr.peripheral_address = DMA_PERIPHERAL_C1RXD;
        break;
#endif
#if CANBUS_HW_NUMBER_OF_MODULES >= 2
    case 2:
        tx_dma_attr.irq = DMA_IRQ_ECAN2TX;
        tx_dma_attr.peripheral_address = DMA_PERIPHERAL_C2TXD;
        rx_dma_attr.irq = DMA_IRQ_ECAN2RX;
        rx_dma_attr.peripheral_address = DMA_PERIPHERAL_C2RXD;
        break;
#endif
    default:
        // Invalid module_number
        // Should never happen!
        // Assertion failed!
        canbus.clean_up(object);
        return CANBUS_E_ASSERT;
    }
    
    // Initialize TX DMA
    if( dma_init( ((canbus_private_t *)(object->private))->tx_dma_, &tx_dma_attr ) < 0 )
    {// Initialization failed, DMA channel was not valid
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }// Else, DMA channel initialization was successful

    // Set TX DMA block size
    dma_set_block_size( ((canbus_private_t *)(object->private))->tx_dma_, 8 );

    // Initialize RX DMA
    if( dma_init( ((canbus_private_t *)(object->private))->rx_dma_, &rx_dma_attr ) < 0 )
    {// Initialization failed, DMA channel was not valid
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }// Else, DMA channel initialization was successful

    // Set RX DMA block size
    dma_set_block_size( ((canbus_private_t *)(object->private))->rx_dma_, 8 );

    // Enable DMA (filters are still disabled, so nothing can be received)
    dma_enable( ((canbus_private_t *)(object->private))->tx_dma_ );
    dma_enable( ((canbus_private_t *)(object->private))->rx_dma_ );

    // Note: DMA interrupts should not be enabled, they are not used

    // Set up FIFO buffer
    
    // Set start of FIFO buffer
    // Start calculation of last FIFO buffer by getting start buffer of FIFO
    switch( ((canbus_private_t *)(object->private))->attr_.fifo_start )
    {
    case CANBUS_FIFO_START_B0:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00000;
        last_fifo_buffer = 0;
        break;
    case CANBUS_FIFO_START_B1:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00001;
        last_fifo_buffer = 1;
        break;
    case CANBUS_FIFO_START_B2:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00010;
        last_fifo_buffer = 2;
        break;
    case CANBUS_FIFO_START_B3:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00011;
        last_fifo_buffer = 3;
        break;
    case CANBUS_FIFO_START_B4:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00100;
        last_fifo_buffer = 4;
        break;
    case CANBUS_FIFO_START_B5:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00101;
        last_fifo_buffer = 5;
        break;
    case CANBUS_FIFO_START_B6:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00110;
        last_fifo_buffer = 6;
        break;
    case CANBUS_FIFO_START_B7:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00111;
        last_fifo_buffer = 7;
        break;
    case CANBUS_FIFO_START_B8:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01000;
        last_fifo_buffer = 8;
        break;
    case CANBUS_FIFO_START_B9:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01001;
        last_fifo_buffer = 9;
        break;
    case CANBUS_FIFO_START_B10:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01010;
        last_fifo_buffer = 10;
        break;
    case CANBUS_FIFO_START_B11:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01011;
        last_fifo_buffer = 11;
        break;
        case CANBUS_FIFO_START_B12:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01100;
        last_fifo_buffer = 12;
        break;
    case CANBUS_FIFO_START_B13:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01101;
        last_fifo_buffer = 13;
        break;
    case CANBUS_FIFO_START_B14:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01110;
        last_fifo_buffer = 14;
        break;
    case CANBUS_FIFO_START_B15:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01111;
        last_fifo_buffer = 15;
        break;
    case CANBUS_FIFO_START_B16:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10000;
        last_fifo_buffer = 16;
        break;
    case CANBUS_FIFO_START_B17:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10001;
        last_fifo_buffer = 17;
        break;
    case CANBUS_FIFO_START_B18:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10010;
        last_fifo_buffer = 18;
        break;
    case CANBUS_FIFO_START_B19:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10011;
        last_fifo_buffer = 19;
        break;
    case CANBUS_FIFO_START_B20:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10100;
        last_fifo_buffer = 20;
        break;
    case CANBUS_FIFO_START_B21:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10101;
        last_fifo_buffer = 21;
        break;
    case CANBUS_FIFO_START_B22:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10110;
        last_fifo_buffer = 22;
        break;
    case CANBUS_FIFO_START_B23:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10111;
        last_fifo_buffer = 23;
        break;
    case CANBUS_FIFO_START_B24:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11000;
        last_fifo_buffer = 24;
        break;
    case CANBUS_FIFO_START_B25:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11001;
        last_fifo_buffer = 25;
        break;
    case CANBUS_FIFO_START_B26:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11010;
        last_fifo_buffer = 26;
        break;
    case CANBUS_FIFO_START_B27:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11011;
        last_fifo_buffer = 27;
        break;
    case CANBUS_FIFO_START_B28:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11100;
        last_fifo_buffer = 28;
        break;
    default:
        // Unknown or invalid start for FIFO buffer
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    
    switch( ((canbus_private_t *)(object->private))->attr_.fifo_length )
    {
    case CANBUS_FIFO_LENGTH_4:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b000;
        last_fifo_buffer += 4;
        break;
    case CANBUS_FIFO_LENGTH_6:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b001;
        last_fifo_buffer += 6;
        break;
    case CANBUS_FIFO_LENGTH_8:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b010;
        last_fifo_buffer += 8;
        break;
    case CANBUS_FIFO_LENGTH_12:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b011;
        last_fifo_buffer += 12;
        break;
    case CANBUS_FIFO_LENGTH_16:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b100;
        last_fifo_buffer += 16;
        break;
    case CANBUS_FIFO_LENGTH_24:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b101;
        last_fifo_buffer += 24;
        break;
    case CANBUS_FIFO_LENGTH_32:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b110;
        last_fifo_buffer += 32;
        break;
    default:
        // Unknown or invalid length of FIFO buffer
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }

    // Check if DMA buffer is large enough for FIFO
    if( (((canbus_private_t *)(object->private))->rx_dma_.buffer_a_size)/8 < last_fifo_buffer )
    {// DMA buffer is too small
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }// Else, DMA buffer is at least large enough to hold FIFO buffer.
    
    // Set buffer window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

    // Set mode to DISABLE
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
        = CANBUS_OPMODE_DISABLE;

    // Wait for DISABLE mode
    while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
           ->opmode != CANBUS_OPMODE_DISABLE )
    {
    }
}
    
/**
 * @brief Set the mode of the CAN bus hardware.
 *
 * @details Nothing here.
 *
 * @private
 */
static int canbus_set_mode(canbus_t *object,
                           canbus_mode_t mode)
{
    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return CANBUS_E_OBJECT;
    }

    switch( mode )
    {
    case CANBUS_MODE_DISABLE:
        // Set mode to DISABLE
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
            = CANBUS_OPMODE_DISABLE;
        
        // Wait for DISABLE mode
        while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
               ->opmode != CANBUS_OPMODE_DISABLE )
        {
        }
        break;
    case CANBUS_MODE_NORMAL:
        // Set mode to NORMAL
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
            = CANBUS_OPMODE_NORMAL;
        
        // Wait for NORMAL mode
        while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
               ->opmode != CANBUS_OPMODE_NORMAL )
        {
        }
        break;
    case CANBUS_MODE_LISTEN:
        // Set mode to LISTENONLY
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
            = CANBUS_OPMODE_LISTENONLY;
        
        // Wait for LISTENONLY mode
        while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
               ->opmode != CANBUS_OPMODE_LISTENONLY )
        {
        }
        break;
    case CANBUS_MODE_DEBUG_LOOPBACK:
        // Set mode to LOOPBACK
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
            = CANBUS_OPMODE_LOOPBACK;
        
        // Wait for LOOPBACK mode
        while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
               ->opmode != CANBUS_OPMODE_LOOPBACK )
        {
        }
        break;
    case CANBUS_MODE_DEBUG_LISTENALL:
        // Set mode to LISTENALL
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
            = CANBUS_OPMODE_LISTENALL;
        
        // Wait for LISTENALL mode
        while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
               ->opmode != CANBUS_OPMODE_LISTENALL )
        {
        }
        break;
    default:
        // Unknown mode
        return CANBUS_E_INPUT;
    }
}

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
 * @private
 */
static int canbus_notify_on(canbus_t *object,
                            int notification)
{
}
    
/**
 * @brief Set the value of a mask in the CAN bus module.
 *
 * @details Nothing here.
 *
 * @private
 */
static int canbus_set_mask(canbus_t *object,
                           canbus_mask_t mask_num,
                           canbus_header_t *mask_value)
{
    // Check for a valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    if( mask_num == CANBUS_MASK_M0 || mask_num == CANBUS_MASK_ALL )
    {
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0SID)) \
            ->sid = mask_value->sid;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0SID)) \
            ->mide = mask_value->ide;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0SID)) \
            ->eid_h = mask_value->eid_h;
        ((canbus_cirxmneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0EID)) \
            ->eid_l = mask_value->eid_l;
    }
    if( mask_num == CANBUS_MASK_M1 || mask_num == CANBUS_MASK_ALL )
    {
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID)) \
            ->sid = mask_value->sid;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID)) \
            ->mide = mask_value->ide;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID)) \
            ->eid_h = mask_value->eid_h;
        ((canbus_cirxmneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1EID)) \
            ->eid_l = mask_value->eid_l;
    }
    if( mask_num == CANBUS_MASK_M2 || mask_num == CANBUS_MASK_ALL )
    {
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID)) \
            ->sid = mask_value->sid;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID)) \
            ->mide = mask_value->ide;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID)) \
            ->eid_h = mask_value->eid_h;
        ((canbus_cirxmneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2EID)) \
            ->eid_l = mask_value->eid_l;
    }

    // Switch to buffer window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

    return CANBUS_E_NONE;
}
    
/**
 * @brief Assign a mask to a filter in the CAN bus module.
 *
 * @details Nothing here.
 *
 * @private
 */
static int canbus_assign_mask(canbus_t *object,
                              canbus_mask_t mask_num,
                              canbus_filter_t filter_num)
{
    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // Check for valid mask_num input
    // Mask must be M0, M1, or M2, not NONE or ALL (or something else)
    if( !(mask_num == CANBUS_MASK_M0            \
          || mask_num == CANBUS_MASK_M1         \
          || mask_num == CANBUSS_MASK_M2) )
    {// Invalid input
        return CANBUS_E_INPUT;
    }

    // Check for avlid filter_num input
    // Filter must be F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, or F15,
    // not NONE (or something else)
    if( !(filter_num == CANBUS_FILTER_F0        \
          || filter_num == CANBUS_FILTER_F1     \
          || filter_num == CANBUS_FILTER_F2     \
          || filter_num == CANBUS_FILTER_F3     \
          || filter_num == CANBUS_FILTER_F4     \
          || filter_num == CANBUS_FILTER_F5     \
          || filter_num == CANBUS_FILTER_F6     \
          || filter_num == CANBUS_FILTER_F7     \
          || filter_num == CANBUS_FILTER_F8     \
          || filter_num == CANBUS_FILTER_F9     \
          || filter_num == CANBUS_FILTER_F10    \
          || filter_num == CANBUS_FILTER_F11    \
          || filter_num == CANBUS_FILTER_F12    \
          || filter_num == CANBUS_FILTER_F13    \
          || filter_num == CANBUS_FILTER_F14    \
          || filter_num == CANBUS_FILTER_F15    \
          || filter_num == CANBUS_FILTER_ALL) )
    {// Invalid input
        return CANBUS_E_INPUT;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    if( filter_num == CANBUS_FILTER_F0 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 0
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f0msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 0
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f0msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 0
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f0msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F1 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 1
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f1msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 1
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f1msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 1
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f1msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F2 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 2
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f2msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 2
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f2msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 2
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f2msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F3 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 3
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f3msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 3
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f3msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 3
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f3msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F4 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 4
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f4msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 4
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f4msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 4
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f4msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F5 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 5
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f5msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 5
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f5msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 5
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f5msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F6 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 6
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f6msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 6
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f6msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 6
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f6msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F7 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 7
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f7msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 7
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f7msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 7
            ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f7msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F8 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 8
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f8msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 8
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f8msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 8
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f8msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F9 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 9
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f9msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 9
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f9msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 9
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f9msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F10 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 10
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f10msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 10
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f10msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 10
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f10msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F11 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 11
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f11msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 11
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f11msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 11
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f11msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F12 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 12
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f12msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 12
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f12msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 12
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f12msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F13 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 13
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f13msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 13
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f13msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 13
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f13msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F14 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 14
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f14msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 14
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f14msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 14
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f14msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_F15 )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 15
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 15
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 15
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_ALL )
    {
        if( mask_num == CANBUS_MASK_M0 )
        {// Assign mask 0 to filter 15
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = 0b00;
        }
        else if( mask_num == CANBUS_MASK_M1 )
        {// Assign mask 1 to filter 15
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = 0b01;
        }
        else
        {// Assign mask 2 to filter 15
            ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)   \
                                          + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = 0b10;
        }
    }
    else if( filter_num == CANBUS_FILTER_NONE )
    {
        // Do nothing
    }
    else
    {// Catch-all for invalid input
        // Switch to buffer window
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

        // Return an error
        return CANBUS_E_INPUT;
    }

    // Switch to buffer window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

    return CANBUS_E_NONE;
}

/**
 * @brief Set the value of a filter in the CAN bus module.
 *
 * @details Nothing here.
 *
 * @private
 */
static int canbus_set_filter(canbus_t *object,
                             canbus_filter_t filter_num,
                             canbus_header_t *filter_value)
{
    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    if( filter_num == CANBUS_FILTER_F0 )
    {// Set filter 0
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F1 )
    {// Set filter 1
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F2 )
    {// Set filter 2
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F3 )
    {// Set filter 3
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F4 )
    {// Set filter 4
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F5 )
    {// Set filter 5
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F6 )
    {// Set filter 6
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F7 )
    {// Set filter 7
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F8 )
    {// Set filter 8
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F9 )
    {// Set filter 9
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F10 )
    {// Set filter 10
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F11 )
    {// Set filter 11
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F12 )
    {// Set filter 12
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F13 )
    {// Set filter 13
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F14 )
    {// Set filter 14
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_F15 )
    {// Set filter 15
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_ALL )
    {// Set all filters
        // Set filter 0
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 1
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 2
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 3
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 4
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 5
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 6
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 7
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 8
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 9
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 10
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 11
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 12
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 13
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 14
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14EID)) \
            ->eid_l = filter_value->eid_l;

        // Set filter 15
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->mide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->eid_h = filter_value->eid_h;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15EID)) \
            ->eid_l = filter_value->eid_l;
    }
    else if( filter_num == CANBUS_FILTER_NONE )
    {
        // Do nothing
    }
    else
    {// Catch-all for invalid input
        // Switch to buffer window
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

        // Return error
        return CANBUS_E_INPUT;
    }   
    
    // Switch to buffer window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

    return CANBUS_E_NONE;
}

/**
 * @brief Set which buffer a message should go to when it hits a particular filter.
 *
 * @details Nothing here.
 *
 * @note The default buffer for a received message is B0, however, if a buffer is opened as a
 * transmit buffer and tries to receive a message it could lead to unknown behavior. The only
 * exception to this is if the message is marked as 'auto_tx', in which case a received RTR
 * message matching a filter pointing to that buffer will cause the message to be sent.
 *
 * @private
 */
static int canbus_set_buffer_pointer(canbus_t *object,
                                     canbus_filter_t filter_num,
                                     canbus_buffer_t buffer_num)
{
}

/**
 * @brief Enable a filter.
 *
 * @details The given filter will now be checked against incoming messages, looking for a
 * match. If there is a match the message will be sent to the buffer specified using
 * @ref set_buffer_pointer().
 *
 * If a filter is enabled and it's buffer pointer points to a buffer that is not open for
 * receiving, then no messages will be received by that filter. As soon as the buffer is
 * opened as RX messages may be recieved by the filter into that buffer.
 *
 * @private
 */
static int canbus_enable_filter(canbus_t *object,
                                canbus_filter_t filter_num)
{
}

/**
 * @brief Disable a filter.
 *
 * @details The given filter will stop checking incoming messages.
 *
 * @private
 */
static int canbus_disable_filter(canbus_t *object,
                                 canbus_filter_t filter_num)
{
}

/**
 * @brief Open a buffer for either reading or writing.
 *
 * @details Open a single buffer for either reading or writing. Only B0-B7 may be opened for
 * writing and only B0-B14 and the FIFO buffer may be opened for reading. Any other inputs will
 * be ignored.
 *
 * Opening a buffer as RX is not enough for it to receive messages. It must also have a filter
 * buffer pointer pointing at it with that filter enabled.
 *
 * @private
 */
static int canbus_open(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_direction_t direction)
{
}

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
 * @private
 */
static int canbus_write(canbus_t *object,
                        canbus_buffer_t buffer_num,
                        const canbus_message_t *message,
                        canbus_priority_t priority)
{
}

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
 * @private
 */
static int canbus_abort_write(canbus_t *object,
                              canbus_buffer_t buffer_num)
{
}
    
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
 * @private
 */
static int canbus_read(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message)
{
}

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
 * @private
 */
static int canbus_peek(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message)
{
}

/**
 * @brief Check if the specified buffer is empty.
 *
 * @details Check if the specified buffer is empty.
 *
 * @param[in]  canbus     The canbus_t object to check.
 * @param[in]  buffer_num The buffer to check.
 * @return If the buffer is empty, <cc>true</cc>; otherwise <cc>false</cc>.
 *
 * @private
 */
static bool canbus_is_empty(canbus_t *object,
                            canbus_buffer_t buffer_num)
{
}

/**
 * @brief Check if the specified buffer is open in the specified direction.
 *
 * @details Check if the specified buffer is opened in the specified direction. If
 * CANBUS_DIRECTION_TXRX is given the function will return <cc>true</cc> if either direction
 * is open.
 *
 * @param[in]  canbus     The canbus_t object to check.
 * @param[in]  buffer_num The buffer of which to check the open state.
 * @param[in]  direction  The direction to check when checking for an open state.
 * @return A boolean value, <cc>true</cc> if the buffer is open in the specified direction or
 * <cc>false</cc> otherwise.
 *
 * @private
 */
static bool canbus_is_open(canbus_t *object,
                           canbus_buffer_t buffer_num,
                           canbus_direction_t direction)
{
}

/**
 * @brief Check if a CAN bus object is valid.
 *
 * @details Nothing here.
 *
 * @private
 */
static bool canbus_is_valid(canbus_t *object)
{
    return ( object != NULL \
             && object->module_number > 0 \
             && object->module_number <= CANBUS_HW_NUMBER_OF_MODULES \
             && object->private != NULL );
}

/**
 * @brief Close the specified buffer.
 *
 * @details Close the specified buffer in both RX and TX directions. If CANBUS_BUFFER_ALL is
 * given then all buffers will attempt to close.
 *
 * If a buffer is opened as TX and has a message waiting for transmission then attempting to
 * close it will fail. The message must be aborted first then the buffer may be closed. If
 * there is a message waiting to be read then the message will be lost and the buffer closed.
 *
 * @param[in]  canbus     The canbus_t object to work on.
 * @param[in]  buffer_num The buffer to close.
 * @return A @ref canbus_error_t value.
 *
 * @private
 */
static int canbus_close(canbus_t *object,
                        canbus_buffer_t buffer_num)
{
}

/**
 * @brief Free any dynamically allocated memory and shutdown the hardware module.
 *
 * @details Nothing here.
 *
 * @private
 */
static void canbus_clean_up(canbus_t *object)
{
    // Check for valid object pointer
    if( object != NULL )
    {// Valid object pointer
        // Check for valid private object
        if( object->private != NULL )
        {// Valid private object

            // Check for valid base address
            if( CANBUS_BASE_ADDRESS(object) != NULL )
            {// Valid base address

                // Set configuration mode
                ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
                    = CANBUS_OPMODE_CONFIGURATION;
                
                // Wait for configuration mode
                while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
                       ->opmode != CANBUS_OPMODE_CONFIGURATION )
                {
                }
                
                // Set all SFRs to default values
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1) = CANBUS_SFR_DEFAULT_CiCTRL1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL2) = CANBUS_SFR_DEFAULT_CiCTRL2;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiVEC) = CANBUS_SFR_DEFAULT_CiVEC;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL) = CANBUS_SFR_DEFAULT_CiFCTRL;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFIFO) = CANBUS_SFR_DEFAULT_CiFIFO;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiINTF) = CANBUS_SFR_DEFAULT_CiINTF;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiINTE) = CANBUS_SFR_DEFAULT_CiINTE;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiEC) = CANBUS_SFR_DEFAULT_CiEC;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1) = CANBUS_SFR_DEFAULT_CiCFG1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2) = CANBUS_SFR_DEFAULT_CiCFG2;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1) = CANBUS_SFR_DEFAULT_CiFEN1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFMSKSEL1) = CANBUS_SFR_DEFAULT_CiFMSKSEL1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFMSKSEL2) = CANBUS_SFR_DEFAULT_CiFMSKSEL2;
                
                // Set buffer window
                ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;
                
                // Set buffer SFRs to default values
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL1) = CANBUS_SFR_DEFAULT_CiRXFUL1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL2) = CANBUS_SFR_DEFAULT_CiRXFUL2;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXOVF1) = CANBUS_SFR_DEFAULT_CiRXOVF1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXOVF2) = CANBUS_SFR_DEFAULT_CiRXOVF2;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON) = CANBUS_SFR_DEFAULT_CiTR01CON;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON) = CANBUS_SFR_DEFAULT_CiTR23CON;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON) = CANBUS_SFR_DEFAULT_CiTR45CON;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON) = CANBUS_SFR_DEFAULT_CiTR67CON;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXD) = CANBUS_SFR_DEFAULT_CiRXD;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTXD) = CANBUS_SFR_DEFAULT_CiTXD;
                
                // Set filter window
                ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;
                
                // Set filter SFRs to default values
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1) = CANBUS_SFR_DEFAULT_CiBUFPNT1;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2) = CANBUS_SFR_DEFAULT_CiBUFPNT2;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3) = CANBUS_SFR_DEFAULT_CiBUFPNT3;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4) = CANBUS_SFR_DEFAULT_CiBUFPNT4;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0SID) = CANBUS_SFR_DEFAULT_CiRXM0SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0EID) = CANBUS_SFR_DEFAULT_CiRXM0EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID) = CANBUS_SFR_DEFAULT_CiRXM1SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1EID) = CANBUS_SFR_DEFAULT_CiRXM1EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID) = CANBUS_SFR_DEFAULT_CiRXM2SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2EID) = CANBUS_SFR_DEFAULT_CiRXM2EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID) = CANBUS_SFR_DEFAULT_CiRXF0SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0EID) = CANBUS_SFR_DEFAULT_CiRXF0EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID) = CANBUS_SFR_DEFAULT_CiRXF1SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1EID) = CANBUS_SFR_DEFAULT_CiRXF1EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID) = CANBUS_SFR_DEFAULT_CiRXF2SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2EID) = CANBUS_SFR_DEFAULT_CiRXF2EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID) = CANBUS_SFR_DEFAULT_CiRXF3SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3EID) = CANBUS_SFR_DEFAULT_CiRXF3EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID) = CANBUS_SFR_DEFAULT_CiRXF4SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4EID) = CANBUS_SFR_DEFAULT_CiRXF4EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID) = CANBUS_SFR_DEFAULT_CiRXF5SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5EID) = CANBUS_SFR_DEFAULT_CiRXF5EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID) = CANBUS_SFR_DEFAULT_CiRXF6SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6EID) = CANBUS_SFR_DEFAULT_CiRXF6EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID) = CANBUS_SFR_DEFAULT_CiRXF7SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7EID) = CANBUS_SFR_DEFAULT_CiRXF7EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID) = CANBUS_SFR_DEFAULT_CiRXF8SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8EID) = CANBUS_SFR_DEFAULT_CiRXF8EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID) = CANBUS_SFR_DEFAULT_CiRXF9SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9EID) = CANBUS_SFR_DEFAULT_CiRXF9EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID) = CANBUS_SFR_DEFAULT_CiRXF10SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10EID) = CANBUS_SFR_DEFAULT_CiRXF10EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID) = CANBUS_SFR_DEFAULT_CiRXF11SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11EID) = CANBUS_SFR_DEFAULT_CiRXF11EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID) = CANBUS_SFR_DEFAULT_CiRXF12SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12EID) = CANBUS_SFR_DEFAULT_CiRXF12EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID) = CANBUS_SFR_DEFAULT_CiRXF13SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13EID) = CANBUS_SFR_DEFAULT_CiRXF13EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID) = CANBUS_SFR_DEFAULT_CiRXF14SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14EID) = CANBUS_SFR_DEFAULT_CiRXF14EID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID) = CANBUS_SFR_DEFAULT_CiRXF15SID;
                *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15EID) = CANBUS_SFR_DEFAULT_CiRXF15EID;
                
                // Set buffer window
                ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;
            }

            // Clean up DMA channels
            dma_cleanup( ((canbus_private_t *)(object->private))->tx_dma_ );
            dma_cleanup( ((canbus_private_t *)(object->private))->rx_dma_ );

            // Free DMA channel memory
            free( ((canbus_private_t *)(object->private))->tx_dma_ );
            free( ((canbus_private_t *)(object->private))->rx_dma_ );

            // Free private object
            free( object->private );
        }
    }
}

/* ***** Canbus Object ISR ***** */

/**
 * @brief The ISR for the canbus object.
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
 * @private
 */
static void canbus_isr(canbus_t *object)
{
}



int can_init(can_module_t *module)
{
    //! @TODO Allow for partial init (only RX or only TX).
    //! @TODO Move PPS settings to kernel.

    // Set up PPS
    PPSUnLock;
    PPSOutput(PPS_C1TX,PPS_RP9);
    PPSInput(PPS_C1RX,PPS_RP8);
    PPSLock;

    // Check for valid can_base_address
    if( module->can_base_address == NULL )
    {
        return -1;
    }

    // Check for valid tx_dma_base_address
    if( module->tx_dma_base_address == NULL )
    {
        return -1;
    }

    // Check for valid rx_dma_base_address
    if( module->rx_dma_base_address == NULL )
    {
        return -1;
    }

    // Check for valid can_buffer
    if( module->can_buffer == NULL )
    {
        return -1;
    }
    
    // Switch to configuration mode
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) = (*(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) & 0xF8FF) | (CAN_MODE_CONFIG<<8);
    while( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) & 0x00E0) != (CAN_MODE_CONFIG)<<5 );
    module->open_mode = CAN_MODE_CONFIG;
    
    // Set up bit timing
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCFG1_OFFSET) = (module->bit_timing&(SYNC_JUMP_MASK | BAUD_PRE_MASK));
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCFG2_OFFSET) = ((module->bit_timing&ENABLE_WAKE)>>5) | (module->bit_timing&PHASE_SEG2_MASK)>>6 | ((module->bit_timing&ENABLE_PHASE_SEG2_PROG)>>10) | ((module->bit_timing&TRIPLE_SAMPLE)>>12) | (module->bit_timing&(PHASE_SEG1_MASK | PROP_SEG_MASK))>>8;
    
    // Switch to filter window
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) |= 0x0001;
    
    // Disable all filters by default
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiFEN1_OFFSET) = 0x0000;
    
    // Set up filter buffer pointers
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiBUFPNT1_OFFSET) = 0xFFFF;
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiBUFPNT2_OFFSET) = 0xFFFF;
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiBUFPNT3_OFFSET) = 0xFFFF;
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiBUFPNT4_OFFSET) = 0xFFFF;
    
    // Switch to buffer window
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
    
    // Set up TX buffers
    // TR0: Highest Priority
    // TR1: Highest Priority
    // TR2: High Priority
    // TR3: High Priority
    // TR4: Low Priority
    // TR5: Low Priority
    // TR6: Lowest Priority
    // TR7: Lowest Priority
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET) = (1<<15) | (0b11<<8) | (1<<7) | (0b11);
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET) = (1<<15) | (0b10<<8) | (1<<7) | (0b10);
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET) = (1<<15) | (0b01<<8) | (1<<7) | (0b01);
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET) = (1<<15) | (0b00<<8) | (1<<7) | (0b00);
    
    // Set up RX FIFO buffer
    // Start buffer: 8
    // Number of buffers: 24
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiFCTRL_OFFSET) = (0xA008);
    
    // Set up CAN interrupts
    //! @TODO Write error handling functions.
    //*(module->can_base_address + CANBUS_SFR_OFFSET_CiINTE_OFFSET) |= (0x0003);
    
    //! @TODO Change CAN mode to disable
    
    // Set up DMA channel for TX
    *(module->tx_dma_base_address + DMAxCON_OFFSET) = 0x2020;
    *(module->tx_dma_base_address + DMAxREQ_OFFSET) = 0x0046;
    *(module->tx_dma_base_address + DMAxSTA_OFFSET) = (unsigned int)module->can_buffer - (unsigned int)&_DMA_BASE;
    *(module->tx_dma_base_address + DMAxPAD_OFFSET) = (volatile unsigned int)(module->can_base_address + CANBUS_SFR_OFFSET_CiTXD_OFFSET);
    *(module->tx_dma_base_address + DMAxCNT_OFFSET) = 0x0007;
    
    // Set up DMA channel for RX
    *(module->rx_dma_base_address + DMAxCON_OFFSET) = 0x0020;
    *(module->rx_dma_base_address + DMAxREQ_OFFSET) = 0x0022;
    *(module->rx_dma_base_address + DMAxSTA_OFFSET) = (unsigned int)module->can_buffer - (unsigned int)&_DMA_BASE;
    *(module->rx_dma_base_address + DMAxPAD_OFFSET) = (volatile unsigned int)(module->can_base_address + CANBUS_SFR_OFFSET_CiRXD_OFFSET);
    *(module->rx_dma_base_address + DMAxCNT_OFFSET) = 0x0007;

    return 0;
}



int can_open(can_module_t *module, unsigned int mode)
{
    //! @TODO Move open_mode below setting the CAN mode
    // Set open_mode
    module->open_mode = mode;
    
    // Enable TX DMA channel
    if( (module->open_mode & WRITE) == WRITE )
    {
        *(module->tx_dma_base_address + DMAxCON_OFFSET) |= 0x8000;
    }
    
    // Enable RX DMA channel
    if( (module->open_mode & READ) == READ )
    {
        *(module->rx_dma_base_address + DMAxCON_OFFSET) |= 0x8000;
    }

    // Set default mask
    if( module->set_mask(module, 0, carlcomm_mask) == -1 )
    {// Setting default mask failed
        return -1;
    }

    // Set default filter
    if( module->set_filter(module, 0, carlcomm_filter, 0) == -1 )
    {// Setting default filter failed
        return -1;
    }

    // Enable default filter
    if( module->enable_filter(module, 0) == -1 )
    {// Enabling default filter failed
        return -1;
    }
    
    // Set CAN mode
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) = (*(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) & 0xF8FF) | ((module->open_mode & 0x0007)<<8);
    while( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) & 0x00E0) != (module->open_mode & 0x0007)<<5 );

    return 0;
}

int can_write(can_module_t *module, const carlmessage_t *message)
{
    volatile unsigned int *write_ptr;
    unsigned int write_buffer_num;
    
    if( module == NULL)
    {
        return -1;
    }

    if( message == NULL )
    {
        return module->tx_ready;
    }

    if( (module->open_mode&WRITE) != WRITE )
    {
        return -1;
    }

    // If buffer is full, flush and continue
    if( module->tx_ready == (0xFF>>(8-CAN_TX_BUFFER_LENGTH)) )
    {
        if( module->flush_tx(module) == -1 )
        {
            return -1;
        }
    }

    // Find next empty, unlocked buffer that is not currently being sent
    if( !(module->tx_ready&0x01) && !(module->tx_locks&0x01) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0008) )
    {// Buffer TR0 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x01;

        // Set write_ptr
        write_ptr = module->can_buffer;

        // Store write_buffer_num for later
        write_buffer_num = 0;
    }
    else if( !(module->tx_ready&0x02) && !(module->tx_locks&0x02) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0800) )
    {// Buffer TR1 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x02;

        // Set write_ptr
        write_ptr = module->can_buffer + 8;

        // Store write_buffer_num for later
        write_buffer_num = 1;
    }
    else if( !(module->tx_ready&0x04) && !(module->tx_locks&0x04) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0008) )
    {// Buffer TR2 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x04;

        // Set write_ptr
        write_ptr = module->can_buffer + 16;

        // Store write_buffer_num for later
        write_buffer_num = 2;
    }
    else if( !(module->tx_ready&0x08) && !(module->tx_locks&0x08) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0800) )
    {// Buffer TR3 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x08;

        // Set write_ptr
        write_ptr = module->can_buffer + 24;

        // Store write_buffer_num for later
        write_buffer_num = 3;
    }
    else if( !(module->tx_ready&0x10) && !(module->tx_locks&0x10) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0008) )
    {// Buffer TR4 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 32;

        // Store write_buffer_num for later
        write_buffer_num = 4;
    }
    else if( !(module->tx_ready&0x20) && !(module->tx_locks&0x20) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0800) )
    {// Buffer TR5 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 40;

        // Store write_buffer_num for later
        write_buffer_num = 5;
    }
    else if( !(module->tx_ready&0x40) && !(module->tx_locks&0x40) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0008) )
    {// Buffer TR6 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 48;

        // Store write_buffer_num for later
        write_buffer_num = 6;
    }
    else if( !(module->tx_ready&0x80) && !(module->tx_locks&0x80) && !(*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0800) )
    {// Buffer TR7 is empty and unlocked
        // Get lock
        module->tx_locks |= 0x10;

        // Set write_ptr
        write_ptr = module->can_buffer + 56;

        // Store write_buffer_num for later
        write_buffer_num = 7;
    }
    else
    {// No buffers available to write
        return -1;
    }
    
        
    // Write carlmessage to buffer

    // Always an extended id (ide = 1)
    *write_ptr = (message->header.id.standard_id<<2) | (1<<1) | (1);
    *(write_ptr+1) = (message->header.id.extended_id>>6);
    *(write_ptr+2) = (message->header.id.extended_id<<10) | (message->dlc);
    if( message->rtr )
    {// Set RTR flag
        *(write_ptr+2) |= (1<<9);
    }
    else
    {// Clear RTR flag
        *(write_ptr+2) &= ~(1<<9);
    }

    // Copy data to buffer
    *(write_ptr+3) = (message->data[1]<<8) | (message->data[0]);
    *(write_ptr+4) = (message->data[3]<<8) | (message->data[2]);
    *(write_ptr+5) = (message->data[5]<<8) | (message->data[4]);
    *(write_ptr+6) = (message->data[7]<<8) | (message->data[6]);

    // Set tx_ready bit
    module->tx_ready |= 1<<(write_buffer_num);

    // Release buffer lock
    module->tx_locks &= ~(1<<(write_buffer_num));

    return write_buffer_num;
}


int can_read(can_module_t *module, carlmessage_t *message)
{
    unsigned int read_buffer_num;
    volatile unsigned int *read_ptr = NULL;

    // Check for valid module pointer
    if( module == NULL )
    {
        return -1;
    }

    // Check for valid message pointer
    if( message == NULL )
    {
        //! @TODO Return number of messages in buffer
        return 0;
    }

    // Check for correct open_mode
    if( ((module->open_mode&READ) != READ) || ((module->open_mode&0x0007) == CAN_MODE_DISABLE) || ((module->open_mode&0x0007) == CAN_MODE_CONFIG) )
    {
        return -1;
    }

    // Check for available rx_lock
    if( module->rx_lock )
    {
        return -1;
    }

    // Get rx_lock
    module->rx_lock = true;

    // Check for message in next read buffer
    read_buffer_num = (*(module->can_base_address + CANBUS_SFR_OFFSET_CiFIFO_OFFSET)&0x003F);
    if( read_buffer_num < 16 )
    {// Use CANBUS_SFR_OFFSET_CiRXFUL1
        if( *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXFUL1_OFFSET)&(1<<read_buffer_num) )
        {// A new message exists
            // Set read_ptr to start of can buffer + read_buffer_num*8
            read_ptr = module->can_buffer + read_buffer_num*8;
        }
    }
    else if( read_buffer_num < 32 )
    {// Use CANBUS_SFR_OFFSET_CiRXFUL2
        if( *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXFUL2_OFFSET)&(1<<(read_buffer_num-16)) )
        {// A new message exists
            // Set read_ptr to start of can buffer + read_buffer_num*8
            read_ptr = module->can_buffer + read_buffer_num*8;
        }
    }
    else
    {// Unknown value of read_buffer_num!
        // Release lock and return error
        module->rx_lock = false;
        return -1;
    }

    // If read_ptr is not NULL a new message exists
    if( read_ptr != NULL )
    {// A new message exists and read_ptr points to it

        // Zero message header (since some garbage bits remain)
        message->header.uint32 = 0x00000000;
        
        // Copy buffer into message
        
        // Should always be an extended id
        message->header.id.standard_id = (*read_ptr)>>2;
        message->header.id.extended_id = (*(read_ptr+1))<<6 | (*(read_ptr+2))>>10;
        if( (*(read_ptr+2))&0x0200 )
        {// Set rtr flag
            message->rtr = true;
        }
        else
        {// Clear rtr flag
            message->rtr = false;
        }
        
        message->dlc = (*(read_ptr+2))&0x000F;
        message->data[0] = (*(read_ptr+3))&0x00FF;
        message->data[1] = ((*(read_ptr+3))&0xFF00)>>8;
        message->data[2] = (*(read_ptr+4))&0x00FF;
        message->data[3] = ((*(read_ptr+4))&0xFF00)>>8;
        message->data[4] = (*(read_ptr+5))&0x00FF;
        message->data[5] = ((*(read_ptr+5))&0xFF00)>>8;
        message->data[6] = (*(read_ptr+6))&0x00FF;
        message->data[7] = ((*(read_ptr+6))&0xFF00)>>8;

        if( read_buffer_num < 16 )
        {// Use CANBUS_SFR_OFFSET_CiRXFUL1
            // Clear RXFUL bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXFUL1_OFFSET) &= ~(1<<read_buffer_num);
        }
        else if( read_buffer_num < 32 )
        {// Use CANBUS_SFR_OFFSET_CiRXFUL2
            // Clear RXFUL bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXFUL2_OFFSET) &= ~(1<<(read_buffer_num-16));
        }
        else
        {// Shouldn't ever happen!
            module->rx_lock = false;
            return -1;
        }
        
        // Release lock
        module->rx_lock = false;

        // Return the filter number that accepted the message
        return *(read_ptr+7)>>8;
    }
    else
    {// No new messages received

        // Release rx_lock
        module->rx_lock = false;
        
        // No messages received, so return a value not corresponding to a filter number
        return 32;
    }
}

int can_close(can_module_t *module)
{
    int return_val = 0;
    // Flush all remaining messages
    if( module->flush_tx(module) == -1 )
    {
        return_val = -1;
    }
    
    // Set open_mode to CAN_MODE_DISABLE | !READ | !WRITE
    module->open_mode &= ~(READ | WRITE | 0x0007);
    module->open_mode |= CAN_MODE_DISABLE;
    
    // Disable TX DMA channel
    *(module->tx_dma_base_address + DMAxCON_OFFSET) &= 0x7FFF;
    
    // Disable RX DMA channel
    *(module->rx_dma_base_address + DMAxCON_OFFSET) &= 0x7FFF;
    
    // Set CAN mode
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) = (*(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) & 0xF8FF) | ((module->open_mode & 0x0007)<<8);
    while( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) & 0x00E0) != (module->open_mode & 0x0007)<<5 );

    return return_val;
}

int can_flush_tx(can_module_t *module)
{
    //! @TODO Add errno type error reporting scheme.
    int return_value = 0;
    
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

#if (CAN_TX_BUFFER_LENGTH > 0)
    // Check status of TR0
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x01) && !(module->tx_locks&0x01) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x01);
        }
    }
#endif
    
#if (CAN_TX_BUFFER_LENGTH > 1)
    // Check status of TR1
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x02) && !(module->tx_locks&0x02) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR01CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x02);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 2)
    // Check status of TR2
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x04) && !(module->tx_locks&0x04) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x04);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 3)
    // Check status of TR3
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x08) && !(module->tx_locks&0x08) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR23CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x08);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 4)
    // Check status of TR4
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x10) && !(module->tx_locks&0x10) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x10);
        }
    }
#endif
    
#if (CAN_TX_BUFFER_LENGTH > 5)
    // Check status of TR5
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x20) && !(module->tx_locks&0x20) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR45CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x20);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 6)
    // Check status of TR6
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0008) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0040) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0020) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0010) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x40) && !(module->tx_locks&0x40) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET) |= 0x0008;

            // Clear tx_ready
            module->tx_ready &= ~(0x40);
        }
    }
#endif

#if (CAN_TX_BUFFER_LENGTH > 7)
    // Check status of TR7
    if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x0800) )
    {// TXREQ bit set
        // Check for abort
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x4000) )
        {// Message was aborted
            return_value = -1;
        }
        // Check for lost arbitration
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x2000) )
        {// Message lost arbitration
            // Resend message?
            return_value = -1;
        }
        // Check for bus error
        if( (*(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET)&0x1000) )
        {// Bus error occurred while sending message
            // Resend message?
            return_value = -1;
        }
    }
    else
    {// TXREQ bit is clear
        // Check for a message that is ready to send and not locked
        if( (module->tx_ready&0x80) && !(module->tx_locks&0x80) )
        {// Ready to send and not locked
            // Set TXREQ bit
            *(module->can_base_address + CANBUS_SFR_OFFSET_CiTR67CON_OFFSET) |= 0x0800;

            // Clear tx_ready
            module->tx_ready &= ~(0x80);
        }
    }
#endif

    return return_value;
}

int can_flush_rx(can_module_t *module)
{
    // The hardware FIFO takes care of the RX buffer
    
    return 0;
}

int can_set_bit_timing(can_module_t *module, uint32_t bit_timing)
{
    if( module == NULL )
    {
        return -1;
    }

    module->bit_timing = bit_timing;
    
    return 0;
}

int can_set_mask(can_module_t *module, unsigned int mask_num, carlmessage_header_t mask_val)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Only modify masks if module is not readable or writable
    //! @TODO Add module lock so the mask modification is atomic.
    if( module->open_mode&READ == READ || module->open_mode&WRITE == WRITE )
    {
        return -1;
    }

    // Check for valid mask number
    if( mask_num > 2 )
    {
        return -1;
    }

    //! The module should normally be in buffer window
    // Set filter window
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) |= 0x0001;

    switch(mask_num)
    {
    case 0:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXM0SID_OFFSET) = (mask_val.id.standard_id)<<5 | (1<<3) | (mask_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXM0EID_OFFSET) = (mask_val.id.extended_id&0x0FFFF);
        break;
    case 1:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXM1SID_OFFSET) = (mask_val.id.standard_id)<<5 | (1<<3) | (mask_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXM1EID_OFFSET) = (mask_val.id.extended_id&0x0FFFF);
        break;
    case 2:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXM2SID_OFFSET) = (mask_val.id.standard_id)<<5 | (1<<3) | (mask_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXM2EID_OFFSET) = (mask_val.id.extended_id&0x0FFFF);
        break;
    default:
        // Return to buffer window
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
        return -1;
    }

    // Return to buffer window
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
    
    return 0;
}

int can_set_filter(can_module_t *module, unsigned int filter_num, carlmessage_header_t filter_val, unsigned int mask_num)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Only modify filters if module is not readable or writable
    //! @TODO Add module lock so the filter modification is atomic.
    if( module->open_mode&READ == READ || module->open_mode&WRITE == WRITE )
    {
        return -1;
    }

    // Check for valid filter and mask numbers
    if( filter_num > 15 || mask_num > 2 )
    {
        return -1;
    }

    //! The module should normally be in buffer window
    // Set filter window
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) |= 0x0001;

    switch(filter_num)
    {
    case 0:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF0SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF0EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num);
        break;
    case 1:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF1SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF1EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<2);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<2);
        break;
    case 2:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF2SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF2EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<4);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<4);
        break;
    case 3:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF3SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF3EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<6);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<6);
        break;
    case 4:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF4SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF4EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<8);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<8);
        break;
    case 5:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF5SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF5EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<10);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<10);
        break;
    case 6:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF6SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF6EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<12);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<12);
        break;
    case 7:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF7SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF7EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) &= (0x0003<<14);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL1_OFFSET) |= (mask_num<<14);
        break;
    case 8:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF8SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF8EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num);
        break;
    case 9:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF9SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF9EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<2);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<2);
        break;
    case 10:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF10SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF10EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<4);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<4);
        break;
    case 11:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF11SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF11EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<6);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<6);
        break;
    case 12:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF12SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF12EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<8);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<8);
        break;
    case 13:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF13SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF13EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<10);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<10);
        break;
    case 14:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF14SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF14EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<12);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<12);
        break;
    case 15:
        // Always use extended id
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF15SID_OFFSET) = (filter_val.id.standard_id)<<5 | (1<<3) | (filter_val.id.extended_id)>>16;
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiRXF15EID_OFFSET) = (filter_val.id.extended_id&0x0FFFF);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) &= (0x0003<<14);
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiFMSKSEL2_OFFSET) |= (mask_num<<14);
        break;
    default:
        // Switch to buffer window
        *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
        return -1;
    }

    // Switch to buffer window
    *(module->can_base_address + CANBUS_SFR_OFFSET_CiCTRL1_OFFSET) &= 0xFFFE;
    
    return 0;
}

int can_enable_filter(can_module_t *module, unsigned int filter_num)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Check for valid filter number
    if( filter_num > 15 )
    {
        return -1;
    }

    *(module->can_base_address + CANBUS_SFR_OFFSET_CiFEN1_OFFSET) |= (1<<filter_num);

    return 0;
}

int can_disable_filter(can_module_t *module, unsigned int filter_num)
{
    // Check for valid module
    if( module == NULL )
    {
        return -1;
    }

    // Check for valid filter number
    if( filter_num > 15 )
    {
        return -1;
    }

    *(module->can_base_address + CANBUS_SFR_OFFSET_CiFEN1_OFFSET) &= ~(1<<filter_num);

    return 0;
}
