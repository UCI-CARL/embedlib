/*************************************************************************//**
 * @file interrupts.h
 * 
 * @brief Interrupt library header.
 * 
 * @createdon{8/5/2010}
 * @createdby{Liam Bucci}
 * @projectname{PIC C Library}
 * @projecttype{PIC C Embedded Library}
 * @subprojectname{Interrupts}
 * @subprojecttype{Interrupt Library}
 * @subprojectdescription
 * Embedded library for PIC MCUs (written in C and asm) which sets and
 * configures chip specific interrupt settings depending on the project's
 * "interrupts.def" file. It also defines macros which can be used to access
 * and control interrupt settings and flags.
 * 
 * @pcbnum{N/A}
 * @pcbname{N/A}
 * @pcbdescription
 * N/A
 * 
 * @notes
 * 
 * @todo Add ECAN interrupt services (C18)
 * @todo Add priorities for peripheral interrupts (C18)
 * @todo Add initialization functions (C30)
 ****************************************************************************/

/* Enable/Disable/Priority/Flag Macros *****************************
 * 
 * Enable Macros: The enable macros, when called will set the enable bit for
 * the specific interrupt for which they are written.
 *
 * Disable Macros: The disable macros, when called with clear the enable bit
 * for the specific interrupt for which they are written.
 *
 * Priority Macros: The priority macros take a value from 0-7 (anything above
 * will be removed and only the lowest 3 bits will be used). The value passed
 * in will be used to set the priority level of the specific interrupt.
 *
 * Flag Macros: The flag macros return the flag bit of the specific
 * interrupt for which they are written. 1 means the interrupt has occurred,
 * and 0 means it has not. These macros can also be used to set the flag
 * bits (e.g. INTERRUPTS_FLG_DMA0()=0).
 *
 ***************************************************************************/

#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H


#if defined(__C30__) //Compiler Check

/**
 * @defgroup C30InterruptsLibrary C30 Interrupts Library
 * 
 * @{
 */

/* Check for valid processor */
#if defined(__dsPIC33F__)
	#include <p33Fxxxx.h>
#elif defined(__PIC24H__)
	#include <p24Hxxxx.h>
#else
	#error "INTERRUPTS: Unknown processor type for C30 Compiler (Requires dsPIC33F or PIC24H!!!)"
#endif


/**
 * @name DMAInterruptMacros
 * DMA Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_DMA0()	IEC0bits.DMA0IE=1			/**< Enable DMA channel 0 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA1()	IEC0bits.DMA1IE=1			/**< Enable DMA channel 1 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA2()	IEC1bits.DMA2IE=1			/**< Enable DMA channel 2 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA3()	IEC2bits.DMA3IE=1			/**< Enable DMA channel 3 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA4()	IEC2bits.DMA4IE=1			/**< Enable DMA channel 4 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA5()	IEC3bits.DMA5IE=1			/**< Enable DMA channel 5 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA6()	IEC4bits.DMA6IE=1			/**< Enable DMA channel 6 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_EN_DMA7()	IEC4bits.DMA7IE=1			/**< Enable DMA channel 7 data transfer complete interrupt @hideinitializer */

#define INTERRUPTS_DIS_DMA0()	IEC0bits.DMA0IE=0			/**< Disable DMA channel 0 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA1()	IEC0bits.DMA1IE=0			/**< Disable DMA channel 1 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA2()	IEC1bits.DMA2IE=0			/**< Disable DMA channel 2 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA3()	IEC2bits.DMA3IE=0			/**< Disable DMA channel 3 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA4()	IEC2bits.DMA4IE=0			/**< Disable DMA channel 4 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA5()	IEC3bits.DMA5IE=0			/**< Disable DMA channel 5 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA6()	IEC4bits.DMA6IE=0			/**< Disable DMA channel 6 data transfer complete interrupt @hideinitializer */
#define INTERRUPTS_DIS_DMA7()	IEC4bits.DMA7IE=0			/**< Disable DMA channel 7 data transfer complete interrupt @hideinitializer */

