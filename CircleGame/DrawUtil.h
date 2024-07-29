#pragma once

struct SDL_Renderer;

void drawArrow(SDL_Renderer *rend, int x1, int y1, int x2, int y2, float min);

void drawOctagon(SDL_Renderer *rend, int r, int x, int y);
void drawOctagonEx(SDL_Renderer *rend, int r, int x, int y, int explode);