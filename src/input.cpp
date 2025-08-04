
#include <SFML/Window.hpp>
#include <SFML/Window/Joystick.hpp>
#include <atomic>
#include <raylib.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "createitem.hpp"
#include "config.hpp"
#include "keys.hpp"
#include "keypress.hpp"

std::atomic<bool> running(true);

struct button_state {
    int joystick_button;
    sf::Keyboard::Key key0;
    sf::Keyboard::Key key1;
    bool held_bool;
    int press_counter;
    float hold_timer;
    std::vector<Rectangle> trail_vector;
};

std::vector<button_state> button_states;

void input_thread(configStruct FWIVConfig, std::vector<Rectangle> recVector) {

    auto& generalC = FWIVConfig.generalConfig;
    auto& bindingC = FWIVConfig.bindingConfig;

    // yeah idk how else ima do this
    button_states.push_back({bindingC.green_binding.joystick_button, translateKey(bindingC.green_binding.keyboard_button_0), translateKey(bindingC.green_binding.keyboard_button_1), false, 0, 0.f});
    button_states.push_back({bindingC.red_binding.joystick_button, translateKey(bindingC.red_binding.keyboard_button_0), translateKey(bindingC.red_binding.keyboard_button_1), false, 0, 0.f});
    button_states.push_back({bindingC.yellow_binding.joystick_button, translateKey(bindingC.yellow_binding.keyboard_button_0), translateKey(bindingC.yellow_binding.keyboard_button_1), false, 0, 0.f});
    button_states.push_back({bindingC.blue_binding.joystick_button, translateKey(bindingC.blue_binding.keyboard_button_0), translateKey(bindingC.blue_binding.keyboard_button_1), false, 0, 0.f});
    button_states.push_back({bindingC.orange_binding.joystick_button, translateKey(bindingC.orange_binding.keyboard_button_0), translateKey(bindingC.orange_binding.keyboard_button_1), false, 0, 0.f});
    button_states.push_back({bindingC.strum_up_binding.joystick_button, translateKey(bindingC.strum_up_binding.keyboard_button_0), translateKey(bindingC.strum_up_binding.keyboard_button_1), false, 0, 0.f});
    button_states.push_back({bindingC.strum_down_binding.joystick_button, translateKey(bindingC.strum_down_binding.keyboard_button_0), translateKey(bindingC.strum_down_binding.keyboard_button_1), false, 0, 0.f});

    // sf::Clock clock;
    auto lastTime = std::chrono::steady_clock::now();

    while (running.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;

        float deltaTime = elapsed.count();
        float moveDistance = deltaTime * generalC.trail_speed;

        lastTime = currentTime;

        sf::Joystick::update();

        if (sf::Joystick::isConnected(bindingC.controller_id)) {

            // 100 povY is a strum, and -100 is a strum down
            // it'd be safe to do 90 and -90 incase of weird controller firmware or something, idk

            bool strumUpHeld;
            bool strumDownHeld;
            float povY = 0.f;
            if (bindingC.dpad_axis) {
                povY = sf::Joystick::getAxisPosition(bindingC.controller_id, sf::Joystick::Axis::PovY);
                strumUpHeld = (povY > 90);
                strumDownHeld = (povY < -90);
            }


            // for (auto& state_of_button: button_states) {
            for (size_t i = 0; i < button_states.size(); i++) {
                auto& state_of_button = button_states[i];

                bool held = false;

                // D-pad Axis checking (for Guitars that also use buttons for strumming, such as Raphnets)
                if (state_of_button.joystick_button == bindingC.strum_up_binding.joystick_button && bindingC.dpad_axis) {
                    held = strumUpHeld;
                } else if (state_of_button.joystick_button == bindingC.strum_down_binding.joystick_button && bindingC.dpad_axis) {
                    held = strumDownHeld;
                } else {
                    held = isBindPressed(bindingC.controller_id, state_of_button.joystick_button, state_of_button.key0, state_of_button.key1);
                }

                if (held) {

                    if (!state_of_button.held_bool) { 
                        // Detects first frame the button is held for
                        state_of_button.press_counter++;
                        state_of_button.hold_timer = 0.0f;
                        state_of_button.held_bool = true;

                        // Trail Logic
                        Rectangle trail_rec = CreateTrail(recVector[i], generalC.trail_speed, generalC.trail_width);
                        state_of_button.trail_vector.push_back(trail_rec);
                    } 

                    // Detects every frame the button is held for
                    if (!state_of_button.trail_vector.empty()) {
                        // I am going to clamp this at .2 pixels since some tick perfect inputs aren't even rendered lol
                        state_of_button.trail_vector.back().height = std::max(state_of_button.hold_timer * generalC.trail_speed, 0.5f);
                    }

                    state_of_button.hold_timer += deltaTime;

                } else { 
                    // Detects every frame the button is released for
                    state_of_button.held_bool = false;
                }

                // If it is the first trail (it is being held currently and growing in height), then ignore it.
                size_t trailCount = state_of_button.trail_vector.size();
                for (size_t j = 0; j + 1 < trailCount; ++j) {
                    state_of_button.trail_vector[j].y += moveDistance;
                }

                if (!held && trailCount > 0) {
                    state_of_button.trail_vector.back().y += moveDistance;
                }

                while (!state_of_button.trail_vector.empty() && state_of_button.trail_vector.front().y >= generalC.height) {
                    state_of_button.trail_vector.erase(state_of_button.trail_vector.begin());
                }
            }
        }
        // This will probably run at 100% CPU usage if I don't limit the polling rate !!!!! :)
        std::this_thread::sleep_for(std::chrono::microseconds(1000000 / generalC.polling_rate));
    }
}