#include "Score.h"

#include "NumberStrings.h"

float highscore = 0.0f;
float highscorecount = 0.0f;

void Score::tick(float delta, const Collisions &collisions)
{
	if (score > highscore) {
		pb = true;
		highscore = score;
	}

	streaktimerem -= delta;
	if (collisions.didDrawCollisions() || streakc != streakccount) {
		streaktimerem = 1.f;
	}

	if (streaktimerem <= 0) {
		streakc = 0;
		streakccount = 0;
	}

	if (!collisions.didDrawCollisions()) {
		streakn = 0;
	}
}

static void tickCounter(float delta, bool moved, float &count, float real) {
	if (count == real)
		return;

	float diff = real - count;
	int inv = 1;
	if (diff < 0) {
		diff = -diff;
		inv = -1;
	}
	
	float speed = 90;
	if (diff > 1000)
		speed = 600;
	else if (diff > 300)
		speed = 140;

	if (!moved)
		speed *= 2;

	count += speed * delta * inv;
	if (inv < 0 && count < real)
		count = real;
	else if (inv > 0 && count > real)
		count = real;
}

void Score::tickCounters(float delta, bool moved)
{
	tickCounter(delta, moved, highscorecount, highscore);
	tickCounter(delta, moved, scorecount, score);
	tickCounter(delta, moved, streakccount, streakc);
}

float Score::streakStrengthMult() const
{
	return 1 + sqrtf(streakn) / 4;
}

int Score::streakDarkness() const
{
	return (int)(sqrtf(streakn) * 5);
}

int Score::countHit()
{
	++streakn;
	int s = 10
		* (1 + streakMult())
		;
	score += s;
	streakc += s;
	return s;
}

int Score::streakMult()
{
	return streakn / 4;
}

bool Score::wasStreakHit()
{
	return streakn % 4 == 0;
}

void Score::draw(SDL_Renderer *rend)
{
	drawnum(rend,
		highscorecount,
		10, 10, SDL_Color{ 0, 255, 0, 255 });
	int noff = drawnum(rend,
		scorecount,
		10, 30, pb ? SDL_Color{ 255, 128, 0, 255 } : SDL_Color{ 128, 128, 128, 255 });
	if (streakc > 0) {
		noff += drawnumstr(rend, "+", 1, 20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
		drawnum(rend,
			streakccount,
			20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
	}
}
