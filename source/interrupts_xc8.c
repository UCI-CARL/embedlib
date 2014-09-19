/* -*- mode: C++; tab-width: 4; indent-tabs-mode: t; -*- */
/** @file {interrupts.c} *****************************************************
 * @brief Interrupt library code.
 *
 * @createdon{8/5/2010}
 * @createdby{Liam Bucci}
 * @projectname{PIC C Library}
 * @projecttype{PIC C Embedded Library}
 * @subprojectname{Interrupts}
 * @subprojecttype{Interrupt Library}
 * @subprojectdescription
 * Embedded library for PIC MCUs (written in C and asm) which sets and
 * configures chip specific interrupt settings depending on the projects
 * "interrupts.def" file. It also defines macros which can be used to access
 * and control interrupt settings and flags.
 *
 * @version {0.0.2}
 *
 * @pcbnum{N/A}
 * @pcbname{N/A}
 * @pcbdescription
 * 
 * @notes 
 *
 * @todo Add priorities for peripheral interrupts (C18)
 * @todo Change UART TX interrupt method since it will continue to interrupt while TXREG is empty!
 * 
 ****************************************************************************/

#include <interrupts.def>

#include <interrupts/interrupts.h>


/**
 * Initialize all interrupts according to interrupts.def project file. This function should be called
 * during the initialization routine of the project.
 */
