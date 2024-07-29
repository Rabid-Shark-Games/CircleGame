#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "Score.h"
#include "Collisions.h"

struct Faller {
	float x;
	float y;
	float v = 0;
	float activationtime = 0;
	bool animating;
	float animtime = 0;

	void fall(float delta);

	void begin_animate();

	void animate(float delta);

	void respawn(float y = -10);

	void draw(SDL_Renderer *rend) const;

	bool intersects(int px, int py) const;
};

struct Fallers {
	std::vector<Faller> _fallers;

	Fallers();

	void process(float delta, int px, int py, Score *s, Collisions *collisions);

	void draw(SDL_Renderer *rend);
};