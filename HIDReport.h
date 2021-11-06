#ifndef HID_REPORT_H
#define HID_REPORT_H

#include <stdio.h>
#include <stdint.h>

#include "PadConfig.h"

struct hid_report {
    uint16_t btns;
    uint8_t hat;
    uint8_t d1[8];
    uint8_t velocity[4];
    uint8_t d2[12];
};

void HIDReport_Set(struct hid_report *report, struct midi_mapping map, uint8_t vel);
void HIDReport_Clear(struct hid_report *report, struct midi_mapping map);
void HIDReport_Age(struct hid_report *report);
void HIDReport_SetStartBtn(struct hid_report *r);
void HIDReport_ClearStartBtn(struct hid_report *r);

#endif /* HID_REPORT_H */
