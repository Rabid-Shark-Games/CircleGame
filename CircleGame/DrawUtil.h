#pragma once

#include <SDL2/SDL.h>

void drawarrow(SDL_Renderer *rend, int x1, int y1, int x2, int y2, float min);

void drawocto(SDL_Renderer *rend, int r, int x, int y);
void drawocto2(SDL_Renderer *rend, int r, int x, int y, int explode);