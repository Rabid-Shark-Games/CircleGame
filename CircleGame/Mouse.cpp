#include "Mouse.h"

#include <cmath>

float Mouse::getDistance(int x, int y)
{
    return sqrtf((this->x - x) * (this->x - x) + (this->y - y) * (this->y - y));
}
