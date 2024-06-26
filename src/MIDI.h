#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>
#include <stdio.h>

#include "HIDReport.h"

#define MIDI_BAUD_RATE 31250
#define MIDI_SYSTEM_TIMING_CLOCK 0xf8

#define MIDI_IS_STATUS_BYTE(b) ((b) & (1 << 7))

enum MIDI_StatusCode
{
  MIDI_NoteOff = 0x08,
  MIDI_NoteOn,
  MIDI_PolyphonicKeyPressure,
  MIDI_ControlChange,
  MIDI_ProgramChange,
  MIDI_ChannelPressure,
  MIDI_PitchBend
};

void
MIDI_Task(struct hid_report* cur);

/* Add MIDI byte the the queue to be handled later */
void
MIDI_EnqueueByte(uint8_t b);

#endif /* MIDI_H */
