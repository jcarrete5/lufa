#include <stdlib.h>
#include <stdbool.h>
#include <util/atomic.h>

#include "MIDI.h"
#include "padconfig.h"


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
    int count;
    int data_idx;
    union MIDIByte data[3];
}
state = {
    .data_idx = 1,
    .count = 0
};
static struct node *volatile head = NULL, *volatile tail = NULL;

/* Latest MIDI status byte received. Determines how to parse subsequent bytes. */
#define CUR_STATUS state.data[0].status

static void HandleStatus(uint8_t byte, FILE *s) {
    CUR_STATUS.code = byte >> 4;
    CUR_STATUS.channel = byte & 0x0f;
    state.data_idx = 1;

    fprintf(s, "[%d] (%#02x) Code: %x; Channel: %u\r\n",
            state.count++, byte, CUR_STATUS.code, CUR_STATUS.channel + 1);
}

static void HandleByte(uint8_t byte, struct hid_report *cur, FILE *s) {
    /* Ignore timing clock status messages as they are not needed */
    if (byte == MIDI_SYSTEM_TIMING_CLOCK) return;

    if (MIDI_IS_STATUS_BYTE(byte)) {
        HandleStatus(byte, s);
    } else {
        switch (CUR_STATUS.code) {
            case MIDI_NoteOn:
                if (state.data_idx == 1) {
                    state.data[state.data_idx++].note = byte;
                    fprintf(s, "\tMIDI Note: %u\r\n", byte);
                } else if (state.data_idx == 2) {
                    uint8_t vel = byte;
                    state.data[state.data_idx++].velocity = vel;
                    fprintf(s, "\tVelocity: %u\r\n", vel);

                    int32_t btns = midi_btn_map[state.data[1].note];
                    int8_t vel_idx = midi_vel_map[state.data[1].note];

                    /* Ignore unmapped midi notes */
                    if (btns >= 0) {
                        HIDReport_Update(cur, (uint32_t)btns, vel, vel_idx);
                    }
                }
                break;
            default:
                fprintf(s, "\t?: %#02x\r\n", byte);
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

void MIDI_Task(struct hid_report *r, FILE *s) {
    uint8_t byte;

    if (DequeueByte(&byte)) {
        HandleByte(byte, r, s);
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