#define INTERRUPTS_PRI_DMA0(x)	IPC1bits.DMA0IP=(x)			/**< Set DMA channel 0 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA1(x)	IPC3bits.DMA1IP=(x)			/**< Set DMA channel 1 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA2(x)	IPC6bits.DMA2IP=(x)			/**< Set DMA channel 2 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA3(x)	IPC9bits.DMA3IP=(x)			/**< Set DMA channel 3 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA4(x)	IPC11bits.DMA4IP=(x)		/**< Set DMA channel 4 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA5(x)	IPC15bits.DMA5IP=(x)		/**< Set DMA channel 5 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA6(x)	IPC17bits.DMA6IP=(x)		/**< Set DMA channel 6 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_DMA7(x)	IPC17bits.DMA7IP=(x)		/**< Set DMA channel 7 data transfer interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_DMA0()	IFS0bits.DMA0IF				/**< Get DMA channel 0 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA1()	IFS0bits.DMA1IF				/**< Get DMA channel 1 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA2()	IFS1bits.DMA2IF				/**< Get DMA channel 2 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA3()	IFS2bits.DMA3IF				/**< Get DMA channel 3 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA4()	IFS2bits.DMA4IF				/**< Get DMA channel 4 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA5()	IFS3bits.DMA5IF				/**< Get DMA channel 5 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA6()	IFS4bits.DMA6IF				/**< Get DMA channel 6 data transfer interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_DMA7()	IFS4bits.DMA7IF				/**< Get DMA channel 7 data transfer interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name UARTInterruptMacros
 * UART Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_U1TX()			IEC0bits.U1TXIE=1			/**< Enable UART1 transmitter interrupt @hideinitializer */
#define INTERRUPTS_EN_U1RX()			IEC0bits.U1RXIE=1			/**< Enable UART1 receiver interrupt @hideinitializer */
#define INTERRUPTS_EN_U1ERROR()			IEC4bits.U1EIE=1			/**< Enable UART1 error interrupt @hideinitializer */
#define INTERRUPTS_EN_U2TX()			IEC1bits.U2TXIE=1			/**< Enable UART2 transmitter interrupt @hideinitializer */
#define INTERRUPTS_EN_U2RX()			IEC1bits.U2RXIE=1			/**< Enable UART2 receiver interrupt @hideinitializer */
#define INTERRUPTS_EN_U2ERROR()			IEC4bits.U2EIE=1			/**< Enable UART2 error interrupt @hideinitializer */

#define INTERRUPTS_DIS_U1TX()			IEC0bits.U1TXIE=0			/**< Disable UART1 transmitter interrupt @hideinitializer */
#define INTERRUPTS_DIS_U1RX()			IEC0bits.U1RXIE=0			/**< Disable UART1 receiver interrupt @hideinitializer */
#define INTERRUPTS_DIS_U1ERROR()		IEC4bits.U1EIE=0			/**< Disable UART1 error interrupt @hideinitializer */
#define INTERRUPTS_DIS_U2TX()			IEC1bits.U2TXIE=0			/**< Disable UART2 transmitter interrupt @hideinitializer */
#define INTERRUPTS_DIS_U2RX()			IEC1bits.U2RXIE=0			/**< Disable UART2 receiver interrupt @hideinitializer */
#define INTERRUPTS_DIS_U2ERROR()		IEC4bits.U2EIE=0			/**< Disable UART2 error interrupt @hideinitializer */

#define INTERRUPTS_PRI_U1TX(x)			IPC3bits.U1TXIP=(x)			/**< Set UART1 transmitter interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_U1RX(x)			IPC2bits.U1RXIP=(x)			/**< Set UART1 receiver interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_U1ERROR(x)		IPC16bits.U1EIP=(x)			/**< Set UART1 error interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_U2TX(x)			IPC7bits.U2TXIP=(x)			/**< Set UART2 transmitter interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_U2RX(x)			IPC7bits.U2RXIP=(x)			/**< Set UART2 receiver interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_U2ERROR(x)		IPC16bits.U2EIP=(x)			/**< Set UART2 error interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_U1TX()			IFS0bits.U1TXIF				/**< Get UART1 transmitter interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_U1RX()			IFS0bits.U1RXIF				/**< Get UART1 receiver interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_U1ERROR()		IFS4bits.U1EIF				/**< Get UART1 error interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_U2TX()			IFS1bits.U2TXIF				/**< Get UART2 transmitter interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_U2RX()			IFS1bits.U2RXIF				/**< Get UART2 receiver interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_U2ERROR()		IFS4bits.U2EIF				/**< Get UART2 error interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name TimerInterruptMacros
 * Timer Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_TIMER1()		IEC0bits.T1IE=1			/**< Enable Timer1 overflow interrupt @hideinitializer */
#define INTERRUPTS_EN_TIMER2()		IEC0bits.T2IE=1			/**< Enable Timer2 overflow interrupt @hideinitializer */
#define INTERRUPTS_EN_TIMER3()		IEC0bits.T3IE=1			/**< Enable Timer3 overflow interrupt @hideinitializer */
#define INTERRUPTS_EN_TIMER4()		IEC1bits.T4IE=1			/**< Enable Timer4 overflow interrupt @hideinitializer */
#define INTERRUPTS_EN_TIMER5()		IEC1bits.T5IE=1			/**< Enable Timer5 overflow interrupt @hideinitializer */

