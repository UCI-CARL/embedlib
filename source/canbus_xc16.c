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
//
// Standard C include files
#include <stdlib.h>
#include <stdbool.h>

// Microchip peripheral libraries
#include <xc.h>

// Include board information
#include <board.def>

// Include local library code
#include <bitops.h>
#include <dma_channel.h>

// Include user definitions
//#include <canbus.def>

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

/*
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
*/

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
    unsigned int win :1;
    unsigned int :2;
    unsigned int cancap :1;
    unsigned int :1;
    unsigned int opmode :3;
    unsigned int reqop :3;
    unsigned int :1;
    unsigned int abat :1;
    unsigned int csidl :1;
    unsigned int :2;
};
typedef struct canbus_cictrl1_bits_s canbus_cictrl1_bits_t;

struct canbus_cictrl2_bits_s
{
    unsigned int dncnt :5;
    unsigned int :11;
};
typedef struct canbus_cictrl2_bits_s canbus_cictrl2_bits_t;

struct canbus_civec_bits_s
{
    unsigned int icode :7;
    unsigned int :1;
    unsigned int filhit :5;
    unsigned int :3;
};
typedef struct canbus_civec_bits_s canbus_civec_bits_t;

struct canbus_cifctrl_bits_s
{
    unsigned int fsa :5;
    unsigned int :8;
    unsigned int dmabs :3;
};
typedef struct canbus_cifctrl_bits_s canbus_cifctrl_bits_t;

struct canbus_cififo_bits_s
{
    unsigned int fnrb :6;
    unsigned int :2;
    unsigned int fbp :6;
    unsigned int :2;
};
typedef struct canbus_cififo_bits_s canbus_cififo_bits_t;

struct canbus_ciintf_bits_s
{
    unsigned int tbif :1;
    unsigned int rbif :1;
    unsigned int rbovif :1;
    unsigned int fifoif :1;
    unsigned int :1;
    unsigned int errif :1;
    unsigned int wakif :1;
    unsigned int ivrif :1;
    unsigned int ewarn :1;
    unsigned int rxwar :1;
    unsigned int txwar :1;
    unsigned int rxbp :1;
    unsigned int txbp :1;
    unsigned int txbo :1;
    unsigned int :2;
};
typedef struct canbus_ciintf_bits_s canbus_ciintf_bits_t;

struct canbus_ciinte_bits_s
{
    unsigned int tbie :1;
    unsigned int rbie :1;
    unsigned int rbovie :1;
    unsigned int fifoie :1;
    unsigned int :1;
    unsigned int errie :1;
    unsigned int wakie :1;
    unsigned int ivrie :1;
    unsigned int :8;
};
typedef struct canbus_ciinte_bits_s canbus_ciinte_bits_t;

struct canbus_ciec_bits_s
{
    unsigned int rerrcnt :8;
    unsigned int terrcnt :8;
};
typedef struct canbus_ciec_bits_s canbus_ciec_bits_t;

struct canbus_cicfg1_bits_s
{
    unsigned int brp :6;
    unsigned int sjw :2;
    unsigned int :8;
};
typedef struct canbus_cicfg1_bits_s canbus_cicfg1_bits_t;

struct canbus_cicfg2_bits_s
{
    unsigned int prseg :3;
    unsigned int seg1ph :3;
    unsigned int sam :1;
    unsigned int seg2phts :1;
    unsigned int seg2ph :3;
    unsigned int :3;
    unsigned int wakfil :1;
    unsigned int :1;
};
typedef struct canbus_cicfg2_bits_s canbus_cicfg2_bits_t;

struct canbus_cifen1_bits_s
{
    unsigned int flten0 :1;
    unsigned int flten1 :1;
    unsigned int flten2 :1;
    unsigned int flten3 :1;
    unsigned int flten4 :1;
    unsigned int flten5 :1;
    unsigned int flten6 :1;
    unsigned int flten7 :1;
    unsigned int flten8 :1;
    unsigned int flten9 :1;
    unsigned int flten10 :1;
    unsigned int flten11 :1;
    unsigned int flten12 :1;
    unsigned int flten13 :1;
    unsigned int flten14 :1;
    unsigned int flten15 :1;
};
typedef struct canbus_cifen1_bits_s canbus_cifen1_bits_t;

struct canbus_cifmsksel1_bits_s
{
    unsigned int f0msk :2;
    unsigned int f1msk :2;
    unsigned int f2msk :2;
    unsigned int f3msk :2;
    unsigned int f4msk :2;
    unsigned int f5msk :2;
    unsigned int f6msk :2;
    unsigned int f7msk :2;
};
typedef struct canbus_cifmsksel1_bits_s canbus_cifmsksel1_bits_t;

struct canbus_cifmsksel2_bits_s
{
    unsigned int f8msk :2;
    unsigned int f9msk :2;
    unsigned int f10msk :2;
    unsigned int f11msk :2;
    unsigned int f12msk :2;
    unsigned int f13msk :2;
    unsigned int f14msk :2;
    unsigned int f15msk :2;
};
typedef struct canbus_cifmsksel2_bits_s canbus_cifmsksel2_bits_t;

struct canbus_cirxful1_bits_s
{
    unsigned int rxful0 :1;
    unsigned int rxful1 :1;
    unsigned int rxful2 :1;
    unsigned int rxful3 :1;
    unsigned int rxful4 :1;
    unsigned int rxful5 :1;
    unsigned int rxful6 :1;
    unsigned int rxful7 :1;
    unsigned int rxful8 :1;
    unsigned int rxful9 :1;
    unsigned int rxful10 :1;
    unsigned int rxful11 :1;
    unsigned int rxful12 :1;
    unsigned int rxful13 :1;
    unsigned int rxful14 :1;
    unsigned int rxful15 :1;
};
typedef struct canbus_cirxful1_bits_s canbus_cirxful1_bits_t;

struct canbus_cirxful2_bits_s
{
    unsigned int rxful16 :1;
    unsigned int rxful17 :1;
    unsigned int rxful18 :1;
    unsigned int rxful19 :1;
    unsigned int rxful20 :1;
    unsigned int rxful21 :1;
    unsigned int rxful22 :1;
    unsigned int rxful23 :1;
    unsigned int rxful24 :1;
    unsigned int rxful25 :1;
    unsigned int rxful26 :1;
    unsigned int rxful27 :1;
    unsigned int rxful28 :1;
    unsigned int rxful29 :1;
    unsigned int rxful30 :1;
    unsigned int rxful31 :1;
};
typedef struct canbus_cirxful2_bits_s canbus_cirxful2_bits_t;

struct canbus_cirxovf1_bits_s
{
    unsigned int rxovf0 :1;
    unsigned int rxovf1 :1;
    unsigned int rxovf2 :1;
    unsigned int rxovf3 :1;
    unsigned int rxovf4 :1;
    unsigned int rxovf5 :1;
    unsigned int rxovf6 :1;
    unsigned int rxovf7 :1;
    unsigned int rxovf8 :1;
    unsigned int rxovf9 :1;
    unsigned int rxovf10 :1;
    unsigned int rxovf11 :1;
    unsigned int rxovf12 :1;
    unsigned int rxovf13 :1;
    unsigned int rxovf14 :1;
    unsigned int rxovf15 :1;
};
typedef struct canbus_cirxovf1_bits_s canbus_cirxovf1_bits_t;

