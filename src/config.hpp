#pragma once

#include <toml.hpp>
#include <algorithm>
#include <raylib.h>

// These are used for transparency
constexpr int MIN_ALPHA = 0;
constexpr int MAX_ALPHA = 255;

struct general_config final {
    int width = 610;
    int height = 800;
    int fps = 60;
    int polling_rate = 2000;

    // Customization Part of General
    std::string custom_font = "FiraCode-Bold.ttf";

    int rect_width = 70;
    int rect_height = 70;
    int rect_padding = 10;
    int rect_top_line_only = false;

    int press_counter_font_spacing = 2;
    int press_counter_font_size = 26;
    int press_counter_font_padding = 15;
    int press_counter_y_offset = 0;

    int hold_timer_font_spacing = 0;
    int hold_timer_font_size = 14;
    int hold_timer_font_padding = 5;
    int hold_timer_y_offset = 0;
    int hold_timer_decimals = 6;

    int trail_speed = 700;
    int trail_width = 60;
    int trail_offset = -1;
};

struct colors_config final {
    Color green_fret_color = GREEN;
    Color red_fret_color = RED;
    Color yellow_fret_color = YELLOW;
    Color blue_fret_color = BLUE;
    Color orange_fret_color = ORANGE;
    Color strum_up_color = MAGENTA;
    Color strum_down_color = MAGENTA;

    // I might add released transparency somewhere down the line,
    // or switch to RGB values.
    int underlay_transparency = 0;
    int background_transparency = 0;
    int outline_transparency = 255;
    int hold_transparency = 190;
    int trail_transparency = 190;
};

struct input_binding final {
    int joystick_button = 0;
    std::string keyboard_button_0 = "a";
    std::string keyboard_button_1 = "s";
};

struct bindings_config final {
    int controller_id = 0;
    bool dpad_axis = true;

    input_binding green_binding = {12, "1", "z"};
    input_binding red_binding = {11, "2", "x"};
    input_binding yellow_binding = {10, "3", "c"};
    input_binding blue_binding = {9, "4", "v"};
    input_binding orange_binding = {8, "5", "b"};

    input_binding strum_up_binding = {6, "up_arrow", "null"};
    input_binding strum_down_binding = {5, "down_arrow", "null"};
};

struct configStruct final {
    general_config generalConfig{};
    colors_config colorConfig{};
    bindings_config bindingConfig{};
};

extern configStruct FWIVConfig;

void loadConfig();