#define INTERRUPTS_DIS_TIMER1()		IEC0bits.T1IE=0			/**< Disable Timer1 overflow interrupt @hideinitializer */
#define INTERRUPTS_DIS_TIMER2()		IEC0bits.T2IE=0			/**< Disable Timer2 overflow interrupt @hideinitializer */
#define INTERRUPTS_DIS_TIMER3()		IEC0bits.T3IE=0			/**< Disable Timer3 overflow interrupt @hideinitializer */
#define INTERRUPTS_DIS_TIMER4()		IEC1bits.T4IE=0			/**< Disable Timer4 overflow interrupt @hideinitializer */
#define INTERRUPTS_DIS_TIMER5()		IEC1bits.T5IE=0			/**< Disable Timer5 overflow interrupt @hideinitializer */

#define INTERRUPTS_PRI_TIMER1(x)	IPC0bits.T1IP=(x)		/**< Set Timer1 overflow interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_TIMER2(x)	IPC1bits.T2IP=(x)		/**< Set Timer2 overflow interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_TIMER3(x)	IPC2bits.T3IP=(x)		/**< Set Timer3 overflow interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_TIMER4(x)	IPC6bits.T4IP=(x)		/**< Set Timer4 overflow interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_TIMER5(x)	IPC7bits.T5IP=(x)		/**< Set Timer5 overflow interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_TIMER1()		IFS0bits.T1IF			/**< Get Timer1 overflow interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_TIMER2()		IFS0bits.T2IF			/**< Get Timer2 overflow interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_TIMER3()		IFS0bits.T3IF			/**< Get Timer3 overflow interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_TIMER4()		IFS1bits.T4IF			/**< Get Timer4 overflow interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_TIMER5()		IFS1bits.T5IF			/**< Get Timer5 overflow interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name ExternalInterruptMacros
 * External Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_INT0()	IEC0bits.INT0IE=1			/**< Enable INT0 external interrupt @hideinitializer */
#define INTERRUPTS_EN_INT1()	IEC1bits.INT1IE=1			/**< Enable INT1 external interrupt @hideinitializer */
#define INTERRUPTS_EN_INT2()	IEC1bits.INT2IE=1			/**< Enable INT2 external interrupt @hideinitializer */

#define INTERRUPTS_DIS_INT0()	IEC0bits.INT0IE=0			/**< Disable INT0 external interrupt @hideinitializer */
#define INTERRUPTS_DIS_INT1()	IEC1bits.INT1IE=0			/**< Disable INT1 external interrupt @hideinitializer */
#define INTERRUPTS_DIS_INT2()	IEC1bits.INT2IE=0			/**< Disable INT2 external interrupt @hideinitializer */

#define INTERRUPTS_PRI_INT0(x)	IPC0bits.INT0IP=(x)			/**< Set INT0 external interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_INT1(x)	IPC1bits.INT1IP=(x)			/**< Set INT1 external interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_INT2(x)	IPC1bits.INT2IP=(x)			/**< Set INT2 external interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_INT0()	IFS0bits.INT0IF				/**< Get INT0 external interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_INT1()	IFS1bits.INT1IF				/**< Get INT1 external interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_INT2()	IFS1bits.INT2IF				/**< Get INT2 external interrupt flag @hideinitializer */

#define INTERRUPTS_EDG_INT0_NEG	INTCON2bits.INT0EP=1		/**< Set INT0 external interrupt to negative trigger @hideinitializer */
#define INTERRUPTS_EDG_INT0_POS	INTCON2bits.INT0EP=0		/**< Set INT0 external interrupt to positive trigger @hideinitializer */
#define INTERRUPTS_EDG_INT1_NEG	INTCON2bits.INT1EP=1		/**< Set INT1 external interrupt to negative trigger @hideinitializer */
#define INTERRUPTS_EDG_INT1_POS	INTCON2bits.INT1EP=0		/**< Set INT1 external interrupt to positive trigger @hideinitializer */
#define INTERRUPTS_EDG_INT2_NEG	INTCON2bits.INT2EP=1		/**< Set INT2 external interrupt to negative trigger @hideinitializer */
#define INTERRUPTS_EDG_INT2_POS	INTCON2bits.INT2EP=0		/**< Set INT2 external interrupt to positive trigger @hideinitializer */

/**
 * @}
 */

/**
 * @name ADConverterInterruptMacros
 * A/D Converter Interrupt Macros
 * @{
 */
#define INTERRUPTS_EN_AD1()		IEC0bits.AD1IE=1			/**< Enable AD1 conversion complete interrupt @hideinitializer */