struct canbus_cirxovf2_bits_s
{
    unsigned int rxovf16 :1;
    unsigned int rxovf17 :1;
    unsigned int rxovf18 :1;
    unsigned int rxovf19 :1;
    unsigned int rxovf20 :1;
    unsigned int rxovf21 :1;
    unsigned int rxovf22 :1;
    unsigned int rxovf23 :1;
    unsigned int rxovf24 :1;
    unsigned int rxovf25 :1;
    unsigned int rxovf26 :1;
    unsigned int rxovf27 :1;
    unsigned int rxovf28 :1;
    unsigned int rxovf29 :1;
    unsigned int rxovf30 :1;
    unsigned int rxovf31 :1;
};
typedef struct canbus_cirxovf2_bits_s canbus_cirxovf2_bits_t;

struct canbus_citrmncon_bits_s
{
    unsigned int txmpri :2;
    unsigned int rtrenm :1;
    unsigned int txreqm :1;
    unsigned int txerrm :1;
    unsigned int txlarbm :1;
    unsigned int txabtm :1;
    unsigned int txenm :1;

    unsigned int txnpri :2;
    unsigned int rtrenn :1;
    unsigned int txreqn :1;
    unsigned int txerrn :1;
    unsigned int txlarbn :1;
    unsigned int txabtn :1;
    unsigned int txenn :1;;
};
typedef struct canbus_citrmncon_bits_s canbus_citrmncon_bits_t;

struct canbus_cibufpnt_bits_s
{
    unsigned int fabp :4;
    unsigned int fbbp :4;
    unsigned int fcbp :4;
    unsigned int fdbp :4;
};
typedef struct canbus_cibufpnt_bits_s canbus_cibufpnt_bits_t;

union canbus_cirxmnsid_bits_u
{
    struct
    {
        unsigned int eid16 :1;
        unsigned int eid17 :1;
        unsigned int :1;
        unsigned int mide :1;
        unsigned int :1;
        unsigned int sid0 :1;
        unsigned int sid1 :1;
        unsigned int sid2 :1;
        unsigned int sid3 :1;
        unsigned int sid4 :1;
        unsigned int sid5 :1;
        unsigned int sid6 :1;
        unsigned int sid7 :1;
        unsigned int sid8 :1;
        unsigned int sid9 :1;
        unsigned int sid10 :1;
    };
    struct
    {
        unsigned int eid_h :2;
        unsigned int :3;
        unsigned int sid :11;
    };
};
typedef union canbus_cirxmnsid_bits_u canbus_cirxmnsid_bits_t;

union canbus_cirxmneid_bits_u
{
    unsigned int eid_l;
    struct
    {
        unsigned int eid0 :1;
        unsigned int eid1 :1;
        unsigned int eid2 :1;
        unsigned int eid3 :1;
        unsigned int eid4 :1;
        unsigned int eid5 :1;
        unsigned int eid6 :1;
        unsigned int eid7 :1;
        unsigned int eid8 :1;
        unsigned int eid9 :1;
        unsigned int eid10 :1;
        unsigned int eid11 :1;
        unsigned int eid12 :1;
        unsigned int eid13 :1;
        unsigned int eid14 :1;
        unsigned int eid15 :1;
    };
};
typedef union canbus_cirxmneid_bits_u canbus_cirxmneid_bits_t;

union canbus_cirxfnsid_bits_u
{
    struct
    {
        unsigned int eid16 :1;
        unsigned int eid17 :1;
        unsigned int :1;
        unsigned int exide :1;
        unsigned int :1;
        unsigned int sid0 :1;
        unsigned int sid1 :1;
        unsigned int sid2 :1;
        unsigned int sid3 :1;
        unsigned int sid4 :1;
        unsigned int sid5 :1;
        unsigned int sid6 :1;
        unsigned int sid7 :1;
        unsigned int sid8 :1;
        unsigned int sid9 :1;
        unsigned int sid10 :1;
    };
    struct
    {
        unsigned int eid_h :2;
        unsigned int :3;
        unsigned int sid :11;
    };
};
typedef union canbus_cirxfnsid_bits_u canbus_cirxfnsid_bits_t;

union canbus_cirxfneid_bits_u
{
    unsigned int eid_l;
    struct
    {
        unsigned int eid0 :1;
        unsigned int eid1 :1;
        unsigned int eid2 :1;
        unsigned int eid3 :1;
        unsigned int eid4 :1;
        unsigned int eid5 :1;
        unsigned int eid6 :1;
        unsigned int eid7 :1;
        unsigned int eid8 :1;
        unsigned int eid9 :1;
        unsigned int eid10 :1;
        unsigned int eid11 :1;
        unsigned int eid12 :1;
        unsigned int eid13 :1;
        unsigned int eid14 :1;
        unsigned int eid15 :1;
    };
};
typedef union canbus_cirxfneid_bits_u canbus_cirxfneid_bits_t;

// A buffer is enabled if there is space in the DMA buffer for it.
union canbus_buffer_exists_u
{
    long all;

    struct
    {
        unsigned int b0_exists :1;
        unsigned int b1_exists :1;
        unsigned int b2_exists :1;
        unsigned int b3_exists :1;
        unsigned int b4_exists :1;
        unsigned int b5_exists :1;
        unsigned int b6_exists :1;
        unsigned int b7_exists :1;
        unsigned int b8_exists :1;
        unsigned int b9_exists :1;
        unsigned int b10_exists :1;
        unsigned int b11_exists :1;
        unsigned int b12_exists :1;
        unsigned int b13_exists :1;
        unsigned int b14_exists :1;
        unsigned int b15_exists :1;
        unsigned int b16_exists :1;
        unsigned int b17_exists :1;
        unsigned int b18_exists :1;
        unsigned int b19_exists :1;
        unsigned int b20_exists :1;
        unsigned int b21_exists :1;
        unsigned int b22_exists :1;
        unsigned int b23_exists :1;
        unsigned int b24_exists :1;
        unsigned int b25_exists :1;
        unsigned int b26_exists :1;
        unsigned int b27_exists :1;
        unsigned int b28_exists :1;
        unsigned int b29_exists :1;
        unsigned int b30_exists :1;
        unsigned int b31_exists :1;
    };
};
typedef union canbus_buffer_exists_u canbus_buffer_exists_t;
    

/* ***** Canbus Private Object ***** */

struct canbus_private_s
{
    canbus_attr_t attr_;
    volatile unsigned int *base_address_;
    dma_channel_t *tx_dma_;
    dma_channel_t *rx_dma_;
    canbus_buffer_exists_t buffer_exists_;
};
typedef struct canbus_private_s canbus_private_t;


/* ***** Public Function Implementation Prototypes ***** */

static int canbus_init(canbus_t *object,
                       canbus_attr_t *attr,
                       unsigned int tx_dma_channel,
                       unsigned int rx_dma_channel,
                       volatile unsigned int dma_buffer[][8],
                       unsigned int num_buffers);
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
static int canbus_connect(canbus_t *object,
                          canbus_filter_t filter_num,
                          canbus_buffer_t buffer_num);
static int canbus_disconnect(canbus_t *object,
                             canbus_filter_t filter_num);
static int canbus_write(canbus_t *object,
                        canbus_buffer_t buffer_num,
                        const canbus_message_t *message,
                        canbus_priority_t priority);
static int canbus_abort_write(canbus_t *object,
                              canbus_buffer_t buffer_num);
static int canbus_read(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message);
static int canbus_peek(canbus_t *object,
                       canbus_buffer_t buffer_num,
                       canbus_message_t *message);
static bool canbus_is_empty(canbus_t *object,
                            canbus_buffer_t buffer_num);
static bool canbus_buffer_exists(canbus_t *object,
                                 canbus_buffer_t buffer_num);
