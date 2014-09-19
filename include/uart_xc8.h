/*************************************************************************//**
 * @file usart.h
 * 
 * @brief USART library header.
 * 
 * @createdon{7/14/2010}
 * @createdby{Liam Bucci}
 * @projectname{PIC C Library}
 * @projecttype{PIC C Embedded Library}
 * @subprojectname{USART}
 * @subprojecttype{USART Library}
 * @subprojectdescription
 * Embedded library for PIC MCUs which sets and configures chip specific
 * USART settings depending on the project's "usart.def" file. Also, provides
 * functionality for input and output of bytes through the USART hardware
 * module and defines macros which can be used to access and control USART
 * settings and flags.
 * 
 * @pcbnum{N/A}
 * @pcbname{N/A}
 * @pcbdescription
 * N/A
 * 
 * @notes
 * 
 * @todo Add buffering of RX/TX.
 * @todo Incorporate synchronous mode RX/TX.
 * @todo Incorporate 9-bit RX/TX.
 ****************************************************************************/

#ifndef _USART_H
#define _USART_H


#if defined(__C30__) //Compiler Check

//Check for valid processor and include matching header
#if defined(__dsPIC33FJ128MC802__)
	#include <p33FJ128MC802.h>
#else
	#error "INTERRUPTS: Invalid/unknown processor for compiler (Microchip C30)!"
#endif //defined(__processor)

/**
 * @defgroup C30USARTLibrary C30 USART Library
 */

/**
 * @name UART1Settings
 * UART1 Settings
 * @{
 */
#define USART_UART1_EN()			U1MODEbits.UARTEN=1			/**< Enable UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS()			U1MODEbits.UARTEN=0			/**< Disable UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_IDLE()		U1MODEbits.USIDL=1			/**< Enable idle mode in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_IDLE()		U1MODEbits.USIDL=0			/**< Disable idle mode in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_IRDA()		U1MODEbits.IREN=1			/**< Enable IrDA encoder/decoder in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_IRDA()		U1MODEbits.IREN=0			/**< Disable IrDA encoder/decoder in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_RTS()		U1MODEbits.RTSMD=0			/**< Enable RTS hardware flow control in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_RTS()		U1MODEbits.RTSMD=1			/**< Disable RTS hardware flow control in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_MODE(x)			U1MODEbits.UEN=((x)&0x3)	/**< Set pin modes in UART1 module @param x Pin mode (0-3) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_WAKE()		U1MODEbits.WAKE=1			/**< Enable wake on start bit in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_WAKE()		U1MODEbits.WAKE=0			/**< Disable wake on start bit in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_LPBACK()		U1MODEbits.LPBACK=1			/**< Enable loopback mode in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_LPBACK()	U1MODEbits.LPBACK=0			/**< Disable loopback mode in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_AUTOBAUD()	U1MODEbits.ABAUD=1			/**< Enable automatic baud rate measurement on next byte in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_AUTOBAUD()	U1MODEbits.ABAUD=0			/**< Disable automatic baud rate measurement on next byte in UART1 module (or auto-baud finished) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_RXINVERT()	U1MODEbits.URXINV=0			/**< Enable RX polarity inversion (idle = '1') in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_RXINVERT()	U1MODEbits.URXINV=1			/**< Disable RX polarity inversion (idle = '0') in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_BRGHIGH()	U1MODEbits.BRGH=1			/**< Enable high baud rate generator (4 clocks per bit-period) in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_BRGHIGH()	U1MODEbits.BRGH=0			/**< Disable high baud rate generator (16 clocks per bit-period) in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_PARITY_DATA(x)	U1MODEbits.PDSEL=((x)&0x3)	/**< Set parity and data bit modes @param x Mode (0-3) UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_1STOP()			U1MODEbits.STSEL=0			/**< Use one stop bit in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_2STOP()			U1MODEbits.STSEL=1			/**< Use two stop bits in UART1 module @hideinitializer @ingroup C30USARTLibrary */
/**
 * @}
 */

/**
 * @name UART2Settings
 * UART2 Settings
 * @{
 */
