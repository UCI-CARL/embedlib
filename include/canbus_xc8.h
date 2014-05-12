/*************************************************************************//**
 * @file ecan.h
 * 
 * @brief ECAN library header.
 * 
 * @createdon{9/30/2011}
 * @createdby{Liam Bucci}
 * @projectname{PIC C Library}
 * @projecttype{PIC C Embedded Library}
 * @subprojectname{ECAN}
 * @subprojecttype{ECAN Library}
 * @subprojectdescription
 * Embedded library for PIC MCUs which sets and configures chip specific
 * ECAN settings depending on the project's "ecan.def" file. Also, provides
 * functionality for input and output of messages through the ECAN hardware
 * module and defines macros which can be used to access and control ECAN
 * settings and flags.
 * 
 * @pcbnum{N/A}
 * @pcbname{N/A}
 * @pcbdescription
 * N/A
 * 
 * @notes
 * 
 ****************************************************************************/

#ifndef _ECAN_H
#define _ECAN_H

#include <ecan.def>

#define ENABLE	1
#define DISABLE	0


#if defined(__C30__)

/**
 * @defgroup C30ECANLibrary C30 ECAN Library
 * C30 ECAN Library
 * @{
 */

#if defined(__dsPIC33F__)
	#include <p33Fxxxx.h>
#elif defined(__PIC24H__)
	#include <p24Hxxxx.h>
#else
	#error "ECAN: Unknown processor type for C30 Compiler (Requires dsPIC33F or PIC24H!!!)"
#endif

/**
 * @name ECAN Control Macros
 * @brief ECAN Control Macros
 *
 * These macros control the basic operating blocks of the ECAN1 module.
 * @{
 */

