#include <toml++/toml.hpp>
#include "config.hpp"
#include <iostream>
#include <filesystem>
#include <fstream> // Required for file stream operations
#include <string>  // Required for std::string

void createDefaultConfig() {
    std::string default_config = R"([general]
width = 610 # I wouldn't recommend changing this yet!!
height = 800
custom_font = "FiraCode-Bold.ttf" # If blank or invalid, will fall back to a default font.
press_counter_font_spacing = 2
press_counter_font_size = 26
press_counter_font_padding = 15
hold_timer_font_spacing = 0
hold_timer_font_size = 14
hold_timer_font_padding = 5
trail_speed = 700
trail_width = 60
trail_offset = -1 # 1 = starts at top, 0 = starts in the middle, -1 = starts at the bottom
# ^^^ this also doesn't work for now!!

[colors] # Default raylib color palette :)
green_fret = "#00e430"
red_fret = "#e62937"
yellow_fret = "#fdf900"
blue_fret = "#0079f1"
orange_fret = "#ffa100"
strum_up_color = "#873cbe"
strum_down_color = "#873cbe"
hold_transparency = 190
trail_transparency = 190

# For bindings, please check https://hardwaretester.com/gamepad to put in your bindings!
[bindings]
controller_id = 0
dpad_axis = true # This setting should only be false if you are on a raphnet, or another HID joystick adapter
green_binding = 0
red_binding = 1
yellow_binding = 4
blue_binding = 3
orange_binding = 6
strum_up_binding = 12
strum_down_binding = 13)";

    std::cout << "Default Settings not found! Writing Now...\n";

    std::ofstream output_file("config.toml");

    if (output_file.is_open()) {
        output_file << default_config;
        output_file.close();

        std::cout << "Default Settings Written!\n";
    } else {
        std::cerr << "Failed to open config.toml\n";
    }
}

void checkIfConfigExists(bool corrupt_toml) {
    if (!std::filesystem::exists("config.toml") || corrupt_toml) {
        createDefaultConfig();
    }
}

toml::table loadConfig(bool retry) {
    checkIfConfigExists(false);

    try {
        toml::table tbl = toml::parse_file("config.toml");
        return tbl;
    } 
    catch (const toml::parse_error& err) {
        std::cerr << err << "\n";
        std::cout << "config.toml Corrupt! Overwriting..." << "\n";
        checkIfConfigExists(true);

        if (retry) {
            return loadConfig(false);
        } else {
            std::cerr << "Failed to load config after retry. Exiting...\n";
            std::exit(EXIT_FAILURE);  // Force quit with failure code
        }
    }
}