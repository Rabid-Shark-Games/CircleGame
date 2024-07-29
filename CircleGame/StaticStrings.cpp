#include "StaticStrings.h"

StaticString Str::streak("Streak! x");
StaticString Str::upgrade("Get an upgrade");
StaticString Str::level("Level ");
StaticString Str::maxLevel("Maximum Level");
StaticString Str::ropeUpgrade("Flight");
StaticString Str::ropeUpgradeDesc("Increase the amount of time you can stay in the air.");
StaticString Str::upgradeHint("Press 'B' to spend points on upgrades.");
StaticString Str::exit("Close");

void Str::load(SDL_Renderer *rend)
{
	streak.load(rend);
	upgrade.load(rend);
	level.load(rend);
	maxLevel.load(rend);
	ropeUpgrade.load(rend);
	ropeUpgradeDesc.load(rend);
	upgradeHint.load(rend);
	exit.load(rend);
}
