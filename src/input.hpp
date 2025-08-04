#pragma once

#include <atomic>
#include <vector>
#include <raylib.h>
#include "config.hpp"

enum class ControllerBinding {};

struct button_state {
    int joystick_button;
    sf::Keyboard::Key key0;
    sf::Keyboard::Key key1;
    bool held_bool;
    int press_counter;
    float hold_timer;
    std::vector<Rectangle> trail_vector;
};

extern std::atomic<bool> running;
extern std::vector<button_state> button_states;

void input_thread(
    configStruct FWIVConfig,
    std::vector<Rectangle> recVector
);