static bool canbus_is_valid(canbus_t *object);
static int canbus_get_direction(canbus_t *object,
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
    .connect = canbus_connect,
    .disconnect = canbus_disconnect,
    .write = canbus_write,
    .abort_write = canbus_abort_write,
    .read = canbus_read,
    .peek = canbus_peek,
    .is_empty = canbus_is_empty,
    .buffer_exists = canbus_buffer_exists,
    .is_valid = canbus_is_valid,
    .get_direction = canbus_get_direction,
    .clean_up = canbus_clean_up,
    .isr = canbus_isr
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
                       volatile unsigned int dma_buffer[][8],
                       unsigned int num_buffers)
{
    unsigned int i;
    dma_attr_t tx_dma_attr;
    dma_attr_t rx_dma_attr;
    unsigned int first_fifo_buffer;
    unsigned int last_fifo_buffer;

#ifdef DEBUG__
    canbus_private_t *debug_private;
    unsigned int debug_value = 0;
#endif

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

#ifdef DEBUG__
    debug_private = (canbus_private_t *)(object->private);
#endif

#if (CANBUS_HW_NUMBER_OF_MODULES < 1) || (CANBUS_HW_NUMBER_OF_MODULES > 2)
#error "CANBUS(__file__,__line__): CANBUS_HW_NUMBER_OF_MODULES is out of bounds. Expected a number \
from 1 to 2."
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
    ((canbus_private_t *)(object->private))->attr_ = *attr;

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
        = ((canbus_private_t *)(object->private))->attr_.bit_timing.pre;
    ((canbus_cicfg1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1))->sjw \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing.sync_jump;
    ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2))->prseg \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing.prop_seg;
    ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2))->seg1ph \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing.phase_seg1;
    if( ((canbus_private_t *)(object->private))->attr_.bit_timing.phase_seg2_prog \
        == CANBUS_BIT_TIMING_PHASE_SEG2_PROG_EN )
    {
        ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2)) \
            ->seg2phts = CANBUS_BIT_TIMING_PHASE_SEG2_PROG_EN;
        ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2)) \
            ->seg2ph = ((canbus_private_t *)(object->private))->attr_.bit_timing.phase_seg2_prog;
    }
    ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG2))->sam \
        = ((canbus_private_t *)(object->private))->attr_.bit_timing.sample;

    // Set module configuration
    if( ((canbus_private_t *)(object->private))->attr_.module.wakeup == CANBUS_MODULE_WAKEUP_EN )
    {
        ((canbus_cicfg2_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCFG1)) \
            ->wakfil = 1;
    }
    if( ((canbus_private_t *)(object->private))->attr_.module.cpuidle \
        == CANBUS_MODULE_CPUIDLE_STOP )
    {
        ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
            ->csidl = 1;
    }
    if( ((canbus_private_t *)(object->private))->attr_.module.timestamp \
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

    // Set DMA channels
    //! @todo Remove this ugly cast away from const! The DMA code needs to allow for dynamic allocation!
    *((unsigned int *)(&((canbus_private_t *)(object->private))->tx_dma_->channel_number)) = tx_dma_channel;
    *((unsigned int *)(&((canbus_private_t *)(object->private))->rx_dma_->channel_number)) = rx_dma_channel;

    // Set DMA buffer pointers and lengths
    *((volatile unsigned int **)(&((canbus_private_t *)(object->private))->tx_dma_->buffer_a)) = (volatile unsigned int *)dma_buffer;
    *((unsigned int *)(&((canbus_private_t *)(object->private))->tx_dma_->buffer_a_size)) = num_buffers*8;
    *((volatile unsigned int **)(&((canbus_private_t *)(object->private))->rx_dma_->buffer_a)) = (volatile unsigned int *)dma_buffer;
    *((unsigned int *)(&((canbus_private_t *)(object->private))->rx_dma_->buffer_a_size)) = num_buffers*8;
    
    
    // Set up DMA attributes
    tx_dma_attr.config = DMA_CONFIG_OPMODE_CONTINUOUS     \
        | DMA_CONFIG_PINGPONG_DIS                         \
        | DMA_CONFIG_ADDRMODE_PERIPHERAL_IND              \
        | DMA_CONFIG_NULLWRITE_DIS                        \
        | DMA_CONFIG_DIR_TO_PERIPHERAL                    \
        | DMA_CONFIG_DATASIZE_WORD;
    rx_dma_attr.config = DMA_CONFIG_OPMODE_CONTINUOUS \
        | DMA_CONFIG_PINGPONG_DIS \
        | DMA_CONFIG_ADDRMODE_PERIPHERAL_IND \
        | DMA_CONFIG_NULLWRITE_DIS \
        | DMA_CONFIG_DIR_FROM_PERIPHERAL \
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
    /** @todo Remove CANBUS_BUFFER_Bx enum and use CANBUS_BUFFER_Bx enum instead */
    switch( ((canbus_private_t *)(object->private))->attr_.fifo.start )
    {
    case CANBUS_BUFFER_B0:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00000;
        first_fifo_buffer = 0;
        break;
    case CANBUS_BUFFER_B1:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00001;
        first_fifo_buffer = 1;
        break;
    case CANBUS_BUFFER_B2:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00010;
        first_fifo_buffer = 2;
        break;
    case CANBUS_BUFFER_B3:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00011;
        first_fifo_buffer = 3;
        break;
    case CANBUS_BUFFER_B4:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00100;
        first_fifo_buffer = 4;
        break;
    case CANBUS_BUFFER_B5:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00101;
        first_fifo_buffer = 5;
        break;
    case CANBUS_BUFFER_B6:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00110;
        first_fifo_buffer = 6;
        break;
    case CANBUS_BUFFER_B7:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b00111;
        first_fifo_buffer = 7;
        break;
    case CANBUS_BUFFER_B8:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01000;
        first_fifo_buffer = 8;
        break;
    case CANBUS_BUFFER_B9:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01001;
        first_fifo_buffer = 9;
        break;
    case CANBUS_BUFFER_B10:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01010;
        first_fifo_buffer = 10;
        break;
    case CANBUS_BUFFER_B11:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01011;
        first_fifo_buffer = 11;
        break;
    case CANBUS_BUFFER_B12:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01100;
        first_fifo_buffer = 12;
        break;
    case CANBUS_BUFFER_B13:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01101;
        first_fifo_buffer = 13;
        break;
    case CANBUS_BUFFER_B14:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01110;
        first_fifo_buffer = 14;
        break;
    case CANBUS_BUFFER_B15:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b01111;
        first_fifo_buffer = 15;
        break;
    case CANBUS_BUFFER_B16:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10000;
        first_fifo_buffer = 16;
        break;
    case CANBUS_BUFFER_B17:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10001;
        first_fifo_buffer = 17;
        break;
    case CANBUS_BUFFER_B18:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10010;
        first_fifo_buffer = 18;
        break;
    case CANBUS_BUFFER_B19:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10011;
        first_fifo_buffer = 19;
        break;
    case CANBUS_BUFFER_B20:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10100;
        first_fifo_buffer = 20;
        break;
    case CANBUS_BUFFER_B21:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10101;
        first_fifo_buffer = 21;
        break;
    case CANBUS_BUFFER_B22:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10110;
        first_fifo_buffer = 22;
        break;
    case CANBUS_BUFFER_B23:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b10111;
        first_fifo_buffer = 23;
        break;
    case CANBUS_BUFFER_B24:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11000;
        first_fifo_buffer = 24;
        break;
    case CANBUS_BUFFER_B25:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11001;
        first_fifo_buffer = 25;
        break;
    case CANBUS_BUFFER_B26:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11010;
        first_fifo_buffer = 26;
        break;
    case CANBUS_BUFFER_B27:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11011;
        first_fifo_buffer = 27;
        break;
    case CANBUS_BUFFER_B28:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->fsa = 0b11100;
        first_fifo_buffer = 28;
        break;
    default:
        // Unknown or invalid start for FIFO buffer
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    
    switch( ((canbus_private_t *)(object->private))->attr_.fifo.length )
    {
    case CANBUS_FIFO_LENGTH_4:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b000;
        last_fifo_buffer = first_fifo_buffer + 4;
        break;
    case CANBUS_FIFO_LENGTH_6:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b001;
        last_fifo_buffer = first_fifo_buffer + 6;
        break;
    case CANBUS_FIFO_LENGTH_8:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b010;
        last_fifo_buffer = first_fifo_buffer + 8;
        break;
    case CANBUS_FIFO_LENGTH_12:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b011;
        last_fifo_buffer = first_fifo_buffer + 12;
        break;
    case CANBUS_FIFO_LENGTH_16:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b100;
        last_fifo_buffer = first_fifo_buffer + 16;
        break;
    case CANBUS_FIFO_LENGTH_24:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b101;
        last_fifo_buffer = first_fifo_buffer + 24;
        break;
    case CANBUS_FIFO_LENGTH_32:
        ((canbus_cifctrl_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFCTRL)) \
            ->dmabs = 0b110;
        last_fifo_buffer = first_fifo_buffer + 32;
        break;
    default:
        // Unknown or invalid length of FIFO buffer
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }

    // Check if DMA buffer is large enough for FIFO
    if( (((canbus_private_t *)(object->private))->rx_dma_->buffer_a_size)/8 < last_fifo_buffer )
    {// DMA buffer is too small
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }// Else, DMA buffer is at least large enough to hold FIFO buffer.
    
    // Set buffer window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 0;

    // Set buffer directions
    // Set B0 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b0 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer == 0 )
    {// B0 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B0 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txenm = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b0;
    }
    // Set B1 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b1 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 1 )
    {// B1 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B1 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txenn = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b1;
    }
    // Set B2 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b2 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 2 )
    {// B2 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B2 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txenm = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b2;
    }
    // Set B3 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b3 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 3 )
    {// B3 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B3 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txenn = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b3;
    }
    // Set B4 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b4 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 4 )
    {// B4 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B4 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txenm = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b4;
    }
    // Set B5 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b5 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 5 )
    {// B5 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B5 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txenn = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b5;
    }
    // Set B6 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b6 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 6 )
    {// B6 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B6 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txenm = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b6;
    }
    // Set B7 direction
    if( ((canbus_private_t *)(object->private))->attr_.buffer_dir.b7 == CANBUS_DIRECTION_TX \
        && first_fifo_buffer <= 7 )
    {// B7 is TX and is part of FIFO
        // Buffers in FIFO must not be TX
        canbus.clean_up(object);
        return CANBUS_E_INPUT;
    }
    else
    {// B7 is not part of FIFO or is marked RX
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txenn = ((canbus_private_t *)(object->private))->attr_.buffer_dir.b7;
    }

    // Buffers for which there is space in DMA "exist"
    for(i=0; i<((((canbus_private_t *)(object->private))->rx_dma_->buffer_a_size)/8); ++i)
    {// Mark buffers, starting at B0, which have space in the DMA buffer as existing
        ((canbus_private_t *)(object->private))->buffer_exists_.all |= (1<<i);
    }
    
    // Set mode to DISABLE
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->reqop \
        = CANBUS_OPMODE_DISABLE;

    // Wait for DISABLE mode
    while( ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1)) \
           ->opmode != CANBUS_OPMODE_DISABLE )
    {
    }

    return CANBUS_E_NONE;
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

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

    // This should never happen!
    return CANBUS_E_ASSERT;
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
    return CANBUS_E_NONE;
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

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
            ->eid_h = ((mask_value->eid)&0x00030000)>>16;
        ((canbus_cirxmneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM0EID)) \
            ->eid_l = (mask_value->eid)&0x0000FFFF;
    }
    if( mask_num == CANBUS_MASK_M1 || mask_num == CANBUS_MASK_ALL )
    {
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID)) \
            ->sid = mask_value->sid;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID)) \
            ->mide = mask_value->ide;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1SID)) \
            ->eid_h = ((mask_value->eid)&0x00030000)>>16;
        ((canbus_cirxmneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM1EID)) \
            ->eid_l = (mask_value->eid)&0x0000FFFF;
    }
    if( mask_num == CANBUS_MASK_M2 || mask_num == CANBUS_MASK_ALL )
    {
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID)) \
            ->sid = mask_value->sid;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID)) \
            ->mide = mask_value->ide;
        ((canbus_cirxmnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2SID)) \
            ->eid_h = ((mask_value->eid)&0x00030000)>>16;
        ((canbus_cirxmneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXM2EID)) \
            ->eid_l = (mask_value->eid)&0x0000FFFF;
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    unsigned int assign_mask = 0;
    
    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // If filter NONE is selected, do nothing
    if( filter_num == CANBUS_FILTER_NONE )
    {
        // Do nothing
        return CANBUS_E_NONE;
    }

    // Determine which mask to assign
    if( mask_num == CANBUS_MASK_M0 )
    {// Assign mask 0
        assign_mask = 0b00;
    }
    else if( mask_num == CANBUS_MASK_M1 )
    {// Assign mask 1
        assign_mask = 0b01;
    }
    else if( mask_num == CANBUS_MASK_M2 )
    {// Assign mask 2
        assign_mask = 0b10;
    }
    else
    {// Catch-all for invalid input
        // Return error
        return CANBUS_E_INPUT;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    if( filter_num == CANBUS_FILTER_F0 )
    {
        // Assign mask to filter 0
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f0msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F1 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f1msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F2 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f2msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F3 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f3msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F4 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f4msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F5 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f5msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F6 )
    {
        // Assign mask 0 to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f6msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F7 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f7msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F8 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f8msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F9 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f9msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F10 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f10msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F11 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f11msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F12 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f12msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F13 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f13msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F14 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f14msk = assign_mask;
    }
    else if( filter_num == CANBUS_FILTER_F15 )
    {
        // Assign mask to filter 1
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = assign_mask;
    }
    // Using an else if for this so that everytime we run the function we don't have to check every
    // possible value. Most of the time filter_num will be a single filter not ALL, so this portion
    // will only rarely be run
    else if( filter_num == CANBUS_FILTER_ALL )
    {
        // Assign mask to all filters
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f0msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f1msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f2msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f3msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f4msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f5msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f6msk = assign_mask;
        ((canbus_cifmsksel1_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL1))->f7msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f8msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f9msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f10msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f11msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f12msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f13msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f14msk = assign_mask;
        ((canbus_cifmsksel2_bits_t *)(CANBUS_BASE_ADDRESS(object)       \
                                      + CANBUS_SFR_OFFSET_CiFMSKSEL2))->f15msk = assign_mask;
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // If NONE, then do nothing and exit
    if( filter_num == CANBUS_FILTER_NONE )
    {
        // Do nothing
        return CANBUS_E_NONE;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    if( filter_num == CANBUS_FILTER_F0 )
    {// Set filter 0
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F1 )
    {// Set filter 1
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F2 )
    {// Set filter 2
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F3 )
    {// Set filter 3
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F4 )
    {// Set filter 4
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F5 )
    {// Set filter 5
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F6 )
    {// Set filter 6
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F7 )
    {// Set filter 7
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F8 )
    {// Set filter 8
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F9 )
    {// Set filter 9
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F10 )
    {// Set filter 10
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F11 )
    {// Set filter 11
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F12 )
    {// Set filter 12
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F13 )
    {// Set filter 13
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F14 )
    {// Set filter 14
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    else if( filter_num == CANBUS_FILTER_F15 )
    {// Set filter 15
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
    }
    // Using an else if for this so that everytime we run the function we don't have to check every
    // possible value. Most of the time filter_num will be a single filter not ALL, so this portion
    // will only rarely be run
    else if( filter_num == CANBUS_FILTER_ALL )
    {// Set all filters
        // Set filter 0
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF0EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 1
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF1EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 2
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF2EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 3
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF3EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 4
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF4EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 5
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF5EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 6
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF6EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 7
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF7EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 8
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF8EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 9
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF9EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 10
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF10EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 11
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF11EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 12
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF12EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 13
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF13EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 14
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF14EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;

        // Set filter 15
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->sid = filter_value->sid;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->exide = filter_value->ide;
        ((canbus_cirxfnsid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15SID)) \
            ->eid_h = ((filter_value->eid)&0x00030000)>>16;
        ((canbus_cirxfneid_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXF15EID)) \
            ->eid_l = (filter_value->eid)&0x0000FFFF;
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
static int canbus_connect(canbus_t *object,
                          canbus_filter_t filter_num,
                          canbus_buffer_t buffer_num)
{
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    unsigned int buffer_pointer = 0;
    
    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // If filter is NONE, then do nothing and exit
    if( filter_num == CANBUS_FILTER_NONE )
    {
        return CANBUS_E_NONE;
    }

    // Check for valid buffer_num
    // Only buffers 0-14 and FIFO are valid, cannot be buffers 15-31, NONE, or ALL
    if( buffer_num >= CANBUS_BUFFER_B15 && buffer_num != CANBUS_BUFFER_FIFO )
    {// Buffer is invalid
        return CANBUS_E_INPUT;
    }

    // Check if buffer pointer points to a buffer in the FIFO region
    if( buffer_num >= ((canbus_private_t *)(object->private))->attr_.fifo.start \
        && buffer_num != CANBUS_BUFFER_FIFO )
    {// Filter buffer pointer points to (or past) the FIFO region
        // Return an error
        return CANBUS_E_INPUT;
    }

    // Check if buffer_num is marked as TX
    if( canbus.get_direction(object, buffer_num) == CANBUS_DIRECTION_TX )
    {// Buffer is marked as TX
        return CANBUS_E_INPUT;
    }

    // Set buffer_pointer
    if( buffer_num == CANBUS_BUFFER_FIFO )
    {// buffer_num is the FIFO buffer
        // Set buffer_pointer to the hardware value for FIFO buffer pointer
        buffer_pointer = 0x000F;
    }
    else
    {// buffer_num is a buffer from 0-14
        // Set buffer_pointer to the hardware value for the specific buffer (i.e. buffer_num)
        buffer_pointer = buffer_num;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    // Set filter_num to point to buffer_num
    if( filter_num == CANBUS_FILTER_F0 )
    {// Filter 0 selected
        // Set filter 0 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fabp = buffer_pointer;
        // Enable filter 0
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten0 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F1 )
    {// Filter 1 selected
        // Set filter 1 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fbbp = buffer_pointer;
        // Enable filter 1
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten1 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F2 )
    {// Filter 2 selected
        // Set filter 2 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fcbp = buffer_pointer;
        // Enable filter 2
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten2 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F3 )
    {// Filter 3 selected
        // Set filter 3 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fdbp = buffer_pointer;
        // Enable filter 3
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten3 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F4 )
    {// Filter 4 selected
        // Set filter 4 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fabp = buffer_pointer;
        // Enable filter 4
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten4 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F5 )
    {// Filter 5 selected
        // Set filter 5 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fbbp = buffer_pointer;
        // Enable filter 5
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten5 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F6 )
    {// Filter 6 selected
        // Set filter 6 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fcbp = buffer_pointer;
        // Enable filter 6
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten6 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F7 )
    {// Filter 7 selected
        // Set filter 7 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fdbp = buffer_pointer;
        // Enable filter 7
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten7 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F8 )
    {// Filter 8 selected
        // Set filter 8 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fabp = buffer_pointer;
        // Enable filter 8
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten8 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F9 )
    {// Filter 9 selected
        // Set filter 9 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fbbp = buffer_pointer;
        // Enable filter 9
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten9 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F10 )
    {// Filter 10 selected
        // Set filter 10 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fcbp = buffer_pointer;
        // Enable filter 10
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten10 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F11 )
    {// Filter 11 selected
        // Set filter 11 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fdbp = buffer_pointer;
        // Enable filter 11
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten11 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F12 )
    {// Filter 12 selected
        // Set filter 12 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fabp = buffer_pointer;
        // Enable filter 12
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten12 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F13 )
    {// Filter 13 selected
        // Set filter 13 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fbbp = buffer_pointer;
        // Enable filter 13
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten13 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F14 )
    {// Filter 14 selected
        // Set filter 14 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fcbp = buffer_pointer;
        // Enable filter 14
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten14 = 1;
    }
    else if( filter_num == CANBUS_FILTER_F15 )
    {// Filter 15 selected
        // Set filter 15 buffer pointer
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fdbp = buffer_pointer;
        // Enable filter 15
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten15 = 1;
    }
    // Using an else if for this so that everytime we run the function we don't have to check for
    // ALL. Most of the time filter_num will be a single filter not ALL, so this portion will only
    // rarely be run.
    else if( filter_num == CANBUS_FILTER_ALL )
    {// Point all filters at buffer_num and enable them
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fabp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten0 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fbbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten1 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fcbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten2 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT1)) \
            ->fdbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten3 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fabp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten4 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fbbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten5 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fcbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten6 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT2)) \
            ->fdbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten7 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fabp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten8 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fbbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten9 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fcbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten10 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT3)) \
            ->fdbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten11 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fabp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten12 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fbbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten13 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fcbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten14 = 1;
        
        ((canbus_cibufpnt_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiBUFPNT4)) \
            ->fdbp = buffer_pointer;
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten15 = 1;
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
 * @brief Disconnect the specified filter.
 *
 * @details Nothing here.
 *
 * @private
 */
int canbus_disconnect(canbus_t *object,
                      canbus_filter_t filter_num)
{
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Object is invalid
        return CANBUS_E_OBJECT;
    }

    // If filter is NONE, then do nothing and exit
    if( filter_num == CANBUS_FILTER_NONE )
    {
        return CANBUS_E_NONE;
    }

    // Switch to filter window
    ((canbus_cictrl1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiCTRL1))->win = 1;

    // Disable filter
    if( filter_num == CANBUS_FILTER_F0 )
    {// Filter 0 selected
        // Disable filter 0
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten0 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F1 )
    {// Filter 1 selected
        // Disable filter 1
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten1 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F2 )
    {// Filter 2 selected
        // Disable filter 2
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten2 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F3 )
    {// Filter 3 selected
        // Disable filter 3
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten3 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F4 )
    {// Filter 4 selected
        // Disable filter 4
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten4 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F5 )
    {// Filter 5 selected
        // Disable filter 5
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten5 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F6 )
    {// Filter 6 selected
        // Disable filter 6
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten6 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F7 )
    {// Filter 7 selected
        // Disable filter 7
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten7 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F8 )
    {// Filter 8 selected
        // Disable filter 8
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten8 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F9 )
    {// Filter 9 selected
        // Disable filter 9
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten9 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F10 )
    {// Filter 10 selected
        // Disable filter 10
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten10 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F11 )
    {// Filter 11 selected
        // Disable filter 11
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten11 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F12 )
    {// Filter 12 selected
        // Disable filter 12
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten12 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F13 )
    {// Filter 13 selected
        // Disable filter 13
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten13 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F14 )
    {// Filter 14 selected
        // Disable filter 14
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten14 = 0;
    }
    else if( filter_num == CANBUS_FILTER_F15 )
    {// Filter 15 selected
        // Disable filter 15
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten15 = 0;
    }
    // Using an else if for this so that everytime we run the function we don't have to check for
    // ALL. Most of the time filter_num will be a single filter not ALL, so this portion will only
    // rarely be run.
    else if( filter_num == CANBUS_FILTER_ALL )
    {// Point all filters at buffer_num and enable them
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten0 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten1 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten2 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten3 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten4 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten5 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten6 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten7 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten8 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten9 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten10 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten11 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten12 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten13 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten14 = 0;
        
        ((canbus_cifen1_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFEN1)) \
            ->flten15 = 0;
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
 * @return A @ref canbus_error_t value or the number of messages written (i.e. <cc>0</cc> or
 * <cc>1</cc>).
 *
 * @private
 */
static int canbus_write(canbus_t *object,
                        canbus_buffer_t buffer_num,
                        const canbus_message_t *message,
                        canbus_priority_t priority)
{
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    unsigned int i=0;
    
    // Check for a valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return CANBUS_E_OBJECT;
    }

    // If buffer NONE is selected do nothing and exit
    if( buffer_num == CANBUS_BUFFER_NONE )
    {
        return 0;
    }
    
    // Only buffers 0-7 may be written to.
    if( buffer_num >= CANBUS_BUFFER_B8 \
        || buffer_num == CANBUS_BUFFER_FIFO \
        || buffer_num == CANBUS_BUFFER_ALL )
    {
        return CANBUS_E_INPUT;
    }

    // Check if buffer exists
    if( !canbus.buffer_exists(object, buffer_num) )
    {// Buffer doesn't exist in DMA RAM
        return CANBUS_E_WRITE;
    }
    
    // Check that buffer_num is marked as TX
    if( canbus.get_direction(object, buffer_num) != CANBUS_DIRECTION_TX )
    {// Buffer is not marked TX
        return CANBUS_E_WRITE;
    }

    // Check if the buffer is empty
    if( !canbus.is_empty(object, buffer_num) )
    {// Buffer has a message in it already
        return CANBUS_E_AGAIN;
    }

    // Check if message is valid
    if( message->dlc > 8 )
    {// Invalid message
        return CANBUS_E_INPUT;
    }

    // Write message to buffer
    // Note: We can use buffer_num directly since the buffer must be 0-7 which maps directly.
    // Write message buffer word 0
    if( message->header.ide )
    {// Message uses an extended ID
        *(((canbus_private_t *)(object->private))->tx_dma_->buffer_a + buffer_num*8) \
            = (message->header.sid<<2) + 0x0003;
    }
    else
    {// Message uses a standard ID
        *(((canbus_private_t *)(object->private))->tx_dma_->buffer_a + buffer_num*8) \
            = (message->header.sid<<2);
    }
    
    // Write message buffer word 1
    *(((canbus_private_t *)(object->private))->tx_dma_->buffer_a + buffer_num*8 + 1) \
        = (message->header.eid>>6);

    // Write message buffer word 2
    *(((canbus_private_t *)(object->private))->tx_dma_->buffer_a + buffer_num*8 + 2) \
        = (((message->header.eid)&0x0000003F)<<10) + message->dlc;

    // Write message buffer words 3-6
    for(i=0; i<4; ++i)
    {
        *(((canbus_private_t *)(object->private))->tx_dma_->buffer_a + buffer_num*8 + 3+i) \
            = (message->data[2*i+1]<<8) + message->data[2*i];
    }

    // Set CiTRmnCON bits
    switch( buffer_num )
    {
    case CANBUS_BUFFER_B0:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txmpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txreqm = 1;
        break;
    case CANBUS_BUFFER_B1:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txnpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txreqn = 1;
        break;
    case CANBUS_BUFFER_B2:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txmpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txreqm = 1;
        break;
    case CANBUS_BUFFER_B3:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txnpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txreqn = 1;
        break;
    case CANBUS_BUFFER_B4:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txmpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txreqm = 1;
        break;
    case CANBUS_BUFFER_B5:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txnpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txreqn = 1;
        break;
    case CANBUS_BUFFER_B6:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txmpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txreqm = 1;
        break;
    case CANBUS_BUFFER_B7:
        // Set priority
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txnpri = priority;
        // Mark as ready to transmit
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txreqn = 1;
        break;
    default:
        // Invalid input (this shouldn't happen!)
        return CANBUS_E_ASSERT;
    }

    return CANBUS_E_NONE;
}

/**
 * @brief Abort any current transmission attempts in the specified buffer.
 *
 * @details If there is a current message waiting for transmission in the specified buffer
 * abort the transmission. If there is no current message don't do anything.
 *
 * @param[in]  canbus     The canbus_t object to work on.
 * @param[in]  buffer_num The buffer from which to abort any transmissions.
 * @return A <cc>true</cc> if a message was successfully aborted, a <cc>false</cc> if there was no
 * message in the buffer. If there was an error then a @ref canbus_error_t value.
 *
 * @private
 */
static int canbus_abort_write(canbus_t *object,
                              canbus_buffer_t buffer_num)
{
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return CANBUS_E_OBJECT;
    }

    // If buffer NONE is selected do nothing and exit
    if( buffer_num == CANBUS_BUFFER_NONE )
    {
        return false;
    }
    
    // Only buffers 0-7 may be TX and therefore have a message to abort.
    if( buffer_num >= CANBUS_BUFFER_B8 \
        || buffer_num == CANBUS_BUFFER_FIFO \
        || buffer_num == CANBUS_BUFFER_ALL )
    {
        return CANBUS_E_INPUT;
    }

    // Check if buffer exists
    if( !canbus.buffer_exists(object, buffer_num) )
    {// Buffer doesn't exist in DMA RAM
        return CANBUS_E_INPUT;
    }
    
    // Check that buffer_num is marked as TX
    if( canbus.get_direction(object, buffer_num) != CANBUS_DIRECTION_TX )
    {// Buffer is not marked TX
        return CANBUS_E_INPUT;
    }

    // Check if the buffer is empty
    if( canbus.is_empty(object, buffer_num) )
    {// Buffer is empty
        return false;
    }

    // Abort message
    switch( buffer_num )
    {
    case CANBUS_BUFFER_B0:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txabtm = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR01CON))->txabtm == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B1:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR01CON)) \
            ->txabtn = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR01CON))->txabtn == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B2:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txabtm = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR23CON))->txabtm == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B3:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR23CON)) \
            ->txabtn = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR23CON))->txabtn == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B4:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txabtm = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR45CON))->txabtm == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B5:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR45CON)) \
            ->txabtn = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR45CON))->txabtn == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B6:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txabtm = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR67CON))->txabtm == 1 )
        {
        }
        return true;
        break;
    case CANBUS_BUFFER_B7:
        // Abort message
        ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiTR67CON)) \
            ->txabtn = 1;
        // Wait for message to be aborted
        while( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                            + CANBUS_SFR_OFFSET_CiTR67CON))->txabtn == 1 )
        {
        }
        return true;
        break;
    default:
        // Invalid input (this shouldn't happen!)
        return CANBUS_E_ASSERT;
    }
}
    
