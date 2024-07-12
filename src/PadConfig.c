#include "PadConfig.h"

struct midi_mapping red_pad = {
    .color = RED_COLOR,
    .pad_type = PAD_TYPE,
    .vel_byte = RED_VEL,
};
struct midi_mapping red_cymbal = {
    .color = RED_COLOR,
    .pad_type = CYMBAL_TYPE,
    .vel_byte = RED_VEL,
};
struct midi_mapping yellow_pad = {
    .color = YELLOW_COLOR,
    .pad_type = PAD_TYPE,
    .vel_byte = YELLOW_VEL,
};
struct midi_mapping yellow_cymbal = {
    .color = YELLOW_COLOR,
    .pad_type = CYMBAL_TYPE,
    .vel_byte = YELLOW_VEL,
};
struct midi_mapping blue_pad = {
    .color = BLUE_COLOR,
    .pad_type = PAD_TYPE,
    .vel_byte = BLUE_VEL,
};
struct midi_mapping blue_cymbal = {
    .color = BLUE_COLOR,
    .pad_type = CYMBAL_TYPE,
    .vel_byte = BLUE_VEL,
};
struct midi_mapping green_pad = {
    .color = GREEN_COLOR,
    .pad_type = PAD_TYPE,
    .vel_byte = GREEN_VEL,
};
struct midi_mapping green_cymbal = {
    .color = GREEN_COLOR,
    .pad_type = CYMBAL_TYPE,
    .vel_byte = GREEN_VEL,
};
struct midi_mapping kick = {
    .color = ORANGE_COLOR,
    .pad_type = NULL_TYPE,
    .vel_byte = NULL_VEL,
};
