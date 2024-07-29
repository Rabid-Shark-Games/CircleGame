#include "Mouse.h"

#include <cmath>

float Mouse::getDistance(int x, int y)
{
    return sqrtf((this->x - x) * (this->x - x) + (this->y - y) * (this->y - y));
}

void Mouse::setXY(int x, int y)
{
    if (x < 0)
        x = 0;
    else if (x > 800)
        x = 800;
    if (y < 0)
        y = 0;
    else if (y > 600)
        y = 600;
    this->x = x;
    this->y = y;
}
