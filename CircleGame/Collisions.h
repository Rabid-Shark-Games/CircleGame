#pragma once

#include <SDL2/SDL.h>
#include <vector>

struct Collisions {
	struct Collision {
		SDL_Point p;
		float rem;
		int amt;
		bool operator ==(Collision *c) {
			return this == c;
		}
	};
	std::vector<Collision> collisions;
	std::vector<Collision> streaks;
	bool diddraw = false;

	void add_collision(SDL_Point p, int amt);

	void mark_streak(SDL_Point where, int num);

	void process(float delta);

	unsigned char getopac(float lifetime);

	void draw(SDL_Renderer *rend);
};