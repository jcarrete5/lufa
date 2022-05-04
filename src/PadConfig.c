#include <stdint.h>

#include "PadConfig.h"

#define X               {.color = NULL_COLOR, .pad_type = NULL_TYPE, .vel_byte = NULL_VEL}
#define RED_PAD         {.color = RED_COLOR, .pad_type = PAD_TYPE, .vel_byte = RED_VEL}
#define RED_CYM         {.color = RED_COLOR, .pad_type = CYMBAL_TYPE, .vel_byte = RED_VEL}
#define YEL_PAD         {.color = YELLOW_COLOR, .pad_type = PAD_TYPE, .vel_byte = YELLOW_VEL}
#define YEL_CYM         {.color = YELLOW_COLOR, .pad_type = CYMBAL_TYPE, .vel_byte = YELLOW_VEL}
#define BLU_PAD         {.color = BLUE_COLOR, .pad_type = PAD_TYPE, .vel_byte = BLUE_VEL}
#define BLU_CYM         {.color = BLUE_COLOR, .pad_type = CYMBAL_TYPE, .vel_byte = BLUE_VEL}
#define GRN_PAD         {.color = GREEN_COLOR, .pad_type = PAD_TYPE, .vel_byte = GREEN_VEL}
#define GRN_CYM         {.color = GREEN_COLOR, .pad_type = CYMBAL_TYPE, .vel_byte = GREEN_VEL}
#define KICK            {.color = ORANGE_COLOR, .pad_type = NULL_TYPE, .vel_byte = NULL_VEL}

const struct midi_mapping null_mapping = X;

/* Maps MIDI note number to a midi_mapping struct */
const struct midi_mapping midi_map[128] = {
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X, YEL_CYM,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,    KICK,       X, RED_PAD,       X,
       X,       X, YEL_CYM, GRN_PAD,       X, BLU_PAD, YEL_CYM,       X,
 YEL_PAD, GRN_CYM,       X, BLU_CYM,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
       X,       X,       X,       X,       X,       X,       X,       X,
};
