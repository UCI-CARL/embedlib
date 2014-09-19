/* -*- mode: C++; tab-width: 4; indent-tabs-mode: t; -*- */
/** @file {uart.c} **********************************************************
 * 
 * @brief UART/USART library header.
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
 * @version {0.0.3}
 * 
 * @pcbnum{N/A}
 * @pcbname{N/A}
 * @pcbdescription
 * N/A
 * 
 * @notes

 * 
 ****************************************************************************/

#if defined(__18CXX)

// Microchip peripheral library include files
#include <delays.h>

#include <board.def>
#include <uart.def>

#include <uart/uart.h>



union
{
	struct
	{
		unsigned char low;
		unsigned char high;
	};
	unsigned int int16;
} spbrg;

unsigned char usart_rx(unsigned char *rx_byte)
{
	unsigned int timecount=0; //Timeout counter
	unsigned char temp; //Temporary storage for Framing Error
	
	while(!BAUDCONbits.RCIDL && timecount < USART_RX_TIMEOUT)
	{
		Delay100TCYx(50); //Delay 1ms @ FOSC=20MHz
		timecount++;
	}
	
	if(timecount >= USART_RX_TIMEOUT) //Timeout occured
	{
		return 1; //Exit status: Error
	}
	
	if(RCSTAbits.FERR) //Framing error occured
	{
		temp = RCREG;
		return 2; //Exit status: Error
	}

	if(RCSTAbits.OERR)
	{
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1; //Reset receiver
		return 3; //Exit status: Error
	}
	
	*rx_byte = RCREG;

	return 0; //Exit status: Success
}


unsigned char usart_tx(unsigned char *tx_byte)
{
	unsigned int timecount = 0; //Timeout counter

	while(!TXSTAbits.TRMT && timecount < USART_TX_TIMEOUT) //Wait for transmit register to be empty
	{
		Delay100TCYx(50); //Delay 1ms @ FOSC=20MHz
		timecount++;
	}
	if(timecount >= USART_TX_TIMEOUT)
	{
		return 1; //Exit status: Error
	}

	TXREG = *tx_byte; //Fill transmit register
	
	#if defined(USART_TX_BLOCKING)
		timecount = 0;
		while(!TXSTAbits.TRMT && timecount < USART_TX_TIMEOUT)
		{
			Delay100TCYx(50); //Delay 1ms @ FOSC=20MHz
			timecount++;
		}
	
		if(timecount >= USART_TX_TIMEOUT)
		{
			return 1; //Exit status: Error
		}
	#endif //defined(USART_TX_BLOCKING)
	
	return 0; //Exit status: Success
}

unsigned char usart_tx_chars(unsigned char *tx_chars, unsigned char n)
{
	unsigned char i=0;
	unsigned int timecount = 0; //Timeout counter

	for(i=0;i<n;i++)
	{
		timecount = 0;
		while(!TXSTAbits.TRMT && timecount < USART_TX_TIMEOUT) //Wait for transmit register to be empty
		{
			Delay100TCYx(50); //Delay 1ms @ FOSC=20MHz
			timecount++;
		}
		if(timecount >= USART_TX_TIMEOUT)
		{
			return i; //Exit status: Error
		}
	
		TXREG = tx_chars[i]; //Fill transmit register
		
		#if defined(USART_TX_BLOCKING)
			timecount = 0;
			while(!TXSTAbits.TRMT && timecount < USART_TX_TIMEOUT)
			{
				Delay100TCYx(50); //Delay 1ms @ FOSC=20MHz
				timecount++;
			}
			if(timecount >= USART_TX_TIMEOUT)
			{
				return i; //Exit status: Error
			}
		#endif //defined(USART_TX_BLOCKING)
	}
	
	return i; //Exit status: Success
}


/*
unsigned char tx_byte_literal(unsigned char tx_buf)
{
	unsigned int t = 0;
	while (!TXSTAbits.TRMT && t < USART_TX_TIMEOUT*10) // wait for register to be empty
	{
		Delay100TCYx(5);
		t++;
	}
	if(t >= USART_TX_TIMEOUT*10)
		return 1;
	TXREG = tx_buf;
	t = 0;
	while(!TXSTAbits.TRMT && t < USART_TX_TIMEOUT*10)
	{
		Delay100TCYx(5);
		t++;
	}
	if(t >= USART_TX_TIMEOUT*10)
		return 1;
	
	return 0; //Exit status: Success
}
*/

/*
unsigned char rx_string(unsigned char *rx_buf, int buf_size)
{
	int i;
	unsigned char temp;
	
	for(i=0;i<buf_size;i++)
	{
		if(usart_rx(&rx_buf[i]))
		{
			return 1;
		}
	}
	return 0;
}
*/

