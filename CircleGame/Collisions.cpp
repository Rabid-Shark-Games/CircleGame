#include "Collisions.h"

#include "NumberStrings.h"
#include "StaticStrings.h"

void Collisions::add_collision(SDL_Point p, int amt) {
	for (Collision &c : collisions) {
		if (c.rem <= 0) {
			c.p = p;
			c.rem = 1.f;
			c.amt = amt;
			return;
		}
	}
	Collision c;
	c.p = p;
	c.rem = 1.f;
	c.amt = amt;
	collisions.push_back(c);
}

void Collisions::mark_streak(SDL_Point where, int num) {
	for (Collision &c : streaks) {
		if (c.rem <= 0) {
			c.p = where;
			c.rem = sqrtf(num) * 2;
			c.amt = num;
			return;
		}
	}
	Collision c;
	c.p = where;
	c.rem = 2.f;
	c.amt = num;
	streaks.push_back(c);
}

void Collisions::process(float delta) {
	for (Collision &c : collisions) {
		c.rem -= delta;
	}
	for (Collision &c : streaks) {
		c.rem -= delta;
	}
}

unsigned char Collisions::getopac(float lifetime) {
	int ret = lifetime * 500;
	if (ret > 255)
		return 255;
	return ret;
}

void Collisions::draw(SDL_Renderer *rend) {
	diddraw = false;
	for (const Collision &c : collisions) {
		if (c.rem <= 0)
			continue;
		drawnumcen(rend, c.amt, c.p.x, c.p.y, SDL_Color{ 0, 0, 255, getopac(c.rem) });
		diddraw = true;
	}

	for (const Collision &c : streaks) {
		if (c.rem <= 0)
			continue;
		SDL_Color col = SDL_Color{ 128, 0, 255, getopac(c.rem) };
		int o1 = measurenum(c.amt) / 2;
		int o2 = Str::streak.drawcen(rend, c.p.x - o1, c.p.y, col);
		drawnum(rend, c.amt, c.p.x - o1 + o2, c.p.y - 10, col);
	}
}
