#pragma once

#include <atomic>
#include <vector>
#include <raylib.h>

struct button_state {
    int button_bind;
    bool held_bool;
    int press_counter;
    float hold_timer;
    std::vector<Rectangle> trail_vector;
};

extern std::atomic<bool> running;
extern std::vector<button_state> button_states;

void input_thread(
    int controller_id,
    bool dpad_axis, 
    int green_bind,
    int red_bind,
    int yellow_bind,
    int blue_bind,
    int orange_bind,
    int strum_up_bind,
    int strum_down_bind,

    // For trailing rectangles
    int screen_height, // This is for optimization reasons. (Delete the trail after its off-screen)
    int trail_speed,
    std::vector<Rectangle> recVector
);