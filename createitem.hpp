#pragma once

#include <raylib.h>
#include <vector>

std::vector<Rectangle> CreateFrets();

Color Transparentify(Color color, int alpha_value);

Rectangle CreateTrail(Rectangle fretSquare, int trailSpeed, int trailWidth);