#define ECAN_C1_STOP()				C1CTRL1bits.CSIDL=1				/**< Stop ECAN1 module at next idle @hideinitializer */
#define ECAN_C1_START()				C1CTRL1bits.CSIDL=0				/**< Start ECAN1 module in idle mode @hideinitializer */
#define ECAN_C1_ABORT()				C1CTRL1bits.ABAT=1				/**< Abort all pending transmissions (hardware cleared) @hideinitializer */
#define ECAN_C1_SET_OPMODE(x)		C1CTRL1bits.REQOP=(x)			/**< Set operation mode for ECAN1 module @param x Operation mode (0-7) @hideinitializer */
#define ECAN_C1_GET_OPMODE()		C1CTRL1bits.OPMODE				/**< Get current operation mode for ECAN1 module @hideinitializer */
#define ECAN_C1_EN_CANCAPTURE()		C1CTRL1bits.CANCAP=1			/**< Enable input capture based on ECAN1 RX timer event @hideinitializer */
#define ECAN_C1_DIS_CANCAPTURE()	C1CTRL1bits.CANCAP=0			/**< Disable input capture based on ECAN1 RX timer event @hideinitializer */
#define ECAN_C1_USE_FILTERSFR()		C1CTRL1bits.WIN=1				/**< Use filter window for ECAN1 module SFRs @hideinitializer */
#define ECAN_C1_USE_BUFFERSFR()		C1CTRL1bits.WIN=0				/**< Use buffer window for ECAN1 module SFRs @hideinitializer */
#define ECAN_C1_SET_DEVICENET(x)	C1CTRL2bits.DNCNT=(x)			/**< Set DeviceNet compare bits @param x Data bits to check with EID (0-18) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANBaudRateConfigurationMacros
 * ECAN Baud Rate Configuration Macros
 * @{
 */

#define ECAN_C1_SET_SYNCJUMP(x)		C1CFG1bits.SJW=(x)				/**< Set synchronization jump width @param x Sync Jump Width (0-3) @hideinitializer */
#define ECAN_C1_GET_SYNCJUMP()		C1CFG1bits.SJW					/**< Get synchronization jump width @hideinitializer */
#define ECAN_C1_SET_PRESCALER(x)	C1CFG1bits.BRP=(x)				/**< Set baud rate prescaler @param x Baud Rate Prescaler (0-63) @hideinitializer */
#define ECAN_C1_GET_PRESCALER()		C1CFG1bits.BRP					/**< Get baud rate prescaler @hideinitializer */
#define ECAN_C1_EN_FILTERWAKE()		C1CFG2bits.WAKFIL=1				/**< Enable wake-up on CAN bus line filter @hideinitializer */
#define ECAN_C1_DIS_FILTERWAKE()	C1CFG2bits.WAKFIL=0				/**< Disable wake-up on CAN bus line filter @hideinitializer */
#define ECAN_C1_SET_PHASESEG2(x)	C1CFG2bits.SEG2PH=(x)			/**< Set Phase Segment 2 length @param x Phase Segment 2 length (0-7) @hideinitializer */
#define ECAN_C1_GET_PHASESEG2()		C1CFG2bits.SEG2PH				/**< Get Phase Segment 2 length @hideinitializer */
#define ECAN_C1_EN_PROGPHSEG2()		C1CFG2bits.SEG2PHTS=1			/**< Enable free programming of phase segment 2 length @hideinitializer */
#define ECAN_C1_DIS_PROGPHSEG2()	C1CFG2bits.SEG2PHTS=0			/**< Disable free programming of phase segment 2 length. Use maximum of phase segment 1 length or IPT, whichever is greater @hideinitializer */
#define ECAN_C1_EN_THREESAMPLE()	C1CFG2bits.SAM=1				/**< Enable three samples per sample point @hideinitializer */
#define ECAN_C1_DIS_THREESAMPLE()	C1CFG2bits.SAM=0				/**< Disable three samples per sample point, only use one sample per point @hideinitializer */
#define ECAN_C1_SET_PHASESEG1(x)	C1CFG2bits.SEG1PH=(x)			/**< Set Phase Segment 1 length @param x Phase Segment 1 length (0-7) @hideinitializer */
#define ECAN_C1_GET_PHASESEG1(x)	C1CFG2bits.SEG1PH				/**< Get Phase Segment 1 length @hideinitializer */
#define ECAN_C1_SET_PROPSEG(x)		C1CFG2bits.PRSEG=(x)			/**< Set Propagation Time Segment length @param x Propagation Time Segment length (0-7) @hideinitializer */
#define ECAN_C1_GET_PROPSEG			C1CFG2bits.PRSEG				/**< Get Propagation Time Segment length @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANAcceptanceFilterEnableMacros
 * ECAN Acceptance Filter Enable Macros
 * @{
 */

#define ECAN_C1_SET_FILTER0(x)		C1FEN1bits.FLTEN0=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 0 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER1(x)		C1FEN1bits.FLTEN1=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 1 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER2(x)		C1FEN1bits.FLTEN2=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 2 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER3(x)		C1FEN1bits.FLTEN3=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 3 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER4(x)		C1FEN1bits.FLTEN4=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 4 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER5(x)		C1FEN1bits.FLTEN5=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 5 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER6(x)		C1FEN1bits.FLTEN6=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 6 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER7(x)		C1FEN1bits.FLTEN7=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 7 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER8(x)		C1FEN1bits.FLTEN8=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 8 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER9(x)		C1FEN1bits.FLTEN9=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 9 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER10(x)		C1FEN1bits.FLTEN10=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 10 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER11(x)		C1FEN1bits.FLTEN11=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 11 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER12(x)		C1FEN1bits.FLTEN12=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 12 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER13(x)		C1FEN1bits.FLTEN13=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 13 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER14(x)		C1FEN1bits.FLTEN14=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 14 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_SET_FILTER15(x)		C1FEN1bits.FLTEN15=(x)			/**< Set enable/disable value for ECAN 1 message acceptance filter 15 @param x Enable/Disable (1 or 0) @hideinitializer */

#define ECAN_C1_GET_FILTER0			C1FEN1bits.FLTEN0			/**< Get enable/disable value for ECAN 1 message acceptance filter 0 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER1			C1FEN1bits.FLTEN1			/**< Get enable/disable value for ECAN 1 message acceptance filter 1 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER2			C1FEN1bits.FLTEN2			/**< Get enable/disable value for ECAN 1 message acceptance filter 2 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER3			C1FEN1bits.FLTEN3			/**< Get enable/disable value for ECAN 1 message acceptance filter 3 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER4			C1FEN1bits.FLTEN4			/**< Get enable/disable value for ECAN 1 message acceptance filter 4 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER5			C1FEN1bits.FLTEN5			/**< Get enable/disable value for ECAN 1 message acceptance filter 5 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER6			C1FEN1bits.FLTEN6			/**< Get enable/disable value for ECAN 1 message acceptance filter 6 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER7			C1FEN1bits.FLTEN7			/**< Get enable/disable value for ECAN 1 message acceptance filter 7 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER8			C1FEN1bits.FLTEN8			/**< Get enable/disable value for ECAN 1 message acceptance filter 8 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER9			C1FEN1bits.FLTEN9			/**< Get enable/disable value for ECAN 1 message acceptance filter 9 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER10		C1FEN1bits.FLTEN10			/**< Get enable/disable value for ECAN 1 message acceptance filter 10 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER11		C1FEN1bits.FLTEN11			/**< Get enable/disable value for ECAN 1 message acceptance filter 11 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER12		C1FEN1bits.FLTEN12			/**< Get enable/disable value for ECAN 1 message acceptance filter 12 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER13		C1FEN1bits.FLTEN13			/**< Get enable/disable value for ECAN 1 message acceptance filter 13 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER14		C1FEN1bits.FLTEN14			/**< Get enable/disable value for ECAN 1 message acceptance filter 14 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */
#define ECAN_C1_GET_FILTER15		C1FEN1bits.FLTEN15			/**< Get enable/disable value for ECAN 1 message acceptance filter 15 @return 1 if filter is enabled @return 0 if filter is disabled @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANAcceptanceFilterStandardIdentifierRegisterMacros
 * ECAN Acceptance Filter Standard Identifier Register Macros
 * @{
 */

#define ECAN_C1_SET_SID0(x)			C1RXF0SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 0 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID0			C1RXF0SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 0 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID1(x)			C1RXF1SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 1 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID1			C1RXF1SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 1 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID2(x)			C1RXF2SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 2 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID2			C1RXF2SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 2 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID3(x)			C1RXF3SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 3 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID3			C1RXF3SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 3 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID4(x)			C1RXF4SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 4 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID4			C1RXF4SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 4 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID5(x)			C1RXF5SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 5 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID5			C1RXF5SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 5 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID6(x)			C1RXF6SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 6 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID6			C1RXF6SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 6 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID7(x)			C1RXF7SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 7 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID7			C1RXF7SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 7 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID8(x)			C1RXF8SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 8 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID8			C1RXF8SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 8 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID9(x)			C1RXF9SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 9 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID9			C1RXF9SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 9 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID10(x)		C1RXF10SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 10 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID10			C1RXF10SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 10 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID11(x)		C1RXF11SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 11 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID11			C1RXF11SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 11 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID12(x)		C1RXF12SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 12 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID12			C1RXF12SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 12 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID13(x)		C1RXF13SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 13 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID13			C1RXF13SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 13 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID14(x)		C1RXF14SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 14 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID14			C1RXF14SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 14 @return 11-bit Standard ID @hideinitializer */

#define ECAN_C1_SET_SID15(x)		C1RXF15SIDbits.SID=(x)		/**< Set 11-bit Standard ID corresponding to Filter 15 which will be matched against for message acceptance @param x Standard ID to be matched (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SID15			C1RXF15SIDbits.SID			/**< Get 11-bit Standard ID corresponding to Filter 15 @return 11-bit Standard ID @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANAcceptanceFilterExtendedIdentifierRegisterMacros
 * ECAN Acceptance Filter Extended Identifier Register Macros
 * @{
 */

#define ECAN_C1_SET_EXTEND0(x)		C1RXF0SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 0 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND0			C1RXF0SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 0 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID0(x)			C1RXF0SIDbits.SID=((x)&0x7FF);C1RXF0EID=(((x)>>11)&0xFFFF);C1RXF0SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 0 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID0			((unsigned long)((C1RXF0SIDbits.EID<<27)+(C1RXF0EID<<11)+C1RXF0SIDbits.SID))				/**< Get Extended ID corresponding to Filter 0 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND1(x)		C1RXF1SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 1 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND1			C1RXF1SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 1 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID1(x)			C1RXF1SIDbits.SID=((x)&0x7FF);C1RXF1EID=(((x)>>11)&0xFFFF);C1RXF1SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 1 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID1			((unsigned long)((C1RXF1SIDbits.EID<<27)+(C1RXF1EID<<11)+C1RXF1SIDbits.SID))				/**< Get Extended ID corresponding to Filter 1 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND2(x)		C1RXF2SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 2 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND2			C1RXF2SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 2 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID2(x)			C1RXF2SIDbits.SID=((x)&0x7FF);C1RXF2EID=(((x)>>11)&0xFFFF);C1RXF2SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 2 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID2			((unsigned long)((C1RXF2SIDbits.EID<<27)+(C1RXF2EID<<11)+C1RXF2SIDbits.SID))				/**< Get Extended ID corresponding to Filter 2 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND3(x)		C1RXF3SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 3 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND3			C1RXF3SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 3 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID3(x)			C1RXF3SIDbits.SID=((x)&0x7FF);C1RXF3EID=(((x)>>11)&0xFFFF);C1RXF3SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 3 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID3			((unsigned long)((C1RXF3SIDbits.EID<<27)+(C1RXF3EID<<11)+C1RXF3SIDbits.SID))				/**< Get Extended ID corresponding to Filter 3 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND4(x)		C1RXF4SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 4 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND4			C1RXF4SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 4 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID4(x)			C1RXF4SIDbits.SID=((x)&0x7FF);C1RXF4EID=(((x)>>11)&0xFFFF);C1RXF4SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 4 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID4			((unsigned long)((C1RXF4SIDbits.EID<<27)+(C1RXF4EID<<11)+C1RXF4SIDbits.SID))				/**< Get Extended ID corresponding to Filter 4 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND5(x)		C1RXF5SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 5 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND5			C1RXF5SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 5 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID5(x)			C1RXF5SIDbits.SID=((x)&0x7FF);C1RXF5EID=(((x)>>11)&0xFFFF);C1RXF5SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 5 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID5			((unsigned long)((C1RXF5SIDbits.EID<<27)+(C1RXF5EID<<11)+C1RXF5SIDbits.SID))				/**< Get Extended ID corresponding to Filter 5 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND6(x)		C1RXF6SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 6 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND6			C1RXF6SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 6 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID6(x)			C1RXF6SIDbits.SID=((x)&0x7FF);C1RXF6EID=(((x)>>11)&0xFFFF);C1RXF6SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 6 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID6			((unsigned long)((C1RXF6SIDbits.EID<<27)+(C1RXF6EID<<11)+C1RXF6SIDbits.SID))				/**< Get Extended ID corresponding to Filter 6 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND7(x)		C1RXF7SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 7 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND7			C1RXF7SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 7 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID7(x)			C1RXF7SIDbits.SID=((x)&0x7FF);C1RXF7EID=(((x)>>11)&0xFFFF);C1RXF7SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 7 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID7			((unsigned long)((C1RXF7SIDbits.EID<<27)+(C1RXF7EID<<11)+C1RXF7SIDbits.SID))				/**< Get Extended ID corresponding to Filter 7 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND8(x)		C1RXF8SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 8 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND8			C1RXF8SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 8 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID8(x)			C1RXF8SIDbits.SID=((x)&0x7FF);C1RXF8EID=(((x)>>11)&0xFFFF);C1RXF8SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 8 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID8			((unsigned long)((C1RXF8SIDbits.EID<<27)+(C1RXF8EID<<11)+C1RXF8SIDbits.SID))				/**< Get Extended ID corresponding to Filter 8 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND9(x)		C1RXF9SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 9 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND9			C1RXF9SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 9 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID9(x)			C1RXF9SIDbits.SID=((x)&0x7FF);C1RXF9EID=(((x)>>11)&0xFFFF);C1RXF9SIDbits.EID=((x)>>27)		/**< Set Extended ID corresponding to Filter 9 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID9			((unsigned long)((C1RXF9SIDbits.EID<<27)+(C1RXF9EID<<11)+C1RXF9SIDbits.SID))				/**< Get Extended ID corresponding to Filter 9 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND10(x)		C1RXF10SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 10 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND10		C1RXF10SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 10 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID10(x)		C1RXF10SIDbits.SID=((x)&0x7FF);C1RXF10EID=(((x)>>11)&0xFFFF);C1RXF10SIDbits.EID=((x)>>27)	/**< Set Extended ID corresponding to Filter 10 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID10			((unsigned long)((C1RXF10SIDbits.EID<<27)+(C1RXF10EID<<11)+C1RXF10SIDbits.SID))				/**< Get Extended ID corresponding to Filter 10 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND11(x)		C1RXF11SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 11 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND11		C1RXF11SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 11 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID11(x)		C1RXF11SIDbits.SID=((x)&0x7FF);C1RXF11EID=(((x)>>11)&0xFFFF);C1RXF11SIDbits.EID=((x)>>27)	/**< Set Extended ID corresponding to Filter 11 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID11			((unsigned long)((C1RXF11SIDbits.EID<<27)+(C1RXF11EID<<11)+C1RXF11SIDbits.SID))				/**< Get Extended ID corresponding to Filter 11 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND12(x)		C1RXF12SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 12 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND12		C1RXF12SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 12 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID12(x)		C1RXF12SIDbits.SID=((x)&0x7FF);C1RXF12EID=(((x)>>11)&0xFFFF);C1RXF12SIDbits.EID=((x)>>27)	/**< Set Extended ID corresponding to Filter 12 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID12			((unsigned long)((C1RXF12SIDbits.EID<<27)+(C1RXF12EID<<11)+C1RXF12SIDbits.SID))				/**< Get Extended ID corresponding to Filter 12 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND13(x)		C1RXF13SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 13 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND13		C1RXF13SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 13 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID13(x)		C1RXF13SIDbits.SID=((x)&0x7FF);C1RXF13EID=(((x)>>11)&0xFFFF);C1RXF13SIDbits.EID=((x)>>27)	/**< Set Extended ID corresponding to Filter 13 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID13			((unsigned long)((C1RXF13SIDbits.EID<<27)+(C1RXF13EID<<11)+C1RXF13SIDbits.SID))				/**< Get Extended ID corresponding to Filter 13 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND14(x)		C1RXF14SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 14 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND14		C1RXF14SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 14 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID14(x)		C1RXF14SIDbits.SID=((x)&0x7FF);C1RXF14EID=(((x)>>11)&0xFFFF);C1RXF14SIDbits.EID=((x)>>27)	/**< Set Extended ID corresponding to Filter 14 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID14			((unsigned long)((C1RXF14SIDbits.EID<<27)+(C1RXF14EID<<11)+C1RXF14SIDbits.SID))				/**< Get Extended ID corresponding to Filter 14 @return Extended ID (29-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_EXTEND15(x)		C1RXF15SIDbits.EXIDE=(x)		/**< Set enable/disable bit for Extended Identifiers for Filter 15 @param x Enable/Disable (1 or 0) @hideinitializer */
#define ECAN_C1_GET_EXTEND15		C1RXF15SIDbits.EXIDE			/**< Get enable/disable bit for Extended Identifiers for Filter 15 @return 1 Extended Identifiers enabled @return 0 Extended Identifiers disabled @hideinitializer */
#define ECAN_C1_SET_EID15(x)		C1RXF15SIDbits.SID=((x)&0x7FF);C1RXF15EID=(((x)>>11)&0xFFFF);C1RXF15SIDbits.EID=((x)>>27)	/**< Set Extended ID corresponding to Filter 15 which will be matched against for message acceptance @param x Extended ID to be matched (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EID15			((unsigned long)((C1RXF15SIDbits.EID<<27)+(C1RXF15EID<<11)+C1RXF15SIDbits.SID))				/**< Get Extended ID corresponding to Filter 15 @return Extended ID (29-bit unsigned integer) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANAcceptanceFilterMaskStandardIdentifierMacros
 * ECAN Acceptance Filter Mask Standard Identifier Macros
 * @{
 */

#define ECAN_C1_SET_SIDMASK0(x)		C1RXM0SIDbits.SID=(x)		/**< Set Mask 0 for Standard Identifier Filters @param x Mask for Standard Identifier (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SIDMASK0		C1RXM0SIDbits.SID			/**< Get Mask 0 for Standard Identifier Filters @return Mask for Standard Identifier (11-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_SIDMASK1(x)		C1RXM1SIDbits.SID=(x)		/**< Set Mask 1 for Standard Identifier Filters @param x Mask for Standard Identifier (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SIDMASK1		C1RXM1SIDbits.SID			/**< Get Mask 1 for Standard Identifier Filters @return Mask for Standard Identifier (11-bit unsigned integer) @hideinitializer */

#define ECAN_C1_SET_SIDMASK2(x)		C1RXM2SIDbits.SID=(x)		/**< Set Mask 2 for Standard Identifier Filters @param x Mask for Standard Identifier (11-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_SIDMASK2		C1RXM2SIDbits.SID			/**< Get Mask 2 for Standard Identifier Filters @return Mask for Standard Identifier (11-bit unsigned integer) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANAcceptanceFilterMaskExtendedIdentifierMacros
 * ECAN Acceptance Filter Mask Extended Identifier Macros
 * @{
 */

#define ECAN_C1_SET_MODEMASK0(x)	C1RXM0SIDbits.MIDE=(x)		/**< Set Mask 0 Identifier Receive Mode @param x Identifier Receive Mode (1 or 0) @hideinitializer */
#define ECAN_C1_GET_MODEMASK0		C1RXM0SIDbits.MIDE			/**< Get Mask 0 Identifier Receive Mode @return 1 if matching only message types corresponding to EXIDE bit @return 0 if matching any message type (SID or EID) @hideinitializer */

#define ECAN_C1_SET_EIDMASK0(x)		C1RXM0SIDbits.SID=((x)&0x7FF);C1RXM0EID=(((x)>>11)&0xFFFF);C1RXM0SIDbits.EID=((x)>>27)		/**< Set Mask 0 Extended Identifier @param x Mask Extended Identifier (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EIDMASK0		((unsigned long)((C1RXM0SIDbits.EID<<27)+(C1RXM0EID<<11)+C1RXM0SIDbits.SID)))				/**< Get Mask 0 Extended Identifier @return Mask Extended Identifier (29-bit unsigned integer) @hideinitializer */


#define ECAN_C1_SET_MODEMASK1(x)	C1RXM1SIDbits.MIDE=(x)		/**< Set Mask 1 Identifier Receive Mode @param x Identifier Receive Mode (1 or 0) @hideinitializer */
#define ECAN_C1_GET_MODEMASK1		C1RXM1SIDbits.MIDE			/**< Get Mask 1 Identifier Receive Mode @return 1 if matching only message types corresponding to EXIDE bit @return 0 if matching any message type (SID or EID) @hideinitializer */

#define ECAN_C1_SET_EIDMASK1(x)		C1RXM1SIDbits.SID=((x)&0x7FF);C1RXM1EID=(((x)>>11)&0xFFFF);C1RXM1SIDbits.EID=((x)>>27)		/**< Set Mask 1 Extended Identifier @param x Mask Extended Identifier (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EIDMASK1		((unsigned long)((C1RXM1SIDbits.EID<<27)+(C1RXM1EID<<11)+C1RXM1SIDbits.SID)))				/**< Get Mask 1 Extended Identifier @return Mask Extended Identifier (29-bit unsigned integer) @hideinitializer */


#define ECAN_C1_SET_MODEMASK2(x)	C1RXM2SIDbits.MIDE=(x)		/**< Set Mask 2 Identifier Receive Mode @param x Identifier Receive Mode (1 or 0) @hideinitializer */
#define ECAN_C1_GET_MODEMASK2		C1RXM2SIDbits.MIDE			/**< Get Mask 2 Identifier Receive Mode @return 1 if matching only message types corresponding to EXIDE bit @return 0 if matching any message type (SID or EID) @hideinitializer */

#define ECAN_C1_SET_EIDMASK2(x)		C1RXM2SIDbits.SID=((x)&0x7FF);C1RXM2EID=(((x)>>11)&0xFFFF);C1RXM2SIDbits.EID=((x)>>27)		/**< Set Mask 2 Extended Identifier @param x Mask Extended Identifier (29-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_EIDMASK2		((unsigned long)((C1RXM2SIDbits.EID<<27)+(C1RXM2EID<<11)+C1RXM2SIDbits.SID)))				/**< Get Mask 2 Extended Identifier @return Mask Extended Identifier (29-bit unsigned integer) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANFilterMaskSelectionMacros
 * ECAN Filter Mask Selection Macros
 * @{
 */

#define ECAN_C1_SET_F0MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 0 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F0MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 0 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F1MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 1 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F1MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 1 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F2MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 2 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F2MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 2 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F3MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 3 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F3MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 3 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F4MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 4 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F4MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 4 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F5MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 5 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F5MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 5 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F6MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 6 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F6MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 6 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F7MASK(x)		C1FMSKSEL1bits.F0MSK=(x)		/**< Set Filter 7 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F7MASK			C1FMSKSET1bits.F0MSK			/**< Get Filter 7 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F8MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 8 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F8MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 8 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F9MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 9 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F9MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 9 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F10MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 10 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F10MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 10 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F11MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 11 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F11MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 11 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F12MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 12 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F12MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 12 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F13MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 13 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F13MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 13 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F14MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 14 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F14MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 14 Mask Selection @return Mask Selection (0-2) @hideinitializer */

#define ECAN_C1_SET_F15MASK(x)		C1FMSKSEL2bits.F0MSK=(x)		/**< Set Filter 15 Mask Selection @param x Mask Selection (0-2) @hideinitializer */
#define ECAN_C1_GET_F15MASK			C1FMSKSET2bits.F0MSK			/**< Get Filter 15 Mask Selection @return Mask Selection (0-2) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANFilterBufferPointerMacros
 * ECAN Filter Buffer Pointer Macros
 * @{
 */

#define ECAN_C1_SET_F0BUFFER(x)		C1BUFPNT1bits.F0BP=(x)			/**< Set Filter 0 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F0BUFFER		C1BUFPNT1bits.F0BP				/**< Get Filter 0 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F1BUFFER(x)		C1BUFPNT1bits.F0BP=(x)			/**< Set Filter 1 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F1BUFFER		C1BUFPNT1bits.F0BP				/**< Get Filter 1 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F2BUFFER(x)		C1BUFPNT1bits.F0BP=(x)			/**< Set Filter 2 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F2BUFFER		C1BUFPNT1bits.F0BP				/**< Get Filter 2 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F3BUFFER(x)		C1BUFPNT1bits.F0BP=(x)			/**< Set Filter 3 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F3BUFFER		C1BUFPNT1bits.F0BP				/**< Get Filter 3 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F4BUFFER(x)		C1BUFPNT2bits.F0BP=(x)			/**< Set Filter 4 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F4BUFFER		C1BUFPNT2bits.F0BP				/**< Get Filter 4 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F5BUFFER(x)		C1BUFPNT2bits.F0BP=(x)			/**< Set Filter 5 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F5BUFFER		C1BUFPNT2bits.F0BP				/**< Get Filter 5 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F6BUFFER(x)		C1BUFPNT2bits.F0BP=(x)			/**< Set Filter 6 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F6BUFFER		C1BUFPNT2bits.F0BP				/**< Get Filter 6 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F7BUFFER(x)		C1BUFPNT2bits.F0BP=(x)			/**< Set Filter 7 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F7BUFFER		C1BUFPNT2bits.F0BP				/**< Get Filter 7 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F8BUFFER(x)		C1BUFPNT3bits.F0BP=(x)			/**< Set Filter 8 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F8BUFFER		C1BUFPNT3bits.F0BP				/**< Get Filter 8 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F9BUFFER(x)		C1BUFPNT3bits.F0BP=(x)			/**< Set Filter 9 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F9BUFFER		C1BUFPNT3bits.F0BP				/**< Get Filter 9 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F10BUFFER(x)	C1BUFPNT3bits.F0BP=(x)			/**< Set Filter 10 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F10BUFFER		C1BUFPNT3bits.F0BP				/**< Get Filter 10 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F11BUFFER(x)	C1BUFPNT3bits.F0BP=(x)			/**< Set Filter 11 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F11BUFFER		C1BUFPNT3bits.F0BP				/**< Get Filter 11 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F12BUFFER(x)	C1BUFPNT4bits.F0BP=(x)			/**< Set Filter 12 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F12BUFFER		C1BUFPNT4bits.F0BP				/**< Get Filter 12 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F13BUFFER(x)	C1BUFPNT4bits.F0BP=(x)			/**< Set Filter 13 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F13BUFFER		C1BUFPNT4bits.F0BP				/**< Get Filter 13 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F14BUFFER(x)	C1BUFPNT4bits.F0BP=(x)			/**< Set Filter 14 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F14BUFFER		C1BUFPNT4bits.F0BP				/**< Get Filter 14 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

#define ECAN_C1_SET_F15BUFFER(x)	C1BUFPNT4bits.F0BP=(x)			/**< Set Filter 15 Buffer Pointer @param x Buffer Pointer (0-15) @hideinitializer */
#define ECAN_C1_GET_F15BUFFER		C1BUFPNT4bits.F0BP				/**< Get Filter 15 Buffer Pointer @return Buffer Pointer (0-15) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANReceiveBufferFullMacros
 * ECAN Receive Buffer Full Macros
 * @{
 */

#define ECAN_C1_IS_RXBUF0FULL()		C1RXFUL1bits.RXFUL0				/**< Get status of RX Buffer 0 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF1FULL()		C1RXFUL1bits.RXFUL1				/**< Get status of RX Buffer 1 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF2FULL()		C1RXFUL1bits.RXFUL2				/**< Get status of RX Buffer 2 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF3FULL()		C1RXFUL1bits.RXFUL3				/**< Get status of RX Buffer 3 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF4FULL()		C1RXFUL1bits.RXFUL4				/**< Get status of RX Buffer 4 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF5FULL()		C1RXFUL1bits.RXFUL5				/**< Get status of RX Buffer 5 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF6FULL()		C1RXFUL1bits.RXFUL6				/**< Get status of RX Buffer 6 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF7FULL()		C1RXFUL1bits.RXFUL7				/**< Get status of RX Buffer 7 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF8FULL()		C1RXFUL1bits.RXFUL8				/**< Get status of RX Buffer 8 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF9FULL()		C1RXFUL1bits.RXFUL9				/**< Get status of RX Buffer 9 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF10FULL()	C1RXFUL1bits.RXFUL10			/**< Get status of RX Buffer 10 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF11FULL()	C1RXFUL1bits.RXFUL11			/**< Get status of RX Buffer 11 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF12FULL()	C1RXFUL1bits.RXFUL12			/**< Get status of RX Buffer 12 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF13FULL()	C1RXFUL1bits.RXFUL13			/**< Get status of RX Buffer 13 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF14FULL()	C1RXFUL1bits.RXFUL14			/**< Get status of RX Buffer 14 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF15FULL()	C1RXFUL1bits.RXFUL15			/**< Get status of RX Buffer 15 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF16FULL()	C1RXFUL2bits.RXFUL16			/**< Get status of RX Buffer 16 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF17FULL()	C1RXFUL2bits.RXFUL17			/**< Get status of RX Buffer 17 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF18FULL()	C1RXFUL2bits.RXFUL18			/**< Get status of RX Buffer 18 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF19FULL()	C1RXFUL2bits.RXFUL19			/**< Get status of RX Buffer 19 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF20FULL()	C1RXFUL2bits.RXFUL20			/**< Get status of RX Buffer 20 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF21FULL()	C1RXFUL2bits.RXFUL21			/**< Get status of RX Buffer 21 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF22FULL()	C1RXFUL2bits.RXFUL22			/**< Get status of RX Buffer 22 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF23FULL()	C1RXFUL2bits.RXFUL23			/**< Get status of RX Buffer 23 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF24FULL()	C1RXFUL2bits.RXFUL24			/**< Get status of RX Buffer 24 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF25FULL()	C1RXFUL2bits.RXFUL25			/**< Get status of RX Buffer 25 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF26FULL()	C1RXFUL2bits.RXFUL26			/**< Get status of RX Buffer 26 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF27FULL()	C1RXFUL2bits.RXFUL27			/**< Get status of RX Buffer 27 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF28FULL()	C1RXFUL2bits.RXFUL28			/**< Get status of RX Buffer 28 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF29FULL()	C1RXFUL2bits.RXFUL29			/**< Get status of RX Buffer 29 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF30FULL()	C1RXFUL2bits.RXFUL30			/**< Get status of RX Buffer 30 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */
#define ECAN_C1_IS_RXBUF31FULL()	C1RXFUL2bits.RXFUL31			/**< Get status of RX Buffer 31 (full/empty) @return 1 if buffer is full @return 0 if buffer is empty @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANReceiveBufferOverflowMacros
 * ECAN Receive Buffer Overflow Macros
 * @{
 */

#define ECAN_C1_IS_RXBUF0OVRFLW()	C1RXOVF1bits.RXOVF0				/**< Get status of RX Buffer 0 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF1OVRFLW()	C1RXOVF1bits.RXOVF1				/**< Get status of RX Buffer 1 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF2OVRFLW()	C1RXOVF1bits.RXOVF2				/**< Get status of RX Buffer 2 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF3OVRFLW()	C1RXOVF1bits.RXOVF3				/**< Get status of RX Buffer 3 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF4OVRFLW()	C1RXOVF1bits.RXOVF4				/**< Get status of RX Buffer 4 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF5OVRFLW()	C1RXOVF1bits.RXOVF5				/**< Get status of RX Buffer 5 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF6OVRFLW()	C1RXOVF1bits.RXOVF6				/**< Get status of RX Buffer 6 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF7OVRFLW()	C1RXOVF1bits.RXOVF7				/**< Get status of RX Buffer 7 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF8OVRFLW()	C1RXOVF1bits.RXOVF8				/**< Get status of RX Buffer 8 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF9OVRFLW()	C1RXOVF1bits.RXOVF9				/**< Get status of RX Buffer 9 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF10OVRFLW()	C1RXOVF1bits.RXOVF10			/**< Get status of RX Buffer 10 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF11OVRFLW()	C1RXOVF1bits.RXOVF11			/**< Get status of RX Buffer 11 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF12OVRFLW()	C1RXOVF1bits.RXOVF12			/**< Get status of RX Buffer 12 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF13OVRFLW()	C1RXOVF1bits.RXOVF13			/**< Get status of RX Buffer 13 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF14OVRFLW()	C1RXOVF1bits.RXOVF14			/**< Get status of RX Buffer 14 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF15OVRFLW()	C1RXOVF1bits.RXOVF15			/**< Get status of RX Buffer 15 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF16OVRFLW()	C1RXOVF1bits.RXOVF16			/**< Get status of RX Buffer 16 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF17OVRFLW()	C1RXOVF1bits.RXOVF17			/**< Get status of RX Buffer 17 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF18OVRFLW()	C1RXOVF1bits.RXOVF18			/**< Get status of RX Buffer 18 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF19OVRFLW()	C1RXOVF1bits.RXOVF19			/**< Get status of RX Buffer 19 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF20OVRFLW()	C1RXOVF1bits.RXOVF20			/**< Get status of RX Buffer 20 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF21OVRFLW()	C1RXOVF1bits.RXOVF21			/**< Get status of RX Buffer 21 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF22OVRFLW()	C1RXOVF1bits.RXOVF22			/**< Get status of RX Buffer 22 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF23OVRFLW()	C1RXOVF1bits.RXOVF23			/**< Get status of RX Buffer 23 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF24OVRFLW()	C1RXOVF1bits.RXOVF24			/**< Get status of RX Buffer 24 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF25OVRFLW()	C1RXOVF1bits.RXOVF25			/**< Get status of RX Buffer 25 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF26OVRFLW()	C1RXOVF1bits.RXOVF26			/**< Get status of RX Buffer 26 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF27OVRFLW()	C1RXOVF1bits.RXOVF27			/**< Get status of RX Buffer 27 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF28OVRFLW()	C1RXOVF1bits.RXOVF28			/**< Get status of RX Buffer 28 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF29OVRFLW()	C1RXOVF1bits.RXOVF29			/**< Get status of RX Buffer 29 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF30OVRFLW()	C1RXOVF1bits.RXOVF30			/**< Get status of RX Buffer 30 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */
#define ECAN_C1_IS_RXBUF31OVRFLW()	C1RXOVF1bits.RXOVF31			/**< Get status of RX Buffer 31 (overflow/no overflow) @return 1 if buffer has overflowed @return 0 if buffer hasn't overflowed @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANFIFOControlMacros
 * ECAN FIFO Control Macros
 * @{
 */

#define ECAN_C1_SET_DMABUFSIZE(x)	C1FCTRLbits.DMABS=(x)			/**< Set number of Buffers in DMA RAM @param x DMA Buffers (0-6) @hideinitializer */
#define ECAN_C1_GET_DMABUFSIZE		C1FCTRLbits.DMABS				/**< Get number of Buffers in DMA RAM @return DMA Buffers (0-6) @hideinitializer */
#define ECAN_C1_SET_FIFOSTART(x)	C1FCTRLbits.FSA=(x)				/**< Set Start Area of FIFO @param x Start Area (0-31) @hideinitializer */
#define ECAN_C1_GET_FIFOSTART		C1FCTRLbits.FSA					/**< Get Start Area of FIFO @return Start Area (0-31) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANFIFOStatusMacros
 * ECAN FIFO Status Macros
 * @{
 */

#define ECAN_C1_GET_FIFO_WRPNTR		C1FIFObits.FBP					/**< Get FIFO Write Pointer @return FIFO Write Pointer (5-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_FIFO_REPNTR		C1FIFObits.FNRB					/**< Get FIFO Read Pointer @return FIFO Read Pointer (5-bit unsigned integer) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANTXRXBufferControlMacros
 * ECAN TX/RX Buffer Control Macros
 * @{
 */

#define ECAN_C1_SET_BUF0_DIR(x)		C1TR01CONbits.TXEN0=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF0_DIR		C1TR01CONbits.TXEN0				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF0_ABORT		C1TR01CONbits.TXABT0			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF0_LOSTARB	C1TR01CONbits.TXLARB0			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF0_ERR		C1TR01CONbits.TXERR0			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF0_SEND(x)	C1TR01CONbits.TXREQ0=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF0_RTRREQ(x)	C1TR01CONbits.RTREN0=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF0_RTRREQ		C1TR01CONbits.RTREN0			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF0_PRI(x)		C1TR01CONbits.TX0PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF0_PRI		C1TR01CONbits.TX0PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF1_DIR(x)		C1TR01CONbits.TXEN1=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF1_DIR		C1TR01CONbits.TXEN1				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF1_ABORT		C1TR01CONbits.TXABT1			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF1_LOSTARB	C1TR01CONbits.TXLARB1			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF1_ERR		C1TR01CONbits.TXERR1			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF1_SEND(x)	C1TR01CONbits.TXREQ1=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF1_RTRREQ(x)	C1TR01CONbits.RTREN1=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF1_RTRREQ		C1TR01CONbits.RTREN1			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF1_PRI(x)		C1TR01CONbits.TX1PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF1_PRI		C1TR01CONbits.TX1PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF2_DIR(x)		C1TR23CONbits.TXEN2=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF2_DIR		C1TR23CONbits.TXEN2				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF2_ABORT		C1TR23CONbits.TXABT2			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF2_LOSTARB	C1TR23CONbits.TXLARB2			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF2_ERR		C1TR23CONbits.TXERR2			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF2_SEND(x)	C1TR23CONbits.TXREQ2=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF2_RTRREQ(x)	C1TR23CONbits.RTREN2=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF2_RTRREQ		C1TR23CONbits.RTREN2			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF2_PRI(x)		C1TR23CONbits.TX2PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF2_PRI		C1TR23CONbits.TX2PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF3_DIR(x)		C1TR23CONbits.TXEN3=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF3_DIR		C1TR23CONbits.TXEN3				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF3_ABORT		C1TR23CONbits.TXABT3			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF3_LOSTARB	C1TR23CONbits.TXLARB3			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF3_ERR		C1TR23CONbits.TXERR3			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF3_SEND(x)	C1TR23CONbits.TXREQ3=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF3_RTRREQ(x)	C1TR23CONbits.RTREN3=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF3_RTRREQ		C1TR23CONbits.RTREN3			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF3_PRI(x)		C1TR23CONbits.TX3PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF3_PRI		C1TR23CONbits.TX3PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF4_DIR(x)		C1TR45CONbits.TXEN4=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF4_DIR		C1TR45CONbits.TXEN4				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF4_ABORT		C1TR45CONbits.TXABT4			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF4_LOSTARB	C1TR45CONbits.TXLARB4			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF4_ERR		C1TR45CONbits.TXERR4			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF4_SEND(x)	C1TR45CONbits.TXREQ4=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF4_RTRREQ(x)	C1TR45CONbits.RTREN4=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF4_RTRREQ		C1TR45CONbits.RTREN4			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF4_PRI(x)		C1TR45CONbits.TX4PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF4_PRI		C1TR45CONbits.TX4PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF5_DIR(x)		C1TR45CONbits.TXEN5=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF5_DIR		C1TR45CONbits.TXEN5				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF5_ABORT		C1TR45CONbits.TXABT5			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF5_LOSTARB	C1TR45CONbits.TXLARB5			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF5_ERR		C1TR45CONbits.TXERR5			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF5_SEND(x)	C1TR45CONbits.TXREQ5=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF5_RTRREQ(x)	C1TR45CONbits.RTREN5=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF5_RTRREQ		C1TR45CONbits.RTREN5			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF5_PRI(x)		C1TR45CONbits.TX5PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF5_PRI		C1TR45CONbits.TX5PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF6_DIR(x)		C1TR67CONbits.TXEN6=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF6_DIR		C1TR67CONbits.TXEN6				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF6_ABORT		C1TR67CONbits.TXABT6			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF6_LOSTARB	C1TR67CONbits.TXLARB6			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF6_ERR		C1TR67CONbits.TXERR6			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF6_SEND(x)	C1TR67CONbits.TXREQ6=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF6_RTRREQ(x)	C1TR67CONbits.RTREN6=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF6_RTRREQ		C1TR67CONbits.RTREN6			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF6_PRI(x)		C1TR67CONbits.TX6PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF6_PRI		C1TR67CONbits.TX6PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

#define ECAN_C1_SET_BUF7_DIR(x)		C1TR67CONbits.TXEN7=(x)			/**< Set TX/RX Buffer direction (TX or RX) @param x Buffer Direction (1 or 0) @hideinitializer */
#define ECAN_C1_GET_BUF7_DIR		C1TR67CONbits.TXEN7				/**< Get TX/RX Buffer Direction (TX or RX) @return 1 if buffer is TX @return 0 if buffer is RX @hideinitializer */
#define ECAN_C1_GET_BUF7_ABORT		C1TR67CONbits.TXABT7			/**< Get message aborted status @return 1 if message was aborted @return 0 if message was completed successfully @hideinitializer */
#define ECAN_C1_GET_BUF7_LOSTARB	C1TR67CONbits.TXLARB7			/**< Get message lost arbitration status @return 1 if message lost arbitration @return 0 if message did not lose arbitration @hideinitializer */
#define ECAN_C1_GET_BUF7_ERR		C1TR67CONbits.TXERR7			/**< Get message error status @return 1 if message encountered an error @return 0 if no error occurred during sending @hideinitializer */
#define ECAN_C1_SET_BUF7_SEND(x)	C1TR67CONbits.TXREQ7=(x)		/**< Request message to be sent (or aborted) @param x Request send (or abort send) (0 or 1) @hideinitializer */
#define ECAN_C1_SET_BUF7_RTRREQ(x)	C1TR67CONbits.RTREN7=(x)		/**< Set RTR mode for Buffer @param x RTR mode (0 or 1) @hideinitializer */
#define ECAN_C1_GET_BUF7_RTRREQ		C1TR67CONbits.RTREN7			/**< Get RTR mode for Buffer @return 1 if TXREQ is set on RTR received @return 0 if TXREQ is unaffected on RTR received @hideinitializer */
#define ECAN_C1_SET_BUF7_PRI(x)		C1TR67CONbits.TX7PRI=(x)		/**< Set Buffer transmission priority @param x Transmission priority (0-3) @hideinitializer */
#define ECAN_C1_GET_BUF7_PRI		C1TR67CONbits.TX7PRI			/**< Get Buffer transmission priority @return Transmission priority (0-3) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANTXRXErrorCountMacros
 * ECAN TX/RX Error Count Macros
 * @{
 */

#define ECAN_C1_GET_TXERR			C1ECbits.TERRCNT				/**< Get TX Error Count @return Number of TX errors (8-bit unsigned integer) @hideinitializer */
#define ECAN_C1_GET_RXERR			C1ECbits.RERRCNT				/**< Get RX Error Count @return Number of RX errors (8-bit unsigned integer) @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANTXRXDataMacros
 * ECAN TX/RX Data Macros
 * @{
 */

#define ECAN_C1_SET_RXD(x)			C1RXD=(x)						/**< Set Received Data bits @param x Received Data bits (16-bit value) @hideinitializer */
#define ECAN_C1_GET_RXD				C1RXD							/**< Get Received Data bits @return Received Data bits (16-bit value) @hideinitializer */
#define ECAN_C1_SET_TXD(x)			C1TXD=(x)						/**< Set Transmit Data bits @param x Transmit Data bits (16-bit value) @hideinitializer */
#define ECAN_C1_GET_TXD				C1TXD							/**< Get Transmit Data bits @return Received Data bits (16-bit value) @hideinitializer */

/**
 * @}
 */


/**
 * @} C30ECANLibrary
 */


#elif defined(MCHP_C18)

/**
 * @defgroup C18ECANLibrary
 * C18 ECAN Library
 */

#include <p18cxxx.h>

#define COMSTAT_TXB0		COMSTATbits.TXB0
#define COMSTAT_TXBP		COMSTATbits.TXBP
#define COMSTAT_RXBP		COMSTATbits.RXBP
#define COMSTAT_RXBnOVFL	COMSTATbits.RXB1OVFL
#define COMSTAT_RXB0OVFL	COMSTATbits.RXB0OVFL
#define COMSTAT_FIFOEMPTY	COMSTATbits.FIFOEMPTY
#define COMSTAT_RXB1OVFL	COMSTATbits.RXB1OVFL

#define BRGCON2_SEG2PHTS	BRGCON2bits.SEG2PHTS
#define BRGCON2_SAM			BRGCON2bits.SAM
#define BRGCON3_WAKDIS		BRGCON3bits.WAKDIS
#define BRGCON3_WAKFIL		BRGCON3bits.WAKFIL

#define CIOCON_ENDRHI		CIOCONbits.ENDRHI
#define CIOCON_CANCAP   	CIOCONbits.CANCAP

#define PIR3_RXBnIF			PIR3bits.RXB1IF
#define PIR3_RXB0IF			PIR3bits.RXB0IF
#define PIR3_RXB1IF			PIR3bits.RXB1IF

#define PIR3_IRXIF			PIR3bits.IRXIF

#define CANCON_ABAT			CANCONbits.ABAT
#define RXB0CON_RXFUL		RXB0CONbits.RXFUL
#define RXB1CON_RXFUL		RXB1CONbits.RXFUL
#define RXB0CON_RXB0DBEN	RXB0CONbits.RXBODBEN
#define TXB0CON_TXREQ		TXB0CONbits.TXREQ
#define TXB1CON_TXREQ		TXB1CONbits.TXREQ
#define TXB2CON_TXREQ		TXB2CONbits.TXREQ
#define RXB0CON_FILHIT0		RXB0CONbits.FILHIT0
#define RXB0CON_RXM1		RXB0CONbits.RXM1
#define RXB0CON_RXM0		RXB0CONbits.RXM0

#define BSEL0_B0TXEN		BSEL0bits.B0TXEN
#define BSEL0_B1TXEN		BSEL0bits.B1TXEN
#define BSEL0_B2TXEN		BSEL0bits.B2TXEN
#define BSEL0_B3TXEN		BSEL0bits.B3TXEN
#define BSEL0_B4TXEN		BSEL0bits.B4TXEN
#define BSEL0_B5TXEN		BSEL0bits.B5TXEN

#define RXFCON0_RXF0EN		RXFCON0bits.RXF0EN
#define RXFCON0_RXF1EN		RXFCON0bits.RXF1EN
#define RXFCON0_RXF2EN		RXFCON0bits.RXF2EN
#define RXFCON0_RXF3EN		RXFCON0bits.RXF3EN
#define RXFCON0_RXF4EN		RXFCON0bits.RXF4EN
#define RXFCON0_RXF5EN		RXFCON0bits.RXF5EN
#define RXFCON0_RXF6EN		RXFCON0bits.RXF6EN
#define RXFCON0_RXF7EN		RXFCON0bits.RXF7EN

#define RXFCON1_RXF8EN		RXFCON1bits.RXF8EN
#define RXFCON1_RXF9EN		RXFCON1bits.RXF9EN
#define RXFCON1_RXF10EN		RXFCON1bits.RXF10EN
#define RXFCON1_RXF11EN		RXFCON1bits.RXF11EN
#define RXFCON1_RXF12EN		RXFCON1bits.RXF12EN
#define RXFCON1_RXF13EN		RXFCON1bits.RXF13EN
#define RXFCON1_RXF14EN		RXFCON1bits.RXF14EN
#define RXFCON1_RXF15EN		RXFCON1bits.RXF15EN


#define RXFBCON0_F0BP_0		RXFBCON0bits.F0BP_0
#define RXFBCON0_F0BP_1		RXFBCON0bits.F0BP_1
#define RXFBCON0_F0BP_2		RXFBCON0bits.F0BP_2
#define RXFBCON0_F0BP_3		RXFBCON0bits.F0BP_3
#define RXFBCON0_F1BP_0		RXFBCON0bits.F1BP_0
#define RXFBCON0_F1BP_1		RXFBCON0bits.F1BP_1
#define RXFBCON0_F1BP_2		RXFBCON0bits.F1BP_2
#define RXFBCON0_F1BP_3		RXFBCON0bits.F1BP_3

#define RXFBCON1_F2BP_0		RXFBCON1bits.F2BP_0
#define RXFBCON1_F2BP_1		RXFBCON1bits.F2BP_1
#define RXFBCON1_F2BP_2		RXFBCON1bits.F2BP_2
#define RXFBCON1_F2BP_3		RXFBCON1bits.F2BP_3
#define RXFBCON1_F3BP_0		RXFBCON1bits.F3BP_0
#define RXFBCON1_F3BP_1		RXFBCON1bits.F3BP_1
#define RXFBCON1_F3BP_2		RXFBCON1bits.F3BP_2
#define RXFBCON1_F3BP_3		RXFBCON1bits.F3BP_3

#define RXFBCON2_F4BP_0		RXFBCON2bits.F4BP_0
#define RXFBCON2_F4BP_1		RXFBCON2bits.F4BP_1
#define RXFBCON2_F4BP_2		RXFBCON2bits.F4BP_2
#define RXFBCON2_F4BP_3		RXFBCON2bits.F4BP_3
#define RXFBCON2_F5BP_0		RXFBCON2bits.F5BP_0
#define RXFBCON2_F5BP_1		RXFBCON2bits.F5BP_1
#define RXFBCON2_F5BP_2		RXFBCON2bits.F5BP_2
#define RXFBCON2_F5BP_3		RXFBCON2bits.F5BP_3

#define RXFBCON3_F6BP_0		RXFBCON3BITS.F6BP_0
#define RXFBCON3_F6BP_1		RXFBCON3BITS.F6BP_1
#define RXFBCON3_F6BP_2		RXFBCON3BITS.F6BP_2
#define RXFBCON3_F6BP_3		RXFBCON3BITS.F6BP_3
#define RXFBCON3_F7BP_0		RXFBCON3BITS.F7BP_0
#define RXFBCON3_F7BP_1		RXFBCON3BITS.F7BP_1
#define RXFBCON3_F7BP_2		RXFBCON3BITS.F7BP_2
#define RXFBCON3_F7BP_3		RXFBCON3BITS.F7BP_3

#define RXFBCON4_F8BP_0		RXFBCON4bits.F8BP_0
#define RXFBCON4_F8BP_1		RXFBCON4bits.F8BP_1
#define RXFBCON4_F8BP_2		RXFBCON4bits.F8BP_2
#define RXFBCON4_F8BP_3		RXFBCON4bits.F8BP_3
#define RXFBCON4_F9BP_0		RXFBCON4bits.F9BP_0
#define RXFBCON4_F9BP_1		RXFBCON4bits.F9BP_1
#define RXFBCON4_F9BP_2		RXFBCON4bits.F9BP_2
#define RXFBCON4_F9BP_3		RXFBCON4bits.F9BP_3

#define RXFBCON5_F10BP_0	RXFBCON5bits.F10BP_0
#define RXFBCON5_F10BP_1	RXFBCON5bits.F10BP_1
#define RXFBCON5_F10BP_2	RXFBCON5bits.F10BP_2
#define RXFBCON5_F10BP_3	RXFBCON5bits.F10BP_3
#define RXFBCON5_F11BP_0	RXFBCON5bits.F11BP_0
#define RXFBCON5_F11BP_1	RXFBCON5bits.F11BP_1
#define RXFBCON5_F11BP_2	RXFBCON5bits.F11BP_2
#define RXFBCON5_F11BP_3	RXFBCON5bits.F11BP_3

#define RXFBCON6_F12BP_0	RXFBCON6bits.F12BP_0
#define RXFBCON6_F12BP_1	RXFBCON6bits.F12BP_1
#define RXFBCON6_F12BP_2	RXFBCON6bits.F12BP_2
#define RXFBCON6_F12BP_3	RXFBCON6bits.F12BP_3
#define RXFBCON6_F13BP_0	RXFBCON6bits.F13BP_0
#define RXFBCON6_F13BP_1	RXFBCON6bits.F13BP_1
#define RXFBCON6_F13BP_2	RXFBCON6bits.F13BP_2
#define RXFBCON6_F13BP_3	RXFBCON6bits.F13BP_3

#define RXFBCON7_F14BP_0	RXFBCON7bits.F14BP_0
#define RXFBCON7_F14BP_1	RXFBCON7bits.F14BP_1
#define RXFBCON7_F14BP_2	RXFBCON7bits.F14BP_2
#define RXFBCON7_F14BP_3	RXFBCON7bits.F14BP_3
#define RXFBCON7_F15BP_0	RXFBCON7bits.F15BP_0
#define RXFBCON7_F15BP_1	RXFBCON7bits.F15BP_1
#define RXFBCON7_F15BP_2	RXFBCON7bits.F15BP_2
#define RXFBCON7_F15BP_3	RXFBCON7bits.F15BP_3

#define B0CON_TXREQ			B0CONbits.TXREQ
#define B1CON_TXREQ			B1CONbits.TXREQ
#define B2CON_TXREQ			B2CONbits.TXREQ
#define B3CON_TXREQ			B3CONbits.TXREQ
#define B4CON_TXREQ			B4CONbits.TXREQ
#define B5CON_TXREQ			B5CONbits.TXREQ

#define B0CON_RXM1			B0CONbits.RXM1
#define B1CON_RXM1			B1CONbits.RXM1
#define B2CON_RXM1			B2CONbits.RXM1
#define B3CON_RXM1			B3CONbits.RXM1
#define B4CON_RXM1			B4CONbits.RXM1
#define B5CON_RXM1			B5CONbits.RXM1

#define B0CON_RXFUL			B0CONbits.RXFUL
#define B1CON_RXFUL			B1CONbits.RXFUL
#define B2CON_RXFUL			B2CONbits.RXFUL
#define B3CON_RXFUL			B3CONbits.RXFUL
#define B4CON_RXFUL			B4CONbits.RXFUL
#define B5CON_RXFUL			B5CONbits.RXFUL

#define RXM0SIDL_EXIDEN		RXM0SIDLbits.EXIDEN
#define RXM1SIDL_EXIDEN		RXM1SIDLbits.EXIDEN

#define ECANCON_MDSEL1		ECANCONbits.MDSEL1
#define ECANCON_MDSEL0		ECANCONbits.MDSEL0




#elif defined(HITECH_C18)

/**
 * @defgroup HITECHC18ECANLibrary
 * HiTech C18 ECAN Library
 */

#include <pic18.h>

#define COMSTAT_TXB0		TXB0
#define COMSTAT_TXBP		TXBP
#define COMSTAT_RXBP		RXBP
#define CANCON_ABAT			ABAT
#define RXB0CON_RXFUL		RXB0FUL
#define TXB0CON_TXREQ		TXB0REQ
#define RXB0CON_RXM1		RXB0M1
#define RXB0CON_RXM0		RXB0M0

#define BSEL0_B0TXEN		B0TXEN  // configure buffer in Transmit=1 or receive mode=0
#define BSEL0_B1TXEN		B1TXEN
#define BSEL0_B2TXEN		B2TXEN
#define BSEL0_B3TXEN		B3TXEN
#define BSEL0_B4TXEN		B4TXEN
#define BSEL0_B5TXEN		B5TXEN

#define RXFCON0_RXF0EN		RXF0EN
#define RXFCON0_RXF1EN		RXF1EN
#define RXFCON0_RXF2EN		RXF2EN
#define RXFCON0_RXF3EN		RXF3EN
#define RXFCON0_RXF4EN		RXF4EN
#define RXFCON0_RXF5EN		RXF5EN
#define RXFCON0_RXF6EN		RXF6EN
#define RXFCON0_RXF7EN		RXF7EN

#define RXFCON1_RXF8EN		XF8EN
#define RXFCON1_RXF9EN		XF9EN
#define RXFCON1_RXF10EN		RXF10EN
#define RXFCON1_RXF11EN		RXF11EN
#define RXFCON1_RXF12EN		RXF12EN
#define RXFCON1_RXF13EN		RXF13EN
#define RXFCON1_RXF14EN		RXF14EN
#define RXFCON1_RXF15EN		RXF15EN

#define RXFBCON0_F0BP_0		F0BP_01
#define RXFBCON0_F0BP_1		F0BP_1
#define RXFBCON0_F0BP_2		F0BP_2
#define RXFBCON0_F0BP_3		F0BP_3
#define RXFBCON0_F1BP_0		F1BP_0
#define RXFBCON0_F1BP_1		F1BP_1
#define RXFBCON0_F1BP_2		F1BP_2
#define RXFBCON0_F1BP_3		F1BP_3

#define RXFBCON1_F2BP_0		F2BP_01
#define RXFBCON1_F2BP_1		F2BP_1
#define RXFBCON1_F2BP_2		F2BP_2
#define RXFBCON1_F2BP_3		F2BP_3
#define RXFBCON1_F3BP_0		F3BP_0
#define RXFBCON1_F3BP_1		F3BP_1
#define RXFBCON1_F3BP_2		F3BP_2
#define RXFBCON1_F3BP_3		F3BP_3

#define RXFBCON2_F4BP_0		F4BP_01
#define RXFBCON2_F4BP_1		F4BP_1
#define RXFBCON2_F4BP_2		F4BP_2
#define RXFBCON2_F4BP_3		F4BP_3
#define RXFBCON2_F5BP_0		F5BP_0
#define RXFBCON2_F5BP_1		F5BP_1
#define RXFBCON2_F5BP_2		F5BP_2
#define RXFBCON2_F5BP_3		F5BP_3

#define RXFBCON3_F6BP_0		F6BP_01
#define RXFBCON3_F6BP_1		F6BP_1
#define RXFBCON3_F6BP_2		F6BP_2
#define RXFBCON3_F6BP_3		F6BP_3
#define RXFBCON3_F7BP_0		F7BP_0
#define RXFBCON3_F7BP_1		F7BP_1
#define RXFBCON3_F7BP_2		F7BP_2
#define RXFBCON3_F7BP_3		F7BP_3

#define RXFBCON4_F8BP_0		F8BP_01
#define RXFBCON4_F8BP_1		F8BP_1
#define RXFBCON4_F8BP_2		F8BP_2
#define RXFBCON4_F8BP_3		F8BP_3
#define RXFBCON4_F9BP_0		F9BP_0
#define RXFBCON4_F9BP_1		F9BP_1
#define RXFBCON4_F9BP_2		F9BP_2
#define RXFBCON4_F9BP_3		F9BP_3

#define RXFBCON5_F10BP_0	F10BP_01
#define RXFBCON5_F10BP_1	F10BP_1
#define RXFBCON5_F10BP_2	F10BP_2
#define RXFBCON5_F10BP_3	F10BP_3
#define RXFBCON5_F11BP_0	F11BP_0
#define RXFBCON5_F11BP_1	F11BP_1
#define RXFBCON5_F11BP_2	F11BP_2
#define RXFBCON5_F11BP_3	F11BP_3

#define RXFBCON6_F12BP_0	F12BP_01
#define RXFBCON6_F12BP_1	F12BP_1
#define RXFBCON6_F12BP_2	F12BP_2
#define RXFBCON6_F12BP_3	F12BP_3
#define RXFBCON6_F13BP_0	F13BP_0
#define RXFBCON6_F13BP_1	F13BP_1
#define RXFBCON6_F13BP_2	F13BP_2
#define RXFBCON6_F13BP_3	F13BP_3

#define RXFBCON7_F14BP_0	F14BP_01
#define RXFBCON7_F14BP_1	F14BP_1
#define RXFBCON7_F14BP_2	F14BP_2
#define RXFBCON7_F14BP_3	F14BP_3
#define RXFBCON7_F15BP_0	F15BP_0
#define RXFBCON7_F15BP_1	F15BP_1
#define RXFBCON7_F15BP_2	F15BP_2
#define RXFBCON7_F15BP_3	F15BP_3

#define B0CON_TXREQ			B0TXREQ
#define B1CON_TXREQ			B1TXREQ
#define B2CON_TXREQ			B2TXREQ
#define B3CON_TXREQ			B3TXREQ
#define B4CON_TXREQ			B4TXREQ
#define B5CON_TXREQ			B5TXREQ

#define B0CON_RXFUL			B0RXFUL
#define B1CON_RXFUL			B1RXFUL
#define B2CON_RXFUL			B2RXFUL
#define B3CON_RXFUL			B3RXFUL
#define B4CON_RXFUL			B4RXFUL
#define B5CON_RXFUL			B5RXFUL

#define B0CON_RXM1			B0RXM1
#define B1CON_RXM1			B1RXM1
#define B2CON_RXM1			B2RXM1
#define B3CON_RXM1			B3RXM1
#define B4CON_RXM1			B4RXM1
#define B5CON_RXM1			B5RXM1

#define RXB1CON_RXFUL		RXB1RXFUL
#define TXB1CON_TXREQ		TXB1REQ
#define TXB2CON_TXREQ		TXB2REQ

#define RXM0SIDL_EXIDEN		RXM0EXIDM
#define RXM1SIDL_EXIDEN		RXM1EXIDEN

#define ECANCON_MDSEL1		MDSEL1
#define ECANCON_MDSEL0		MDSEL0

#define COMSTAT_FIFOEMPTY	FIFOEMPTY
#define COMSTAT_RXBnOVFL	RXB1OVFL
#define COMSTAT_RXB0OVFL	RXB0OVFL
#define COMSTAT_RXB1OVFL	RXB1OVFL

#define BRGCON2_SEG2PHT		SEG2PHT
#define BRGCON2_SAM			SAM
#define BRGCON3_WAKDIS		WAKDIS
#define BRGCON3_WAKFIL		WAKFIL

#define CIOCON_ENDRHI		ENDRHI
#define CIOCON_CANCAP		CANCAP

#define PIR3_RXBnIF			RXB1IF
#define PIR3_IRXIF			IRXIF
#define PIR3_RXB0IF			RXB0IF
#define PIR3_RXB1IF			RXB1IF

#define RXB0CON_FILHIT0		RXB0FILHIT0

#define RXB0CON_RXB0DBEN	RXBODBEN
static volatile near bit RXBODBEN   @ ((unsigned)&RXB0CON*8)+2;

#else
	#error "ECAN: Unknown compiler!!!"
#endif




/*********************************************************************
 *
 * General purpose typedef's
 *
 * Remove these definations if they are already defined in one of your
 * application files.
 ********************************************************************/
typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
typedef unsigned char BYTE;

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    BYTE Val;
} BYTE_VAL;


#define ECAN_LIB_MODE_FIXED     0
#define ECAN_LIB_MODE_RUN_TIME  1

#define ECAN_TX2_MODE_DISABLE   0
#define ECAN_TX2_MODE_ENABLE    1

    #define ECAN_INIT_NORMAL            0x00
    #define ECAN_INIT_LOOPBACK          0x40
    #define ECAN_INIT_CONFIGURATION     0x80
    #define ECAN_INIT_DISABLE           0x20
    #define ECAN_INIT_LISTEN_ONLY       0X60

#define ECAN_RXFn_ENABLE        1
#define ECAN_RXFn_DISABLE       0


// Enable auto RTR code when at least one buffer is configured in
// auto RTR mode.
#if (ECAN_FUNC_MODE_VAL != MODE_0 )
    #if ( (ECAN_B0_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B1_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B2_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B3_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B4_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B5_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) )
        #define ECAN_ENABLE_AUTO_RTR
    #endif
#endif


/*********************************************************************
 * Function:        void ECANInitialize(void)
 *
 * Overview:        Use this function to initialize ECAN module with
 *                  options defined in ECAN.def file.
 *                  You may manually edit ECAN.def file as per your
 *                  requirements, or use Microchip Application
 *                  Maestro tool.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    All pending transmissions are aborted.
 ********************************************************************/
void ECANInitialize(void);


/*********************************************************************
 *
 * ECAN_TX_MSG_FLAGS
 *
 * This enumeration values define flags related to transmission of a
 * CAN message.  There could be more than one this flag
 * ORed together to form multiple flags.
 *
 *********************************************************************/
typedef enum _ECAN_TX_MSG_FLAGS
 {
    ECAN_TX_PRIORITY_BITS    = 0b00000011,
    ECAN_TX_PRIORITY_0       = 0b00000000,
    ECAN_TX_PRIORITY_1       = 0b00000001,
    ECAN_TX_PRIORITY_2       = 0b00000010,
    ECAN_TX_PRIORITY_3       = 0b00000011,

    ECAN_TX_FRAME_BIT        = 0b00100000,
    ECAN_TX_STD_FRAME        = 0b00000000,
    ECAN_TX_XTD_FRAME        = 0b00100000,

    ECAN_TX_RTR_BIT          = 0b01000000,
    ECAN_TX_NO_RTR_FRAME     = 0b00000000,
    ECAN_TX_RTR_FRAME        = 0b01000000
} ECAN_TX_MSG_FLAGS;


/*********************************************************************
 * Function:        BOOL ECANSendMessage(unsigned long id,
 *                                      BYTE *data,
 *                                      BYTE dataLen,
 *                                      ECAN_TX_MSG_FLAGS msgFlags)
 *
 * Overview:        Use this function to transmit a CAN message.
 *                  This function searches for empty transmit buffer
 *                  and loads it with given messages. Buffer is then
 *                  marked for ready to transmit.
 *
 * PreCondition:    None
 *
 * Input:           id          - CAN message identifier.
 *                                Only 11 or 29 bits may be used
 *                                depending on standard or extended
 *                                message type as specified in
 *                                msgFlags parameter.
 *                  data        - Data bytes of upto 8 bytes in length
 *                  dataLen     - Data length from 0 thru 8.
 *                                If 0, data may be NULL.
 *                  msgFlags    - One or ECAN_TX_MSG_FLAGS values ORed
 *                                together
 *
 * Output:          TRUE, if an empty buffer was found and loaded with
 *                  given data
 *                  FALSE, if otherwise.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOL ECANSendMessage( unsigned long id,
                     BYTE *data,
                     BYTE dataLen,
                     ECAN_TX_MSG_FLAGS msgFlags);

/*********************************************************************
 * Function:        BOOL ECANLoadRTRBuffer(BYTE buffer,
 *                                         unsigned long id,
 *                                         BYTE *data,
 *                                         BYTE dataLen,
 *                                         BYTE type)
 *
 * Overview:        Use this function to update automatic RTR buffer.
 *
 *
 * PreCondition:    None
 *
 * Input:           buffer      - Buffer number that is to be loaded
 *                  id          - CAN message identifier.
 *                                Only 11 or 29 bits may be used
 *                                depending on standard or extended
 *                                message type as specified in
 *                                type parameter.
 *                  data        - Data bytes of upto 8 bytes in length
 *                  dataLen     - Data length from 0 thru 8.
 *                                If 0, data may be NULL.
 *                  type        - Buffer type
 *                                Must be ECAN_MSG_STD for Standard
 *                                        ECAN_MSG_XTD for Extended
 *
 * Output:          TRUE, if given data was successfully loaded
 *                  FALSE, if RTR buffer was in process of transmitting
 *                          automated response.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#if ( defined(ECAN_ENABLE_AUTO_RTR) )
    BOOL ECANLoadRTRBuffer(BYTE buffer,
                           unsigned long id,
                           BYTE *data,
                           BYTE dataLen,
                           BYTE type);
#endif


/*********************************************************************
 *
 * ECAN_RX_MSG_FLAGS
 *
 * This enumeration values define flags related to reception of a ECAN
 * message.  There could be more than one this flag
 * ORed together to form multiple flags.
 * If a particular bit is set, corresponding meaning is TRUE or else
 * it will be FALSE.
 *
 * e.g.
 *      if (msgFlag & ECAN_RX_OVERFLOW)
 *      {
 *          // Receiver overflow has occured.
 *          ...
 *      }
 *
 ********************************************************************/
typedef enum _ECAN_RX_MSG_FLAGS
{
    ECAN_RX_OVERFLOW     = 0b00001000,
    ECAN_RX_INVALID_MSG  = 0b00010000,
    ECAN_RX_XTD_FRAME    = 0b00100000,
    ECAN_RX_RTR_FRAME    = 0b01000000,
    ECAN_RX_DBL_BUFFERED = 0b10000000
} ECAN_RX_MSG_FLAGS;

/*********************************************************************
 * Function:        BOOL ECANReceiveMessage(unsigned long *id,
 *                                  BYTE *data,
 *                                  BYTE *dataLen,
 *                                  ECAN_RX_MSG_FLAGS *msgFlags)
 *
 * Overview:        Use this function to check for full receive buffer
 *                  and extract received data into local buffers.
 *
 * PreCondition:    None
 *
 * Input:           id          - Pointer to buffer that will be
 *                                populated with receive ID.
 *                  data        - Pointer to buffer that will be
 *                                populated with data if there is any
 *                  dataLen     - Pointer to buffer that will be
 *                                populated with count of bytes
 *                                copied in data buffer.
 *                  msgFlags    - Pointer to buffer that will be
 *                                copied with information about
 *                                received message. More than
 *                                one information is ORed together.
 *
 * Output:          TRUE, if a full receive buffer was found and
 *                  given parameters were populated.
 *                  FALSE, if otherwise.
 *
 * Side Effects:    None
 *
 * Note:            If you need to know the filter number that caused
 *                  this message to get accepted, call
 *                  ECANGetFilterHitInfo().
 *
 ********************************************************************/
BOOL ECANReceiveMessage( unsigned long* id,
                        BYTE *Data,
                        BYTE *DataLen,
                        ECAN_RX_MSG_FLAGS *MsgFlags);


/*********************************************************************
 * Macro:           BYTE ECANGetFilterHitInfo(void)
 *
 * Overview:        Use this macro to extract filter number that
 *                  caused a message to get accepted.
 *                  You must call this macro immediately after calling
 *                  ECANReceiveMessage function.
 *
 * PreCondition:    ECANReceiveMessage is called and returned TRUE
 *
 * Input:           None
 *
 * Output:          Number of filter that caused acceptance.
 *                  0 means RXF0, 1 means RXF1 and so on.
 *
 * Side Effects:    None
 *
 * Note:            This macro will return filter hit information
 *                  for previously received message only.
 *
 ********************************************************************/
#define ECANGetFilterHitInfo()          (_ECANRxFilterHitInfo.Val)
    extern BYTE_VAL _ECANRxFilterHitInfo;


/*********************************************************************
 *
 * ECAN_OP_MODE
 *
 * This enumeration values define codes related to ECAN module
 * operation mode. ECANSetOperationMode() routine requires this code.
 * These values must be used by itself
 * i.e. it cannot be ORed to form * multiple values.
 *
 ********************************************************************/
typedef enum _ECAN_OP_MODE
{
    ECAN_OP_MODE_BITS    = 0xe0,   // Use this to access opmode bits
    ECAN_OP_MODE_NORMAL  = 0x00,
    ECAN_OP_MODE_SLEEP   = 0x20,
    ECAN_OP_MODE_LOOP    = 0x40,
    ECAN_OP_MODE_LISTEN  = 0x60,
    ECAN_OP_MODE_CONFIG  = 0x80
} ECAN_OP_MODE;

/*********************************************************************
 * Function:        void ECANSetOperationMode(ECAN_OP_MODE mode)
 *
 * Overview:        Use this function to switch ECAN module into
 *                  given operational mode.
 *                  You may not need to call this function if
 *                  your application does not require run-time
 *                  changes in ECAN mode.
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type ECAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a blocking call.  It will not return until
 *                  requested mode is set.
 ********************************************************************/
void ECANSetOperationMode(ECAN_OP_MODE mode);

/*********************************************************************
 * Macro:           void ECANSetOperationModeNoWait(ECAN_OP_MODE mode)
 *
 * Overview:        Use this macro to request operation mode but
 *                  do not want macro to wait for it get accepted.
 *                  You must use ECANGetOperationMode() to esnure
 *                  requested operation mode is accepted before
 *                  performing operation critical steps.
 *                  You may not need to call this macro if your
 *                  application does not require run-time changes
 *                  in ECAN mode.
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type enum ECAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a non-blocking call.
 *                  It does not verify that
 *                  ECAN module is switched to requested mode or not.
 *                  Caller must use ECANGetOperationMode() to verify
 *                  correct operation mode before performing mode
 *                  specific operation.
 *
 ********************************************************************/
#define ECANSetOperationModeNoWait(mode)     CANCON = mode

/*********************************************************************
 * Macro:           ECAN_OP_MODE ECANGetOperationMode()
 *
 * Overview:        Use this macro to obtain current operation mode
 *                  ECAN module.
 *                  You may not need to call this macro if your
 *                  application does not require run-time changes.
 * PreCondition:
 *
 * Input:
 *
 * Output:          Current operational mode of ECAN module is returned
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANGetOperationMode() (CANSTAT & ECAN_OP_MODE_BITS)


/*********************************************************************
 * Macro:           ECANSetFunctionalMode(mode)
 *
 * Overview:        Use this macro to set functional mode of ECAN.
 *                  You may not need to call this macro if
 *                  your application will not change mode at run-time.
 *
 * PreCondition:    ECAN must be in configuration mode
 *                  and ECAN_LIB_MODE_VAL = ECAN_LIB_MODE_RUN_TIME
 *
 * Input:           mode        - New mode
 *                                Allowable values are
 *                                  ECAN_MODE_0 for Mode 0
 *                                  ECAN_MODE_1 for Mode 1
 *                                  ECAN_MODE_2 for Mode 2
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    #define ECANSetFunctionalMode(mode)      \
            ECANCON_MDSEL1 = mode >> 7; \
            ECANCON_MDSEL0 = mode >> 6;
#endif

    #define ECAN_MODE_0     0x00
    #define ECAN_MODE_1     0x40
    #define ECAN_MODE_2     0x80


/*********************************************************************
 * Macro:           BYTE ECANGetFunctionalMode()
 *
 * Overview:        Use this macro to get functional mode of ECAN.
 *                  You may not need to call this macro if
 *                  your application will not change mode at run-time.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          mode    -   ECAN_MODE_0
 *                              ECAN_MODE_1
 *                              ECAN_MODE_2
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANGetFunctionalMode()      (ECANCON & 0b11000000)


/*********************************************************************
 * Macro:           ECANSetBusSampleMode(mode)
 *
 * Overview:        Use this macro to set CAN bus sample mode.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change
 *                  to sampling mode.
 *
 * PreCondition:    ECAN must be in configuration mode
 *
 * Input:           mode        - Bus sample mode
 *                     Allowable values are
 *                      ECAN_BUS_SAMPLE_MODE_ONCE to sample once
 *                      ECAN_BUS_SAMPLE_THRICE to sample thrice.
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetBusSampleMode(mode)   BRGCON2_SAM = mode

    #define ECAN_BUS_SAMPLE_MODE_ONCE       0
    #define ECAN_BUS_SAMPLE_MODE_THRICE     1



/*********************************************************************
 * Macro:           ECANSetWakeupMode(mode)
 *
 * Overview:        Use this macro to set CAN bus activity bus mode.
 *                  You may not need to call this macro if your
 *                  application does not require run-time changes.
 *
 * PreCondition:    ECAN must be in configuration mode
 *
 * Input:           mode        - CAN Bus activity wakeup mode
 *                     Allowable values are
 *                      ECAN_WAKEUP_MODE_ENABLE to enable bus wakeup
 *                      ECAN_WAKEUP_MODE_DISABLE to disable bus wakeup
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetWakeupMode(mode)      BRGCON3_WAKDIS = mode

    #define ECAN_WAKEUP_MODE_ENABLE         0
    #define ECAN_WAKEUP_MODE_DISABLE        1


/*********************************************************************
 * Macro:           ECANSetFilterMode(mode)
 *
 * Overview:        Use this macro to CAN wakeup low pass filter mode.
 *                  You may not need to call this macro if your application
 *                  does not require run-time changes.
 *
 * PreCondition:    ECAN must be in configuration mode
 *
 * Input:           mode        - CAN wakeup filter mode
 *                     Allowable values are
 *                      ECAN_FILTER_MODE_DISABLE to not use wakeup filter
 *                      ECAN_FILTER_MODE_ENABLE to use wakeup filter
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetFilterMode(mode)      BRGCON3_WAKFIL = mode

    #define ECAN_FILTER_MODE_DISABLE        0
    #define ECAN_FILTER_MODE_ENABLE         1

/*********************************************************************
 * Macro:           ECANSetTxDriveMode(mode)
 *
 * Overview:        Use this macro to set CANTX pin drive options.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           mode        - CANTX drive mode when trnasmitting
 *                                recessive bit.
 *                     Allowable values are
 *                      ECAN_TXDRIVE_MODE_TRISTATE to drive tri-state
 *                      ECAN_TXDRIVE_MODE_VDD to drive to Vdd
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetTxDriveMode(mode)     CIOCON_ENDRHI = mode

    #define ECAN_TXDRIVE_MODE_TRISTATE  0
    #define ECAN_TXDRIVE_MODE_VDD       1


/*********************************************************************
 * Macro:           ECANSetCANTX2Source(source)
 *
 * Overview:        Use this macro to set CANTX2 pin source.
 *                  This macro automatically enables CANTX2 pin.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           source      - CANTX2 source
 *                     Allowable values are
 *                      ECAN_TX2_CAN_CLOCK to output CAN clock on TX2
 *                      ECAN_TX2_CAN_DATA to output complete CAN
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetCANTX2Mode(mode)         CIOCON_TX2SRC = mode << 7; \
                                        CIOCON_TX2EN = 1
    #define ECAN_TX2_SOURCE_COMP    0
    #define ECAN_TX2_SOURCE_CLOCK   1


/*********************************************************************
 * Macro:           ECANDisableCANTX2()
 *
 * Overview:        Use this macro to disable CANTX2 pin.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANDisableCANTX2()              CIOCON_TX2EN = 0


/*********************************************************************
 * Macro:           ECANSetCaptureMode(mode)
 *
 * Overview:        Use this macro to set CAN CAPTURE mode.  This is
 *                  mainly used to timestamp incoming CAN messages.
 *                  You would also need to setup and enable CCP module
 *                  to obtain timeestamp information.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           mode        - Capture mode
 *                                Allowable values are
 *                                  ECAN_CAPTURE_MODE_DISABLE
 *                                  ECAN_CAPTURE_MODE_ENABLE
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetCaptureMode(mode)     CIOCON_CANCAP = mode

    #define ECAN_CAPTURE_MODE_DISABLE   0
    #define ECAN_CAPTURE_MODE_ENABLE    1

/*********************************************************************
 * Macro:           ECANSetPHSEG2Mode(mode)
 *
 * Overview:        Use this macro to set PHSEG2 mode.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           mode        - PHSEG2 Mode
 *                                Allowable values are
 *                    ECAN_PHSEG2_MODE_AUTOMATIC
 *                      - Max. of PHSEG1 of IPT, whichever is greater
 *                    ECAN_PHSEG2_MODE_PROGRAMMABLE
 *                      - Freely programmable
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetPHSEG2Mode(mode)      BRGCON2_SEG2PHTS = mode

    #define ECAN_PHSEG2_MODE_AUTOMATIC      0
    #define ECAN_PHSEG2_MODE_PROGRAMMABLE   1


/*********************************************************************
 * Macro:           ECANSetB0AutoRTRMode(mode)
 *                  ECANSetB1AutoRTRMode(mode)
 *                  ECANSetB2AutoRTRMode(mode)
 *                  ECANSetB3AutoRTRMode(mode)
 *                  ECANSetB4AutoRTRMode(mode)
 *                  ECANSetB5AutoRTRMode(mode)
 *
 * Overview:        Use these macros to set automatic RTR handling
 *                  mode for given programmable buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    ECAN_LIB_MODE_VAL = ECAN_LIB_MODE_RUN_TIME
 *                  ECAN_FUNC_MODE_VAL != ECAN_MODE_0
 *
 * Input:           mode        - AutoRTR mode
 *                  Allowable values are
 *                    ECAN_AUTORTR_MODE_DISABLE
 *                      - To disable automatic RTR handling
 *                    ECAN_AUTORTR_MODE_ENABLE
 *                      - To enable automatic RTR handling
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANSetB0AutoRTRMode(mode)      B0CON_RTREN = mode;     \
                                            BSEL0_B0TXEN = 1
    #define ECANSetB1AutoRTRMode(mode)      B1CON_RTREN = mode;     \
                                            BSEL0_B1TXEN = 1
    #define ECANSetB2AutoRTRMode(mode)      B2CON_RTREN = mode;     \
                                            BSEL0_B2TXEN = 1
    #define ECANSetB3AutoRTRMode(mode)      B3CON_RTREN = mode;     \
                                            BSEL0_B3TXEN = 1
    #define ECANSetB4AutoRTRMode(mode)      B4CON_RTREN = mode;     \
                                            BSEL0_B4TXEN = 1
    #define ECANSetB5AutoRTRMode(mode)      B5CON_RTREN = mode;     \
                                            BSEL0_B5TXEN = 1
#endif

    #define ECAN_AUTORTR_MODE_DISABLE   0
    #define ECAN_AUTORTR_MODE_ENABLE    1



/*********************************************************************
 * Macro:           ECANSetRXMnValue(val, type)
 *
 * Overview:        Use these macros to set specific mask value.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    ECAN must be in Configuration mode
 *
 * Input:           val     - Value to be set
 *                      Actual value would depend on type.
 *                      If type is Standard, value will be 11-bit long
 *                      If type is Extended, value will be 29-bit long
 *                  type    - Mask type
 *                      Allowable values are
 *                          ECAN_MSG_STD for Standard type
 *                          ECAN_MSG_XTD for Extended type
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *
 * Overview:        This function sets mask value for given mask number.
 *                  Type of mask, whether standard or extended is defined
 *                  by type parameter.
 *
 * Note:            The MASK_RXF15 is only available in enhanced and
 *                  FIFO modes
 ********************************************************************/
#define ECANSetRXM0Value(val, type)       \
                    _CANIDToRegs((BYTE*)&RXM0SIDH, val, type)
#define ECANSetRXM1Value(val, type)       \
                    _CANIDToRegs((BYTE*)&RXM1SIDH, val, type)




/*********************************************************************
 * Macro:           ECANSetRXFnValue(val, type)
 *
 * Overview:        Use these macros to set receive filter values.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    ECAN must be in Configuration mode
 *                  To set RXF6-RXF15 values,
 *                  ECAN_LIB_MODE_VAL = ECAN_LIB_MODE_RUN_TIME OR
 *                  ECAN_FUNC_MODE != ECAN_MODE_0
 *
 * Input:           val         - Value to be set
 *                  type        - Type of filter
 *                                Allowable values are
 *                                  ECAN_MSG_STD if this is Standard filter
 *                                  ECAN_MSG_XTD if this is Extended filter
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Use this macro to assign value to a receive filter.
 *
 * Note:            There are total of sixteen macros - one for each
 *                  receive filter. e.g. for RXF0 use ECANSetRXF0Value()
 *                  for RXF2 use ECANSetRXF2Value()
 ********************************************************************/
#define ECANSetRXF0Value(val, type)       \
    RXFCON0_RXF0EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF0SIDH, val, type)

#define ECANSetRXF1Value(val, type)       \
    RXFCON0_RXF1EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF1SIDH, val, type);

#define ECANSetRXF2Value(val, type)       \
    RXFCON0_RXF2EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF2SIDH, val, type);

#define ECANSetRXF3Value(val, type)       \
    RXFCON0_RXF3EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF3SIDH, val, type);

#define ECANSetRXF4Value(val, type)       \
    RXFCON0_RXF4EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF4SIDH, val, type);

#define ECANSetRXF5Value(val, type)       \
    RXFCON0_RXF5EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF5SIDH, val, type);

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANSetRXF6Value(val, type)       \
        RXFCON0_RXF6EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF6SIDH, val, type);

    #define ECANSetRXF7Value(val, type)       \
        RXFCON0_RXF7EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF7SIDH, val, type);

    #define ECANSetRXF8Value(val, type)       \
        RXFCON1_RXF8EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF8SIDH, val, type);

    #define ECANSetRXF9Value(val, type)       \
        RXFCON1_RXF9EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF9SIDH, val, type);

    #define ECANSetRXF10Value(val, type)       \
        RXFCON1_RXF10EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF10SIDH, val, type);

    #define ECANSetRXF11Value(val, type)       \
        RXFCON1_RXF11EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF11SIDH, val, type);

    #define ECANSetRXF12Value(val, type)       \
        RXFCON1_RXF12EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF12SIDH, val, type);

    #define ECANSetRXF13Value(val, type)       \
        RXFCON1_RXF13EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF13SIDH, val, type);

    #define ECANSetRXF14Value(val, type)       \
        RXFCON1_RXF14EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF14SIDH, val, type);

    #define ECANSetRXF15Value(val, type)       \
        RXFCON1_RXF15EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF15SIDH, val, type);
