// GCButtons.hpp

#include "inttypes.hpp"
#include <gccore.h>

enum class GCButton : i16 {
    A, B, X, Y, Start, Z, L, R,
    DPadUp, DPadDown, DPadLeft, DPadRight
};

static const u16 kGCButtonMap[] = {
    PAD_BUTTON_A, PAD_BUTTON_B, PAD_BUTTON_X, PAD_BUTTON_Y,
    PAD_BUTTON_START, PAD_TRIGGER_Z, PAD_TRIGGER_L, PAD_TRIGGER_R,
    PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT
};
