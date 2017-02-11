/*******************************************************************************
  USB Status Indicator LED

  Company:
    Microchip Technology Inc.

  File Name:
    led_usb_status.c

  Summary:
    Indicates the USB device status to the user via an LED.

  Description:
    Indicates the USB device status to the user via an LED.
    * The LED is turned off for suspend mode.
    * It blinks quickly with 50% on time when configured
    * It blinks slowly at a low on time (~5% on, 95% off) for all other states.
*******************************************************************************/

/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

#include <stdint.h>

#include "system.h"
#include "usb_device.h"
#include "io_mapping.h"

void APP_LEDUpdateUSBStatus(void)
{
    static uint16_t ledCount = 0;

    if(USBIsDeviceSuspended() == true)
    {
        LED_Off(LED_USB_DEVICE_STATE);
        LED_Off(LED_USB_SERIAL1_ACTIVITY);
        LED_Off(LED_USB_SERIAL2_ACTIVITY);
        return;
    }

    switch(USBGetDeviceState())
    {
        case CONFIGURED_STATE:
            LED_On(LED_USB_DEVICE_STATE);
            if(ledCount > 50)
            {
                LED_Off(LED_USB_SERIAL1_ACTIVITY);
                LED_Off(LED_USB_SERIAL2_ACTIVITY);
                ledCount = 0;
            }
            break;

        default:
            LED_Off(LED_USB_DEVICE_STATE);
            LED_Off(LED_USB_SERIAL1_ACTIVITY);
            LED_Off(LED_USB_SERIAL2_ACTIVITY);
            break;
    }

    /* Increment the millisecond counter. */
    ledCount++;
}