#endif

    #define ECAN_MSG_STD    0
    #define ECAN_MSG_XTD    1


/*********************************************************************
 * Macro:           ECANSetBnTxRxMode(buffer, mode)
 *
 * Overview:        Use this macro to configure a programmable buffer (B0-B5)
 *                  as either transmit or receive buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    ECAN_LIB_MODE = ECAN_LIB_MODE_RUN_TIME OR
 *                  ECAN_FUNC_MODE_VAL != ECAN_MODE_0
 *
 * Input:           buffer      - Buffer that needs to be setup
 *                                Allowable values are:
 *                                      B0, B1, B2, B3, B4, B5
 *                  mode        - Mode to be set.
 *                                Allowable values are:
 *                                      ECAN_BUFFER_TX, ECAN_BUFFER_RX
 *
 * Output:          None
 *
 * Side Effects:
 *
 * Example:        // Set B0 as Transmit buffer
 *                  ECANSetBnTxRxMode(B0, ECAN_TX)
 *
 *                  // Set B2 as receive buffer
 *                  ECANSetBnTxRxMode(B2, ECAN_RX)
 *
 * Note:            Parameter buffer must be a constant symbol of either
 *                  B0, B1, B2, B3, B4 or B5.
 *                  A variable parameter would result in compiler error.
 *
 *                  e.g. ECANSetBnTxRxMode(myBuffer, ECAN_BUFFER_TX)
 *                       would not compile.
 *
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANSetBnTxRxMode(buffer, mode)          \
                            BSEL0_##buffer##TXEN = mode
#endif

    #define RXB0            0
    #define RXB1            1
    #define B0              2
    #define B1              3
    #define B2              4
    #define B3              5
    #define B4              6
    #define B5              7
    #define TXB0            8
    #define TXB1            9
    #define TXB2            10

    #define ECAN_BUFFER_RX  0
    #define ECAN_BUFFER_TX  1

/*********************************************************************
 * Macro:           ECANSetRXB0DblBuffer(mode)
 *
 * Overview:        Use this macro to configure RXB0 in double buffering mode
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    None
 *
 * Input:           mode        - Double buffer mode
 *                                Allowable values are
 *                                  ECAN_DBL_BUFFER_ENABLE
 *                                  ECAN_DBL_BUFFER_DISABLE,
 *
 * Output:          None
 *
 * Example:         // Enable Double buffering mode
 *                  ECANSetRXB0DblBuffer(ECAN_DBL_BUFFER_ENABLE)
 *
 *
 * Side Effects:
 *
 * Note:            None
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )

    #define ECANSetRXB0DblBuffer(mode)       RXB0CON_RXB0DBEN = mode
#endif

    #define ECAN_DBL_BUFFER_MODE_DISABLE    0
    #define ECAN_DBL_BUFFER_MODE_ENABLE     1


/*********************************************************************
 * Macro:           ECANSetRxBnRxMode(buffer, mode)
 *
 * Overview:        Use this macro to configure receive mode of a fixed receive buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    None
 *
 * Input:           buffer      - Buffer that needs to be configured
 *                                Allowable values are
 *                                  RXB0, RXB1
 *                  mode        - Mode to be setup.
 *                                Allowable values are
 *                                  ECAN_RECEIVE_ALL,
 *                                  ECAN_RECEIVE_STANDARD,
 *                                  ECAN_RECEIVE_EXTENDED,
 *                                  ECAN_RECEIVE_ALL_VALID
 *
 * Output:          None
 *
 * Example:         // Configure RXB0 buffer to receive all valid messages.
 *                  ECANSetRxBnRxMode(RXB0, ECAN_RECEIVE_ALL_VALID)
 *
 *                  // Configure RXB1 buffer to receive only Standard messages.
 *                  ECANSetRxBnRxMode(RXB1, ECAN_RECEIVE_STANDARD)
 *
 * Side Effects:
 *
 * Note:            Parameter buffer must be a constant symbol of either
 *                  RXB0 or RXB1.
 *                  A variable paraemter would result in compiler error.
 *
 *                  e.g. ECANSetRxBnTxRxMode(myBuffer, ECAN_RECEIVE_ALL_VALID)
 *                       would not compiler
 *
 ********************************************************************/