/*
unsigned char tx_chars(unsigned char *tx_buf, int buf_size)
{
	int i;
	
	for(i=0;i<buf_size;i++)
	{
		if(usart_tx(&tx_buf[i]))
		{
			return 1;
		}
	}
	return 0;
}
*/

/*
unsigned char tx_string(const char *string_buf)
{
	int i, len;
	unsigned char cur;
	
	len = strlen(string_buf);
	for(i=0;i<len;i++)
	{
		if(usart_tx((unsigned char *)(&string_buf[i])))
		{
			return i;
		}
	}
	return i;
}
*/

void usart_init(void)
{
	
	#if defined(USART_ENABLE)
		RCSTAbits.SPEN = 1;			//Enable USART pins (TX/RX)
	#else
		RCSTAbits.SPEN = 0;			//Disable USART pins (TX/RX)
	#endif

	#if defined(USART_RX_ENABLE)
		RCSTAbits.CREN = 1;			//Enable receive
	#else
		RCSTAbits.CREN = 0;			//Disable receive
	#endif

	#if defined(USART_9BIT_RX_ENABLE)
		RCSTAbits.RX9 = 1;			//Enable 9-bit receives
	#else
		RCSTAbits.RX9 = 0;			//Disable 9-bit receives
	#endif

	#if defined(USART_TX_ENABLE)
		TXSTAbits.TXEN = 1;			//Enable transmit
	#else
		TXSTAbits.TXEN = 0;			//Disable transmit
	#endif
	
	#if defined(USART_9BIT_TX_ENABLE)
		TXSTAbits.TX9 = 1;			//Enable 9-bit transmissions
	#else
		TXSTAbits.TX9 = 0;			//Disable 9-bit transmissions
	#endif
	
	#if defined(USART_SYNC_ENABLE)
		TXSTAbits.SYNC = 1;			//Enable synchronous mode
	#else
		TXSTAbits.SYNC = 0;			//Enable asynchronous mode
		
		#if defined(USART_INVERTED_ENABLE)
			BAUDCONbits.SCKP = 1;	//Transmit inverted data
		#else
			BAUDCONbits.SCKP = 0;	//Transmit non-inverted data
		#endif
	#endif
	
	#if defined(USART_WAKEUP_ENABLE)
		BAUDCONbits.WUE = 1;		//Enable Wake-up
	#else
		BAUDCONbits.WUE = 0;		//Disable Wake-up
	#endif

	#if defined(USART_BAUDRATE_4800)
		#if (FOSC == 8000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 416;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 16000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 832;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 20000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 1041;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 32000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 1666;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#else
			#error "USART: No valid baud rate at specified FOSC!"
		#endif
	#elif defined(USART_BAUDRATE_9600)
		#if (FOSC == 8000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 207;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 16000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 416;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 20000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 520;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 32000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 832;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#else
			#error "USART: No valid baud rate at specified FOSC!"
		#endif
	#elif defined(USART_BAUDRATE_19200)
		#if (FOSC == 8000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 103;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 16000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 207;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 20000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 259;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#elif (FOSC == 32000000L)
			TXSTAbits.BRGH = 1;
			BAUDCONbits.BRG16 = 1;
			spbrg.int16 = 416;
			SPBRG = spbrg.low;
			SPBRGH = spbrg.high;
		#else
			#error "USART: No valid baud rate at specified FOSC!"
		#endif
	#else
		#error "USART: No baud rate defined!"
	#endif


	/* USART buffering [not finished]
	unsigned char i;
	
	#ifdef USART_RX_BUFFERENABLE
		for(i=0;i<USART_RX_BUFFERLENGTH-1;i++)
		{
			usart_rx_buffer[i].next = &usart_rx_buffer[i+1];
		}
		usart_rx_buffer[USART_RX_BUFFERLENGTH-1].next = &usart_rx_buffer[0];
	#endif //USART_RX_BUFFERENABLE
	
	#ifdef USART_TX_BUFFERENABLE
		for(i=0;i<USART_TX_BUFFERLENGTH-1;i++)
		{
			usart_tx_buffer[i].next = &usart_tx_buffer[i+1];
		}
		usart_tx_buffer[USART_TX_BUFFERLENGTH-1].next = &usart_tx_buffer[0];
	#endif //USART_TX_BUFFERENABLE
	*/
}


/***** HiTech C 16 USART Library *****/

#elif defined(__PICC__) || defined(__PICCLITE__) //Compiler Check

#include <htc.h>

#if !(defined(_16F1823) || defined(_16LF1823)) //Valid processors for library
	#error "USART: Invalid/unknown processor (REQ: 16F1823 or 16LF1823)!"
#endif //!defined(_processor)

#include <string.h>
#include <stdio.h>

