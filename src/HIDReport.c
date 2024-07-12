#include "HIDReport.h"

#include <stdbool.h>
#include <stdint.h>

#include "../Config/AppConfig.h"
#include "PadConfig.h"

#define BUFFER_SIZE 16

static struct {
    int read;
    int write;
    int count;

    struct node {
        struct midi_mapping map;
        uint8_t age;
    } data[BUFFER_SIZE];
} buffer;

/**
 * Convert MIDI velocity value to a velocity that makes sense to rock band.
 */
static uint8_t convert_velocity(uint8_t vel)
{
    /* Counter-intuitively, lower velocity values indicate stronger hits in rock
     * band */
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

static bool has_node_with_color_and_type(enum color c, enum pad_type t)
{
    for (int count = 0, i = buffer.read; count < buffer.count;
         ++count, ++i >= BUFFER_SIZE ? i = 0 : 0) {
        struct node *cur = &buffer.data[i];
        if (cur->map.color == c && cur->map.pad_type == t) {
            return true;
        }
    }
    return false;
}

static bool has_node_with_color_or_type(enum color c, enum pad_type t)
{
    for (int count = 0, i = buffer.read; count < buffer.count;
         ++count, ++i >= BUFFER_SIZE ? i = 0 : 0) {
        struct node *cur = &buffer.data[i];
        if (cur->map.color == c || cur->map.pad_type == t) {
            return true;
        }
    }
    return false;
}

static void update_hat_state(struct hid_report *r)
{
    bool yellow = has_node_with_color_and_type(YELLOW_COLOR, CYMBAL_TYPE);
    bool blue = has_node_with_color_and_type(BLUE_COLOR, CYMBAL_TYPE);

    r->hat = HAT_NEUTRAL;
    if (yellow && !blue) {
        r->hat = HAT_UP;
    } else if (!yellow && blue) {
        r->hat = HAT_DOWN;
    }
}

void hid_report_set(struct hid_report *r, struct midi_mapping map, uint8_t midi_vel)
{
    switch (map.color) {
        case RED_COLOR:
            r->btns |= 1 << CIRCLE;
            break;
        case YELLOW_COLOR:
            r->btns |= 1 << TRIANGLE;
            break;
        case BLUE_COLOR:
            r->btns |= 1 << SQUARE;
            break;
        case GREEN_COLOR:
            r->btns |= 1 << CROSS;
            break;
        case ORANGE_COLOR:
            r->btns |= 1 << BASS;
            break;
        case NULL_COLOR:
            break;
    }

    switch (map.pad_type) {
        case PAD_TYPE:
            r->btns |= 1 << PAD;
            break;
        case CYMBAL_TYPE:
            r->btns |= 1 << CYMBAL;
            break;
        case NULL_TYPE:
            break;
    }

    if (map.vel_byte != NULL_VEL) {
        r->velocity[map.vel_byte] = convert_velocity(midi_vel);
    }

    struct node *n = &buffer.data[buffer.write++];
    n->map = map;
    n->age = MIDI_NOTE_TTL;
    if (buffer.write >= BUFFER_SIZE) {
        buffer.write = 0;
    }
    ++buffer.count;

    update_hat_state(r);
}

void hid_report_clear(struct hid_report *r, struct midi_mapping map)
{
    bool red = has_node_with_color_or_type(RED_COLOR, NULL_TYPE);
    bool yellow = has_node_with_color_or_type(YELLOW_COLOR, NULL_TYPE);
    bool blue = has_node_with_color_or_type(BLUE_COLOR, NULL_TYPE);
    bool green = has_node_with_color_or_type(GREEN_COLOR, NULL_TYPE);
    bool orange = has_node_with_color_or_type(ORANGE_COLOR, NULL_TYPE);
    bool pad = has_node_with_color_or_type(NULL_COLOR, PAD_TYPE);
    bool cymbal = has_node_with_color_or_type(NULL_COLOR, CYMBAL_TYPE);

    switch (map.color) {
        case RED_COLOR:
            if (red) {
                break;
            }
            r->btns &= ~(1 << CIRCLE);
            r->velocity[RED_VEL] = 0;
            break;
        case YELLOW_COLOR:
            if (yellow) {
                break;
            }
            r->btns &= ~(1 << TRIANGLE);
            r->velocity[YELLOW_VEL] = 0;
            break;
        case BLUE_COLOR:
            if (blue) {
                break;
            }
            r->btns &= ~(1 << SQUARE);
            r->velocity[BLUE_VEL] = 0;
            break;
        case GREEN_COLOR:
            if (green) {
                break;
            }
            r->btns &= ~(1 << CROSS);
            r->velocity[GREEN_VEL] = 0;
            break;
        case ORANGE_COLOR:
            if (orange) {
                break;
            }
            r->btns &= ~(1 << BASS);
            break;
        case NULL_COLOR:
            break;
    }

    switch (map.pad_type) {
        case PAD_TYPE:
            if (pad) {
                break;
            }
            r->btns &= ~(1 << PAD);
            break;
        case CYMBAL_TYPE:
            if (cymbal) {
                break;
            }
            r->btns &= ~(1 << CYMBAL);
            break;
        case NULL_TYPE:
            break;
    }

    update_hat_state(r);
}

void hid_report_age(struct hid_report *r)
{
    int count = buffer.count;
    int i = buffer.read;
    while (count-- > 0) {
        struct node *cur = &buffer.data[i];
        if (--cur->age == 0) {
            if (++buffer.read >= BUFFER_SIZE) {
                buffer.read = 0;
            }
            --buffer.count;

            hid_report_clear(r, cur->map);
        }
        if (++i >= BUFFER_SIZE) {
            i = 0;
        }
    }
}

void hid_report_set_start_btn(struct hid_report *r)
{
    r->btns |= 1 << START;
}

void hid_report_clear_start_btn(struct hid_report *r)
{
    r->btns &= ~(1 << START);
}
