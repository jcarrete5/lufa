#ifndef PAD_CONFIG_H
#define PAD_CONFIG_H

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

extern const struct midi_mapping midi_map[];
extern const struct midi_mapping null_mapping;

#endif /* PAD_CONFIG_H */
