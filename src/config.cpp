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

[bindings.green_binding]
joystick_button = 11
keyboard_button_0 = "1"
keyboard_button_1 = "z"

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

    auto& generalC = *config["general"].as_table();
    auto& colorC = *config["colors"].as_table();
    auto& bindingC = *config["bindings"].as_table();

    // General

    FWIVConfig.generalConfig.width = generalC["width"].value_or(610);
    FWIVConfig.generalConfig.height = generalC["width"].value_or(500);
    FWIVConfig.generalConfig.fps = generalC["fps"].value_or(60);
    FWIVConfig.generalConfig.polling_rate = generalC["polling_rate"].value_or(2000);

    FWIVConfig.generalConfig.custom_font = generalC["custom_font"].value_or("");

    FWIVConfig.generalConfig.rect_width = generalC["rect_width"].value_or(70);
    FWIVConfig.generalConfig.rect_height = generalC["rect_height"].value_or(70);
    FWIVConfig.generalConfig.rect_padding = generalC["rect_padding"].value_or(10);
    FWIVConfig.generalConfig.rect_top_line_only = generalC["rect_top_line_only"].value_or(false);

    FWIVConfig.generalConfig.press_counter_font_spacing = generalC["press_counter_font_spacing"].value_or(2);
    FWIVConfig.generalConfig.press_counter_font_size = generalC["press_counter_font_size"].value_or(26);
    FWIVConfig.generalConfig.press_counter_font_padding = generalC["press_counter_font_padding"].value_or(15);
    FWIVConfig.generalConfig.press_counter_y_offset = generalC["press_counter_y_offset"].value_or(0);

    FWIVConfig.generalConfig.hold_timer_font_padding = generalC["hold_timer_font_spacing"].value_or(0);
    FWIVConfig.generalConfig.hold_timer_font_size = generalC["hold_timer_font_size"].value_or(14);
    FWIVConfig.generalConfig.hold_timer_font_padding = generalC["hold_timer_font_padding"].value_or(5);
    FWIVConfig.generalConfig.hold_timer_y_offset = generalC["hold_timer_y_offset"].value_or(5);
    FWIVConfig.generalConfig.hold_timer_decimals = generalC["hold_timer_decimals"].value_or(6);

    if (FWIVConfig.generalConfig.hold_timer_decimals < 3) FWIVConfig.generalConfig.hold_timer_decimals = 3;
    // Minimum Value so no-one disables this value

    FWIVConfig.generalConfig.trail_speed = generalC["trail_speed"].value_or(700);
    FWIVConfig.generalConfig.trail_width = generalC["trail_width"].value_or(60);
    FWIVConfig.generalConfig.trail_offset = generalC["trail_offset"].value_or(-1);

    // Colors

    FWIVConfig.colorConfig.green_fret_color = hexStringToInt(colorC["green_fret"].value_or("#00ff00"));
    FWIVConfig.colorConfig.red_fret_color = hexStringToInt(colorC["red_fret"].value_or("#ff0000"));
    FWIVConfig.colorConfig.yellow_fret_color = hexStringToInt(colorC["yellow_fret"].value_or("#ffff00"));
    FWIVConfig.colorConfig.blue_fret_color = hexStringToInt(colorC["blue_fret"].value_or("#0050ff"));
    FWIVConfig.colorConfig.orange_fret_color = hexStringToInt(colorC["orange_fret"].value_or("#ff8200"));
    FWIVConfig.colorConfig.strum_up_color = hexStringToInt(colorC["strum_up_color"].value_or("#9d00ff"));
    FWIVConfig.colorConfig.strum_down_color = hexStringToInt(colorC["strum_down_color"].value_or("#9d00ff"));

    FWIVConfig.colorConfig.underlay_transparency = colorC["underlay_transparency"].value_or(0);
    FWIVConfig.colorConfig.background_transparency = colorC["background_transparency"].value_or(0);
    FWIVConfig.colorConfig.outline_transparency = colorC["outline_transparency"].value_or(255);
    FWIVConfig.colorConfig.hold_transparency = colorC["hold_transparency"].value_or(190);
    FWIVConfig.colorConfig.trail_transparency = colorC["trail_transparency"].value_or(190);

    // Inputs

    auto& greenBinding = *bindingC["green_binding"].as_table();
    auto& redBinding = *bindingC["red_binding"].as_table();
    auto& yellowBinding = *bindingC["yellow_binding"].as_table();
    auto& blueBinding = *bindingC["blue_binding"].as_table();
    auto& orangeBinding = *bindingC["orange_binding"].as_table();

    auto& strumUpBinding = *bindingC["strum_up_binding"].as_table();
    auto& strumDownBinding = *bindingC["strum_down_binding"].as_table();

    FWIVConfig.bindingConfig.green_binding.joystick_button = greenBinding["joystick_button"].value_or(12);
    FWIVConfig.bindingConfig.green_binding.keyboard_button_0 = greenBinding["keyboard_button_0"].value_or("1");
    FWIVConfig.bindingConfig.green_binding.keyboard_button_1 = greenBinding["keyboard_button_1"].value_or("z");

    FWIVConfig.bindingConfig.red_binding.joystick_button = redBinding["joystick_button"].value_or(11);
    FWIVConfig.bindingConfig.red_binding.keyboard_button_0 = redBinding["keyboard_button_0"].value_or("2");
    FWIVConfig.bindingConfig.red_binding.keyboard_button_1 = redBinding["keyboard_button_1"].value_or("x");

    FWIVConfig.bindingConfig.yellow_binding.joystick_button = yellowBinding["joystick_button"].value_or(10);
    FWIVConfig.bindingConfig.yellow_binding.keyboard_button_0 = yellowBinding["keyboard_button_0"].value_or("3");
    FWIVConfig.bindingConfig.yellow_binding.keyboard_button_1 = yellowBinding["keyboard_button_1"].value_or("c");

    FWIVConfig.bindingConfig.blue_binding.joystick_button = blueBinding["joystick_button"].value_or(9);
    FWIVConfig.bindingConfig.blue_binding.keyboard_button_0 = blueBinding["keyboard_button_0"].value_or("4");
    FWIVConfig.bindingConfig.blue_binding.keyboard_button_1 = blueBinding["keyboard_button_1"].value_or("v");

    FWIVConfig.bindingConfig.orange_binding.joystick_button = orangeBinding["joystick_button"].value_or(8);
    FWIVConfig.bindingConfig.orange_binding.keyboard_button_0 = orangeBinding["keyboard_button_0"].value_or("5");
    FWIVConfig.bindingConfig.orange_binding.keyboard_button_1 = orangeBinding["keyboard_button_1"].value_or("b");

    FWIVConfig.bindingConfig.strum_up_binding.joystick_button = strumUpBinding["joystick_button"].value_or(6);
    FWIVConfig.bindingConfig.strum_up_binding.keyboard_button_0 = strumUpBinding["keyboard_button_0"].value_or("Up");
    FWIVConfig.bindingConfig.strum_up_binding.keyboard_button_1 = strumUpBinding["keyboard_button_1"].value_or("null");

    FWIVConfig.bindingConfig.strum_down_binding.joystick_button = strumDownBinding["joystick_button"].value_or(5);
    FWIVConfig.bindingConfig.strum_down_binding.keyboard_button_0 = strumDownBinding["keyboard_button_0"].value_or("Down");
    FWIVConfig.bindingConfig.strum_down_binding.keyboard_button_1 = strumDownBinding["keyboard_button_1"].value_or("null");
}