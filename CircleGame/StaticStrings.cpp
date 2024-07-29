#include "StaticStrings.h"

StaticString Str::streak("Streak! x");

void Str::load(SDL_Renderer *rend)
{
	streak.load(rend);
}