#define USART_UART2_EN()			U2MODEbits.UARTEN=1			/**< Enable UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS()			U2MODEbits.UARTEN=0			/**< Disable UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_IDLE()		U2MODEbits.USIDL=1			/**< Enable idle mode in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_IDLE()		U2MODEbits.USIDL=0			/**< Disable idle mode in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_IRDA()		U2MODEbits.IREN=1			/**< Enable IrDA encoder/decoder in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_IRDA()		U2MODEbits.IREN=0			/**< Disable IrDA encoder/decoder in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_RTS()		U2MODEbits.RTSMD=0			/**< Enable RTS hardware flow control in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_RTS()		U2MODEbits.RTSMD=1			/**< Disable RTS hardware flow control in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_MODE(x)			U2MODEbits.UEN=((x)&0x3)	/**< Set pin modes in UART2 module @param x Pin mode (0-3) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_WAKE()		U2MODEbits.WAKE=1			/**< Enable wake on start bit in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_WAKE()		U2MODEbits.WAKE=0			/**< Disable wake on start bit in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_LPBACK()		U2MODEbits.LPBACK=1			/**< Enable loopback mode in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_LPBACK()	U2MODEbits.LPBACK=0			/**< Disable loopback mode in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_AUTOBAUD()	U2MODEbits.ABAUD=1			/**< Enable automatic baud rate measurement on next character in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_AUTOBAUD()	U2MODEbits.ABAUD=0			/**< Disable automatic baud rate measurement on next character in UART2 module (or auto-baud finished) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_RXINVERT()	U2MODEbits.URXINV=0			/**< Enable RX polarity inversion (idle = '1') in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_RXINVERT()	U2MODEbits.URXINV=1			/**< Disable RX polarity inversion (idle = '0') in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_BRGHIGH()	U2MODEbits.BRGH=1			/**< Enable high baud rate generator (4 clocks per bit-period) in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_BRGHIGH()	U2MODEbits.BRGH=0			/**< Disable high baud rate generator (16 clocks per bit-period) in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_PARITYDATA(x)	U2MODEbits.PDSEL=((x)&0x3)	/**< Set parity and data bit modes @param x Mode (0-3) UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_1STOP()			U2MODEbits.STSEL=0			/**< Use one stop bit in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_2STOP()			U2MODEbits.STSEL=1			/**< Use two stop bits in UART2 module @hideinitializer @ingroup C30USARTLibrary */
/**
 * @}
 */
/**
 * @todo: Fix USART_UART1_TXINTMODE(x) to reflect that UTXISEL is broken into two separate bits
 */
#define USART_UART1_TXINTMODE(x)	U1STAbits.UTXISEL=((x)&0x3)	/**< Set TX interrupt mode in UART1 module @param x Interrupt mode (0-3) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_TXINVERT()	U1STAbits.UTXINV=0			/**< Enable TX polarity inversion (if IrDA is enabled: idle = '0' otherwise idle = '1') in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_TXINVERT()	U1STAbits.UTXINV=1			/**< Enable TX polarity inversion (if IrDA is enabled: idle = '1' otherwise idle = '0') in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_SENDBREAK()		U1STAbits.UTXBRK=1			/**< Send sync break [start,0,0,0,0,0,0,0,0,0,0,0,0,stop] on next transmission (hardware cleared) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_TX()			U1STAbits.UTXEN=1			/**< Enable TX in UART1 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_TX()		U1STAbits.UTXEN=0			/**< Disable TX in UART1 module (abort any pending transmission and reset buffer) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_TXBUF		U1STAbits.UTXBF				/**< Get TX buffer status in UART1 module @return 1 if buffer is full @return 0 if at least one more character can be written to buffer @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_SHFT		U1STAbits.TRMT				/**< Get TX shift register status in UART1 module @return 1 if shift register is empty and no more pending transfers @return 0 if shift register is not empty, transmission in progress or queued @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_RXINTMODE(x)	U1STAbits.URXISEL=((x)&0x3)	/**< Set RX interrupt mode in UART1 module @param x Interrupt mode (0-3) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_EN_ADDRESS()	U1STAbits.ADDEN=1			/**< Enable Address Character Detection while in 9-bit mode in UART1 @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_DIS_ADDRESS()	U1STAbits.ADDEN=0			/**< Disable Address Character Detection while in 9-bit mode in UART1 @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_RXIDLE		U1STAbits.RIDLE				/**< Get RX status in UART1 module @return 1 if receiver is idle @return 0 if receiver is active @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_PERR		U1STAbits.PERR				/**< Get Parity Error status in UART1 module @return 1 if parity error detected in current character (character at top of FIFO) @return 0 if no parity error detected @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_FERR		U1STAbits.FERR				/**< Get Framing Error status in UART1 module @return 1 if framing error detected in current character (character at top of FIFO) @return 0 if no framing error detected @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_OERR		U1STAbits.OERR				/**< Get Overrun Error status in UART1 module, bit must be cleared after an overflow to reset RX buffer and RX shift register. @return 1 if buffer has overflowed @return 0 if no buffer overflow has been detected @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART1_STA_RXAVAIL		U1STAbits.URXDA				/**< Get RX Data Ready status in UART1 module @return 1 if RX buffer has data available @return 0 if RX buffer is empty @hideinitializer @ingroup C30USARTLibrary */


