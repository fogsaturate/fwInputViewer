#include <raylib.h>
#include <vector>

std::vector<Rectangle> CreateFrets() {

    std::vector<Rectangle> keyVector;


    for (int i = 0; i < 7; i++) {
        Rectangle keyRect;

        keyRect.width = 70;
        keyRect.height = 70;

        keyRect.x = 30 + i * 80;
        keyRect.y = 30;

        keyVector.push_back(keyRect);
    }

    return keyVector;
}

Color Transparentify(Color color, int alpha_value) { // for making colors slightly transparent (for trails + button held states)
    Color transparentColor = color;
    transparentColor.a = alpha_value;
    return transparentColor;
}