void interrupts_init()
{

#if defined(__C30__) // Check for C30 Compiler

	/***** Enable/Disable Nested Interrupts *****/
	#if defined(INTERRUPTS_NESTED_ENABLE)
		INTERRUPTS_EN_NEST();
	#else
		INTERRUPTS_DIS_NEST();
	#endif

	/***** Use Standard/Alternate Vector Table *****/
	#if defined(INTERRUPTS_ALT_ENABLE)
		INTERRUPTS_USE_ALT();
	#else
		INTERRUPTS_USE_STD();
	#endif


#elif defined(__18CXX) //Compiler Check

	/***** Core Interrupt Services *****/
	
	/* Timer0 */
	#if defined(INTERRUPTS_TMR0_ENABLE)
		INTCONbits.TMR0IE = 1;
		#if defined(INTERRUPTS_TMR0_HIGHPRIORITY)
			INTCON2bits.TMR0IP = 1;
		#else
			INTCON2bits.TMR0IP = 0;
		#endif
	#else
		INTCONbits.TMR0IE = 0;
	#endif
	
	/* RB Port Change */
	#if defined(INTERRUPTS_RB_ENABLE)
		INTCONbits.RBIE = 1;

		#if defined(INTERRUPTS_RB_HIGHPRIORITY)
			INTCON2bits.RBIP = 1;
		#else
			INTCON2bits.RBIP = 0;
		#endif
	#else
		INTCONbits.RBIE = 0;
	#endif

	/* RB Pull-up Resistors */
	#if defined(INTERRUPTS_RB_PULLUPENABLE)
		INTCON2bits.RBPU = 0;
	#else
		INTCON2bits.RBPU = 1;
	#endif
	
	/* INT0 */
	#if defined(INTERRUPTS_INT0_ENABLE)
		INTCONbits.INT0IE = 1;
		//INT0 is always high priority, so no INT0IP bit.
		#if defined(INTERRUPTS_INT0_RISINGEDGE)
			INTCON2bits.INTEDG0 = 1;
		#else
			INTCON2bits.INTEDG0 = 0;
		#endif
	#else
		INTCONbits.INT0IE = 0;
	#endif
	
	/* INT1 */
	#if defined(INTERRUPTS_INT1_ENABLE)
		INTCON3bits.INT1IE = 1;
		#if defined(INTERRUPTS_INT1_HIGHPRIORITY)
			INTCON3bits.INT1IP = 1;
		#else
			INTCON3bits.INT1IP = 0;
		#endif
		#if defined(INTERRUPTS_INT1_RISINGEDGE)
			INTCON2bits.INTEDG1 = 1;
		#else
			INTCON2bits.INTEDG1 = 0;
		#endif
	#else
		INTCON3bits.INT1IE = 0;
	#endif
	
	/* INT2 */
	#if defined(INTERRUPTS_INT2_ENABLE)
		INTCON3bits.INT2IE = 1;
		#if defined(INTERRUPTS_INT2_HIGHPRIORITY)
			INTCON3bits.INT2IP = 1;
		#else
			INTCON3bits.INT2IP = 0;
		#endif
		#if defined(INTERRUPTS_INT2_RISINGEDGE)
			INTCON2bits.INTEDG2 = 1;
		#else
			INTCON2bits.INTEDG2 = 0;
		#endif
	#else
		INTCON3bits.INT2IE = 0;
	#endif
	
	/***** End Core Interrupt Services *****/
	
	
	/***** Peripheral Interrupt Services *****/
	
	/* Parallel Slave Port (18F4585 and 18F4680 only) */
	#if (defined(__18F4585) || defined(__18F4680))
		#if defined(INTERRUPTS_PSP_ENABLE)
			PIE1bits.PSPIE = 1;
		#else
			PIE1bits.PSPIE = 0;
		#endif
	#endif
	
	/* A/D Converter */
	#if defined(INTERRUPTS_ADC_ENABLE)
		PIE1bits.ADIE = 1;
	#else
		PIE1bits.ADIE = 0;
	#endif
	
	/* EUSART Receive */
	#if defined(INTERRUPTS_USART_RX_ENABLE)
		PIE1bits.RCIE = 1;
        #if defined(INTERRUPTS_USART_RX_HIGHPRIORITY)
            IPR1bits.RCIP = 1;
        #else
            IPR1bits.RCIP = 0;
        #endif
	#else
		PIE1bits.RCIE = 0;
	#endif
	
	/* EUSART Transmit */
	#if defined(INTERRUPTS_USART_TX_ENABLE)
		PIE1bits.TXIE = 1;
        #if defined(INTERRUPTS_USART_TX_HIGHPRIORITY)
            IPR1bits.TXIP = 1;
        #else
            IPR1bits.TXIP = 0;
        #endif
	#else
		PIE1bits.TXIE = 0;
	#endif
	
	/* Master Synchronous Serial Port */
	#if defined(INTERRUPTS_MSSP_ENABLE)
		PIE1bits.SSPIE = 1;
	#else
		PIE1bits.SSPIE = 0;
	#endif
	
	/* Capture/Compare/PWM Module 1 */
	#if defined(INTERRUPTS_CCP1_ENABLE)
		PIE1bits.CCP1IE = 1;
	#else
		PIE1bits.CCP1IE = 0;
	#endif
	
	/* Timer 1 */
	#if defined(INTERRUPTS_TMR1_ENABLE)
		PIE1bits.TMR1IE = 1;
        #if defined(INTERRUPTS_TMR1_HIGHPRIORITY)
            IPR1bits.TMR1IP = 1;
        #else
            IPR1bits.TMR1IP = 0;
        #endif
	#else
		PIE1bits.TMR1IE = 0;
	#endif
	
	/* Timer 2 */
	#if defined(INTERRUPTS_TMR2_ENABLE)
		PIE1bits.TMR2IE = 1;
        #if defined(INTERRUPTS_TMR2_HIGHPRIORITY)
            IPR1bits.TMR2IP = 1;
        #else
            IPR1bits.TMR2IP = 0;
        #endif
	#else
		PIE1bits.TMR2IE = 0;
	#endif
	
	/* Timer 3 */
	#if defined(INTERRUPTS_TMR3_ENABLE)
		PIE2bits.TMR3IE = 1;
	#else
		PIE2bits.TMR3IE = 0;
	#endif
	
	/* Oscillator Fail */
	#if defined(INTERRUPTS_OSCFAIL_ENABLE)
		PIE2bits.OSCFIE = 1;
	#else
		PIE2bits.OSCFIE = 0;
	#endif
	
	/* EEPROM Write */
	#if defined(INTERRUPTS_EEPROM_ENABLE)
		PIE2bits.EEIE = 1;
	#else
		PIE2bits.EEIE = 0;
	#endif
	
	/* MSSP Bus Collision */
	#if defined(INTERRUPTS_MSSP_BUS_ENABLE)
		PIE2bits.BCLIE = 1;
	#else
		PIE2bits.BCLIE = 0;
	#endif
	
	/* High/Low-voltage Detect */
	#if defined(INTERRUPTS_HLVD_ENABLE)
		PIE2bits.HLVDIE = 1;
	#else
		PIE2bits.HLVDIE = 0;
	#endif
	
	/* Enhanced CCP1 (18F4585 and 18F4680 only)*/
	#if (defined(__18F4585) || defined(__18F4680))
		#if defined(INTERRUPTS_ECCP1_ENABLE)
			PIE2bits.ECCP1IE = 1;
		#else
			PIE2bits.ECCP1IE = 0;
		#endif
	#endif
	
	/***** End Peripheral Interrupt Services *****/
	
	
	/***** CAN Peripheral Interrupt Services *****/
	
	// Invalid Message Received 
	#if defined(INTERRUPTS_CAN_IRX_ENABLE)
		PIE3bits.IRXIE = 1;
	#else
		PIE3bits.IRXIE = 0;
	#endif
	
	// Wake-up on Bus Activity 
	#if defined(INTERRUPTS_CAN_WAKE_ENABLE)
		PIE3bits.WAKIE = 1;
	#else
		PIE3bits.WAKIE = 0;
	#endif
	
	// Error on Bus 
	#if defined(INTERRUPTS_CAN_ERR_ENABLE)
		PIE3bits.ERRIE = 1;
	#else
		PIE3bits.ERRIE = 0;
	#endif
	
	// Transmit Buffer 2/n Empty 
	#if defined(INTERRUPTS_CAN_TXB2_ENABLE) || defined(INTERRUPTS_CAN_TXBn_ENABLE)
		PIE3bits.TXB2IE = 1;
	#else
		PIE3bits.TXB2IE = 0;
	#endif
	
	// Transmit Buffer 1 Empty
	#if defined(INTERRUPTS_CAN_TXB1_ENABLE)
		PIE3bits.TXB1IE = 1;
	#else
		PIE3bits.TXB1IE = 0;
	#endif
	
	// Transmit Buffer 0 Empty
	#if defined(INTERRUPTS_CAN_TXB0_ENABLE)
		PIE3bits.TXB0IE = 1;
	#else
		PIE3bits.TXB0IE = 0;
	#endif
	
	// Receive Buffer 1/n Empty
	#if defined(INTERRUPTS_CAN_RXB1_ENABLE) || defined(INTERRUPTS_CAN_RXBn_ENABLE)
		PIE3bits.RXB1IE = 1;
	#else
		PIE3bits.RXB1IE = 0;
	#endif
	
	// Receive Buffer 0 Empty
	#if defined(INTERRUPTS_CAN_RXB0_ENABLE) || defined(INTERRUPTS_CAN_FIFOWM_ENABLE)
		PIE3bits.RXB0IE = 1;
	#else
		PIE3bits.RXB0IE = 0;
	#endif
	
	
	/***** Enable/Disable Global Interrupts *****/
	
	#if defined(INTERRUPTS_PRIORITY_ENABLE)
		RCONbits.IPEN = 1;
		#if defined(INTERRUPTS_GLOBAL_HIGHENABLE)
			INTCONbits.GIE = 1;
		#else
			INTCONbits.GIE = 0;
		#endif
		#if defined(INTERRUPTS_GLOBAL_LOWENABLE)
			INTCONbits.PEIE = 1;
		#else
			INTCONbits.PEIE = 0;
		#endif
	#else
		RCONbits.IPEN = 0;
		#if defined(INTERRUPTS_PERIPHERAL_ENABLE)
			INTCONbits.PEIE = 1;
		#else
			INTCONbits.PEIE = 0;
		#endif
		#if defined(INTERRUPTS_GLOBAL_ENABLE)
			INTCONbits.GIE = 1;
		#else
			INTCONbits.GIE = 0;
		#endif
	#endif	
	/***** End Enable/Disable Global Interrupts *****/


#elif defined(__PICC__) || defined(__PICCLITE__) //Compiler Check

	/***** Core Interrupt Services *****/
	
	/* Timer0 */
	#if defined(INTERRUPTS_TMR0_ENABLE)
		TMR0IE = 1;
	#else
		TMR0IE = 0;
	#endif
	
	/* Interrupt On Change */
	#if defined(INTERRUPTS_IOC_ENABLE)
		IOCIE = 1;
		#if defined(INTERRUPTS_IOC_AP0)
			ANSA0 = 0; //Set AN0 to digital
			IOCAP0 = 1; //Positive edge
		#else
			IOCAP0 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AP1)
			ANSA1 = 0; //Set AN0 to digital
			IOCAP1 = 1; //Positive edge
		#else
			IOCAP1 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AP2)
			ANSA2 = 0; //Set AN0 to digital
			IOCAP2 = 1; //Positive edge
		#else
			IOCAP2 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AP3)
			IOCAP3 = 1; //Positive edge
		#else
			IOCAP3 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AP4)
			ANSA4 = 0; //Set AN0 to digital
			IOCAP4 = 1; //Positive edge
		#else
			IOCAP4 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AP5)
			IOCAP5 = 1; //Positive edge
		#else
			IOCAP5 = 0;
		#endif
		
		#if defined(INTERRUPTS_IOC_AN0)
			ANSA0 = 0; //Set AN0 to digital
			IOCAN0 = 1; //Negative edge
		#else
			IOCAN0 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AN1)
			ANSA1 = 0; //Set AN0 to digital
			IOCAN1 = 1; //Negative edge
		#else
			IOCAN1 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AN2)
			ANSA2 = 0; //Set AN0 to digital
			IOCAN2 = 1; //Negative edge
		#else
			IOCAN2 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AN3)
			IOCAN3 = 1; //Negative edge
		#else
			IOCAN3 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AN4)
			ANSA4 = 0; //Set AN0 to digital
			IOCAN4 = 1; //Negative edge
		#else
			IOCAN4 = 0;
		#endif
		#if defined(INTERRUPTS_IOC_AN5)
			IOCAN5 = 1; //Negative edge
		#else
			IOCAN5 = 0;
		#endif
	#else
		IOCIE = 0;
	#endif

	/* Weak Pull-up PortA */ //NOTE: Should be moved to a separate library!
	#if defined(INTERRUPTS_PULLUP_ENABLE)
		#if defined(INTERRUPTS_PULLUP_RA0)
			WPUA0 = 1;
		#else
			WPUA0 = 0;
		#endif
		#if defined(INTERRUPTS_PULLUP_RA1)
			WPUA1 = 1;
		#else
			WPUA1 = 0;
		#endif
		#if defined(INTERRUPTS_PULLUP_RA2)
			WPUA2 = 1;
		#else
			WPUA2 = 0;
		#endif
		#if defined(INTERRUPTS_PULLUP_RA3)
			WPUA3 = 1;
		#else
			WPUA3 = 0;
		#endif
		#if defined(INTERRUPTS_PULLUP_RA4)
			WPUA4 = 1;
		#else
			WPUA4 = 0;
		#endif
		#if defined(INTERRUPTS_PULLUP_RA5)
			WPUA5 = 1;
		#else
			WPUA5 = 0;
		#endif
	#endif //defined(INTERRUPTS_PULLUP_ENABLE)
	
	/* INT */
	#if defined(INTERRUPTS_INT_ENABLE)
		INTE = 1;
		#if defined(INTERRUPTS_INT_RISINGEDGE)
			INTEDG = 1;
		#else
			INTEDG = 0;
		#endif
	#else
		INTE = 0;
	#endif

	/***** End Core Interrupt Services *****/

	
	/***** Peripheral Interrupt Services *****/
	
	/* Timer 1 Overflow */
	#if defined(INTERRUPTS_TMR1_ENABLE)
		TMR1IE = 1;
	#else
		TMR1IE = 0;
	#endif

	/* Timer 2 Overflow */
	#if defined(INTERRUPTS_TMR2_ENABLE)
		TMR2IE = 1;
	#else
		TMR2IE = 0;
	#endif

	/* CCP1 Interrupt */
	#if defined(INTERRUPTS_CCP1_ENABLE)
		CCP1IE = 1;
	#else
		CCP1IE = 0;
	#endif

	/* MSSP Interrupt */
	#if defined(INTERRUPTS_MSSP_ENABLE)
		SSP1IE = 1;
	#else
		SSP1IE = 0;
	#endif

	/* USART Transmit */
	#if defined(INTERRUPTS_USART_TX_ENABLE)
		TXIE = 1;
	#else
		TXIE = 0;
	#endif

	/* USART Receive */
	#if defined(INTERRUPTS_USART_RX_ENABLE)
		RCIE = 1;
	#else
		RCIE = 0;
	#endif

	/* A/D Converter */
	#if defined(INTERRUPTS_ADC_ENABLE)
		ADIE = 1;
	#else
		ADIE = 0;
	#endif

	/* Timer 1 Gate Interrupt */
	#if defined(INTERRUPTS_TMR1G_ENABLE)
		TMR1GIE = 1;
	#else
		TMR1GIE = 0;
	#endif

	/* MSSP Bus Collision */
	#if defined(INTERRUPTS_MSSP_BUS_ENABLE)
		BCL1IE = 1;
	#else
		BCL1IE = 0;
	#endif

	/* EEPROM Write Completion */
	#if defined(INTERRUPTS_EEPROM_WRITE_ENABLE)
		EEIE = 1;
	#else
		EEIE = 0;
	#endif

	/* Comparator C1 */
	#if defined(INTERRUPTS_C1_ENABLE)
		C1IE = 1;
	#else
		C1IE = 0;
	#endif

	/* Comparator C2 */
	#if defined(INTERRUPTS_C2_ENABLE)
		C2IE = 1;
	#else
		C2IE = 0;
	#endif

	/* Oscillator Fail */
	#if defined(INTERRUPTS_OSC_FAIL_ENABLE)
		OSFIE = 1;
	#else
		OSFIE = 0;
	#endif
	
	/***** End Peripheral Interrupt Services *****/
	
	
	/***** Enable/Disable Global Interrupts *****/
	
	#if defined(INTERRUPTS_PERIPHERAL_ENABLE)
		PEIE = 1;
	#else
		PEIE = 0;
	#endif
	#if defined(INTERRUPTS_GLOBAL_ENABLE)
		GIE = 1;
	#else
		GIE = 0;
	#endif

	/***** End Enable/Disable Global Interrupts *****/

#else //Unknown Compiler
	#error "INTERRUPTS: Unknown compiler!"
#endif //Compiler Check

} //void interrupts_init()
