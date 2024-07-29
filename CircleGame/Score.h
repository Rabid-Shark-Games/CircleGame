#pragma once

#include "Collisions.h"

extern float highscore;

struct Score {
	float score = 0.0f;
	float scorecount = 0.0f;
	float streakccount = 0.0f;
	float streaktimerem = 0.0f;
	bool pb = false;
	int streakn = 0;
	int streakc = 0;

	void tick(float delta, const Collisions &collisions);
	void tickCounters(float delta, bool moved);
	float streakStrengthMult() const;
	int streakDarkness() const;
	int countHit();
	int streakMult();
	bool wasStreakHit();
	void draw(SDL_Renderer *rend);
};