/**
 * @brief Read a message from the specified buffer.
 *
 * @details Read the next available message from the given buffer. If the buffer is marked TX
 * an error code will be returned. If the read is successful the message is written to the given
 * canbus_message_t pointer and a <cc>1</cc> is returned. If no message is available a
 * <cc>0</cc> is returned and the supplied message buffer is unaffected.
 *
 * A buffer may only receive messages if it is marked as TX and a filter has been connected to it.
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    int return_value = 0;

    return_value = canbus.peek(object, buffer_num, message);
    if( return_value <= 0 )
    {// There was an error during peek() or there was no message to read
        return return_value;
    }// Otherwise a message was read into the message pointer, so mark the buffer as read

    // Since peek() checked the validity of the inputs we can assume they are valid
        
    // Mark the buffer as read
    // Treat the special case of reading from the FIFO
    if( buffer_num == CANBUS_BUFFER_FIFO )
    {// Mark the correct FIFO buffer as read
        // Check which CiRXFUL SFR to use
        if( (((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFIFO)) \
             ->fnrb) <= 0x000F )
        {// Use CiRXFUL1
            // Clear correct RXFUL bit
            *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL1) \
                &= ~(1<<((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) + \
                                                  CANBUS_SFR_OFFSET_CiFIFO))->fnrb);
        }
        else
        {// Use CiRXFUL2
            // Clear correct RXFUL bit
            *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL2) \
                &= ~(1<<((((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) + \
                                                   CANBUS_SFR_OFFSET_CiFIFO))->fnrb) - 16));
        }
    }
    else
    {// Mark the correct buffer as read
        // Check which CiRXFUL SFR to use
        if( buffer_num <= CANBUS_BUFFER_B15 )
        {// Use CiRXFUL1
            // Clear correct RXFUL bit
            *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL1) &= ~(1<<buffer_num);
        }
        else
        {// Use CiRXFUL2
            // Clear correct RXFUL bit
            *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL2) &= ~(1<<(buffer_num-16));
        }
    }

    return 1;
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
#ifdef DEBUG__
    canbus_private_t *debug_private;
    unsigned int debug_value;
    debug_private = (canbus_private_t *)(object->private);
    debug_value = 0;
#endif

    unsigned int i;
    volatile unsigned int *buffer_ptr = NULL;
    
    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return CANBUS_E_OBJECT;
    }

    // May only read from buffers 0-31 and FIFO, not ALL or NONE
    if( buffer_num == CANBUS_BUFFER_ALL || buffer_num == CANBUS_BUFFER_NONE )
    {
        return CANBUS_E_INPUT;
    }

    // Check if buffer exists
    if( !canbus.buffer_exists(object, buffer_num) )
    {// Buffer doesn't exist in DMA RAM
        return CANBUS_E_INPUT;
    }
    
    // Check that buffer_num is marked as RX
    if( canbus.get_direction(object, buffer_num) != CANBUS_DIRECTION_RX )
    {// Buffer is not marked RX
        return CANBUS_E_INPUT;
    }

    // Check if buffer is part of FIFO region
    if( buffer_num >= ((canbus_private_t *)(object->private))->attr_.fifo.start \
        && buffer_num != CANBUS_BUFFER_FIFO )
    {// Filter buffer pointer points to a buffer in or past the FIFO region
        // Return an error
        return CANBUS_E_INPUT;
    }

    // Check if the buffer is empty
    if( canbus.is_empty(object, buffer_num) )
    {// Buffer is empty
        // No message to read
        return 0;
    }

    // Treat the special case of reading from the FIFO
    if( buffer_num == CANBUS_BUFFER_FIFO )
    {// Read from the FIFO buffer
        // Read from the buffer specified by CiFIFO.FNRB (i.e. next read buffer)
        buffer_ptr = ((canbus_private_t *)(object->private))->rx_dma_->buffer_a + \
            ((((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiFIFO)) \
             ->fnrb) * 8);
    }
    else
    {// Read from a normal buffer
        // Read from the buffer specified by buffer_num
        buffer_ptr = ((canbus_private_t *)(object->private))->rx_dma_->buffer_a + (buffer_num*8);
    }

    // Read message contained in buffer_ptr
    // Check if the message is extended or standard
    if( *buffer_ptr & (0x0001) )
    {// IDE bit is set (i.e. extended message ID)
        message->header.ide = 1;
        message->header.sid = ((*buffer_ptr)&0x1FFC)>>2;
        message->header.eid = (((*(buffer_ptr+1))&0x0FFF)<<6) + (((*(buffer_ptr+2))&0xFC00)>>10);
        message->header.rtr = ((*(buffer_ptr+2))&0x0200)>>9;
    }
    else
    {// IDE bit is not set (i.e. standard message ID)
        message->header.ide = 0;
        message->header.sid = ((*buffer_ptr)&0x1FFC)>>2;
        message->header.rtr = ((*buffer_ptr)&0x0002)>>1;
    }

    // Read the data length code
    message->dlc = ((*(buffer_ptr+2))&0x000F);

    // Read in the data characters
    for(i=0; i<message->dlc; ++i)
    {
        if( i%2 == 0 )
        {// Even data character
            // Read lower byte of message data
            message->data[i] = ((*(buffer_ptr+(i/2)+3))&0x00FF);
        }
        else
        {// Odd data character
            // Read upper byte of message data
            message->data[i] = ((*(buffer_ptr+(i/2)+3))&0xFF00)>>8;
        }
    }

    // Read the filter that enabled the message to be received
    message->filter = ((*(buffer_ptr+7))&0x1F00)>>8;

    return 1;
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return false;
    }

    // May only check state of buffers 0-31 and FIFO, not ALL or NONE
    if( buffer_num == CANBUS_BUFFER_ALL || buffer_num == CANBUS_BUFFER_NONE )
    {
        return false;
    }

    // Check if buffer exists
    if( !canbus.buffer_exists(object, buffer_num) )
    {// Buffer doesn't exist in DMA RAM
        return false;
    }

    // Check if buffer is part of FIFO region
    if( buffer_num >= ((canbus_private_t *)(object->private))->attr_.fifo.start \
        && buffer_num != CANBUS_BUFFER_FIFO )
    {// Filter buffer pointer points to a buffer in or past the FIFO region
        // Return an error
        return false;
    }
    
    // Check the direction of buffer_num
    if( canbus.get_direction(object, buffer_num) == CANBUS_DIRECTION_RX )
    {// Buffer is marked RX
        // Check the special case of the FIFO buffer
        if( buffer_num == CANBUS_BUFFER_FIFO )
        {// Check the FIFO buffer for unread messages
            // Check which CiRXFUL SFR to use
            if( (((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                           + CANBUS_SFR_OFFSET_CiFIFO))->fnrb) <= 0x000F )
            {// Use CiRXFUL1
                // Check the correct RXFUL bit
                // Return true if RXFUL bit is false (no message waiting to be read)
                return !( *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL1) \
                         & (1<<((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                         + CANBUS_SFR_OFFSET_CiFIFO))->fnrb) );
            }
            else
            {// Use CiRXFUL2
                // Check the correct RXFUL bit
                // Return true if RXFUL bit is false (no message waiting to be read)
                return !(*(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL2) \
                         & (1<<((((canbus_cififo_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                          + CANBUS_SFR_OFFSET_CiFIFO))->fnrb)-16)));
            }
        }
        else
        {// Check if the given buffer is unread
            // Check which CiRXFUL SFR to use
            if( buffer_num <= CANBUS_BUFFER_B15 )
            {// Use CiRXFUL1
                // Check the correct RXFUL bit
                // Return true if RXFUL bit is false (no message waiting to be read)
                return !( *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL1) \
                         & (1<<buffer_num) );
            }
            else
            {// Use CiRXFUL2
                // Clear correct RXFUL bit
                // Return true if RXFUL bit is false (no message waiting to be read)
                return !( *(CANBUS_BASE_ADDRESS(object) + CANBUS_SFR_OFFSET_CiRXFUL2) \
                          & (1<<(buffer_num-16)) );
            }
        }
    }
    else
    {// Buffer is marked TX
        // Check the specified buffer for a message awaiting transmission
        switch( buffer_num )
        {
        case CANBUS_BUFFER_B0:
            // Check if there is a message currently in B0
            // Return true if TXREQm is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR01CON))->txreqm );
        case CANBUS_BUFFER_B1:
            // Check if there is a message currently in B1
            // Return true if TXREQn is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR01CON))->txreqn );
        case CANBUS_BUFFER_B2:
            // Check if there is a message currently in B2
            // Return true if TXREQm is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR23CON))->txreqm );
        case CANBUS_BUFFER_B3:
            // Check if there is a message currently in B3
            // Return true if TXREQn is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR23CON))->txreqn );
        case CANBUS_BUFFER_B4:
            // Check if there is a message currently in B4
            // Return true if TXREQm is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR45CON))->txreqm );
        case CANBUS_BUFFER_B5:
            // Check if there is a message currently in B5
            // Return true if TXREQn is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR45CON))->txreqn );
        case CANBUS_BUFFER_B6:
            // Check if there is a message currently in B6
            // Return true if TXREQm is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR67CON))->txreqm );
        case CANBUS_BUFFER_B7:
            // Check if there is a message currently in B7
            // Return true if TXREQn is false (no message waiting to be sent)
            return !( ((canbus_citrmncon_bits_t *)(CANBUS_BASE_ADDRESS(object) \
                                                   + CANBUS_SFR_OFFSET_CiTR67CON))->txreqn );
        default:
            // This should never happen, only buffers 0-7 are able to be TX!
            return false;
        }
    }
}

/**
 * @brief Check if a buffer exists in DMA RAM.
 *
 * @details Nothing here.
 *
 * @private
 */
