#include "Timer.h"

#include <SDL2/SDL.h>

Timer::Timer()
{
	_lastcount = SDL_GetPerformanceCounter();
}

void Timer::tick()
{
	unsigned long long curcount = SDL_GetPerformanceCounter();
	_delta = (float)(curcount - _lastcount) / SDL_GetPerformanceFrequency();
	_lastcount = curcount;
}
