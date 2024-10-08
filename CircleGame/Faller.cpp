#include "Faller.h"

#include <SDL2/SDL.h>
#include "Constants.h"
#include "DrawUtil.h"
#include "Upgrade.h"
#include "Score.h"
#include "Collisions.h"

void Faller::fall(float delta, float mult) {
	if (animating) {
		animate(delta);
		return;
	}
	if (activationtime > 0) {
		activationtime -= delta;
		return;
	}
	v += 60.f * delta;
	y += v * delta * mult;

	if (y > 600 + 10)
		respawn();
}

void Faller::begin_animate() {
	animating = true;
	animtime = 0;
}

void Faller::animate(float delta) {
	animtime += delta;
	if (animtime >=
		0.3f
		) {
		respawn();
	}
}

void Faller::respawn(float y) {
	animating = false;
	this->y = y;
	x = (float)(10 + rand() % (800 - 20));
	v = 0;
}

void Faller::draw(SDL_Renderer *rend) const {
	drawOctagonEx(rend, 5, (int)x, (int)y, animating ? animtime * animtime * 30 : 0);
}

bool Faller::intersects(int px, int py) const {
	if (animating)
		return false;
	float a = x - px;
	float b = y - py;
	return sqrtf(a * a + b * b) < 20 + 8;
}

Fallers::Fallers() {
	for (int i = 0; i < NUM_FALLERS; i++) {
		_fallers.push_back(Faller{ (float)i * (800 / NUM_FALLERS) + (400 / NUM_FALLERS) - 5, 100, 0, ((float)rand() / RAND_MAX) * 1.3f });
		Faller f;
		f.respawn(-50);
		f.activationtime = ((float)rand() / RAND_MAX) * 2.3f;
		_fallers.push_back(f);
	}
}

void Fallers::process(float delta, int px, int py, Score &s, Collisions &collisions, const Upgrades &u) {
	for (Faller &faller : _fallers) {
		faller.fall(delta, u.fsmult);

		if (faller.intersects(px, py)) {
			int pts = s.countHit();
			collisions.addCollisionText((int)faller.x, (int)(faller.y + 20), pts);
			if (s.wasStreakHit())
				collisions.addStreakText((int)faller.x, (int)(faller.y + 40), s.streakMult());
			faller.begin_animate();
		}
	}
}

void Fallers::draw(SDL_Renderer *rend) {
	SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
	for (const Faller &faller : _fallers) {
		faller.draw(rend);
	}
}