#define INTERRUPTS_DIS_AD1()	IEC0bits.AD1IE=0			/**< Disable AD1 conversion complete interrupt @hideinitializer */

#define INTERRUPTS_PRI_AD1(x)	IPC3bits.AD1IP=(x)			/**< Set AD1 conversion complete interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_AD1()	IFS0bits.AD1IF				/**< Get AD1 conversion complete interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name OutputCompareInterruptMacros
 * Output Compare Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_OC1()		IEC0bits.OC1IE=1			/**< Enable Output Compare 1 interrupt @hideinitializer */
#define INTERRUPTS_EN_OC2()		IEC0bits.OC2IE=1			/**< Enable Output Compare 2 interrupt @hideinitializer */
#define INTERRUPTS_EN_OC3()		IEC1bits.OC3IE=1			/**< Enable Output Compare 3 interrupt @hideinitializer */
#define INTERRUPTS_EN_OC4()		IEC1bits.OC4IE=1			/**< Enable Output Compare 4 interrupt @hideinitializer */

#define INTERRUPTS_DIS_OC1()	IEC0bits.OC1IE=0			/**< Disable Output Compare 1 interrupt @hideinitializer */
#define INTERRUPTS_DIS_OC2()	IEC0bits.OC2IE=0			/**< Disable Output Compare 2 interrupt @hideinitializer */
#define INTERRUPTS_DIS_OC3()	IEC1bits.OC3IE=0			/**< Disable Output Compare 3 interrupt @hideinitializer */
#define INTERRUPTS_DIS_OC4()	IEC1bits.OC4IE=0			/**< Disable Output Compare 4 interrupt @hideinitializer */

#define INTERRUPTS_PRI_OC1(x)	IPC0bits.OC1IP=(x)			/**< Set Output Compare 1 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_OC2(x)	IPC1bits.OC2IP=(x)			/**< Set Output Compare 2 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_OC3(x)	IPC6bits.OC3IP=(x)			/**< Set Output Compare 3 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_OC4(x)	IPC6bits.OC4IP=(x)			/**< Set Output Compare 4 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_OC1()	IFS0bits.OC1IF				/**< Get Output Compare 1 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_OC2()	IFS0bits.OC2IF				/**< Get Output Compare 2 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_OC3()	IFS1bits.OC3IF				/**< Get Output Compare 3 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_OC4()	IFS1bits.OC4IF				/**< Get Output Compare 4 interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name InputCaptureInterruptMacros
 * Input Capture Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_IC1()		IEC0bits.IC1IE=1			/**< Enable Input Capture channel 1 interrupt @hideinitializer */
#define INTERRUPTS_EN_IC2()		IEC0bits.IC2IE=1			/**< Enable Input Capture channel 2 interrupt @hideinitializer */
#define INTERRUPTS_EN_IC7()		IEC1bits.IC7IE=1			/**< Enable Input Capture channel 7 interrupt @hideinitializer */
#define INTERRUPTS_EN_IC8()		IEC1bits.IC8IE=1			/**< Enable Input Capture channel 8 interrupt @hideinitializer */

#define INTERRUPTS_DIS_IC1()	IEC0bits.IC1IE=0			/**< Disable Input Capture channel 1 interrupt @hideinitializer */
#define INTERRUPTS_DIS_IC2()	IEC0bits.IC2IE=0			/**< Disable Input Capture channel 2 interrupt @hideinitializer */
#define INTERRUPTS_DIS_IC7()	IEC1bits.IC3IE=0			/**< Disable Input Capture channel 7 interrupt @hideinitializer */
#define INTERRUPTS_DIS_IC8()	IEC1bits.IC4IE=0			/**< Disable Input Capture channel 8 interrupt @hideinitializer */

#define INTERRUPTS_PRI_IC1(x)	IPC0bits.IC1IP=(x)			/**< Set Input Capture channel 1 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_IC2(x)	IPC1bits.IC2IP=(x)			/**< Set Input Capture channel 2 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_IC7(x)	IPC5bits.IC7IP=(x)			/**< Set Input Capture channel 7 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_IC8(x)	IPC5bits.IC8IP=(x)			/**< Set Input Capture channel 8 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_IC1()	IFS0bits.IC1IF				/**< Get Input Capture channel 1 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_IC2()	IFS0bits.IC2IF				/**< Get Input Capture channel 2 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_IC7()	IFS1bits.IC7IF				/**< Get Input Capture channel 7 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_IC8()	IFS1bits.IC8IF				/**< Get Input Capture channel 8 interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name ChangeNotificationInterruptMacros
 * Change Notification Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_CN()		IEC1bits.CNIE=1			/**< Enable Change Notification interrupt @hideinitializer */

