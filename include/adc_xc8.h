/**************************************************
 * adc.h
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

#ifndef _ADC_H
#define _ADC_H

/***** A/D Converter Macros *****/
#define ADC_ON() ADCON0bits.ADON=1
#define ADC_OFF() ADCON0bits.ADON=0
#define ADC_START() ADCON0bits.GO_DONE=1
#define ADC_CHANNEL(c) ADCON0|=c<<2;
//#if defined(ADC_FORMAT_LEFTJUSTIFY)
	#define ADC_READ_10BIT() ((((unsigned int)ADRESH)<<2)|(ADRESL>>6))
	#define ADC_READ_8BIT() ADRESH
//#else
//	#define ADC_READ_10BIT() (((unsigned int)ADRESH)<<8)|ADRESL
//	#define ADC_READ_8BIT() ((ADRESH<<6)|(ADRESL>>2))
//#endif

/***** A/D Converter Constants *****/
#define ADC_MAXCHANNEL 10

void adc_init(void);
unsigned char adc_convert(unsigned char channel);
unsigned int adc_read(unsigned char bits);

#endif //_ADC_H