static bool canbus_buffer_exists(canbus_t *object,
                                     canbus_buffer_t buffer_num)
{
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    // Check for a valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return CANBUS_E_OBJECT;
    }

    // Check if buffer exists
    switch( buffer_num )
    {
    case CANBUS_BUFFER_B0:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b0_exists;

    case CANBUS_BUFFER_B1:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b1_exists;

    case CANBUS_BUFFER_B2:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b2_exists;
        
    case CANBUS_BUFFER_B3:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b3_exists;
        
    case CANBUS_BUFFER_B4:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b4_exists;
        
    case CANBUS_BUFFER_B5:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b5_exists;
        
    case CANBUS_BUFFER_B6:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b6_exists;
        
    case CANBUS_BUFFER_B7:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b7_exists;
        
    case CANBUS_BUFFER_B8:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b8_exists;
        
    case CANBUS_BUFFER_B9:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b9_exists;

    case CANBUS_BUFFER_B10:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b10_exists;
        
    case CANBUS_BUFFER_B11:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b11_exists;
        
    case CANBUS_BUFFER_B12:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b12_exists;
        
    case CANBUS_BUFFER_B13:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b13_exists;
        
    case CANBUS_BUFFER_B14:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b14_exists;
        
    case CANBUS_BUFFER_B15:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b15_exists;
        
    case CANBUS_BUFFER_B16:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b16_exists;
        
    case CANBUS_BUFFER_B17:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b17_exists;
        
    case CANBUS_BUFFER_B18:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b18_exists;
        
    case CANBUS_BUFFER_B19:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b19_exists;
        
    case CANBUS_BUFFER_B20:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b20_exists;
        
    case CANBUS_BUFFER_B21:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b21_exists;
        
    case CANBUS_BUFFER_B22:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b22_exists;
        
    case CANBUS_BUFFER_B23:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b23_exists;
        
    case CANBUS_BUFFER_B24:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b24_exists;
        
    case CANBUS_BUFFER_B25:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b25_exists;
        
    case CANBUS_BUFFER_B26:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b26_exists;
        
    case CANBUS_BUFFER_B27:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b27_exists;
        
    case CANBUS_BUFFER_B28:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b28_exists;
        
    case CANBUS_BUFFER_B29:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b29_exists;

    case CANBUS_BUFFER_B30:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b30_exists;
        
    case CANBUS_BUFFER_B31:
        return ((canbus_private_t *)(object->private))->buffer_exists_.b31_exists;

    case CANBUS_BUFFER_FIFO:
        return true;

    case CANBUS_BUFFER_NONE:
    case CANBUS_BUFFER_ALL:
    default:
        // Invalid input
        return false;
    }
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    return ( object != NULL                                          \
             && object->module_number > 0                            \
             && object->module_number <= CANBUS_HW_NUMBER_OF_MODULES \
             && object->private != NULL );
}