#define INTERRUPTS_DIS_CN()		IEC1bits.CNIE=0			/**< Disable Change Notification interrupt @hideinitializer */

#define INTERRUPTS_PRI_CN(x)	IPC4bits.CNIP=(x)		/**< Set Change Notification interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_CN()		IFS1bits.CNIF			/**< Get Change Notification interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name ComparatorInterruptMacros
 * Comparator Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_CM()		IEC1bits.CMIE=1			/**< Enable Comparator interrupt @hideinitializer */

#define INTERRUPTS_DIS_CM()		IEC1bits.CMIE=0			/**< Disable Comparator interrupt @hideinitializer */

#define INTERRUPTS_PRI_CM(x)	IPC4bits.CMIP=(x)		/**< Set Comparator interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_CM()		IFS1bits.CMIF			/**< Get Comparator interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name I2CInterruptMacros
 * I2C Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_MI2C()	IEC1bits.MI2C1IE=1			/**< Enable I2C1 Master Events interrupt @hideinitializer */
#define INTERRUPTS_EN_SI2C()	IEC1bits.SI2C1IE=1			/**< Enable I2C1 Slave Events interrupt @hideinitializer */

#define INTERRUPTS_DIS_MI2C()	IEC1bits.MI2C1IE=0			/**< Disable I2C Master Events interrupt @hideinitializer */
#define INTERRUPTS_DIS_SI2C()	IEC1bits.SI2C1IE=0			/**< Disable I2C Slave Events interrupt @hideinitializer */

#define INTERRUPTS_PRI_MI2C(x)	IPC4bits.MI2C1IP=(x)		/**< Set I2C Master Events interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_SI2C(x)	IPC4bits.SI2C1IP=(x)		/**< Set I2C Slave Events interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_MI2C()	IFS1bits.MI2C1IF			/**< Get I2C Master Events interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_SI2C()	IFS1bits.SI2C1IF			/**< Get I2C Slave Events interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name ParallelMasterPortInterruptMacros
 * Parallel Master Port Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_PMP()		IEC2bits.PMPIE=1			/**< Enable Parallel Master Port interrupt @hideinitializer */

#define INTERRUPTS_DIS_PMP()	IEC2bits.PMPIE=0			/**< Disable Parallel Master Port interrupt @hideinitializer */

#define INTERRUPTS_PRI_PMP(x)	IPC11bits.PMPIP=(x)			/**< Set Parallel Master Port interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_PMP()	IFS2bits.PMPIF				/**< Get Parallel Master Port interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name QuadratureEncoderInterfaceInterruptMacros
 * Quadrature Encoder Interface Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_QEI1()	IEC3bits.QEI1IE=1			/**< Enable Quadrature Encoder Interface 1 interrupt @hideinitializer */
#define INTERRUPTS_EN_QEI2()	IEC4bits.QEI2IE=1			/**< Enable Quadrature Encoder Interface 2 interrupt @hideinitializer */

#define INTERRUPTS_DIS_QEI1()	IEC3bits.QEI1IE=0			/**< Disable Quadrature Encoder Interface 1 interrupt @hideinitializer */
#define INTERRUPTS_DIS_QEI2()	IEC4bits.QEI2IE=0			/**< Disable Quadrature Encoder Interface 2 interrupt @hideinitializer */

#define INTERRUPTS_PRI_QEI1(x)	IPC14bits.QEI1IP=(x)		/**< Set Quadrature Encoder Interface 1 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_QEI2(x)	IPC18bits.QEI2IP=(x)		/**< Set Quadrature Encoder Interface 2 interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_QEI1()	IFS3bits.QEI1IF				/**< Get Quadrature Encoder Interface 1 interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_QEI2()	IFS4bits.QEI2IF				/**< Get Quadrature Encoder Interface 2 interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name PWMInterruptMacros
 * PWM Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_PWM1()	IEC3bits.PWM1IE=1				/**< Enable PWM1 Error interrupt @hideinitializer */
#define INTERRUPTS_EN_FLTA1()	IEC3bits.FLTA1IE=1				/**< Enable PWM1 Fault A interrupt @hideinitializer */
#define INTERRUPTS_EN_PWM2()	IEC4bits.PWM2IE=1				/**< Enable PWM2 Error interrupt @hideinitializer */
#define INTERRUPTS_EN_FLTA2()	IEC4bits.FLTA2IE=1				/**< Enable PWM2 Fault A interrupt @hideinitializer */

