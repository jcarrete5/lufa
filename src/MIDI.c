#include <stdbool.h>
#include <stdlib.h>
#include <util/atomic.h>

#include "../Config/AppConfig.h"
#include "MIDI.h"
#include "PadConfig.h"

union midi_byte
{
  struct
  {
    unsigned int code : 4;
    unsigned int channel : 4;
  } status;
  uint8_t note;
  uint8_t velocity;
  uint8_t ctrl_num;
  uint8_t prog_num;
};

static struct
{
  int data_idx;
  union midi_byte data[3];
} state = {
  .data_idx = 1,
};

/* Latest MIDI status byte received. Determines how to parse subsequent bytes.
 */
#define CUR_STATUS state.data[0].status

/// MIDI circular buffer size.
#define BUFFER_SIZE 256u

/**
 * @brief Circular buffer for incoming MIDI bytes.
 *
 * @note Buffer data may be modified from an interrupt handler so access to the
 * buffer data should always be atomic.
 */
static struct
{
  unsigned int read;  ///< Current read index for buffer.
  unsigned int write; ///< Current write index for buffer.
  int count;                 ///< Incremented on write, decremented on read.
  uint8_t data[BUFFER_SIZE]; ///< Buffer data.
} buffer = {
  .read = 0,
  .write = 0,
  .count = 0,
  .data = { 0 },
};

static void
handle_status(uint8_t byte)
{
  CUR_STATUS.code = byte >> 4;
  CUR_STATUS.channel = byte & 0x0f;
  state.data_idx = 1;
}

static void
handle_byte(uint8_t byte, struct hid_report* cur)
{
  /* Ignore timing clock status messages as they are not needed */
  if (byte == MIDI_SYSTEM_TIMING_CLOCK)
    return;

  if (MIDI_IS_STATUS_BYTE(byte)) {
    handle_status(byte);
  } else {
    switch (CUR_STATUS.code) {
      case MIDI_NoteOn:
        if (state.data_idx == 1) {
          state.data[state.data_idx++].note = byte;
        } else if (state.data_idx == 2) {
          uint8_t vel = byte;
          state.data[state.data_idx++].velocity = vel;

          /* Ignore vel = 0. Notes are signalled off after a period of time
           * instead. */
          if (vel == 0)
            break;

          const struct midi_mapping* map = GetMIDIMapping(state.data[1].note);
          if (map == NULL) {
            break;
          }

          hid_report_set(cur, *map, vel);
        }
        break;
      default:
        // TODO: Set some LED output to indicate an unhandled message type
        // received
        break;
    }
  }
}

/**
 * @brief Dequeue a byte from the queue and store the result in *out.
 *
 * @return true if there was data to dequeue otherwise return false.
 */
static bool
dequeue_byte(uint8_t* out)
{
  // buffer data may be modified from within an interrupt handler so make
  // access to buffer atomic.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    if (buffer.count == 0) {
      return false;
    }
    *out = buffer.data[buffer.read++];
    if (buffer.read >= BUFFER_SIZE) {
      buffer.read = 0;
    }
    --buffer.count;
  }

  return true;
}

void
midi_task(struct hid_report* r)
{
  uint8_t byte;
  uint8_t count = 0;

  while (count++ < MIDI_TASK_MAX_NUM_PROCESS_BYTES && dequeue_byte(&byte)) {
    handle_byte(byte, r);
  }
}

void
midi_enqueue_byte(uint8_t b)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    buffer.data[buffer.write++] = b;
    if (buffer.write >= BUFFER_SIZE) {
      buffer.write = 0;
    }
    ++buffer.count;
  }
}
