#pragma once

struct Mouse {
	int x = 0;
	int y = 0;
	bool dragging = false;

	float getDistance(int x, int y);
};