#define INTERRUPTS_DIS_PWM1()	IEC3bits.PWM1IE=0				/**< Disable PWM1 Error interrupt @hideinitializer */
#define INTERRUPTS_DIS_FLTA1()	IEC3bits.FLTA1IE=0				/**< Disable PWM1 Fault A interrupt @hideinitializer */
#define INTERRUPTS_DIS_PWM2()	IEC4bits.PWM2IE=0				/**< Disable PWM2 Error interrupt @hideinitializer */
#define INTERRUPTS_DIS_FLTA2()	IEC4bits.FLTA2IE=0				/**< Disable PWM2 Fault A interrupt @hideinitializer */

#define INTERRUPTS_PRI_PWM1(x)	IPC14bits.PWM1IP=(x)			/**< Set PWM1 Error interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_FLTA1(x)	IPC15bits.FLTA1IP=1(x)			/**< Set PWM1 Fault A interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_PWM2(x)	IPC18bits.PWM2IP=1(x)			/**< Set PWM2 Error interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_FLTA2(x)	IPC18bits.FLTA2IP=1(x)			/**< Set PWM2 Fault A interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_PWM1()	IFS3bits.PWM1IF					/**< Get PWM1 Error interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_FLTA1()	IFS3bits.FLTA1IF				/**< Get PWM1 Fault A interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_PWM2()	IFS4bits.PWM2IF					/**< Get PWM2 Error interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_FLTA2()	IFS4bits.FLTA2IF				/**< Get PWM2 Fault A interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name ECANInterruptMacros
 * ECAN Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_ECAN1TX()			IEC4bits.C1TXIE=1			/**< Enable ECAN1 Transmitter interrupt @hideinitializer */
#define INTERRUPTS_EN_ECAN1RX()			IEC2bits.C1RXIE=1			/**< Enable ECAN1 Receiver interrupt @hideinitializer */
#define INTERRUPTS_EN_ECAN1_EVENT()		IEC2bits.C1IE=1				/**< Enable ECAN1 Event interrupt @hideinitializer */

#define INTERRUPTS_DIS_ECAN1TX()		IEC4bits.C1TXIE=0			/**< Disable ECAN1 Transmitter interrupt @hideinitializer */
#define INTERRUPTS_DIS_ECAN1RX()		IEC2bits.C1RXIE=0			/**< Disable ECAN1 Receiver interrupt @hideinitializer */
#define INTERRUPTS_DIS_ECAN1_EVENT()	IEC2bits.C1IE=0				/**< Disable ECAN1 Event interrupt @hideinitializer */

#define INTERRUPTS_PRI_ECAN1TX(x)		IPC17bits.C1TXIP=(x)		/**< Set ECAN1 Transmitter interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_ECAN1RX(x)		IPC8bits.C1RXIP=(x)			/**< Set ECAN1 Receiver interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_ECAN1_EVENT(x)	IPC8bits.C1IP=(x)			/**< Set ECAN1 Event interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_ECAN1TX()		IFS4bits.C1TXIF				/**< Get ECAN1 Transmitter interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_ECAN1RX()		IFS2bits.C1RXIF				/**< Get ECAN1 Receiver interrupt flag @hideinitializer */
#define INTERRUPTS_FLG_ECAN1_EVENT()	IFS2bits.C1IF				/**< Get ECAN1 Event interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name SPIInterruptMacros
 * SPI Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_SPI1_EVENT()		IEC0bits.SPI1IE=1			/**< Enable SPI1 Event interrupt @hideinitializer */
#define INTERRUPTS_EN_SPI1_ERROR()		IEC0bits.SPI1EIE=1			/**< Enable SPI1 Error interrupt @hideinitializer */
#define INTERRUPTS_EN_SPI2_EVENT()		IEC2bits.SPI2IE=1			/**< Enable SPI2 Event interrupt @hideinitializer */
#define INTERRUPTS_EN_SPI2_ERROR()		IEC2bits.SPI2EIE=1			/**< Enable SPI2 Error interrupt @hideinitializer */

#define INTERRUPTS_DIS_SPI1_EVENT()		IEC0bits.SPI1IE=0			/**< Disable SPI1 Event interrupt @hideinitializer */
#define INTERRUPTS_DIS_SPI1_ERROR()		IEC0bits.SPI1EIE=0			/**< Disable SPI1 Error interrupt @hideinitializer */
#define INTERRUPTS_DIS_SPI2_EVENT()		IEC2bits.SPI2IE=0			/**< Disable SPI2 Event interrupt @hideinitializer */
#define INTERRUPTS_DIS_SPI2_ERROR()		IEC2bits.SPI2EIE=0			/**< Disable SPI2 Error interrupt @hideinitializer */