#define USART_UART2_TXINTMODE(x)	U2STAbits.UTXISEL=((x)&0x3)	/**< Set TX interrupt mode in UART2 module @param x Interrupt mode (0-3) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_TXINVERT()	U2STAbits.UTXINV=0			/**< Enable TX polarity inversion (if IrDA is enabled: idle = '0' otherwise idle = '1') in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_TXINVERT()	U2STAbits.UTXINV=1			/**< Enable TX polarity inversion (if IrDA is enabled: idle = '1' otherwise idle = '0') in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_SENDBREAK()		U2STAbits.UTXBRK=1			/**< Send sync break [start,0,0,0,0,0,0,0,0,0,0,0,0,stop] on next transmission (hardware cleared) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_TX()			U2STAbits.UTXEN=1			/**< Enable TX in UART2 module @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_TX()		U2STAbits.UTXEN=0			/**< Disable TX in UART2 module (abort any pending transmission and reset buffer) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_TXBUF		U2STAbits.UTXBF				/**< Get TX buffer status in UART2 module @return 1 if buffer is full @return 0 if at least one more character can be written to buffer @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_SHFT		U2STAbits.TRMT				/**< Get TX shift register status in UART2 module @return 1 if shift register is empty and no more pending transfers @return 0 if shift register is not empty, transmission in progress or queued @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_RXINTMODE(x)	U2STAbits.URXISEL=((x)&0x3)	/**< Set RX interrupt mode in UART2 module @param x Interrupt mode (0-3) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_EN_ADDRESS()	U2STAbits.ADDEN=1			/**< Enable Address Character Detection while in 9-bit mode in UART2 @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_DIS_ADDRESS()	U2STAbits.ADDEN=0			/**< Disable Address Character Detection while in 9-bit mode in UART2 @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_RXIDLE		U2STAbits.RIDLE				/**< Get RX status in UART2 module @return 1 if receiver is idle @return 0 if receiver is active @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_PERR		U2STAbits.PERR				/**< Get Parity Error status in UART2 module @return 1 if parity error detected in current character (character at top of FIFO) @return 0 if no parity error detected @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_FERR		U2STAbits.FERR				/**< Get Framing Error status in UART2 module @return 1 if framing error detected in current character (character at top of FIFO) @return 0 if no framing error detected @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_OERR		U2STAbits.OERR				/**< Get Overrun Error status in UART2 module, bit must be cleared after an overflow to reset RX buffer and RX shift register. @return 1 if buffer has overflowed @return 0 if no buffer overflow has been detected @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_STA_RXAVAIL		U2STAbits.URXDA				/**< Get RX Data Ready status in UART2 module @return 1 if RX buffer has data available @return 0 if RX buffer is empty @hideinitializer @ingroup C30USARTLibrary */

#define USART_UART1_BRG(x)			U1BRG=(unsigned int)(x)		/**< Set Baud Rate Generator divisor in UART1 module @param x Baud rate divisor (16-bit integer) @hideinitializer @ingroup C30USARTLibrary */
#define USART_UART2_BRG(x)			U2BRG=(unsigned int)(x)		/**< Set Baud Rate Generator divisor in UART2 module @param x Baud rate divisor (16-bit integer) @hideinitializer @ingroup C30USARTLibrary */

/**
 * Set the baud rate for UART1 module.
 * @param x Baud rate in bps (16-bit integer)
 * @hideinitializer
 * @ingroup C30USARTLibrary
 */
#define USART_UART1_SETBAUDRATE(x)	U1BRG = U1MODEbits.BRGH ? (unsigned int)(((((FOSC)>>1)/(x))>>2)-1) : (unsigned int)(((((FOSC)>>1)/(x))>>4)-1)

/**
 * Set the baud rate for UART2 module.
 * @param x Baud rate in bps (16-bit integer)
 * @hideinitializer
 * @ingroup C30USARTLibrary
 */
#define USART_UART2_SETBAUDRATE(x)	U2BRG = U2MODEbits.BRGH ? (unsigned int)(((((FOSC)>>1)/(x))>>2)-1) : (unsigned int)(((((FOSC)>>1)/(x))>>4)-1)



#elif defined(__18CXX) //Compiler Check

//Check for valid processor and include matching header
#if defined(__18F2680)
	#include <c18/p18f2680.h>
#else
	#error "USART: Invalid/unknown processor for compiler (Microchip C18)!"
#endif //defined(__processor)

/**
 * @defgroup C18USARTLibrary C18 USART Library
 */


#elif defined(__PICC__) || defined(__PICCLITE__) //Compiler Check

#include <htc.h>

#if !(defined(_16F1823) || defined(_16LF1823)) //Valid processors for library
	#error "USART: Invalid/unknown processor (REQ: 16F1823 or 16LF1823)!"
#endif //!defined(_processor)

/**
 * @defgroup PICCUSARTLibrary PICC USART Library
 */


#else //Unknown Compiler
	#error "USART: Unknown compiler!"
#endif //Compiler Check


/* See usart.c for documentation and definition */
unsigned char usart_rx(unsigned char *rx_byte);
/* See usart.c for documentation and definition */
unsigned char usart_tx(unsigned char *tx_byte);
/* See usart.c for documentation and definition */
unsigned char usart_tx_chars(unsigned char *tx_chars, unsigned char n);

/* See usart.c for documentation and definition */
void usart_init(void);

#endif //_USART_H

