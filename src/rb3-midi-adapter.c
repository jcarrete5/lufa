#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>

#include "Descriptors.h"
#include "MIDI.h"
#include "HIDReport.h"


#define START_BTN 1
#define START_BTN_PIN PIND
#define START_BTN_PORT PORTD

/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY      LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)


static struct hid_report cur_hid_report = {
    .btns = 0x0000,
    .hat = HAT_NEUTRAL,
    .d1 = {0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00},
    .velocity = {0x00, 0x00, 0x00, 0x00},
    .d2 = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00}
};

/**
 * Buffer to hold the previously generated HID report, for comparison purposes
 * inside the HID class driver.
 */
static uint8_t PrevHIDReportBuffer[sizeof(struct hid_report)];

USB_ClassInfo_HID_Device_t Generic_HID_Interface = {
    .Config = {
        .InterfaceNumber = INTERFACE_ID_GenericHID,
        .ReportINEndpoint = {
            .Address = GENERIC_IN_EPADDR,
            .Size = GENERIC_EPSIZE,
            .Banks = 1,
        },
        .PrevReportINBuffer = PrevHIDReportBuffer,
        .PrevReportINBufferSize = sizeof(PrevHIDReportBuffer),
    },
};

void PinTask(void)
{
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

void USART_Init(void)
{
    /* Set baud rate */
    #define BAUD MIDI_BAUD_RATE
    #include <util/setbaud.h>
    UBRR1 = UBRR_VALUE;
    #undef BAUD

    /* Enable USART receiver and receiver interrupts */
    UCSR1B |= _BV(RXEN1) | _BV(RXCIE1);

    /* Configure Rx pin as input */
    PORTD |= _BV(PORTD2);
}

/** USART receiver interrupt to handle incoming MIDI data on the RX pin */
ISR(USART1_RX_vect)
{
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
	bool ConfigSuccess = 1;
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

bool CALLBACK_HID_Device_CreateHIDReport(
    USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
    uint8_t* const ReportID,
    const uint8_t ReportType,
    void* ReportData,
    uint16_t* const ReportSize)
{
    memcpy(ReportData, &cur_hid_report, sizeof(cur_hid_report));
	*ReportSize = sizeof(struct hid_report);
    HIDReport_Age(&cur_hid_report);
	return true;
}

void CALLBACK_HID_Device_ProcessHIDReport(
    USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
    const uint8_t ReportID,
    const uint8_t ReportType,
    const void* ReportData,
    const uint16_t ReportSize)
{
    // TODO: Maybe signal something?
}

/** Configures the board hardware and chip peripherals */
void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~_BV(WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    USART_Init();
    LEDs_Init();
    USB_Init();

    /* Configure input pins */
    START_BTN_PORT |= _BV(START_BTN);
}

int main(void)
{
    SetupHardware();
    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
    GlobalInterruptEnable();
    for (;;) {
        HID_Device_USBTask(&Generic_HID_Interface);
        PinTask();
        MIDI_Task(&cur_hid_report);
    }
}
