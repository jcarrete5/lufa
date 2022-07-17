<%!
    import json

    with open("Config/PadConfig.json") as file:
        config = json.load(file)

    # Validate configuration
    valid_keys = {
        "red pad",
        "red cymbal",
        "yellow pad",
        "yellow cymbal",
        "blue pad",
        "blue cymbal",
        "green pad",
        "green cymbal",
        "kick",
    }
    used = set()
    for k, v in config.items():
        try:
            valid_keys.remove(k)
        except KeyError:
            raise ValueError(
                f"invalid pad value specified or specified more than once: {k}"
            )
        for note in v:
            if note in used:
                raise ValueError("note value already used")
            if note < 0 or note > 127:
                raise ValueError(
                    "note value must be in the half-open range [0, 128)"
                )
            used.add(note)

    del used, note, k, v, valid_keys
%>\
<%def name="gen_cases(key, return_value)">\
    % for note in config[key]:
    case ${note}:
    % endfor
    % if config[key]:
        return ${return_value};
    % endif
</%def>\
#ifndef PADCONFIG_H
#define PADCONFIG_H

#include <stddef.h>
#include <stdint.h>

/* Button flag bit positions */
#define SELECT      8
#define START       9u
#define PAD         10
#define CYMBAL      11
#define PS          12
#define SQUARE      0   /* Blue */
#define CROSS       1   /* Green */
#define CIRCLE      2   /* Red */
#define TRIANGLE    3   /* Yellow */
#define BASS        4
#define HAT_PEDAL   5

/* Hat states */
#define HAT_NEUTRAL     0x08
#define HAT_UP          0x00
#define HAT_DOWN        0x04
#define HAT_LEFT        0x06
#define HAT_RIGHT       0x02

enum color {
    RED_COLOR,
    YELLOW_COLOR,
    BLUE_COLOR,
    GREEN_COLOR,
    ORANGE_COLOR,
    NULL_COLOR,
};

enum pad_type {
    PAD_TYPE,
    CYMBAL_TYPE,
    NULL_TYPE,
};

enum vel_byte {
    YELLOW_VEL,
    RED_VEL,
    GREEN_VEL,
    BLUE_VEL,
    NULL_VEL,
};

struct midi_mapping {
    enum color color :3;
    enum pad_type pad_type :2;
    enum vel_byte vel_byte :3;
};

extern struct midi_mapping red_pad;
extern struct midi_mapping red_cymbal;
extern struct midi_mapping yellow_pad;
extern struct midi_mapping yellow_cymbal;
extern struct midi_mapping blue_pad;
extern struct midi_mapping blue_cymbal;
extern struct midi_mapping green_pad;
extern struct midi_mapping green_cymbal;
extern struct midi_mapping kick;

static inline const struct midi_mapping *GetMIDIMapping(uint8_t midi_note) {
    switch (midi_note) {
${gen_cases("red pad", "&red_pad")}\
${gen_cases("red cymbal", "&red_cymbal")}\
${gen_cases("yellow pad", "&yellow_pad")}\
${gen_cases("yellow cymbal", "&yellow_cymbal")}\
${gen_cases("blue pad", "&blue_pad")}\
${gen_cases("blue cymbal", "&blue_cymbal")}\
${gen_cases("green pad", "&green_pad")}\
${gen_cases("green cymbal", "&green_cymbal")}\
${gen_cases("kick", "&kick")}\
    default:
        return NULL;
    }
}

#endif