#define ECANSetRxBnRxMode(buffer, mode)      \
        buffer##CON_RXM1 = mode >> 1; \
        buffer##CON_RXM0 = mode;

    #define ECAN_RECEIVE_ALL_VALID  0
    #define ECAN_RECEIVE_STANDARD   1
    #define ECAN_RECEIVE_EXTENDED   2
    #define ECAN_RECEIVE_ALL        3



/*********************************************************************
 * Macro:           ECANSetBnRxMode(buffer, mode)
 *
 * Overview:        Use this macro to configure receive mode of a programmable
 *                  receive buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    None
 *
 * Input:           buffer      - Buffer that needs to be configured
 *                                Allowable values are
 *                                  B0, B1, B2, B3, B4, B5
 *                  mode        - Mode to be setup.
 *                                Allowable values are
 *                                  ECAN_RECEIVE_ALL,
 *                                  ECAN_RECEIVE_ALL_VALID
 *
 * Output:          None
 *
 * Example:         // Configure B0 buffer to receive all valid messages.
 *                  ECANSetBnRxMode(B0, ECAN_RECEIVE_ALL_VALID)
 *
 *                  // Configure B1 buffer to receive only Standard messages.
 *                  ECANSetBnRxMode(RXB1, ECAN_RECEIVE_STANDARD)
 *
 * Side Effects:
 *
 * Note 1:          Parameter buffer must be a constant symbol of either
 *                  B0, B1, B2, B3, B4 or B5.
 *                  A variable paraemter would result in compiler error.
 *
 *                  e.g. ECANSetBnTxRxMode(myBuffer, ECAN_RECEIVE_ALL_VALID)
 *                       would not compiler
 *
 *      2:          This macro should be used in Mode 1 or 2 only.
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )
    #define ECANSetBnRxMode(buffer, mode)      ##buffer##CON_RXM1 = mode

#endif

/*********************************************************************
 * Macro:           BYTE ECANGetTxErrorCount()
 *
 * Overview:        Use this macro to get current TXERRCNT value
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current transmit error count as defined by
 *                  CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANGetTxErrorCount()            (TXERRCNT)


/*********************************************************************
 * Macro:           BYTE ECANGetRxErrorCount()
 *
 * Overview:        Use this macro to get current RXERRCNT value.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current receive error count as defined by
 *                  CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANGetRxErrorCount()            (RXERRCNT)


/*********************************************************************
 * Macro:           BOOL ECANIsBusOff()
 *
 * Overview:        Use this macro to check if CAN module is in
 *                  Bus off condition.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN Module is off due to excessive error
 *                  FALSE is it is not off.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANIsBusOff()                   (COMSTAT_TXB0)


/*********************************************************************
 * Macro:           BOOL ECANIsTxPassive()
 *
 * Overview:        Use this macro to check if CAN module is in
 *                  Tx passive mode.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN transmit module is error passive as
 *                  defined by CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANIsTxPassive()                (COMSTAT_TXBP)


/*********************************************************************
 * Macro:           BYTE ECANIsRxPassive()
 *
 * Overview:        Use this macro to check if CAN is in Rx passive.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN receive module is error active as
 *                  defined by CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANIsRxPassive()                (COMSTAT_RXBP)


/*********************************************************************
 * Macro:           void ECANAbortAll()
 *
 * Overview:        Use this macro to request abort for all tx buffers.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANAbortAll()                   (CANCON_ABAT = 1)

/*********************************************************************
 * Macro:           BOOL ECANIsAllAborted()
 *
 * Overview:        Use this macro to see if previous ECANAbortAll
 *                  is complete or not.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: if there are no pending messages.
 *                  FALSE: if there abort is still in progress.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANIsAllAborted()               (CANCON_ABAT)


/*********************************************************************
 * Macro:           ECANLinkRXFnFmToBuffer(RXFnBuffer, RXFmBuffer)
 *                      n = { 0, 2, 4, 6, 8, 10, 12, 14 }
 *                      m = ( 1, 3, 5, 7, 9, 11, 13, 15 }
 *                      m = n + 1
 *
 * Overview:        Use this macro to link RXFn and RXFm to
 *                  any of RXB0, RXB1, B0, B1, B2, B3, B4, B5 buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    ECAN must be in Configuration mode
 *                  Given buffer must be configured as receive buffer.
 *                      (See ECANSetRxBnTxRxMode and ECANSetBnTxRxMode)
 *
 *
 * Input:           RXFnBuffer  - Buffer that is to be linked
 *                                with RXFn filter
 *                  RXFmBuffer  - Buffer that is to be linked
 *                                with RXFm filter
 *
 *                                Allowable values for both parameters
 *                                  RXB0, RXB1, B0, B1, B2, B3, B4, B5
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *
 * Note:            If possible, call this macro with constant
 *                  parameters to reduce generated code.
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANLinkRXF0F1ToBuffer(RXF0Buffer, RXF1Buffer)     \
                RXFBCON0 = (RXF1Buffer << 4) | RXF0Buffer
    #define ECANLinkRXF2F3ToBuffer(RXF2Buffer, RXF3Buffer)     \
                RXFBCON1 = (RXF2Buffer << 4) | RXF3Buffer
    #define ECANLinkRXF4F5ToBuffer(RXF4Buffer, RXF5Buffer)     \
                RXFBCON2 = (RXF4Buffer << 4) | RXF5Buffer
    #define ECANLinkRXF6F7ToBuffer(RXF6Buffer, RXF7Buffer)     \
                RXFBCON3 = (RXF6Buffer << 4) | RXF7Buffer
    #define ECANLinkRXF8F9ToBuffer(RXF8Buffer, RXF9Buffer)     \
                RXFBCON4 = (RXF8Buffer << 4) | RXF9Buffer
    #define ECANLinkRXF10F11ToBuffer(RXF10Buffer, RXF11Buffer) \
                RXFBCON5 = (RXF10Buffer << 4) | RXF11Buffer
    #define ECANLinkRXF12F13ToBuffer(RXF12Buffer, RXF13Buffer) \
                RXFBCON6 = (RXF12Buffer << 4) | RXF13Buffer
    #define ECANLinkRXF14F15ToBuffer(RXF14Buffer, RXF15Buffer) \
                RXFBCON7 = (RXF14Buffer << 4) | RXF15Buffer
#endif


/*********************************************************************
 * Macro:           ECANLinkRXF0Thru3ToMask(m0, m1, m2, m3)
 *                  ECANLinkRXF4Thru7ToMask(m4, m5, m6, m7)
 *                  ECANLinkRXF8Thru11ToMask(m8, m9, m10, m11, m12)
 *                  ECANLinkRXF12Thru15ToMask(m13, m14, m15, m16)
 *
 * Overview:        Use this macro to link receive filters to masks.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    ECAN must be in Configuration mode
 *
 * Input:           mn  - Buffer that is to be linked
 *                                with Mask
 *
 *                                Allowable values for both parameters
 *                                  ECAN_RXM0, ECAN_RXM1, ECAN_RXMF15
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *
 * Note:            If possible, call this macro with constant
 *                  parameters to reduce generated code.
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANLinkRXF0Thru3ToMask(m0, m1, m2, m3) \
        MSEL0 = m3 << 6 | m2 << 4 | m1 << 2 | m0;
    #define ECANLinkRXF4Thru7ToMask(m4, m5, m6, m7) \
        MSEL1 = m7 << 6 | m6 << 4 | m5 << 2 | m4;
    #define ECANLinkRXF8Thru11ToMask(m8, m9, m10, m11) \
        MSEL2 = m11 << 6 | m10 << 4 | m9 << 2 | m8;
    #define ECANLinkRXF12Thru15ToMask(m12, m13, m14, m15) \
        MSEL2 = m15 << 6 | m14 << 4 | m13 << 2 | m12;
#endif

    #define ECAN_RXM0       0
    #define ECAN_RXM1       1
    #define ECAN_RXMF15     2


/*********************************************************************
 * Macro:           ECANSetBaudRate(sjw,
 *                                 brp,
 *                                 phseg1,
 *                                 phseg2,
 *                                 propseg)
 *
 * Overview:        Use this macro to change ECAN baud rate. Normally
 *                  you would call this macro only, if your application
 *                  needs run-time baud rate changes.
 *
 * PreCondition:    ECAN is in Configuration mode
 *
 * Input:           sjw     - SJW value of 1-4
 *                  brp     - BRP value of 1-64
 *                  phseg1  - PHSEG1 value of 1-8
 *                  phseg2  - PHSEG2 value of 1-8
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Note:            If possible, call this macro with constant values
 *                  to reduce generated code size.
 ********************************************************************/
