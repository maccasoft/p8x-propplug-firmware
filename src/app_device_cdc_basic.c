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

/** VARIABLES SERIAL 1 *********************************************/

static uint8_t USB_Out_Buffer0[CDC_DATA_OUT_EP_SIZE];
static uint8_t RS232_Out_Data0[CDC_DATA_IN_EP_SIZE];

static unsigned char  NextUSBOut0;
static unsigned char  LastRS232Out0;  // Number of characters in the buffer
static unsigned char  RS232cp0;       // current position within the buffer
static unsigned char  RS232_Out_Data_Rdy0;

/** VARIABLES SERIAL 2 *********************************************/

static uint8_t USB_Out_Buffer1[CDC_DATA_OUT_EP_SIZE];
static uint8_t RS232_Out_Data1[CDC_DATA_IN_EP_SIZE];

static unsigned char  NextUSBOut1;
static unsigned char  LastRS232Out1;  // Number of characters in the buffer
static unsigned char  RS232cp1;       // current position within the buffer
static unsigned char  RS232_Out_Data_Rdy1;


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

    //USART_Initialize();

    // Initialize the arrays
    unsigned char i;
	for (i=0; i<CDC_DATA_OUT_EP_SIZE; i++)
    {
		USB_Out_Buffer0[i] = 0;
        USB_Out_Buffer1[i] = 0;
    }

	NextUSBOut0 = NextUSBOut1 = 0;
	LastRS232Out0 = LastRS232Out1 = 0;
	RS232_Out_Data_Rdy0 = RS232_Out_Data_Rdy1 = 0;
}


#define mTxRdyUSART1()      PIR3bits.TX2IF
#define mTxRdyUSART2()      PIR1bits.TX1IF

#define mDataRdyUSART1()    PIR3bits.RC2IF
#define mDataRdyUSART2()    PIR1bits.RC1IF

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
void APP_DeviceCDCEmulatorTasks()
{
    /* Serial 1 ****************************************************/

	if (RS232_Out_Data_Rdy0 == 0)  // only check for new USB buffer if the old RS232 buffer is
	{						      // empty.  This will cause additional USB packets to be NAK'd
		LastRS232Out0 = getsUSBUSART(0,RS232_Out_Data0,CDC_DATA_IN_EP_SIZE); //until the buffer is free.
		if(LastRS232Out0 > 0)
		{
			RS232_Out_Data_Rdy0 = 1;  // signal buffer full
			RS232cp0 = 0;  // Reset the current position
            LED_On(LED_USB_SERIAL1_ACTIVITY);
		}
	}

    // Check if one or more bytes are waiting in the physical UART transmit
    // queue.  If so, send it out the UART TX pin.
    if (RS232_Out_Data_Rdy0 && mTxRdyUSART1())
	{
        USART_putcUSART1(RS232_Out_Data0[RS232cp0]);
        ++RS232cp0;
        if (RS232cp0 == LastRS232Out0)
            RS232_Out_Data_Rdy0 = 0;
	}

    // Check if we received a character over the physical UART, and we need
    // to buffer it up for eventual transmission to the USB host.
    if (mDataRdyUSART1() && (NextUSBOut0 < (CDC_DATA_OUT_EP_SIZE - 1)))
    {
		USB_Out_Buffer0[NextUSBOut0] = USART_getcUSART1();
		++NextUSBOut0;
		USB_Out_Buffer0[NextUSBOut0] = 0;
	}

    // Check if any bytes are waiting in the queue to send to the USB host.
    // If any bytes are waiting, and the endpoint is available, prepare to
    // send the USB packet to the host.
	if ((USBUSARTIsTxTrfReady(0)) && (NextUSBOut0 > 0))
	{
		putUSBUSART(0, &USB_Out_Buffer0[0], NextUSBOut0);
		NextUSBOut0 = 0;
        LED_On(LED_USB_SERIAL1_ACTIVITY);
	}

    /* Serial 2 ****************************************************/

	if (RS232_Out_Data_Rdy1 == 0)  // only check for new USB buffer if the old RS232 buffer is
	{						      // empty.  This will cause additional USB packets to be NAK'd
		LastRS232Out1 = getsUSBUSART(1,RS232_Out_Data1,CDC_DATA_IN_EP_SIZE); //until the buffer is free.
		if(LastRS232Out1 > 0)
		{
			RS232_Out_Data_Rdy1 = 1;  // signal buffer full
			RS232cp1 = 0;  // Reset the current position
            LED_On(LED_USB_SERIAL2_ACTIVITY);
		}
	}

    // Check if one or more bytes are waiting in the physical UART transmit
    // queue.  If so, send it out the UART TX pin.
    if (RS232_Out_Data_Rdy1 && mTxRdyUSART2())
	{
        USART_putcUSART2(RS232_Out_Data1[RS232cp1]);
        ++RS232cp1;
        if (RS232cp1 == LastRS232Out1)
            RS232_Out_Data_Rdy1 = 0;
	}

    // Check if we received a character over the physical UART, and we need
    // to buffer it up for eventual transmission to the USB host.
    if (mDataRdyUSART2() && (NextUSBOut1 < (CDC_DATA_OUT_EP_SIZE - 1)))
    {
		USB_Out_Buffer1[NextUSBOut1] = USART_getcUSART2();
		++NextUSBOut1;
		USB_Out_Buffer1[NextUSBOut1] = 0;
	}

    // Check if any bytes are waiting in the queue to send to the USB host.
    // If any bytes are waiting, and the endpoint is available, prepare to
    // send the USB packet to the host.
	if ((USBUSARTIsTxTrfReady(1)) && (NextUSBOut1 > 0))
	{
		putUSBUSART(1, &USB_Out_Buffer1[0], NextUSBOut1);
		NextUSBOut1 = 0;
        LED_On(LED_USB_SERIAL2_ACTIVITY);
	}
}
