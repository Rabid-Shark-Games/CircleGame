#pragma once

struct Timer {
	unsigned long long _lastcount;
	float _delta = 0;

	Timer();

	void tick();
	inline float getDelta() {
		return _delta;
	};
};