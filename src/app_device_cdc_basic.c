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

/** INCLUDES *******************************************************/
#include "system.h"

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "usb.h"

#include "app_led_usb_status.h"
#include "app_device_cdc_basic.h"
#include "usb_config.h"
#include "usart.h"
#include "io_mapping.h"

/** VARIABLES ******************************************************/

//static bool buttonPressed;
//static char buttonMessage[] = "Button pressed.\r\n";
static uint8_t USB_Out_Buffer[2][CDC_DATA_OUT_EP_SIZE];
static uint8_t RS232_Out_Data[2][CDC_DATA_IN_EP_SIZE];

static unsigned char  NextUSBOut[2];
static unsigned char  LastRS232Out[2];  // Number of characters in the buffer
static unsigned char  RS232cp[2];       // current position within the buffer
static unsigned char  RS232_Out_Data_Rdy[2];


/*********************************************************************
* Function: void APP_DeviceCDCEmulatorInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCEmulatorInitialize()
{
    CDCInitEP();

    USART_Initialize();

    // Initialize the arrays
    unsigned char i;
	for (i=0; i<CDC_DATA_OUT_EP_SIZE; i++)
    {
		USB_Out_Buffer[0][i] = 0;
        USB_Out_Buffer[1][i] = 0;
    }

	NextUSBOut[0] = NextUSBOut[1] = 0;
	LastRS232Out[0] = LastRS232Out[1] = 0;
	RS232_Out_Data_Rdy[0] = RS232_Out_Data_Rdy[1] = 0;
}


static int mTxRdyUSART(int index)
{
    if (index == 0)
        return TXSTA2bits.TRMT;
    if (index == 1)
        return TXSTA1bits.TRMT;
    return 0;
}

static int mDataRdyUSART(int index)
{
    if (index == 0)
        return PIR3bits.RC2IF;
    if (index == 1)
        return PIR1bits.RC1IF;
    return 0;
}

/*********************************************************************
* Function: void APP_DeviceCDCEmulatorTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCDCEmulatorInitialize() and APP_DeviceCDCEmulatorStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCEmulatorTasks(int index)
{
    
	if (RS232_Out_Data_Rdy[index] == 0)  // only check for new USB buffer if the old RS232 buffer is
	{						      // empty.  This will cause additional USB packets to be NAK'd
		LastRS232Out[index] = getsUSBUSART(index,RS232_Out_Data[index],CDC_DATA_IN_EP_SIZE); //until the buffer is free.
		if(LastRS232Out[index] > 0)
		{
			RS232_Out_Data_Rdy[index] = 1;  // signal buffer full
			RS232cp[index] = 0;  // Reset the current position
            LED_On(index == 0 ? LED_USB_SERIAL1_ACTIVITY : LED_USB_SERIAL2_ACTIVITY);
		}
	}

    // Check if one or more bytes are waiting in the physical UART transmit
    // queue.  If so, send it out the UART TX pin.
    if (RS232_Out_Data_Rdy[index] && mTxRdyUSART(index))
	{
        USART_putcUSART(index, RS232_Out_Data[index][RS232cp[index]]);
        ++RS232cp[index];
        if (RS232cp[index] == LastRS232Out[index])
            RS232_Out_Data_Rdy[index] = 0;
	}

    // Check if we received a character over the physical UART, and we need
    // to buffer it up for eventual transmission to the USB host.
    if (mDataRdyUSART(index) && (NextUSBOut[index] < (CDC_DATA_OUT_EP_SIZE - 1)))
    {
		USB_Out_Buffer[index][NextUSBOut[index]] = USART_getcUSART(index);
		++NextUSBOut[index];
		USB_Out_Buffer[index][NextUSBOut[index]] = 0;
	}

    // Check if any bytes are waiting in the queue to send to the USB host.
    // If any bytes are waiting, and the endpoint is available, prepare to
    // send the USB packet to the host.
	if ((USBUSARTIsTxTrfReady(index)) && (NextUSBOut[index] > 0))
	{
		putUSBUSART(index, &USB_Out_Buffer[index][0], NextUSBOut[index]);
		NextUSBOut[index] = 0;
        LED_On(index == 0 ? LED_USB_SERIAL1_ACTIVITY : LED_USB_SERIAL2_ACTIVITY);
	}
}
