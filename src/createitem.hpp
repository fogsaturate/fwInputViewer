#pragma once

#include <raylib.h>
#include <vector>

std::vector<Rectangle> CreateFrets(int width, int height, int padding);
std::vector<Vector2> CreateLines(int width, int padding, bool end);

Color Transparentify(Color color, int alpha_value);

Rectangle CreateTrail(Rectangle fretSquare, int trailSpeed, int trailWidth);