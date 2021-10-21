#ifndef MIDI_H
#define MIDI_H

#include <stdio.h>
#include <stdint.h>

#define MIDI_BAUD_RATE 31250
#define MIDI_SYSTEM_TIMING_CLOCK 0xf8

#define MIDI_IS_STATUS_BYTE(b) ((b) & (1<<7))

enum MIDI_StatusCode {
    MIDI_NoteOff                = 0x08,
    MIDI_NoteOn                 = 0x09,
    MIDI_PolyphonicKeyPressure  = 0x0a,
    MIDI_ControlChange          = 0x0b,
    MIDI_ProgramChange          = 0x0c,
    MIDI_ChannelPressure        = 0x0d,
    MIDI_PitchBend              = 0x0e
};

enum MIDI_Channel {
    MIDI_ChannelDrums = 0x09
};

/* TODO: Remove USBSerialStream argument; Only used for debugging */
void MIDI_HandleByte(uint8_t b, FILE *USBSerialStream);

#endif /* MIDI_H */
