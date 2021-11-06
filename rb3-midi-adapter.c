/*
             LUFA Library
     Copyright (C) Dean Camera, 2021.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2021  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  This file contains the main tasks of the application and is responsible for
 *  the initial application hardware configuration.
 */

#include "rb3-midi-adapter.h"

static struct hid_report cur_hid_report = {
    .btns = 0x0000,
    .hat = HAT_NEUTRAL,
    .d1 = {0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00},
    .velocity = {0x00, 0x00, 0x00, 0x00},
    .d2 = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00}
};

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevHIDReportBuffer[sizeof(struct hid_report)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
{
    .Config =
        {
            .InterfaceNumber              = INTERFACE_ID_GenericHID,
            .ReportINEndpoint             =
                {
                    .Address              = GENERIC_IN_EPADDR,
                    .Size                 = GENERIC_EPSIZE,
                    .Banks                = 1,
                },
            .PrevReportINBuffer           = PrevHIDReportBuffer,
            .PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
        },
};

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
    SetupHardware();

    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
    GlobalInterruptEnable();

    for (;;)
    {
        HID_Device_USBTask(&Generic_HID_Interface);
        PinTask();
        MIDI_Task(&cur_hid_report);
    }
}

void PinTask(void) {
    static int prev = 0;
    int cur = (START_BTN_PIN & _BV(START_BTN)) > 0 ? 0 : 1;
    if (cur == prev) return;
    prev = cur;

    if (cur == 1) {
        HIDReport_SetStartBtn(&cur_hid_report);
    } else {
        HIDReport_ClearStartBtn(&cur_hid_report);
    }
}

/** Configures the board hardware and chip peripherals */
void SetupHardware(void) {
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    USART_Init();
    LEDs_Init();
    USB_Init();

    /* Setup input pins */
    START_BTN_PORT |= _BV(START_BTN);
}

void USART_Init() {
    #define BAUD MIDI_BAUD_RATE
    #include <util/setbaud.h>
    /* Set baud rate */
    UBRR1 = UBRR_VALUE;
    #undef BAUD

    /* Enable USART receiver and receiver interrupts */
    UCSR1B |= _BV(RXEN1) | _BV(RXCIE1);

    /* Configure Rx pin as input */
    PORTD |= _BV(PORTD2);
}

/** USART receiver interrupt to handle incoming MIDI data on the RX pin */
ISR(USART1_RX_vect) {
    MIDI_EnqueueByte((uint8_t)UDR1);
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

	USB_Device_EnableSOFEvents();

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
    memcpy(ReportData, &cur_hid_report, sizeof(cur_hid_report));
	*ReportSize = sizeof(struct hid_report);
    HIDReport_Age(&cur_hid_report);
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    // TODO: Maybe signal something?
}
