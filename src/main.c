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
#include "system_config.h"

#include "app_device_cdc_basic.h"
#include "app_led_usb_status.h"

#include "usb.h"
#include "usb_device.h"
#include "usb_device_cdc.h"

/********************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
MAIN_RETURN main(void)
{
    SYSTEM_Initialize(SYSTEM_STATE_USB_START);

    // Configure all pins as digital ports
    ANCON0 = 0xFF;
    ANCON1 = 0x1F;
    LATA = LATB = LATC = 0;
        
    // Map peripherals
    EECON2 = 0x55; EECON2 = 0xAA; PPSCONbits.IOLOCK = 0;
    RPINR16 = 13; // Map RP13/RC2 to USART2 RX
    RPOR12 = 6;   // Map RP12/RC1 to USART2 TX
    EECON2 = 0x55; EECON2 = 0xAA; PPSCONbits.IOLOCK = 1;

    USBDeviceInit();
    USBDeviceAttach();
    
    while(1)
    {
        SYSTEM_Tasks();

        /* If the USB device isn't configured yet, we can't really do anything
         * else since we don't have a host to talk to.  So jump back to the
         * top of the while loop. */
        if( USBGetDeviceState() < CONFIGURED_STATE )
        {
            /* Jump back to the top of the while loop. */
            continue;
        }

        /* If we are currently suspended, then we need to see if we need to
         * issue a remote wakeup.  In either case, we shouldn't process any
         * keyboard commands since we aren't currently communicating to the host
         * thus just continue back to the start of the while loop. */
        if( USBIsDeviceSuspended() == true )
        {
            /* Jump back to the top of the while loop. */
            continue;
        }

        //Application specific tasks
        APP_DeviceCDCEmulatorTasks();

        USBMaskInterrupts();

        CDCTxService(0);
        CDCTxService(1);

        USBUnmaskInterrupts();

    }//end while
}//end main

bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size)
{
    switch( (int) event )
    {
        case EVENT_TRANSFER:
            break;

        case EVENT_SOF:
            /* We are using the SOF as a timer to time the LED indicator.  Call
             * the LED update function here. */
            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_SUSPEND:
            /* Update the LED status for the suspend event. */
            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_RESUME:
            /* Update the LED status for the resume event. */
            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_CONFIGURED:
            /* When the device is configured, we can (re)initialize the 
             * demo code. */
            APP_DeviceCDCEmulatorInitialize();
            break;

        case EVENT_SET_DESCRIPTOR:
            break;

        case EVENT_EP0_REQUEST:
            /* We have received a non-standard USB request.  The HID driver
             * needs to check to see if the request was for it. */
            USBCheckCDCRequest();
            break;

        case EVENT_BUS_ERROR:
            break;

        case EVENT_TRANSFER_TERMINATED:
            break;

        default:
            break;
    }
    return true;
}

/*******************************************************************************
 End of File
*/
