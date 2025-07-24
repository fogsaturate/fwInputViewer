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

Rectangle CreateTrail(Rectangle fretSquare, int trailSpeed) {

    Rectangle trailRec;

    // trailRec.width = fretSquare.width;
    trailRec.width = fretSquare.width - (5 * 2);
    trailRec.height = 0;

    // I will use this equation when I implement custom trail width.
    trailRec.x = (fretSquare.x + fretSquare.width / 2) - trailRec.width / 2;
    // trailRec.x = fretSquare.x;
    trailRec.y = fretSquare.y + fretSquare.height; // bottom of fretSquare

    return trailRec;
}