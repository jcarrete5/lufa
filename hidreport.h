#ifndef HID_REPORT_H
#define HID_REPORT_H

#include <stdint.h>

struct hid_report {
    uint8_t buttons[3];
    uint8_t d1[8];
    uint8_t velocity[4];
    uint8_t d2[12];
};

void HIDReport_Update(struct hid_report *report, uint32_t btns, uint8_t vel, int8_t vel_idx);
void HIDReport_Send(struct hid_report *report, FILE *stream);
bool HIDReport_AreEqual(struct hid_report *a, struct hid_report *b);

#endif /* HID_REPORT_H */
