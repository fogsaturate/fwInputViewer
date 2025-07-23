#include <toml++/toml.hpp>
#include "config.hpp"
#include <iostream>
#include <filesystem>
#include <fstream> // Required for file stream operations
#include <string>  // Required for std::string

void createDefaultConfig() {
    std::string default_config = R"([general]
width = 575
height = 800
custom_texture = false
trail_speed = 400
trail_width = 50
trail_offset = -1 # 1 = starts at top, 0 = starts in the middle, -1 = starts at the bottom

[colors]
green_fret = "#00ff00"
red_fret = "#ff0000"
yellow_fret = "#ffff00"
blue_fret = "#0050ff"
orange_fret = "#ff8200"

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