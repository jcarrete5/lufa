#include <stdbool.h>
#include <stdlib.h>

#include "HIDReport.h"
#include "PadConfig.h"
#include "../Config/AppConfig.h"


struct node;
struct node {
    struct node *next;
    struct midi_mapping map;
    uint8_t age;
};

static struct node *head = NULL;

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

static bool HasNode(enum color c, enum pad_type t, bool use_or) {
    struct node *cur = head;
    while (cur != NULL) {
        if (use_or) {
            if (cur->map.color == c || cur->map.pad_type == t) {
                return true;
            }
        } else {
            if (cur->map.color == c && cur->map.pad_type == t) {
                return true;
            }
        }
        cur = cur->next;
    }
    return false;
}

static void UpdateHatState(struct hid_report *r) {
    bool yellow = HasNode(YELLOW_COLOR, CYMBAL_TYPE, false);
    bool blue = HasNode(BLUE_COLOR, CYMBAL_TYPE, false);

    r->hat = HAT_NEUTRAL;
    if (yellow && !blue) {
        r->hat = HAT_UP;
    } else if (!yellow && blue) {
        r->hat = HAT_DOWN;
    }
}

void HIDReport_Set(struct hid_report *r, struct midi_mapping map, uint8_t midi_vel) {
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
        default:
            /* Suppress switch warning */
            break;
    }

    switch (map.pad_type) {
        case PAD_TYPE:
            r->btns |= 1 << PAD;
            break;
        case CYMBAL_TYPE:
            r->btns |= 1 << CYMBAL;
            break;
        default:
            /* Suppress switch warning */
            break;
    }

    if (map.vel_byte != NULL_VEL) {
        r->velocity[map.vel_byte] = ConvertVelocity(midi_vel);
    }

    /* TODO: Check for memory allocation errors */
    struct node* new_node = malloc(sizeof(struct node));
    new_node->map = map;
    new_node->age = MIDI_NOTE_TTL;
    new_node->next = head;
    head = new_node;

    UpdateHatState(r);
}

void HIDReport_Clear(struct hid_report *r, struct midi_mapping map) {
    bool red = HasNode(RED_COLOR, NULL_TYPE, true);
    bool yellow = HasNode(YELLOW_COLOR, NULL_TYPE, true);
    bool blue = HasNode(BLUE_COLOR, NULL_TYPE, true);
    bool green = HasNode(GREEN_COLOR, NULL_TYPE, true);
    bool orange = HasNode(ORANGE_COLOR, NULL_TYPE, true);
    bool pad = HasNode(NULL_COLOR, PAD_TYPE, true);
    bool cymbal = HasNode(NULL_COLOR, CYMBAL_TYPE, true);

    switch (map.color) {
        case RED_COLOR:
            if (red) break;
            r->btns &= ~(1 << CIRCLE);
            r->velocity[RED_VEL] = 0;
            break;
        case YELLOW_COLOR:
            if (yellow) break;
            r->btns &= ~(1 << TRIANGLE);
            r->velocity[YELLOW_VEL] = 0;
            break;
        case BLUE_COLOR:
            if (blue) break;
            r->btns &= ~(1 << SQUARE);
            r->velocity[BLUE_VEL] = 0;
            break;
        case GREEN_COLOR:
            if (green) break;
            r->btns &= ~(1 << CROSS);
            r->velocity[GREEN_VEL] = 0;
            break;
        case ORANGE_COLOR:
            if (orange) break;
            r->btns &= ~(1 << BASS);
            break;
        default:
            /* Suppress switch warning */
            break;
    }

    switch (map.pad_type) {
        case PAD_TYPE:
            if (pad) break;
            r->btns &= ~(1 << PAD);
            break;
        case CYMBAL_TYPE:
            if (cymbal) break;
            r->btns &= ~(1 << CYMBAL);
            break;
        default:
            /* Suppress switch warning */
            break;
    }

    UpdateHatState(r);
}

void HIDReport_Age(struct hid_report *r) {
    struct node *cur = head, *prev = NULL;

    while (cur != NULL) {
        if (--cur->age == 0) {
            struct midi_mapping map = cur->map;
            struct node *del = cur;

            cur = cur->next;
            if (prev == NULL) {
                head = cur;
            } else {
                prev->next = cur;
            }
            free(del);

            HIDReport_Clear(r, map);
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void HIDReport_SetStartBtn(struct hid_report *r) {
    r->btns |= 1 << START;
}

void HIDReport_ClearStartBtn(struct hid_report *r) {
    r->btns &= ~(1 << START);
}
