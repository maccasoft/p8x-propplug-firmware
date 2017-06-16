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

#include <usart.h>
#include <stdbool.h>
#include <xc.h>
#include <usb_config.h>
#include <usb_device_cdc.h>

/******************************************************************************
 * Function:        void USART_Initialize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine initializes the UART to the value set by Host Terminal Application
 *
 * Note:
 *
 *****************************************************************************/

void USART_Initialize()
{
    unsigned char c;
     
    UART1_TRISRx = 1;       // RX
    UART1_TRISTx = 0;       // TX
    TXSTA1 = 0x24;          // TX enable BRGH=1
    RCSTA1 = 0x90;          // Single Character RX
    BAUDCON1 = 0x08;        // BRG16 = 1
    c = RCREG1;             // read

    UART2_TRISRx = 1;       // RX
    UART2_TRISTx = 0;       // TX
    TXSTA2 = 0x24;          // TX enable BRGH=1
    RCSTA2 = 0x90;          // Single Character RX
    BAUDCON2 = 0x08;        // BRG16 = 1
    c = RCREG2;             // read

}//end USART_Initialize

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
void USART_putcUSART1(char c)
{
    TXREG2 = c;
}

void USART_putcUSART2(char c)
{
    TXREG1 = c;
}


/******************************************************************************
 * Function:        void USART_mySetLineCodingHandler(void)
 *
 * PreCondition:    USB_CDC_SET_LINE_CODING_HANDLER is defined
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
#if defined(USB_CDC_SET_LINE_CODING_HANDLER)
void USART_mySetLineCodingHandler(void)
{
    uint32_t dwBaud;
    unsigned char c;
     
    //Update the baudrate info in the CDC driver
    CDCSetBaudRate(cdc_notice.GetLineCoding.dwDTERate);

    //Update the baudrate of the UART

    dwBaud = ((GetSystemClock() / 4) / line_coding.dwDTERate) - 1;
    SPBRG1 = SPBRG2 = (uint8_t) dwBaud;
    SPBRGH1 = SPBRGH2 = (uint8_t)((uint16_t) (dwBaud >> 8));
        
    c = RCREG1;             // read
    c = RCREG2;             // read
}
#endif

/******************************************************************************
 * Function:        void USART_getcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          unsigned char c - character to received on the UART
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
unsigned char USART_getcUSART1 ()
{
	char  c;

    if (RCSTA2bits.OERR)        // in case of overrun error
    {                           // we should never see an overrun error, but if we do,
        RCSTA2bits.CREN = 0;    // reset the port
        c = RCREG2;
        RCSTA2bits.CREN = 1;    // and keep going.
    }
    else
    {
        c = RCREG2;
    }

	return c;
}

unsigned char USART_getcUSART2 ()
{
	char  c;

    if (RCSTA1bits.OERR)        // in case of overrun error
    {                           // we should never see an overrun error, but if we do,
        RCSTA1bits.CREN = 0;    // reset the port
        c = RCREG1;
        RCSTA1bits.CREN = 1;    // and keep going.
    }
    else
    {
        c = RCREG1;
    }

	return c;
}
