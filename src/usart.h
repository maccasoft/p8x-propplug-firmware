/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#ifndef USART_H
#define USART_H

#include <stdbool.h>

#define CLOCK_FREQ          48000000
#define GetSystemClock()    CLOCK_FREQ

#define UART1_ENABLE        RCSTA1bits.SPEN

#define UART1_TRISTx        TRISCbits.TRISC6
#define UART1_TRISRx        TRISCbits.TRISC7
#define UART1_Tx            PORTCbits.RC6
#define UART1_Rx            PORTCbits.RC7
#define UART1_DTR           LATBbits.LATB0

#define UART2_ENABLE        RCSTA2bits.SPEN

#define UART2_TRISTx        TRISCbits.TRISC1
#define UART2_TRISRx        TRISCbits.TRISC2
#define UART2_Tx            PORTCbits.RC1
#define UART2_Rx            PORTCbits.RC2

#define mInitDTRPin()       { TRISBbits.TRISB0 = 0; }   // Configure DTR as a digital output.


/*********************************************************************
* Function: void USART_Initialize(void);
*
* Overview: Initializes USART (RS-232 port)
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void USART_Initialize();

/******************************************************************************
 * Function:        void USART_putcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           char c - character to print to the UART
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
void USART_putcUSART(int, char);

/******************************************************************************
 * Function:        void USART_mySetLineCodingHandler(void)
 *
 * PreCondition:    USB_CDC_SET_LINE_CODING_HANDLER mySetLineCodingHandler is defined
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function gets called when a SetLineCoding command
 *                  is sent on the bus.  This function will evaluate the request
 *                  and determine if the application should update the baudrate
 *                  or not.
 *
 * Note:
 *
 *****************************************************************************/
void USART_mySetLineCodingHandler(void);

/******************************************************************************
 * Function:        unsigned char USART_getcUSART()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          unsigned char c - character to received on the UART
 *
 * Side Effects:    None
 *
 * Overview:        Get the input character from the UART
 *
 * Note:
 *
 *****************************************************************************/
unsigned char USART_getcUSART(int);

#endif //USART_H
