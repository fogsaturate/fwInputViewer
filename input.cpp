// #include "button.h"
#include <SFML/Window.hpp>
#include <SFML/Window/Joystick.hpp>
#include <atomic>
#include <raylib.h>
#include <vector>


std::atomic<bool> running(true);

enum class ControllerBinding {
    Green,
    Red,
    Yellow,
    Blue,
    Orange,
    StrumUp,
    StrumDown,
    Unknown
};

struct button_state {
    ControllerBinding button_bind;
    bool held_bool;
    int press_counter;
    float hold_timer;
};

std::vector<button_state> button_states;

int getJoystickBind( // couldn't really think of another way to optimize this LOL
    ControllerBinding button,
    int green_bind,
    int red_bind,
    int yellow_bind,
    int blue_bind,
    int orange_bind,
    int strum_up_bind,
    int strum_down_bind
) {
    switch (button) {
        case ControllerBinding::Green:     
            return green_bind;
        case ControllerBinding::Red:       
            return red_bind;
        case ControllerBinding::Yellow:    
            return yellow_bind;
        case ControllerBinding::Blue:      
            return blue_bind;
        case ControllerBinding::Orange:    
            return orange_bind;
        case ControllerBinding::StrumUp:   
            return strum_up_bind;
        case ControllerBinding::StrumDown: 
            return strum_down_bind;
        default:                          
            return -1;
    }
}

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
    int trail_speed // How fast the rectangles go.
) {

    // Initializing the bindings first
    button_states.push_back({ControllerBinding::Green, false, 0, 0.f});
    button_states.push_back({ControllerBinding::Red, false, 0, 0.f});
    button_states.push_back({ControllerBinding::Yellow, false, 0, 0.f});
    button_states.push_back({ControllerBinding::Blue, false, 0, 0.f});
    button_states.push_back({ControllerBinding::Orange, false, 0, 0.f});
    button_states.push_back({ControllerBinding::StrumUp, false, 0, 0.f});
    button_states.push_back({ControllerBinding::StrumDown, false, 0, 0.f});

    sf::Clock clock;

    while (running.load()) {

        sf::Joystick::update();
        float deltaTime = clock.restart().asSeconds(); // I will be using this for the trailing rectangles

        float povY = 0.f;
        if (dpad_axis) {
            povY = sf::Joystick::getAxisPosition(controller_id, sf::Joystick::Axis::PovY);
        }

        bool strumUpHeld = (povY > 90);
        bool strumDownHeld = (povY < -90);

        if (sf::Joystick::isConnected(controller_id)) {
            for (auto& state_of_button: button_states) {

                int joystick_button = getJoystickBind(
                    state_of_button.button_bind, 
                    green_bind, 
                    red_bind, 
                    yellow_bind, 
                    blue_bind, 
                    orange_bind, 
                    strum_up_bind, 
                    strum_down_bind
                );

                bool held = false;

                if (state_of_button.button_bind == ControllerBinding::StrumUp) {
                    held = strumUpHeld;
                } else if (state_of_button.button_bind == ControllerBinding::StrumDown) {
                    held = strumDownHeld;
                } else {
                    held = sf::Joystick::isButtonPressed(controller_id, joystick_button);
                }

                if (held && !state_of_button.held_bool) {
                    state_of_button.press_counter++;
                }

                if (held) {
                    if (!state_of_button.held_bool) {
                        // this sucks
                        state_of_button.hold_timer = 0.0f;
                        state_of_button.held_bool = true;
                    }
                    state_of_button.hold_timer += deltaTime;
                } else {
                    state_of_button.held_bool = false;
                }
            }
        }
        // This will probably run at 100% CPU usage if I don't limit the polling rate !!!!! :)
        // Limiting to 0.5ms
        sf::sleep(sf::microseconds(500));
    }
}