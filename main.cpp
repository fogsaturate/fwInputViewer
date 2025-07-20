#include <SFML/Window.hpp>
#include <SFML/Window/Joystick.hpp>
#include <raylib.h>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

std::atomic<bool> running(true);

constexpr int BUTTON_COUNT = 7;
constexpr int BUTTON_IDS[BUTTON_COUNT] = { 0, 1, 2, 3, 4, 13, 12 }; // A, B, X, Y, LB (example)

std::array<std::atomic<bool>, BUTTON_COUNT> buttonStates;
std::array<std::atomic<int>, BUTTON_COUNT> buttonPressCounts;

void input_thread() {
    std::array<bool, BUTTON_COUNT> buttonState = { false };

    while (running.load()) {
        auto start = std::chrono::high_resolution_clock::now();
        sf::Joystick::update();
        if (sf::Joystick::isConnected(0)) {
            for (int i = 0; i < BUTTON_COUNT; ++i) {
                int id = BUTTON_IDS[i];
                bool pressed = sf::Joystick::isButtonPressed(0, id);

                if (pressed && !buttonState[i]) {
                    buttonPressCounts[i].fetch_add(1, std::memory_order_relaxed);
                }

                buttonStates[i].store(pressed, std::memory_order_relaxed);
                buttonState[i] = pressed;
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

int main() {
    // Start input thread first
    std::thread inputThread(input_thread);

    InitWindow(800, 600, "Raylib + SFML Input");

    SetTargetFPS(120);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < BUTTON_COUNT; ++i) {
            int y = 30 + i * 30;

            std::string label = "Button " + std::to_string(BUTTON_IDS[i]) + ": ";
            label += buttonStates[i].load();
        }

        EndDrawing();
    }

    running.store(false);
    inputThread.join();

    CloseWindow();
    return 0;
}