#include "def/board.def"
#include "def/usart.def"

#include <usart.h>

volatile union
{
	struct
	{
		unsigned char low;
		unsigned char high;
	} split;
	unsigned int merge;
} spbrg @ 0x19B;

unsigned char usart_rx(unsigned char *rx_byte)
{
	unsigned int timecount = 0; //Timeout counter
	unsigned char temp; //Temporary storage
	
	while(!RCIDL && timecount < USART_RX_TIMEOUT) //Wait for RX to be idle
	{
		__delay_ms(1);
		timecount++;
	}
	
	if(timecount >= USART_RX_TIMEOUT) //Timeout occured
	{
		return 1; //Exit status: Error
	}

	if(FERR) //Framing error occured
	{
		temp = RCREG;
		return 1; //Exit status: Error
	}

	if(OERR) //Overrun error occured
	{
		CREN = 0;
		CREN = 1; //Reset receiver
		return 1; //Exit status: Error
	}
	
	*rx_byte = RCREG;
	return 0; //Exit status: Success
}

unsigned char usart_tx(unsigned char *tx_buf)
{
	unsigned int timecount = 0; //Timeout counter

	while(!TRMT && timecount < USART_TX_TIMEOUT) //Wait for transmit register to be empty
	{
		__delay_ms(1);
		timecount++;
	}
	
	if(timecount >= USART_TX_TIMEOUT)
	{
		return 1; //Exit status: Error
	}

	TXREG = *tx_buf;
	
	#if defined(USART_TX_WAIT)
		timecount = 0;
		while(!TRMT && timecount < USART_TX_TIMEOUT)
		{
			__delay_ms(1);
			timecount++;
		}
		
		if(timecount >= USART_TX_TIMEOUT)
		{
			return 1; //Exit status: Error
		}
	#endif //defined(USART_TX_WAIT)
	
	return 0; //Exit status: Success
}

void usart_init(void)
{

	#if defined(USART_ENABLE)
		SPEN = 1;					//Enable USART pins (TX/RX)
	#else
		SPEN = 0;					//Disable USART pins (TX/RX)
	#endif

	#if defined(USART_RX_ENABLE)
		TRIS_USART_RX = TRIS_IN;	//Set RX pin to input
		CREN = 1;					//Enable receive
	#else
		CREN = 0;					//Disable receive
	#endif

	#if defined(USART_9BIT_RX_ENABLE)
		RX9 = 1;					//Enable 9-bit receives
	#else
		RX9 = 0;					//Disable 9-bit receives
	#endif

	#if defined(USART_TX_ENABLE)
		TRIS_USART_TX = TRIS_OUT;	//Set TX pin to output
		TXEN = 1;					//Enable transmit
	#else
		TXEN = 0;					//Disable transmit
	#endif
	
	#if defined(USART_9BIT_TX_ENABLE)
		TX9 = 1;					//Enable 9-bit transmissions
	#else
		TX9 = 0;					//Disable 9-bit transmissions
	#endif
	
	#if defined(USART_SYNC_ENABLE)
		SYNC = 1;					//Enable synchronous mode
	#else
		SYNC = 0;					//Enable asynchronous mode
		
		#if defined(USART_INVERTED_ENABLE)
			SCKP = 1;				//Transmit inverted data
		#else
			SCKP = 0;				//Transmit non-inverted data
		#endif
	#endif
	
	#if defined(USART_WAKEUP_ENABLE)
		WUE = 1;					//Enable Wake-up
	#else
		WUE = 0;					//Disable Wake-up
	#endif

	#if defined(USART_BAUDRATE_4800)
		#if (FOSC == 8000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 416;
		#elif (FOSC == 16000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 832;
		#elif (FOSC == 20000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 1041;
		#elif (FOSC == 32000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 1666;
		#else
			#error "USART: No valid baud rate at specified FOSC!"
		#endif
	#elif defined(USART_BAUDRATE_9600)
		#if (FOSC == 8000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 207;
		#elif (FOSC == 16000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 416;
		#elif (FOSC == 20000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 520;
		#elif (FOSC == 32000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 832;
		#else
			#error "USART: No valid baud rate at specified FOSC!"
		#endif
	#elif defined(USART_BAUDRATE_19200)
		#if (FOSC == 8000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 103;
		#elif (FOSC == 16000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 207;
		#elif (FOSC == 20000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 259;
		#elif (FOSC == 32000000L)
			BRGH = 1;
			BRG16 = 1;
			spbrg.merge = 416;
		#else
			#error "USART: No valid baud rate at specified FOSC!"
		#endif
	#else
		#error "USART: No baud rate defined!"
	#endif
}

#else
	#error "USART: Unknown compiler!!!"
#endif //Compiler Check


