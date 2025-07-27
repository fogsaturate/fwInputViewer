#include <raylib.h>
#include <vector>

std::vector<Rectangle> CreateFrets(int width, int height, int padding) {

    std::vector<Rectangle> keyVector;


    for (int i = 0; i < 7; i++) {
        Rectangle keyRect;

        keyRect.width = width;
        keyRect.height = height;

        keyRect.x = 30 + i * (width + padding);
        keyRect.y = 30;

        keyVector.push_back(keyRect);
    }

    return keyVector;
}

std::vector<Vector2> CreateLines(int width, int padding, bool end) {

    std::vector<Vector2> keyVector;


    for (int i = 0; i < 7; i++) {
        Vector2 keyVec2;

        keyVec2.x = (30 + (end * width)) + i * (width + padding);
        keyVec2.y = 30;

        keyVector.push_back(keyVec2);
    }

    return keyVector;
}

Color Transparentify(Color color, int alpha_value) { // for making colors slightly transparent (for trails + button held states)
    Color transparentColor = color;
    transparentColor.a = alpha_value;
    return transparentColor;
}

Rectangle CreateTrail(Rectangle fretSquare, int trailSpeed, int trailWidth) {

    Rectangle trailRec;

    // trailRec.width = fretSquare.width;
    trailRec.width = trailWidth;
    trailRec.height = 0; // This will always have an absolute value

    // I will use this equation when I implement custom trail width.
    trailRec.x = (fretSquare.x + fretSquare.width / 2) - trailRec.width / 2;
    // trailRec.x = fretSquare.x;
    trailRec.y = fretSquare.y + fretSquare.height; // bottom of fretSquare

    return trailRec;
}