#include <stdio.h>
#include <stdbool.h>

#include "hidreport.h"


/**
 * Convert MIDI velocity value to a velocity that makes sense to rock band.
 */
static uint8_t ConvertVelocity(uint8_t vel) {
    /* Counter-intuitively, lower velocity values indicate stronger hits in rock band */
    vel = (127 - vel) * 2;
    if (vel < 21) {
        return 0;
    } else if (vel < 63) {
        return 42;
    } else if (vel < 105) {
        return 84;
    } else if (vel < 147) {
        return 126;
    } else if (vel < 189) {
        return 168;
    } else if (vel < 231) {
        return 210;
    } else {
        return 255;
    }
}

void HIDReport_Update(struct hid_report *r, uint32_t btns, uint8_t vel, int8_t vel_idx) {
    if (vel == 0) {
        r->buttons[0] &= ~((btns >> 16) & 0xff);
        r->buttons[1] &= ~((btns >> 8) & 0xff);
        r->buttons[2] &= ~((btns >> 0) & 0xff);
    } else {
        r->buttons[0] |= (btns >> 16) & 0xff;
        r->buttons[1] |= (btns >> 8) & 0xff;
        r->buttons[2] |= (btns >> 0) & 0xff;
    }
    if (vel_idx >= 0) {
        /* Still report velocity 0 if MIDI velocity is 0 */
        r->velocity[vel_idx] = vel == 0 ? 0 : ConvertVelocity(vel);
    }
}

void HIDReport_Send(struct hid_report *r, FILE *s) {
    for (int i = 0; i < 3; ++i) {
        fprintf(s, "%02x", r->buttons[i]);
    }
    for (int i = 0; i < 8; ++i) {
        fprintf(s, "%02x", r->d1[i]);
    }
    for (int i = 0; i < 4; ++i) {
        fprintf(s, "%02x", r->velocity[i]);
    }
    for (int i = 0; i < 12; ++i) {
        fprintf(s, "%02x", r->d2[i]);
    }
    fputs("\r\n", s);
}

bool HIDReport_AreEqual(struct hid_report *a, struct hid_report *b) {
    bool ret = true;

    for (int i = 0; i < 3; ++i) {
        ret = ret && a->buttons[i] == b->buttons[i];
    }
    for (int i = 0; i < 4; ++i) {
        ret = ret && a->velocity[i] == b->velocity[i];
    }

    return ret;
}