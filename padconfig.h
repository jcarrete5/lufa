#ifndef PAD_CONFIG_H
#define PAD_CONFIG_H

#include <stdint.h>

/* Velocity byte */
#define RED_VEL     1
#define YELLOW_VEL  0
#define BLUE_VEL    3
#define GREEN_VEL   2

/* Colors */
#define BLUE        0
#define GREEN       1
#define RED         2
#define YELLOW      3
#define ORANGE      4  /* Bass pedal */
#define YELLOW2     5  /* Hi-hat pedal */

/* Flags */
#define SELECT      0
#define START       1
#define PAD         2
#define CYMBAL      3
#define PS          4

/* Hi-hat states */
#define NEUTRAL     0x08
#define UP          0x00
#define DOWN        0x04
#define LEFT        0x06
#define RIGHT       0x02

/* c is color, f is flags, h is hi-hat state */
#define MASK(c, f, h)   (1ul<<((c)+16) | 1ul<<((f)+8) | (h))
#define MASKNF(c, h)    (1ul<<((c)+16) | (h))

/* Use these in the MIDI map array to assign a midi note to a bit mask */
#define RED_PAD     MASK(RED, PAD, NEUTRAL)
#define YEL_PAD     MASK(YELLOW, PAD, NEUTRAL)
#define BLUE_PAD    MASK(BLUE, PAD, NEUTRAL)
#define GRN_PAD     MASK(GREEN, PAD, NEUTRAL)
#define KICK        MASKNF(ORANGE, NEUTRAL)
#define BLUE_CYM    MASK(BLUE, CYMBAL, DOWN)
#define GRN_CYM     MASK(GREEN, CYMBAL, NEUTRAL)
#define YEL_CYM     MASK(YELLOW, CYMBAL, UP)
#define HAT_PEDAL   MASKNF(YELLOW2, NEUTRAL)

/* TODO: Don't map HAT_PEDAL */
/* Maps MIDI note number to a button state bit mask */
int32_t midi_btn_map[128] = {
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,    YEL_CYM,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,       KICK,         -1,    RED_PAD,         -1,
        -1,         -1,    YEL_CYM,    GRN_PAD,         -1,   BLUE_PAD,    YEL_CYM,         -1,
   YEL_PAD,    GRN_CYM,         -1,   BLUE_CYM,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
};

/* Maps MIDI note number to a velocity index in the hid_report */
int8_t midi_vel_map[128] = {
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1, YELLOW_VEL,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,    RED_VEL,         -1,
        -1,         -1, YELLOW_VEL,  GREEN_VEL,         -1,   BLUE_VEL, YELLOW_VEL,         -1,
YELLOW_VEL,  GREEN_VEL,         -1,   BLUE_VEL,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
        -1,         -1,         -1,         -1,         -1,         -1,         -1,         -1,
};

#undef BLUE
#undef GREEN
#undef RED
#undef YELLOW
#undef ORANGE
#undef YELLOW2
#undef SELECT
#undef START
#undef PAD
#undef CYMBAL
#undef PS
#undef NEUTRAL
#undef UP
#undef DOWN
#undef LEFT
#undef RIGHT
#undef MASK
#undef MASKNF
#undef RED_PAD
#undef YEL_PAD
#undef BLUE_PAD
#undef GRN_PAD
#undef KICK
#undef YEL_CYM
#undef BLUE_CYM
#undef GRN_CYM
#undef HAT_PEDAL

#endif /* PAD_CONFIG_H */
