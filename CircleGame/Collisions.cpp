#include "Collisions.h"

#include "NumberStrings.h"
#include "StaticStrings.h"

static void loadCollision(Collision *c, int x, int y, int amt) {
	c->x = x;
	c->y = y;
	c->rem = 1.f;
	c->amt = amt;
}
static void loadStreak(Collision *c, int x, int y, int num) {
	c->x = x;
	c->y = y;
	c->rem = sqrtf((float)num) * 2;
	c->amt = num;
}

void Collisions::addCollisionText(int x, int y, int amt) {
	for (Collision &c : _collisions) {
		if (c.rem <= 0) {
			loadCollision(&c, x, y, amt);
			return;
		}
	}
	Collision c;
	loadCollision(&c, x, y, amt);
	_collisions.push_back(c);
}

void Collisions::addStreakText(int x, int y, int num) {
	for (Collision &c : _streaks) {
		if (c.rem <= 0) {
			loadStreak(&c, x, y, num);
			return;
		}
	}
	Collision c;
	loadStreak(&c, x, y, num);
	_streaks.push_back(c);
}

void Collisions::process(float delta) {
	for (Collision &c : _collisions) {
		c.rem -= delta;
	}
	for (Collision &c : _streaks) {
		c.rem -= delta;
	}
}

static unsigned char getOpacity(float lifetime) {
	int ret = (int)(lifetime * 500);
	if (ret > 255)
		return 255;
	return ret;
}

void Collisions::draw(SDL_Renderer *rend) {
	_diddraw = false;
	for (const Collision &c : _collisions) {
		if (c.rem <= 0)
			continue;
		drawnumcen(rend, c.amt, c.x, c.y, SDL_Color{ 0, 0, 255, getOpacity(c.rem) });
		_diddraw = true;
	}

	for (const Collision &c : _streaks) {
		if (c.rem <= 0)
			continue;
		SDL_Color col = SDL_Color{ 128, 0, 255, getOpacity(c.rem) };
		int o1 = measurenum(c.amt) / 2;
		int o2 = Str::streak.drawcen(rend, c.x - o1, c.y, col);
		drawnum(rend, c.amt, c.x - o1 + o2, c.y - 10, col);
	}
}

bool Collisions::didDrawCollisions() const
{
	return _diddraw;
}
