#pragma once

#include <vector>

struct Score;
struct SDL_Renderer;
struct Upgrades;
class Collisions;

struct Faller {
	float x = 0;
	float y = 0;
	float v = 0;
	float activationtime = 0;
	bool animating = false;
	float animtime = 0;

	void fall(float delta, float mult);

	void begin_animate();

	void animate(float delta);

	void respawn(float y = -10);

	void draw(SDL_Renderer *rend) const;

	bool intersects(int px, int py) const;
};

class Fallers {
public:
	Fallers();

	void process(float delta, int px, int py, Score &s, Collisions &collisions, const Upgrades &u);
	void draw(SDL_Renderer *rend);
private:
	std::vector<Faller> _fallers;
};