#pragma once

#include <SDL2/SDL.h>

#include "Constants.h"
#include "Mouse.h"

struct Upgrades;

struct Player {
	float px = 400;
	float py = 500;
	float vx = 0;
	float vy = 0;
	float timesinceshoot = time_until_shoot;
	float mousedist = 0;
	int gcx = 0;
	int gcy = 0;
	bool moved = false;

	void startGrab(Mouse m);
	void tryShoot(Mouse m, float strength);
	bool tick(Mouse m, float delta, float strength, Upgrades *u);
	void draw(SDL_Renderer *rend, Mouse m, float strength);
};