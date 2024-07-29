#pragma once

#include <SDL2/SDL.h>

void initnums(SDL_Renderer *rend);

int drawnumstr(SDL_Renderer *rend, const char *num, int c, int x, int y, SDL_Color col);

int measurenum(int num);
int drawnum(SDL_Renderer *rend, int num, int x, int y, SDL_Color col);
int drawnumcen(SDL_Renderer *rend, int num, int x, int y, SDL_Color col);

void drawfloat(SDL_Renderer *rend, float num, int x, int y, SDL_Color col);
void drawfloatcen(SDL_Renderer *rend, float num, int x, int y, SDL_Color col);