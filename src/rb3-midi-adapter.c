#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <string.h>

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>

#include "Descriptors.h"
#include "HIDReport.h"
#include "MIDI.h"
#include "PadConfig.h"

#define START_BTN 1
#define START_BTN_PIN PIND
#define START_BTN_PORT PORTD

/** LED mask for the library LED driver, to indicate that the USB interface is
 * not ready. */
#define LEDMASK_USB_NOTREADY LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is
 * enumerating. */
#define LEDMASK_USB_ENUMERATING (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is
 * ready. */
#define LEDMASK_USB_READY (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred
 * in the USB interface. */
#define LEDMASK_USB_ERROR (LEDS_LED1 | LEDS_LED3)

static struct hid_report cur_hid_report = {
  .btns = 0x0000,
  .hat = HAT_NEUTRAL,
  .d1 = { 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00 },
  .velocity = { 0x00, 0x00, 0x00, 0x00 },
  .d2 = { 0x00,
          0x00,
          0x00,
          0x00,
          0x02,
          0x00,
          0x02,
          0x00,
          0x02,
          0x00,
          0x02,
          0x00 }
};

/**
 * Buffer to hold the previously generated HID report, for comparison purposes
 * inside the HID class driver.
 */
static uint8_t prev_hid_report_buffer[sizeof(struct hid_report)];

USB_ClassInfo_HID_Device_t generic_hid_interface = {
    .Config = {
        .InterfaceNumber = INTERFACE_ID_GenericHID,
        .ReportINEndpoint = {
            .Address = GENERIC_IN_EPADDR,
            .Size = GENERIC_EPSIZE,
            .Banks = 1,
        },
        .PrevReportINBuffer = prev_hid_report_buffer,
        .PrevReportINBufferSize = sizeof(prev_hid_report_buffer),
    },
};

void
pin_task(void)
{
  static int prev = 0;
  int cur = (START_BTN_PIN & _BV(START_BTN)) > 0 ? 0 : 1;
  if (cur == prev)
    return;
  prev = cur;

  if (cur == 1) {
    hid_report_set_start_btn(&cur_hid_report);
  } else {
    hid_report_clear_start_btn(&cur_hid_report);
  }
}

void
usart_init(void)
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
  midi_enqueue_byte(UDR1);
}

/** Event handler for the library USB Connection event. */
void
EVENT_USB_Device_Connect(void)
{
  LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void
EVENT_USB_Device_Disconnect(void)
{
  LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void
EVENT_USB_Device_ConfigurationChanged(void)
{
  bool config_success = HID_Device_ConfigureEndpoints(&generic_hid_interface);
  USB_Device_EnableSOFEvents();
  LEDs_SetAllLEDs(config_success ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void
EVENT_USB_Device_ControlRequest(void)
{
  HID_Device_ProcessControlRequest(&generic_hid_interface);
}

/** Event handler for the USB device Start Of Frame event. */
void
EVENT_USB_Device_StartOfFrame(void)
{
  HID_Device_MillisecondElapsed(&generic_hid_interface);
}

bool
CALLBACK_HID_Device_CreateHIDReport(
  [[maybe_unused]] USB_ClassInfo_HID_Device_t* const hid_interface_info,
  [[maybe_unused]] uint8_t* const report_id,
  [[maybe_unused]] const uint8_t report_type,
  void* report_data,
  uint16_t* const report_size)
{
  memcpy(report_data, &cur_hid_report, sizeof(cur_hid_report));
  *report_size = sizeof(struct hid_report);
  hid_report_age(&cur_hid_report);
  return true;
}

void
CALLBACK_HID_Device_ProcessHIDReport(
  [[maybe_unused]] USB_ClassInfo_HID_Device_t* const hid_interface_info,
  [[maybe_unused]] const uint8_t report_id,
  [[maybe_unused]] const uint8_t report_type,
  [[maybe_unused]] const void* report_data,
  [[maybe_unused]] const uint16_t report_size)
{
  // TODO: Maybe signal something?
}

/** Configures the board hardware and chip peripherals */
void
setup_hardware(void)
{
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~_BV(WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);

  /* Hardware Initialization */
  usart_init();
  LEDs_Init();
  USB_Init();

  /* Configure input pins */
  START_BTN_PORT |= _BV(START_BTN);
}

int
main(void)
{
  setup_hardware();
  LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
  GlobalInterruptEnable();
  while (true) {
    HID_Device_USBTask(&generic_hid_interface);
    pin_task();
    midi_task(&cur_hid_report);
  }
}
