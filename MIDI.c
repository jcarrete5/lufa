#include "MIDI.h"


struct MIDIStatus {
    unsigned int code       :4;
    unsigned int channel    :4;
};

union MIDIDataByte {
    uint8_t note;
    uint8_t velocity;
    uint8_t ctrl_num;
    uint8_t prog_num;
};

static struct {
    int count;
    int data_byte_count;
    struct MIDIStatus status;
    union MIDIDataByte data[2];
}
state = {
    .data_byte_count = 0,
    .count = 0
};

static void HandleStatus(uint8_t byte, FILE *s) {
    state.status.code = (byte & 0xf0) >> 4;
    state.status.channel = byte & 0x0f;
    state.data_byte_count = 0;

    fputs("\r\n", s);
    fprintf(s, "[%d] (%#02x) Code: %x; Channel: %u\r\n",
            state.count++, byte, state.status.code, state.status.channel + 1);
}

void MIDI_HandleByte(uint8_t byte, FILE *s) {
    /* Ignore timing clock status messages as they are not needed */
    if (byte == MIDI_SYSTEM_TIMING_CLOCK) return;

    if (MIDI_IS_STATUS_BYTE(byte)) {
        HandleStatus(byte, s);
    } else {
        /* Ignore messages which are not on the drum channel */
        if (state.status.channel != MIDI_ChannelDrums) return;
        switch (state.status.code) {
            case MIDI_NoteOn:
                if (state.data_byte_count == 0) {
                    state.data[state.data_byte_count++].note = byte;
                    fprintf(s, "MIDI Note: %u\r\n", byte);
                } else if (state.data_byte_count == 1) {
                    state.data[state.data_byte_count++].velocity = byte;
                    fprintf(s, "Velocity: %u\r\n", byte);
                }
                break;
        }
    }
}
