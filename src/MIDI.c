#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <util/atomic.h>

#include "MIDI.h"
#include "PadConfig.h"
#include "../Config/AppConfig.h"


union MIDIByte {
    struct {
        unsigned int code       :4;
        unsigned int channel    :4;
    } status;
    uint8_t note;
    uint8_t velocity;
    uint8_t ctrl_num;
    uint8_t prog_num;
};

struct node;
struct node {
    uint8_t data;
    struct node *next;
};

static struct {
    int data_idx;
    union MIDIByte data[3];
}
state = {
    .data_idx = 1,
};
static struct node *volatile head = NULL, *volatile tail = NULL;

/* Latest MIDI status byte received. Determines how to parse subsequent bytes. */
#define CUR_STATUS state.data[0].status

static void HandleStatus(uint8_t byte) {
    CUR_STATUS.code = byte >> 4;
    CUR_STATUS.channel = byte & 0x0f;
    state.data_idx = 1;
}

static void HandleByte(uint8_t byte, struct hid_report *cur) {
    /* Ignore timing clock status messages as they are not needed */
    if (byte == MIDI_SYSTEM_TIMING_CLOCK) return;

    if (MIDI_IS_STATUS_BYTE(byte)) {
        HandleStatus(byte);
    } else {
        switch (CUR_STATUS.code) {
            case MIDI_NoteOn:
                if (state.data_idx == 1) {
                    state.data[state.data_idx++].note = byte;
                } else if (state.data_idx == 2) {
                    uint8_t vel = byte;
                    state.data[state.data_idx++].velocity = vel;

                    /* Ignore vel = 0. Notes are signalled off after a period of time instead. */
                    if (vel == 0) break;

                    struct midi_mapping map = midi_map[state.data[1].note];

                    if (memcmp(&map, &null_mapping, sizeof(struct midi_mapping)) == 0) break;

                    HIDReport_Set(cur, map, vel);
                }
                break;
            default:
                // TODO: Set some LED output to indicate an unhandled message type received
                break;
        }
    }
}

/*
 * Dequeue a byte from the queue and store the result in *out.
 * Return true if there was data to dequeue otherwise return false.
 */
static bool DequeueByte(uint8_t *out) {
    if (head == NULL) return false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        *out = head->data;
        struct node *t = head->next;
        free(head);
        head = t;
        if (head == NULL) {
            tail = NULL;
        }
    }

    return true;
}

void MIDI_Task(struct hid_report *r) {
    uint8_t byte;
    uint8_t count = 0;

    while (count++ < MIDI_TASK_MAX_NUM_PROCESS_BYTES && DequeueByte(&byte)) {
        HandleByte(byte, r);
    }
}

void MIDI_EnqueueByte(uint8_t b) {
    /* TODO: Maybe add a filter here to avoid wasting time and space with data
       we don't care about i.e. timing bytes. */

    struct node *new_node = malloc(sizeof(struct node));
    new_node->data = b;
    new_node->next = NULL;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (head == NULL) {
            head = new_node;
        } else {
            tail->next = new_node;
        }
        tail = new_node;
    }
}
