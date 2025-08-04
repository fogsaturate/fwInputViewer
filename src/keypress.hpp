#pragma once

#include <SFML/Window.hpp>
#include <string>

sf::Keyboard::Key translateKey(std::string keyStr);

bool isBindPressed(int controller_id, int joystick_button, sf::Keyboard::Key key0, sf::Keyboard::Key key1);

