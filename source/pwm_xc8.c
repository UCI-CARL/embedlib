/**************************************************
 * pwm.c
 *
 * Created On: 9/22/2010 (Brought to spec. date)
 * Created By: Liam Bucci
 * Project Name: PWM
 * Project Type: Embedded Library
 * Project Description: Embedded library for PIC MCUs which sets
 * and configures chip specific PWM settings depending on
 * the projects "pwm.def" file. Also supplies run-time functions
 * to control PWM output.
 * 
 * TODO: Library needs to be updated and fixed!
 * 
 **************************************************/


/***** Check Compiler/Processor Pair *****/

#if defined(__18CXX) //Microchip C18 Compiler
	#if defined(__18F2585)
		#include <p18f2585.h>
	#elif defined(__18F2680)
		#include <p18f2680.h>
	#elif defined(__18F4585)
		#include <p18f4585.h>
	#elif defined(__18F4680)
		#include <p18f4680.h>
	#else
		#error "PWM: Invalid/unknown processor for compiler (Microchip C18)!"
	#endif //defined(__processor)

#elif defined(__PICC__) || defined(__PICCLITE__) //HiTech C 10/12/16 Compiler
	#include <htc.h>

	#if !(defined(__16F1823) || defined(__16LF1823)) //Valid processors for library
		#error "PWM: Invalid/unknown processor (REQ: 16F1823 or 16LF1823)!"
	#endif //defined(__processor)

#else
	#error "PWM: Unknown compiler!"
#endif //Compiler select


#include "def/board.def"
//#include "def/pwm.def"

#include <pwm.h>


/***** Microchip C18 Interrupt Library *****/
#if defined(__18CXX)
#if defined(__18F2585) || defined(__18F2680) \
	|| defined(__18F4585) || defined(__18F4680) //Valid processors

void pwm_init(void)
{
	//Timer2 - used to run PWM
	T2CONbits.T2CKPS0 = 1;	//*******************************
	T2CONbits.T2CKPS1 = 0;	//Timer2 prescaler select [1:4]
	//T2CONbits.TOUTPS0 = 0;	//*******************************
	//T2CONbits.TOUTPS1 = 0;	//Timer2 output postscale select [1:1]
	//T2CONbits.TOUTPS2 = 0;	//
	//T2CONbits.TOUTPS3 = 0;	//*******************************

	PR2 = 255; //1KHz PWM Freq @ 1:16 TMR2Prescale and 20MHz clock

	CCP1CONbits.CCP1M3 = 1;
	CCP1CONbits.CCP1M2 = 1;
	CCP1CONbits.CCP1M1 = 1;
	CCP1CONbits.CCP1M0 = 1;
}

void pwm_start(void)
{
	T2CONbits.TMR2ON = 1;
}

void pwm_set_dutycycle(unsigned int dutycycle)
{
	CCPR1L = (unsigned char)(dutycycle>>2);
	CCP1CONbits.DC1B1 = (unsigned char)(dutycycle & 0b00000010);
	CCP1CONbits.DC1B0 = (unsigned char)(dutycycle & 0b00000001);
}

#endif //Valid processors
#endif //Microchip C18 Compiler



/***** HiTech C 16 Interrupt Library *****/

#if defined(__PICC__) || defined(__PICCLITE__) //Valid Compilers
#if defined(__16F1823) || defined(__16LF1823) //Valid processors

void pwm_init()
{
	TRISC5 = 1; //Turn off CCP1 pin
	
}


#endif //Valid processors
#endif //Valid Compiler

