#ifndef MIDI_H_
#define MIDI_H_

#include <stdint.h>
#include <util/atomic.h>

#include "HIDReport.h"

#define MIDI_BAUD_RATE           31250
#define MIDI_SYSTEM_TIMING_CLOCK 0xf8

#define MIDI_IS_STATUS_BYTE(b) ((b) & (1 << 7))

enum midi_status_code {
    MIDI_NoteOff = 0x08,
    MIDI_NoteOn,
    MIDI_PolyphonicKeyPressure,
    MIDI_ControlChange,
    MIDI_ProgramChange,
    MIDI_ChannelPressure,
    MIDI_PitchBend
};

void midi_task(struct hid_report *cur);

/*
 * @brief Add MIDI byte the the queue to be handled later.
 *
 * This function is usually called from an ISR.
 */
void midi_enqueue_byte(uint8_t b);

/* Add MIDI byte the the queue to be handled later, atomically. */
inline void midi_enqueue_byte_atomic(uint8_t b)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        midi_enqueue_byte(b);
    }
}

#endif /* MIDI_H_ */
