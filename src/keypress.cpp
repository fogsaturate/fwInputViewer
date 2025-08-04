#include <iostream>
#include <string>
#include <SFML/Window.hpp>
#include <keys.hpp>
#include "config.hpp"

// enum class OSType {
//     Windows,
//     Linux,
//     Unknown // aka not supported
// };

// I will use these to optimize linux later, for now i just want this update out lol

// OSType checkOS() {
//     #if defined(_WIN32) || defined(_WIN64)
//         return OSType::Windows;
//     #elif defined(__linux__)
//         return OSType::Linux;
//     #else
//         return OSType::Unknown;
//     #endif
// }

// const OSType currentOS = checkOS();

sf::Keyboard::Key translateKey(std::string keyStr) {
    auto i = KeyCodes.find(keyStr);
    if (i != KeyCodes.end()) {
        return i->second;
    }

    return sf::Keyboard::Key::Unknown;
}

bool isBindPressed(
    int controller_id, 
    int joystick_button,
    sf::Keyboard::Key key0,
    sf::Keyboard::Key key1
) {

    bool buttonPressed = false;

    // i am doing this so keyboard only players can play without getting a bunch of controller disconnected errors
    if (sf::Joystick::isConnected(controller_id)) {
        buttonPressed = sf::Joystick::isButtonPressed(controller_id, joystick_button);
    }

    bool key0Pressed = sf::Keyboard::isKeyPressed(key0);
    bool key1Pressed = sf::Keyboard::isKeyPressed(key1);

    return buttonPressed || key0Pressed || key1Pressed;
}