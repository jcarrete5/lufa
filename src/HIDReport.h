#ifndef HID_REPORT_H_
#define HID_REPORT_H_

#include <stdint.h>

#include "PadConfig.h"

struct hid_report
{
  uint16_t btns;
  uint8_t hat;
  uint8_t d1[8];
  uint8_t velocity[4];
  uint8_t d2[12];
};

void
hid_report_set(struct hid_report* report, struct midi_mapping map, uint8_t vel);

void
hid_report_clear(struct hid_report* report, struct midi_mapping map);

void
hid_report_age(struct hid_report* report);

void
hid_report_set_start_btn(struct hid_report* r);

void
hid_report_clear_start_btn(struct hid_report* r);

#endif /* HID_REPORT_H_ */
