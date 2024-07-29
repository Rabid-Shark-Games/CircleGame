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
		streakn = 0;
		streakc = 0;
	}
}

static void tickHighScoreCounter(float delta, bool moved, float &highscorecount, float &highscore) {
	if (highscorecount == highscore)
		return;

	float speed = 90;
	
	float diff = highscore - highscorecount;
	if (diff > 1000)
		speed = 600;
	else if (diff > 300)
		speed = 140;

	if (!moved)
		speed *= 2;

	highscorecount += speed * delta;
	if (highscorecount > highscore)
		highscorecount = highscore;
}

void Score::tickCounters(float delta, bool moved)
{
	tickHighScoreCounter(delta, moved, highscorecount, highscore);
	streakccount += delta * ((streakc - streakccount > 300) ? 140 : 90);
	if (streakccount > streakc)
		streakccount = streakc;
	
	if (score > scorecount) {
		scorecount += delta * ((score - scorecount > 300) ? 140 : 90) * (moved ? 1 : 4);
		if (scorecount > score)
			scorecount = score;
	}
	else if (score < scorecount) {
		scorecount -= delta * ((scorecount - score > 30) ? 300 : 20) * (moved ? 1 : 4);
		if (scorecount < score)
			scorecount = score;
	}
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
	if (streaktimerem > 0.0f) {
		noff += drawnumstr(rend, "+", 1, 20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
		drawnum(rend,
			streakccount,
			20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
	}
}
