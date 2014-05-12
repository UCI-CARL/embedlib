/**************************************************
 * adc.c
 *
 * Created On: 11/1/2010
 * Created By: Liam Bucci
 * Project Name: ADC
 * Project Type: Embedded Library
 * Project Description: Embedded library for PIC MCUs which sets
 * and configures chip specific analog to digital (A/D) conversion
 * bits as well as provides functions for A/D conversions.
 * 
 * Notes: 
 *
 * TODO: Integrate with Interrupts library for non-blocking A/D
 * conversions.
 * 
 **************************************************/


/***** Microchip C18 A/D Conversion Library *****/

#if defined(__18CXX) //Compiler Check

//Check for valid processor and include matching header
#if defined(__18F2585)
	#include <p18f2585.h>
#elif defined(__18F2680)
	#include <p18f2680.h>
#elif defined(__18F4585)
	#include <p18f4585.h>
#elif defined(__18F4680)
	#include <p18f4680.h>
#else
	#error "ADC: Invalid/unknown processor for compiler (Microchip C18)!"
#endif //defined(__processor)

#include "def\adc.def"
#include "def\board.def"

#include <adc.h>

void adc_init()
{
	#if (!defined(__18F4585) || !defined(__18F4680)) && (defined(ADC_PORTCONFIG_AN7) || defined(ADC_PORTCONFIG_AN6) || defined(ADC_PORTCONFIG_AN5))
		#warning "ADC: AN7/AN6/AN5 not valid A/D converter pins on this processor!"
	#endif
		
	#if defined(ADC_PORTCONFIG_ALL)
		ADCON1bits.PCFG3 = 0;
		ADCON1bits.PCFG2 = 0;
		ADCON1bits.PCFG1 = 0;
		ADCON1bits.PCFG0 = 0;
		TRIS_ADC_AN10 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN9)
		ADCON1bits.PCFG3 = 0;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 0;
		ADCON1bits.PCFG0 = 1;
		TRIS_ADC_AN9 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN8)
		ADCON1bits.PCFG3 = 0;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 0;
		TRIS_ADC_AN8 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN7)
		ADCON1bits.PCFG3 = 0;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 1;
		TRIS_ADC_AN7 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN6)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 0;
		ADCON1bits.PCFG1 = 0;
		ADCON1bits.PCFG0 = 0;
		TRIS_ADC_AN6 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN5)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 0;
		ADCON1bits.PCFG1 = 0;
		ADCON1bits.PCFG0 = 1;
		TRIS_ADC_AN5 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN4)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 0;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 0;
		TRIS_ADC_AN4 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN3)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 0;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 1;
		TRIS_ADC_AN3 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN2)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 0;
		ADCON1bits.PCFG0 = 0;
		TRIS_ADC_AN2 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN1)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 0;
		ADCON1bits.PCFG0 = 1;
		TRIS_ADC_AN1 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_AN0)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 0;
		TRIS_ADC_AN0 = TRIS_IN;
	#elif defined(ADC_PORTCONFIG_NONE)
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 1;
	#else //If no setting defined, set all pins to digital
		#warning "ADC: No port configuration definition found, using default of all digital!"
		ADCON1bits.PCFG3 = 1;
		ADCON1bits.PCFG2 = 1;
		ADCON1bits.PCFG1 = 1;
		ADCON1bits.PCFG0 = 1;
	#endif //PORTCONFIG

	#if defined(ADC_VREFCONFIG_VSS)
		ADCON1bits.VCFG1 = 0;
	#else //Set to VREF-
		ADCON1bits.VCFG1 = 1;
	#endif

	#if defined(ADC_VREFCONFIG_VDD)
		ADCON1bits.VCFG0 = 0;
	#else //Set to VREF+
		ADCON1bits.VCFG0 = 1;
	#endif

	#if defined(ADC_FORMAT_LEFTJUSTIFY)
		ADCON2bits.ADFM = 0;
	#else //Right justify result
		ADCON2bits.ADFM = 1;
	#endif

	#if defined(ADC_ACQTIME_20TAD)
		ADCON2bits.ACQT2 = 1;
		ADCON2bits.ACQT1 = 1;
		ADCON2bits.ACQT0 = 1;
	#elif defined(ADC_ACQTIME_16TAD)
		ADCON2bits.ACQT2 = 1;
		ADCON2bits.ACQT1 = 1;
		ADCON2bits.ACQT0 = 0;
	#elif defined(ADC_ACQTIME_12TAD)
		ADCON2bits.ACQT2 = 1;
		ADCON2bits.ACQT1 = 0;
		ADCON2bits.ACQT0 = 1;
	#elif defined(ADC_ACQTIME_8TAD)
		ADCON2bits.ACQT2 = 1;
		ADCON2bits.ACQT1 = 0;
		ADCON2bits.ACQT0 = 0;
	#elif defined(ADC_ACQTIME_6TAD)
		ADCON2bits.ACQT2 = 0;
		ADCON2bits.ACQT1 = 1;
		ADCON2bits.ACQT0 = 1;
	#elif defined(ADC_ACQTIME_4TAD)
		ADCON2bits.ACQT2 = 0;
		ADCON2bits.ACQT1 = 1;
		ADCON2bits.ACQT0 = 0;
	#elif defined(ADC_ACQTIME_2TAD)
		ADCON2bits.ACQT2 = 0;
		ADCON2bits.ACQT1 = 0;
		ADCON2bits.ACQT0 = 1;
	#elif defined(ADC_ACQTIME_0TAD)
		ADCON2bits.ACQT2 = 0;
		ADCON2bits.ACQT1 = 0;
		ADCON2bits.ACQT0 = 0;
	#else //Set A/D acquisition time to default of 0TAD
		#warning "ADC: No acquisition time definition found, using default of 0TAD!"
		ADCON2bits.ACQT2 = 0;
		ADCON2bits.ACQT1 = 0;
		ADCON2bits.ACQT0 = 0;
	#endif

	#if defined(ADC_CONVCLOCK_FRC)
		ADCON2bits.ADCS2 = 1;
		ADCON2bits.ADCS1 = 1;
		ADCON2bits.ADCS0 = 1;
	#elif defined(ADC_CONVCLOCK_FOSC64)
		ADCON2bits.ADCS2 = 1;
		ADCON2bits.ADCS1 = 1;
		ADCON2bits.ADCS0 = 0;
	#elif defined(ADC_CONVCLOCK_FOSC32)
		ADCON2bits.ADCS2 = 0;
		ADCON2bits.ADCS1 = 1;
		ADCON2bits.ADCS0 = 0;
	#elif defined(ADC_CONVCLOCK_FOSC16)
		ADCON2bits.ADCS2 = 1;
		ADCON2bits.ADCS1 = 0;
		ADCON2bits.ADCS0 = 1;
	#elif defined(ADC_CONVCLOCK_FOSC8)
		ADCON2bits.ADCS2 = 0;
		ADCON2bits.ADCS1 = 0;
		ADCON2bits.ADCS0 = 1;
	#elif defined(ADC_CONVCLOCK_FOSC4)
		ADCON2bits.ADCS2 = 1;
		ADCON2bits.ADCS1 = 0;
		ADCON2bits.ADCS0 = 0;
	#elif defined(ADC_CONVCLOCK_FOSC2)
		ADCON2bits.ADCS2 = 0;
		ADCON2bits.ADCS1 = 0;
		ADCON2bits.ADCS0 = 0;
	#else //Set conversion clock to default of FOSC/4
		#warning "ADC: No conversion clock definition found, using default of FOSC/4!"
		ADCON2bits.ADCS2 = 1;
		ADCON2bits.ADCS1 = 0;
		ADCON2bits.ADCS0 = 0;
	#endif

	#if defined(ADC_ENABLE)
		ADCON0bits.ADON = 1;
	#else //Disable A/D module
		ADCON0bits.ADON = 0;
	#endif
}

unsigned char adc_convert(unsigned char channel)
{
	//Check for channel out of range
	if(channel > ADC_MAXCHANNEL)
	{
		return 1;
	}
	
	//Check for invalid channels for processor type
	#if (!defined(__18F4585) || !defined(__18F4680))
	if(channel == 5 || channel == 6 || channel == 7)
	{
		return 1;
	}
	#endif

	//Turn A/D converter on
	ADC_ON();
	
	//Set channel
	ADC_CHANNEL(channel);

	ADC_START();

	return 0;
}

unsigned int adc_read(unsigned char bits)
{
	if(bits > 10 || bits == 0)
		return 0;

	//TODO: Add A/D convert read for all possible resolutions
	#if defined(ADC_FORMAT_LEFTJUSTIFY)
		if(bits == 10)
			return ((((unsigned int)ADRESH)<<2)|(ADRESL>>6));
		else if(bits == 8)
			return ADRESH;
		else
			return 0;
	#else
		if(bits == 10)
			return (((unsigned int)ADRESH)<<8)|ADRESL;
		else if(bits == 8)
			return ((ADRESH<<6)|(ADRESL>>2));
		else
			return 0;
	#endif
}


#endif //defined(__18CXX)

