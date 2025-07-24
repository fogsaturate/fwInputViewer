#include <SFML/System/Sleep.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Joystick.hpp>
#include <raylib.h>
#include <string>
#include <thread>
#include "input.hpp"
#include "config.hpp"
#include "createitem.hpp"
#include <toml++/toml.hpp>

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

    // Colors

    Color greenColor = hexStringToInt(config["colors"]["green_fret"].value_or("#00ff00"));
    Color redColor = hexStringToInt(config["colors"]["red_fret"].value_or("#ff0000"));
    Color yellowColor = hexStringToInt(config["colors"]["yellow_fret"].value_or("#ffff00"));
    Color blueColor = hexStringToInt(config["colors"]["blue_fret"].value_or("#0050ff"));
    Color orangeColor = hexStringToInt(config["colors"]["orange_fret"].value_or("#ff8200"));

    Color strumUpColor = hexStringToInt(config["colors"]["strum_up_color"].value_or("#9d00ff"));
    Color strumDownColor = hexStringToInt(config["colors"]["strum_down_color"].value_or("#9d00ff"));

    std::vector<Rectangle> fretVector = CreateFrets();

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
        trailSpeed,
        fretVector
    );

    std::vector<Color> fretColors = {
        greenColor,    // For button_states and stuff
        redColor,
        yellowColor,
        blueColor,
        orangeColor, 
        strumUpColor,
        strumDownColor
    };

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(width, height, "fwInput Viewer");

    SetTargetFPS(30);

    int pressCounterFontSize = 26;
    int holdTimerFontSize = 14;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLANK);
        // ClearBackground(BLACK);

        // std::vector<Rectangle> fretVector = CreateFrets();

        for (size_t i = 0; i < fretVector.size(); i++) {
            if (button_states[i].held_bool) {
                DrawRectangleRec(fretVector[i], Transparentify(fretColors[i], 190));
            }
            DrawRectangleLinesEx(fretVector[i], 5.0, fretColors[i]);

            // Press Counter Text Logic

            int currentPressCounter = button_states[i].press_counter;
            std::string pressCountString = std::to_string(currentPressCounter);

            int fretPadding = 15;
            int maxCounterWidth = fretVector[i].x - fretPadding * 2;
            int pressCountWidth = MeasureText(pressCountString.c_str(), pressCounterFontSize);

            while (pressCountWidth > fretVector[i].width && pressCounterFontSize > 2) {
                pressCounterFontSize--;
                pressCountWidth = MeasureText(pressCountString.c_str(), pressCounterFontSize);
            }

            float pressCountX = fretVector[i].x + (fretVector[i].width / 2) - (static_cast<float>(pressCountWidth) / 2);
            float pressCountY = fretVector[i].y + (fretVector[i].height / 2) - (static_cast<float>(pressCounterFontSize) / 2);

            // Held Timer Text Logic

            float holdTimer = button_states[i].hold_timer;
            std::string holdTimerString = std::to_string(holdTimer);

            int padding = 5;
            int maxTimerWidth = fretVector[i].x - padding * 2;
            int holdTimerWidth = MeasureText(holdTimerString.c_str(), holdTimerFontSize);

            while (holdTimerWidth > fretVector[i].width && holdTimerFontSize > 2) {
                holdTimerFontSize--;
                holdTimerWidth = MeasureText(holdTimerString.c_str(), holdTimerFontSize);
            }

            float holdTimerX = fretVector[i].x + (fretVector[i].width / 2) - (static_cast<float>(holdTimerWidth) / 2);
            float holdTimerY = fretVector[i].y - 20;

            // Text Render Call

            DrawText(pressCountString.c_str(), pressCountX, pressCountY, pressCounterFontSize, WHITE);
            DrawText(holdTimerString.c_str(), holdTimerX, holdTimerY, holdTimerFontSize, WHITE);

            for (auto& rect: button_states[i].trail_vector) {
                DrawRectangleRec(rect, Transparentify(fretColors[i], 190));
            }
        }
        EndDrawing();
    }

    running.store(false);
    inputThread.join();

    CloseWindow();
    return 0;
}
