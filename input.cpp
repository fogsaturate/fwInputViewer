#include "button.h"

Button::Button() {
    activated = false;
    activation_counter = 0;
    hold_timer = 0;
}