#define INTERRUPTS_PRI_SPI1_EVENT(x)	IPC2bits.SPI1IP=(x)			/**< Set SPI1 Event interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_SPI1_ERROR(x)	IPC2bits.SPI1EIP=(x)		/**< Set SPI1 Error interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_SPI2_EVENT(x)	IPC8bits.SPI2IP=(x)			/**< Set SPI2 Event interrupt priority @param x Interrupt priority (0-7) @hideinitializer */
#define INTERRUPTS_PRI_SPI2_ERROR(x)	IPC8bits.SPI2EIP=(x)		/**< Set SPI2 Error interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_SPI1_EVENT()		IFS0bits.SPI1IF				/**< Get SPI1 Event interrupt @hideinitializer */
#define INTERRUPTS_FLG_SPI1_ERROR()		IFS0bits.SPI1EIF			/**< Get SPI1 Error interrupt @hideinitializer */
#define INTERRUPTS_FLG_SPI2_EVENT()		IFS2bits.SPI2IF				/**< Get SPI2 Event interrupt @hideinitializer */
#define INTERRUPTS_FLG_SPI2_ERROR()		IFS2bits.SPI2EIF			/**< Get SPI2 Error interrupt @hideinitializer */

/**
 * @}
 */

/**
 * @name RealTimeClockAndCalendarInterruptMacros
 * Real-time Clock and Calendar Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_RTC()		IEC3bits.RTCIE=1			/**< Enable Real-time Clock and Calendar interrupt @hideinitializer */

#define INTERRUPTS_DIS_RTC()	IEC3bits.RTCIE=0			/**< Disable Real-time Clock and Calendar interrupt @hideinitializer */

