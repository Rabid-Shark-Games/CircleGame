#pragma once

#include <SDL2/SDL.h>
#include "Mouse.h"

extern bool menuEverOpened;

struct Score;

struct Upgrades {
	float ropemult = 1.0f;
	int ropelvl = 0;
	float fsmult = 1.0f;
	int fslvl = 0;
	bool menuopen = false;

	void drawMenu(SDL_Renderer *rend, Mouse m, Score *s);
	void mouseUp(Mouse m, Score *s);
	void openMenu();
	void closeMenu();
	void drawMenuHint(SDL_Renderer *rend);
};