/**
 * @brief Get the buffer's direction.
 *
 * @details Nothing here.
 *
 * @private
 */
int canbus_get_direction(canbus_t *object,
                         canbus_buffer_t buffer_num)
{
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif

    // Check for valid object
    if( !canbus.is_valid(object) )
    {// Invalid object
        return CANBUS_E_OBJECT;
    }

    // Check for invalid input
    if( buffer_num == CANBUS_BUFFER_ALL         \
        || buffer_num == CANBUS_BUFFER_NONE )
    {// Invalid input
        return CANBUS_E_INPUT;
    }
    
    // Check buffer for direction
    if( buffer_num >= CANBUS_BUFFER_B8          \
        || buffer_num == CANBUS_BUFFER_FIFO )
    {// Any buffer from B8 and up or FIFO must be RX
        return CANBUS_DIRECTION_RX;
    }
    else if( buffer_num == CANBUS_BUFFER_B0 )
    {// Return buffer 0 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b0;
    }
    else if( buffer_num == CANBUS_BUFFER_B1 )
    {// Return buffer 1 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b1;
    }
    else if( buffer_num == CANBUS_BUFFER_B2 )
    {// Return buffer 2 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b2;
    }
    else if( buffer_num == CANBUS_BUFFER_B3 )
    {// Return buffer 3 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b3;
    }
    else if( buffer_num == CANBUS_BUFFER_B4 )
    {// Return buffer 4 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b4;
    }
    else if( buffer_num == CANBUS_BUFFER_B5 )
    {// Return buffer 5 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b5;
    }
    else if( buffer_num == CANBUS_BUFFER_B6 )
    {// Return buffer 6 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b6;
    }
    else if( buffer_num == CANBUS_BUFFER_B7 )
    {// Return buffer 7 direction
        return ((canbus_private_t *)(object->private))->attr_.buffer_dir.b7;
    }
    else
    {// Unknown input
        return CANBUS_E_INPUT;
    }
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
#ifdef DEBUG__
    canbus_private_t *debug_private = (canbus_private_t *)(object->private);
#endif
    
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
