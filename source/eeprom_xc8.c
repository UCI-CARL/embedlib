/**************************
 * eeprom.c
 * Created On: 11/5/10
 * Created By: Liam Bucci
 * Project Name: EEPROM
 * Project Type: Embedded Library
 * Project Description: Embedded library for PIC MCUs which sets
 * and configures chip specific EEPROM settings as well as providing
 * functionality for reading and writing of bytes to EEPROM memory.
 * 
 * NOTES: 
 * 
 * TODO: Write HiTech C16 section
 * 
 *************************/


/***** Microchip C18 EEPROM Library *****/
#if defined(__18CXX) //Compiler Check

//Check for valid processor and include matching header
#if defined(__18f2585)
	#include <p18f2585.h>
#elif defined(__18F2680)
	#include <p18f2680.h>
#elif defined(__18F4585)
	#include <p18f4585.h>
#elif defined(__18F4680)
	#include <p18f4680.h>
#else
	#error "EEPROM: Selected processor not compatible with library!"
#endif //defined (__processor)

//#include <string.h>
//#include <delays.h>
//#include <stdio.h>
#include<interrupts.h>

#include <board.def>
//#include <eeprom.def>

#include <eeprom.h>


void eeprom_init(void)
{
	
}

char eeprom_read(unsigned int address, unsigned char *byte)
{
	EEADR = (unsigned char)address;
	EEADRH = (unsigned char)(address>>8);

	EECON1bits.EEPGD = 0;
	
	EECON1bits.RD = 1;

	*byte = EEDATA;

	return 0;
}

char eeprom_write(unsigned int address, unsigned char byte)
{
	#if defined(EEPROM_WRITE_CHECK)
	unsigned char test_byte;
	#endif

	if(EECON1bits.WRERR)
		return 1;
	
	EEADR = (unsigned char)address;
	EEADRH = (unsigned char)(address>>8);

	EEDATA = byte;
	
	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;
	EECON1bits.WREN = 1; //Enable writes

	INTERRUPTS_OFF();
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;
	INTERRUPTS_ON();

	EECON1bits.WREN = 0; //Disable writes

	#if defined(EEPROM_WRITE_BLOCKING)
	while(EECON1bits.WR){}
	#endif

	#if defined(EEPROM_WRITE_CHECK)
	eeprom_read(address,&test_byte);
	if(byte == test_byte)
		return 0;
	else
		return 2;

	#else
	if(EECON1bits.WRERR)
		return 1;
	else
		return 0;
	
	#endif
}

/***** End Microchip C18 EEPROM Library *****/


/***** HiTech C 16 EEPROM Library *****/
#elif defined(__PICC__) || defined(__PICCLITE__) //Compiler Check

#include <htc.h>

#if !(defined(_16F1823) || defined(_16LF1823)) //Valid processors for library
	#error "EEPROM: Invalid/unknown processor (REQ: 16F1823 or 16LF1823)!"
#endif //!defined(_processor)

//#include <string.h>
//#include <stdio.h>

#include "def/board.def"
#include "def/eeprom.def"

#include <eeprom.h>


void eeprom_init(void)
{
	//TODO
}

char eeprom_read(unsigned int address, unsigned char *byte)
{
	//TODO
}

char eeprom_write(unsigned int address, unsigned char byte)
{
	//TODO
}

/***** End HiTech C 16 EEPROM Library *****/

#else //Compiler Check
	#error "EEPROM: Unknown compiler!"
#endif //Compiler Check