#define ECANSetBaudRate(sjw, brp, phseg1, phseg2, propseg)           \
        BRGCON1 = ((sjw-1) << 6) | (brp-1);                         \
        BRGCON2 |= (((phseg1-1) << 3) | (propseg-1));               \
        BRGCON3 = phseg2;

/*
 * This is a helper function used by some macros. Normally, you would
 * not need to call this function directly.
 */
void _CANIDToRegs(BYTE* ptr,
                  unsigned long val,
                  BYTE type );



    /*
     * Following are special defs to overcome compiler problem
     * at the time of this re-write.
     * Set following line to "#if 0" after verifiying correct
     * compiler behavior.
     */
    #if 0
        static struct
        {
            unsigned : 7;
            unsigned RXFUL : 1;
        } RXB1CONbits @ 0xf50;
        #define RXB1CON_RXFUL       RXB1CONbits.RXFUL

        static struct
        {
            unsigned TXPRI0:1;
            unsigned TXPRI1:1;
            unsigned :1;
            unsigned TXREQ:1;
            unsigned TXERR:1;
            unsigned TXLARB:1;
            unsigned TXABT:1;
        } TXB1CONbits @ 0xf30;
        #define TXB1CON_TXREQ       TXB1CONbits.TXREQ

        static struct
        {
            unsigned TXPRI0:1;
            unsigned TXPRI1:1;
            unsigned :1;
            unsigned TXREQ:1;
            unsigned TXERR:1;
            unsigned TXLARB:1;
            unsigned TXABT:1;
        } TXB2CONbits @ 0xf20;
        #define TXB2CON_TXREQ       TXB2CONbits.TXREQ
    #else
        
    #endif




#endif //_ECAN_H