#define INTERRUPTS_PRI_RTC(x)	IPC15bits.RTCIP=(x)			/**< Set Real-time Clock and Calendar interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_RTC()	IFS3bits.RTCIF				/**< Get Real-time Clock and Calendar interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name CRCGeneratorInterruptMacros
 * CRC Generator Interrupt Macros
 * @{
 */

#define INTERRUPTS_EN_CRC()		IEC4bits.CRCIE=1			/**< Enable CRC Generator interrupt @hideinitializer */

#define INTERRUPTS_DIS_CRC()	IEC4bits.CRCIE=0			/**< Disable CRC Generator interrupt @hideinitializer */

#define INTERRUPTS_PRI_CRC(x)	IPC16bits.CRCIP=(x)			/**< Set CRC Generator interrupt priority @param x Interrupt priority (0-7) @hideinitializer */

#define INTERRUPTS_FLG_CRC()	IFS4bits.CRCIF				/**< Get CRC Generator interrupt flag @hideinitializer */

/**
 * @}
 */

/**
 * @name TrapMacrosAndFlags
 * Trap Macros and Flags
 * @{
 */

#define INTERRUPTS_EN_ACCUMA_OVERFLOW()		INTCON1bits.OVATE=1		/**< Enable Accumulator A Overflow trap @hideinitializer */
#define INTERRUPTS_EN_ACCUMB_OVERFLOW()		INTCON1bits.OVBTE=1		/**< Enable Accumulator B Overflow trap @hideinitializer */
#define INTERRUPTS_EN_CATACCUM_OVERFLOW()	INTCON1bits.COVTE=1		/**< Enable Catastrophic Accumulator Overflow trap @hideinitializer */

#define INTERRUPTS_DIS_ACCUMA_OVERFLOW()	INTCON1bits.OVATE=0		/**< Disable Accumulator A Overflow trap @hideinitializer */
#define INTERRUPTS_DIS_ACCUMB_OVERFLOW()	INTCON1bits.OVBTE=0		/**< Disable Accumulator B Overflow trap @hideinitializer */
#define INTERRUPTS_DIS_CATACCUM_OVERFLOW()	INTCON1bits.COVTE=0		/**< Disable Catastrophic Accumulator Overflow trap @hideinitializer */

#define INTERRUPTS_FLG_ACCUMA_OVERFLOW()	INTCON1bits.OVAERR		/**< Get Accumulator A Overflow trap flag @hideinitializer */
#define INTERRUPTS_FLG_ACCUMB_OVERFLOW()	INTCON1bits.OVBERR		/**< Get Accumulator B Overflow trap flag @hideinitializer */
#define INTERRUPTS_FLG_CATACCUMA_OVERFLOW()	INTCON1bits.COVAERR		/**< Get Catastrophic Accumulator A Overflow trap flag @hideinitializer */
#define INTERRUPTS_FLG_CATACCUMB_OVERFLOW()	INTCON1bits.COVBERR		/**< Get Catastrophic Accumulator B Overflow trap flag @hideinitializer */
#define INTERRUPTS_FLG_MATH_SHFTACCUM()		INTCON1bits.SFTACERR	/**< Get Shift Accumulator Error trap flag @hideinitializer */
#define INTERRUPTS_FLG_MATH_DIV0()			INTCON1bits.DIV0ERR		/**< Get Divide by 0 Error trap flag @hideinitializer */
#define INTERRUPTS_FLG_DMA_ERR()			INTCON1bits.DMACERR		/**< Get DMA Controller Error trap flag @hideinitializer */
#define INTERRUPTS_FLG_MATH_ERR()			INTCON1bits.MATHERR		/**< Get Arithmetic Error trap flag @hideinitializer */
#define INTERRUPTS_FLG_ADDRESS_ERR()		INTCON1bits.ADDRERR		/**< Get Address Error trap flag @hideinitializer */
#define INTERRUPTS_FLG_STACK_ERR()			INTCON1bits.STKERR		/**< Get Stack Error trap flag @hideinitializer */
#define INTERRUPTS_FLG_OSC_FAIL()			INTCON1bits.OSCFAIL		/**< Get Oscillator Failure trap flag @hideinitializer */

/**
 * @}
 */


/**
 * @name InterruptControlMacros
 * Interrupt Control Macros
 * @{
 */

#define INTERRUPTS_EN_NEST()				INTCON1bits.NSTDIS=0				/**< Enable nested interrupts @hideinitializer */
#define INTERRUPTS_DIS_NEST()				INTCON1bits.NSTDIS=1				/**< Disable nested interrupts @hideinitializer */

#define INTERRUPTS_GET_CPUPRIORITY()		(CORCONbits.IPL3<<4 + SRbits.IPL)	/**< Get current CPU priority @return integer, current CPU priority level (0-15) @hideinitializer */
#define INTERRUPTS_SET_CPUPRIORITY(x)		SRbits.IPL=(x)						/**< Set CPU priority @param x New CPU priority level (0-7) @hideinitializer */

#define INTERRUPTS_GET_PENDINGPRIORITY()	INTTREGbits.ILR						/**< Get pending CPU priority level @return integer, pending CPU priority level (0-15) @hideinitializer */
#define INTERRUPTS_GET_PENDINGVECTOR()		INTTREGbits.VECNUM+8				/**< Get pending interrupt vector @return integer, pending interrupt vector number (0-127) @hideinitializer */

#define INTERRUPTS_USE_STD()				INTCON2bits.ALTIVT=0				/**< Use standard interrupt vectors (0x000004-0x0000FE) @hideinitializer */
#define INTERRUPTS_USE_ALT()				INTCON2bits.ALTIVT=1				/**< Use alternate interrupt vectors (0x000104-0x0001FE) @hideinitializer */

#define INTERRUPTS_OFF(k)					__asm__("DISI k")					/**< Turn off interrupts for k cycles, where k is a 14-bit integer @param k @hideinitializer */

/**
 * @}
 */

/**
 * @}
 * End of C30 Interrupts Library group
 */

#elif defined(__18CXX) //Compiler Check

/**
 * @defgroup C18InterruptsLibrary C18 Interrupts Library
 *
 * @{
 */

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
	#error "INTERRUPTS: Invalid/unknown processor for compiler (Microchip C18)!"
#endif //defined(__processor)



/**
 * @name InterruptControlMacros
 * Interrupt Control Macros
 * @{
 */

#define INTERRUPTS_OFF()	INTCONbits.GIE=0	/**< Turn interrupts off globally @hideinitializer */		
#define INTERRUPTS_ON()		INTCONbits.GIE=1	/**< Turn interrupts on globally @hideinitializer */

/**
 * @}
 */

/**
 * @}
 * End of C18 Interrupts Library group
 */

#elif defined(__PICC__) || defined(__PICCLITE__) //Compiler Check

/**
 * @defgroup PICCInterruptsLibrary PIC C Interrupts Library
 * 
 * @{
 */

#include <htc.h>

#if !(defined(_16F1823) || defined(_16LF1823)) //Valid processors for library
	#error "INTERRUPTS: Invalid/unknown processor (REQ: 16F1823 or 16LF1823)!"
#endif //!defined(_processor)



/**
 * @name InterruptControlMacros
 * Interrupt Control Macros
 * @{
 */

#define INTERRUPTS_OFF()	GIE=0	/**< Turn interrupts off globally @hideinitializer */
#define INTERRUPTS_ON()		GIE=1	/**< Turn interrupts on globally @hideinitializer */

/**
 * @}
 */

/**
 * @}
 * End of PIC C Interrupts Library group
 */


#else //Unknown Compiler
	#error "INTERRUPTS: Unsupported compiler!!!"
#endif //Compiler Check

/* See interrupts.c for documentation */
void interrupts_init(void);


#endif //_INTERRUPTS_H
