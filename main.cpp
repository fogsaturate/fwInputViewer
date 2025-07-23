#include <SFML/System/Sleep.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Joystick.hpp>
#include <raylib.h>
#include <string>
#include <thread>

#include "input.hpp"
#include "config.hpp"
#include <toml++/toml.hpp>



int main() {

    // Read from config first before init
    toml::table config = loadConfig();

    int width = config["general"]["width"].value_or(575);
    int height = config["general"]["height"].value_or(800);

    bool customTexture = config["general"]["custom_texture"].value_or(false);

    int trailSpeed = config["general"]["trail_speed"].value_or(400);
    int trailWidth = config["general"]["trail_width"].value_or(50);
    int trailOffset = config["general"]["trail_offset"].value_or(-1);

    int controllerID = config["bindings"]["controller_id"].value_or(0);
    bool dpadAxis = config["bindings"]["dpad_axis"].value_or(false);

    int greenBind = config["bindings"]["green_binding"].value_or(0);
    int redBind = config["bindings"]["red_binding"].value_or(1);
    int yellowBind = config["bindings"]["yellow_binding"].value_or(4);
    int blueBind = config["bindings"]["blue_binding"].value_or(3);
    int orangeBind = config["bindings"]["orange_binding"].value_or(6);

    int strumUpBind = config["bindings"]["strum_up_binding"].value_or(12);
    int strumDownBind = config["bindings"]["strum_Down_binding"].value_or(13);

    std::thread inputThread(
        input_thread,
        controllerID,
        dpadAxis,
        greenBind,
        redBind,
        yellowBind,
        blueBind,
        orangeBind,
        strumUpBind,
        strumDownBind,
        height,
        trailSpeed
    );

    // SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(width, height, "fwInput Viewer");

    SetTargetFPS(8);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        // ClearBackground(BLANK);
        ClearBackground(GetColor(0x000000));

        Rectangle testRec = {30, 30, 60, 60};
        // for (int i = 0; i < 7; i++) {
        //     DrawRectangleLinesEx(testRec, 5.0, WHITE);
        //     testRec.x += 75;
        // }

        Rectangle fillRec = {55,55,70,70};

        Rectangle barTest = {30,90,60,60};

        std::vector<Rectangle> barList;

        // DrawRectangleRec(barTest,WHITE);

        // for (int i = 0; i < BUTTON_COUNT; ++i) {
        //     int y = 30 + i * 30;5

        //     std::string label = "Button " + std::to_string(BUTTON_IDS[i]) + ": ";
        //     label += buttonStates[i].load();

        //     DrawText(label.c_str(), 50, y, 20, WHITE);
        // }

        // std::string axisText = "Axis: 1 " + std::to_string(joystickAxis.load());

        // DrawText(axisText.c_str(), 50, 270, 20, WHITE);

        EndDrawing();
    }

    running.store(false);
    inputThread.join();

    CloseWindow();
    return 0;
}
