#pragma once

#include <vector>

struct SDL_Renderer;

struct Collision {
	int x, y;
	float rem;
	int amt;
	bool operator ==(Collision *c) {
		return this == c;
	}
};

class Collisions {
public:
	void addCollisionText(int x, int y, int amt);
	void addStreakText(int x, int y, int num);

	void process(float delta);
	void draw(SDL_Renderer *rend);

	bool didDrawCollisions() const;
private:
	std::vector<Collision> _collisions;
	std::vector<Collision> _streaks;
	bool _diddraw = false;
};