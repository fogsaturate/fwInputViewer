#include <toml.hpp>
#include "config.hpp"
#include <iostream>
#include <filesystem>
#include <fstream> // Required for file stream operations
#include <string>  // Required for std::string
#include <raylib.h>

configStruct FWIVConfig{};

Color hexStringToInt(std::string hexString) {
    hexString = hexString.substr(1); // get rid of the # at the start (reading from toml config)

    Color color;
    // Basically what I am doing here is splitting the string into 3 pieces, then converting to hex
    color.r = std::stoi(hexString.substr(0, 2), nullptr, 16);
    color.g = std::stoi(hexString.substr(2, 2), nullptr, 16);
    color.b = std::stoi(hexString.substr(4, 2), nullptr, 16);
    color.a = 255;

    return color;
}

void createDefaultConfig() {
    std::string default_config = R"([general]
width = 610 # I wouldn't recommend changing this yet!! (I did not implement scaling but I will soon!)
height = 500
fps = 60 # Change this to whatever you want, the inputs are frame independent! :)
polling_rate = 2000 # A good middle ground would be 2000 to capture as much inputs as possible
custom_font = "FiraCode-Bold.ttf" # If blank or invalid, will fall back to a default font.
rect_width = 70
rect_height = 70
rect_padding = 10
rect_top_line_only = false # set to true for scorespy-like visual
press_counter_font_spacing = 2 # How spaced out the numbers are
press_counter_font_size = 26 # How big the numbers are
press_counter_font_padding = 15 # This is for shrinking the font size after it reaches a certain threshold
press_counter_y_offset = 0
hold_timer_font_spacing = 0
hold_timer_font_size = 14
hold_timer_font_padding = 5
hold_timer_y_offset = 0
hold_timer_decimals = 6 # minimum value is 3
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
underlay_transparency = 0 # 0 - 255
background_transparency = 0 # 0 - 255
outline_transparency = 255 # 0 - 255
hold_transparency = 190 # 0 - 255
trail_transparency = 190 # 0 - 255

# For bindings, please check https://hardwaretester.com/gamepad to put in your bindings!
# If that doesn't work, then press WIN + R, then type in joy.cpl
# Click on your guitar, go to properties, and look at the buttons manually
# There might be a small chance it is 1 number off, so its practically trial and error

# Another quick note that keyboard is not supported yet !!!

[bindings]
controller_id = 0 # Change this if you have duplicate guitars OR more than one controller, and 0 doesn't work.
dpad_axis = true # This setting should only be false if you are on a raphnet, or another HID joystick adapter
green_binding = 12
red_binding = 11
yellow_binding = 10
blue_binding = 9
orange_binding = 8
strum_up_binding = 8 # Strum bindings aren't used if dpad_axis is true
strum_down_binding = 5

# Useful information that you could use

# HID Game Controller (Santroller) Guitar

# green_binding = 12
# red_binding = 11
# yellow_binding = 10
# blue_binding = 9
# orange_binding = 8

# XInput (Santroller) Guitar

# green_binding = 0
# red_binding = 1
# yellow_binding = 3
# blue_binding = 2
# orange_binding = 4

# Raphnet Guitar

# dpad_axis = false
# green_binding = 0
# red_binding = 1
# yellow_binding = 2
# blue_binding = 3
# orange_binding = 4
# strum_up_binding = 8
# strum_down_binding = 5
)";

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

void checkIfConfigExists() {
    if (!std::filesystem::exists("config.toml")) {
        createDefaultConfig();
    }
}

void loadConfig() {
    checkIfConfigExists();

    try {
        toml::table tbl = toml::parse_file("config.toml");

        // General Config

        parseConfig(tbl);

    } 
    catch (const toml::parse_error& err) {
        std::cout << "config.toml Corrupt!" << std::endl;
        std::cerr << err << std::endl;

        std::cout << "\nIf the program fails to load, delete the config.toml" << std::endl;
    }
}

void parseConfig(toml::table config) {

    // General

    FWIVConfig.generalConfig.width = config["general"]["width"].value_or(610);
    FWIVConfig.generalConfig.height = config["general"]["width"].value_or(500);
    FWIVConfig.generalConfig.fps = config["general"]["fps"].value_or(60);
    FWIVConfig.generalConfig.polling_rate = config["general"]["polling_rate"].value_or(2000);

    FWIVConfig.generalConfig.custom_font = config["general"]["custom_font"].value_or("");
    FWIVConfig.generalConfig.press_counter_font_spacing = config["general"]["custom_font"].value_or(2);
    FWIVConfig.generalConfig.press_counter_font_size = config["general"]["custom_font"].value_or(2);
    FWIVConfig.generalConfig.press_counter_font_padding = config["general"]["press_counter_font_padding"].value_or(15);
    FWIVConfig.generalConfig.press_counter_y_offset = config["general"]["press_counter_y_offset"].value_or(0);

    FWIVConfig.generalConfig.hold_timer_font_padding = config["general"]["hold_timer_font_spacing"].value_or(0);
    FWIVConfig.generalConfig.hold_timer_font_size = config["general"]["hold_timer_font_size"].value_or(14);
    FWIVConfig.generalConfig.hold_timer_font_padding = config["general"]["hold_timer_font_padding"].value_or(5);
    FWIVConfig.generalConfig.hold_timer_y_offset = config["general"]["hold_timer_y_offset"].value_or(5);
    FWIVConfig.generalConfig.hold_timer_decimals = config["general"]["hold_timer_decimals"].value_or(6);

    FWIVConfig.generalConfig.trail_speed = config["general"]["trail_speed"].value_or(700);
    FWIVConfig.generalConfig.trail_width = config["general"]["trail_width"].value_or(60);
    FWIVConfig.generalConfig.trail_offset = config["general"]["trail_offset"].value_or(-1);

    // Colors

    FWIVConfig.colorConfig.green_fret_color = hexStringToInt(config["colors"]["green_fret"].value_or("#00ff00"));
    FWIVConfig.colorConfig.red_fret_color = hexStringToInt(config["colors"]["red_fret"].value_or("#ff0000"));
    FWIVConfig.colorConfig.yellow_fret_color = hexStringToInt(config["colors"]["yellow_fret"].value_or("#ffff00"));
    FWIVConfig.colorConfig.blue_fret_color = hexStringToInt(config["colors"]["blue_fret"].value_or("#0050ff"));
    FWIVConfig.colorConfig.orange_fret_color = hexStringToInt(config["colors"]["orange_fret"].value_or("#ff8200"));
    FWIVConfig.colorConfig.strum_up_color = hexStringToInt(config["colors"]["strum_up_color"].value_or("#9d00ff"));
    FWIVConfig.colorConfig.strum_down_color = hexStringToInt(config["colors"]["strum_down_color"].value_or("#9d00ff"));

    FWIVConfig.colorConfig.underlay_transparency = config["colors"]["underlay_transparency"].value_or(0);
}