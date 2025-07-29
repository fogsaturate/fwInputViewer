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

Font loadFontFallback(const std::string& path, int fontSize) {
    Font font = LoadFontEx(path.c_str(), fontSize, 0, 250);

    if (font.texture.id == 0) {
        font = GetFontDefault();
    }

    return font;
}

int main() {

    // Read from config first before init
    toml::table config = loadConfig();

    int width = config["general"]["width"].value_or(610);
    int height = config["general"]["height"].value_or(800);
    int fps = config["general"]["fps"].value_or(60);
    int pollingRate = config["general"]["polling_rate"].value_or(2000);

    std::string customFont = config["general"]["custom_font"].value_or("");
    int pressCounterFontSpacing = config["general"]["press_counter_font_spacing"].value_or(2);
    int pressCounterFontSize = config["general"]["press_counter_font_size"].value_or(26);
    int pressCounterFontPadding = config["general"]["press_counter_font_padding"].value_or(15);
    int pressCountYOffset = config["general"]["press_counter_y_offset"].value_or(0);
    int holdTimerFontSpacing = config["general"]["hold_timer_font_spacing"].value_or(0);
    int holdTimerFontSize = config["general"]["hold_timer_font_size"].value_or(14);
    int holdTimerFontPadding = config["general"]["hold_timer_font_padding"].value_or(5);
    int holdTimerYOffset = config["general"]["hold_timer_y_offset"].value_or(5);
    int holdTimerDecimals = config["general"]["hold_timer_decimals"].value_or(6);
    if (holdTimerDecimals < 3) holdTimerDecimals = 3; // minimum value so users can't effectively disable this feature

    int trailSpeed = config["general"]["trail_speed"].value_or(700);
    int trailWidth = config["general"]["trail_width"].value_or(60);
    int trailOffset = config["general"]["trail_offset"].value_or(-1);

    int controllerID = config["bindings"]["controller_id"].value_or(0);
    bool dpadAxis = config["bindings"]["dpad_axis"].value_or(true);

    int greenBind = config["bindings"]["green_binding"].value_or(0);
    int redBind = config["bindings"]["red_binding"].value_or(1);
    int yellowBind = config["bindings"]["yellow_binding"].value_or(4);
    int blueBind = config["bindings"]["blue_binding"].value_or(3);
    int orangeBind = config["bindings"]["orange_binding"].value_or(6);

    int strumUpBind = config["bindings"]["strum_up_binding"].value_or(12);
    int strumDownBind = config["bindings"]["strum_down_binding"].value_or(13);

    // Colors

    Color greenColor = hexStringToInt(config["colors"]["green_fret"].value_or("#00ff00"));
    Color redColor = hexStringToInt(config["colors"]["red_fret"].value_or("#ff0000"));
    Color yellowColor = hexStringToInt(config["colors"]["yellow_fret"].value_or("#ffff00"));
    Color blueColor = hexStringToInt(config["colors"]["blue_fret"].value_or("#0050ff"));
    Color orangeColor = hexStringToInt(config["colors"]["orange_fret"].value_or("#ff8200"));

    Color strumUpColor = hexStringToInt(config["colors"]["strum_up_color"].value_or("#9d00ff"));
    Color strumDownColor = hexStringToInt(config["colors"]["strum_down_color"].value_or("#9d00ff"));

    int underlayTransparency = config["colors"]["underlay_transparency"].value_or(0);
    int backgroundTransparency = config["colors"]["background_transparency"].value_or(0);
    int outlineTransparency = config["colors"]["outline_transparency"].value_or(255);
    int holdTransparency = config["colors"]["hold_transparency"].value_or(190);
    int trailTransparency = config["colors"]["trail_transparency"].value_or(190);

    int rectWidth = config["general"]["rect_width"].value_or(70);
    int rectHeight = config["general"]["rect_height"].value_or(70);
    int rectPadding = config["general"]["rect_padding"].value_or(10);
    bool rectTopLineOnly = config["general"]["rect_top_line_only"].value_or(false);

    std::vector<Rectangle> fretVector = CreateFrets(rectWidth, rectHeight, rectPadding);
    std::vector<Vector2> topLineVector;
    std::vector<Vector2> topLineEndVector;
    if (rectTopLineOnly) {
        topLineVector = CreateLines(rectWidth, rectPadding, false);
        topLineEndVector = CreateLines(rectWidth, rectPadding, true);
    }

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
        trailWidth,
        trailSpeed,
        pollingRate,
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

    Color underlay = Transparentify(BLACK, underlayTransparency);
    BlendMode blendMode = underlayTransparency > 0 ? BLEND_ADDITIVE : BLEND_ALPHA; // needed for underlay to work properly

    // bool fontLoaded = false;

    // if (customTTFfont.texture.id == 0) {
    //     // Font failed to load
    //     TraceLog(LOG_ERROR, "Failed to load font.");
    // } else {
    //     fontLoaded = true;
    // }


    SetTraceLogLevel(LOG_WARNING); // This will only log important errors instead of raylib's init prints
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(width, height, "fwInput Viewer");

    SetTargetFPS(fps);

    Font pressCounterTTF = loadFontFallback(customFont, pressCounterFontSize);
    Font holdTimerTTF = loadFontFallback(customFont, holdTimerFontSize);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLANK);
        // ClearBackground(BLACK);

        // std::vector<Rectangle> fretVector = CreateFrets();

        for (size_t i = 0; i < fretVector.size(); i++) {
            DrawRectangleRec(fretVector[i], underlay);
            BeginBlendMode(blendMode);
            DrawRectangleRec(fretVector[i], Transparentify(fretColors[i], backgroundTransparency));
            if (button_states[i].held_bool) {
                DrawRectangleRec(fretVector[i], Transparentify(fretColors[i], holdTransparency));
            }
            EndBlendMode();
            if (rectTopLineOnly) {
                DrawLineEx(topLineVector[i], topLineEndVector[i], 5.0, underlay);
                BeginBlendMode(blendMode);
                DrawLineEx(topLineVector[i], topLineEndVector[i], 5.0, Transparentify(fretColors[i], outlineTransparency));
            } else {
                DrawRectangleLinesEx(fretVector[i], 5.0, underlay);
                BeginBlendMode(blendMode);
                DrawRectangleLinesEx(fretVector[i], 5.0, Transparentify(fretColors[i], outlineTransparency));
            }
            EndBlendMode();

            // Press Counter Text Logic

            int currentPressCounter = button_states[i].press_counter;
            std::string pressCountString = std::to_string(currentPressCounter);

            int fretPadding = pressCounterFontPadding;
            int maxCounterWidth = fretVector[i].x - fretPadding * 2;
            int pressCountWidth = MeasureTextEx(pressCounterTTF, pressCountString.c_str(), pressCounterFontSize, pressCounterFontSpacing).x;

            while (pressCountWidth > fretVector[i].width && pressCounterFontSize > 2) {
                pressCounterFontSize--;
                pressCountWidth = MeasureTextEx(pressCounterTTF, pressCountString.c_str(), pressCounterFontSize, pressCounterFontSpacing).x;
            }

            float pressCountX = fretVector[i].x + (fretVector[i].width / 2) - (static_cast<float>(pressCountWidth) / 2);
            float pressCountY = (fretVector[i].y + (fretVector[i].height / 2) - (static_cast<float>(pressCounterFontSize) / 2)) + pressCountYOffset;

            // Held Timer Text Logic

            float holdTimer = button_states[i].hold_timer;
            //std::string holdTimerString = std::to_string(holdTimer);
            std::stringstream holdTimerStream;
            holdTimerStream << std::fixed << std::setprecision(holdTimerDecimals) << holdTimer;
            std::string holdTimerString = holdTimerStream.str();

            int padding = holdTimerFontPadding;
            int maxTimerWidth = fretVector[i].x - padding * 2;
            int holdTimerWidth = MeasureTextEx(holdTimerTTF, holdTimerString.c_str(), holdTimerFontSize, holdTimerFontSpacing).x;

            while (holdTimerWidth > fretVector[i].width && holdTimerFontSize > 2) {
                holdTimerFontSize--;
                holdTimerWidth = MeasureTextEx(holdTimerTTF, holdTimerString.c_str(), holdTimerFontSize, holdTimerFontSpacing).x;
            }

            float holdTimerX = fretVector[i].x + (fretVector[i].width / 2) - (static_cast<float>(holdTimerWidth) / 2);
            float holdTimerY = (fretVector[i].y - 20) + holdTimerYOffset;

            // Text Render Call

            DrawTextEx(pressCounterTTF, pressCountString.c_str(), {pressCountX, pressCountY}, pressCounterFontSize, pressCounterFontSpacing, WHITE);
            DrawTextEx(holdTimerTTF, holdTimerString.c_str(), {holdTimerX, holdTimerY}, holdTimerFontSize, holdTimerFontSpacing, WHITE);

            for (auto& rect: button_states[i].trail_vector) {
                DrawRectangleRec(rect, Transparentify(fretColors[i], trailTransparency));
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
