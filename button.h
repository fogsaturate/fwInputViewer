#pragma once
#include <chrono>

class Button {
    public:
        Button();

        void update(bool pressed);

        bool isActivated() const;
        int getCounter() const;
        float getHoldTime() const;
    private:
        bool activated = false;
        int activation_counter = 0;
        float hold_timer = 0;
};