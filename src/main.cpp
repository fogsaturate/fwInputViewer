#include <SFML/System/Sleep.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Joystick.hpp>
#include <raylib.h>
#include <string>
#include <thread>
#include "input.hpp"
#include "config.hpp"
#include "createitem.hpp"
#include <toml.hpp>
#include <sstream>
#include <iostream>

// Color hexStringToInt(std::string hexString) {
//     hexString = hexString.substr(1); // get rid of the # at the start (reading from toml config)

//     Color color;
//     // Basically what I am doing here is splitting the string into 3 pieces, then converting to hex
//     color.r = std::stoi(hexString.substr(0, 2), nullptr, 16);
//     color.g = std::stoi(hexString.substr(2, 2), nullptr, 16);
//     color.b = std::stoi(hexString.substr(4, 2), nullptr, 16);
//     color.a = 255;

//     return color;
// }

Font loadFontFallback(const std::string& path, int fontSize) {
    Font font = LoadFontEx(path.c_str(), fontSize, 0, 250);

    if (font.texture.id == 0) {
        font = GetFontDefault();
    }

    return font;
}

int main() {

    // Read from config first before init
    loadConfig();

    // i want my code to be readable, ok
    auto& gCfg = FWIVConfig.generalConfig;
    auto& cCfg = FWIVConfig.colorConfig;
    auto& bCfg = FWIVConfig.bindingConfig;

    std::vector<Rectangle> fretVector = CreateFrets(
        gCfg.rect_width,
        gCfg.rect_height, 
        gCfg.rect_padding
    );

    std::vector<Vector2> topLineVector;
    std::vector<Vector2> topLineEndVector;
    if (FWIVConfig.generalConfig.rect_top_line_only) {
        topLineVector = CreateLines(
            gCfg.rect_width, 
            gCfg.rect_padding, 
            false
        );

        topLineEndVector = CreateLines(
            gCfg.rect_width, 
            gCfg.rect_padding, 
            true
        );
    }

    std::thread inputThread(input_thread, FWIVConfig, fretVector);

    std::vector<Color> fretColors = {
        cCfg.green_fret_color,
        cCfg.red_fret_color,
        cCfg.yellow_fret_color,
        cCfg.blue_fret_color,
        cCfg.orange_fret_color, 
        cCfg.strum_up_color,
        cCfg.strum_down_color
    };

    Color underlay = Transparentify(BLACK, cCfg.underlay_transparency);
    BlendMode blendMode = cCfg.underlay_transparency > 0 ? BLEND_ADDITIVE : BLEND_ALPHA; // needed for underlay to work properly

    // bool fontLoaded = false;

    // if (customTTFfont.texture.id == 0) {
    //     // Font failed to load
    //     TraceLog(LOG_ERROR, "Failed to load font.");
    // } else {
    //     fontLoaded = true;
    // }


    SetTraceLogLevel(LOG_WARNING); // This will only log important errors instead of raylib's init prints
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(gCfg.width, gCfg.height, "fwInput Viewer");

    SetTargetFPS(gCfg.fps);

    Font pressCounterTTF = loadFontFallback(gCfg.custom_font, gCfg.press_counter_font_size);
    Font holdTimerTTF = loadFontFallback(gCfg.custom_font, gCfg.hold_timer_font_size);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLANK);

        // ClearBackground(BLACK);

        // std::vector<Rectangle> fretVector = CreateFrets();

        for (size_t i = 0; i < fretVector.size(); i++) {

            DrawRectangleRec(fretVector[i], underlay);
            BeginBlendMode(blendMode);
            DrawRectangleRec(fretVector[i], Transparentify(fretColors[i], cCfg.background_transparency));
            if (button_states[i].held_bool) {
                DrawRectangleRec(fretVector[i], Transparentify(fretColors[i], cCfg.hold_transparency));
            }
            EndBlendMode();
            if (gCfg.rect_top_line_only) {
                DrawLineEx(topLineVector[i], topLineEndVector[i], 5.0, underlay);
                BeginBlendMode(blendMode);
                DrawLineEx(topLineVector[i], topLineEndVector[i], 5.0, Transparentify(fretColors[i], cCfg.outline_transparency));
            } else {
                DrawRectangleLinesEx(fretVector[i], 5.0, underlay);
                BeginBlendMode(blendMode);
                DrawRectangleLinesEx(fretVector[i], 5.0, Transparentify(fretColors[i], cCfg.outline_transparency));
            }
            EndBlendMode();

            // Press Counter Text Logic

            int currentPressCounter = button_states[i].press_counter;
            std::string pressCountString = std::to_string(currentPressCounter);

            int fretPadding = gCfg.press_counter_font_padding;
            int maxCounterWidth = fretVector[i].x - fretPadding * 2;
            int pressCountWidth = MeasureTextEx(pressCounterTTF, pressCountString.c_str(), gCfg.press_counter_font_size, gCfg.press_counter_font_spacing).x;

            while (pressCountWidth > fretVector[i].width && gCfg.press_counter_font_size > 2) {
                gCfg.press_counter_font_size--;
                pressCountWidth = MeasureTextEx(pressCounterTTF, pressCountString.c_str(), gCfg.press_counter_font_size, gCfg.press_counter_font_spacing).x;
            }

            float pressCountX = fretVector[i].x + (fretVector[i].width / 2) - (static_cast<float>(pressCountWidth) / 2);
            float pressCountY = (fretVector[i].y + (fretVector[i].height / 2) - (static_cast<float>(gCfg.press_counter_font_size) / 2)) + gCfg.press_counter_y_offset;

            // Held Timer Text Logic

            float holdTimer = button_states[i].hold_timer;
            //std::string holdTimerString = std::to_string(holdTimer);
            std::stringstream holdTimerStream;
            holdTimerStream << std::fixed << std::setprecision(gCfg.hold_timer_decimals) << holdTimer;
            std::string holdTimerString = holdTimerStream.str();
            int padding = gCfg.hold_timer_font_padding;
            int maxTimerWidth = fretVector[i].x - padding * 2;
            int holdTimerWidth = MeasureTextEx(holdTimerTTF, holdTimerString.c_str(), gCfg.hold_timer_font_size, gCfg.hold_timer_font_spacing).x;

            while (holdTimerWidth > fretVector[i].width && gCfg.hold_timer_font_size > 2) {
                gCfg.hold_timer_font_size--;
                holdTimerWidth = MeasureTextEx(holdTimerTTF, holdTimerString.c_str(), gCfg.hold_timer_font_size, gCfg.hold_timer_font_spacing).x;
            }

            float holdTimerX = fretVector[i].x + (fretVector[i].width / 2) - (static_cast<float>(holdTimerWidth) / 2);
            float holdTimerY = (fretVector[i].y - 20) + gCfg.hold_timer_y_offset;

            // Text Render Call

            DrawTextEx(pressCounterTTF, pressCountString.c_str(), {pressCountX, pressCountY}, gCfg.press_counter_font_size, gCfg.press_counter_font_spacing, WHITE);
            DrawTextEx(holdTimerTTF, holdTimerString.c_str(), {holdTimerX, holdTimerY}, gCfg.hold_timer_font_size, gCfg.hold_timer_font_spacing, WHITE);
            for (auto& rect: button_states[i].trail_vector) {
                DrawRectangleRec(rect, Transparentify(fretColors[i], cCfg.trail_transparency));
            }

        }
        EndDrawing();
    }

    running.store(false);
    inputThread.join();

    UnloadFont(pressCounterTTF);
    UnloadFont(holdTimerTTF);

    CloseWindow();
    return 0;
}
