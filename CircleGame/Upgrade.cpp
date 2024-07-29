#include "Upgrade.h"

#include "DrawUtil.h"
#include "StaticStrings.h"
#include "NumberStrings.h"
#include "Score.h"

bool menuEverOpened = false;

constexpr int menuCenX = (800 - 128) / 2;
constexpr int menuCenY = (600 - 128) / 2;

static void drawRopeGraphic(SDL_Renderer *rend, int x, int y) {
	SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
	drawocto(rend, 16, x + 128 - 16, y + 16);

	SDL_RenderDrawLine(rend, x + 32, y + 128 - 32, x + 128 - 16, y + 16);

	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	drawocto(rend, 32, x + 32, y + 128 - 32);
}

static bool isMouseHovering(Mouse m, int x, int y, int w, int h) {
	return m.x >= (x - 10) && m.x < (x + w + 10) &&
		m.y >= (y - 10) && m.y < (y + h + 10);
}

static bool isMouseHovering(Mouse m, int x, int y) {
	return isMouseHovering(m, x, y, 128, 128);
}

static SDL_Color getGraphicColor(Mouse m, int x, int y, int w, int h, bool canAfford) {
	return isMouseHovering(m, x, y, w, h)
		? SDL_Color{ 255, 255, 0, (unsigned char)(canAfford ? 255 : 128) }
		: SDL_Color{ 255, 255, 255, (unsigned char)(canAfford ? 255 : 128) };
}

static SDL_Color getGraphicColor(Mouse m, int x, int y, bool canAfford) {
	return getGraphicColor(m, x, y, 128, 128, canAfford);
}

static void drawGraphicOutline(SDL_Renderer *rend, int x, int y, Mouse m, bool canAfford) {
	SDL_Color col = getGraphicColor(m, x, y, canAfford);
	SDL_SetRenderDrawColor(rend, col.r, col.g, col.b, col.a);
	SDL_Rect r;
	r.x = x - 10;
	r.y = y - 10;
	r.w = 128 + 20;
	r.h = 128 + 20;
	SDL_RenderDrawRect(rend, &r);
}

static void drawGraphicTitle(SDL_Renderer *rend, int x, int y, RendString *str, Mouse m, int cost, bool canAfford) {
	SDL_Color col = getGraphicColor(m, x, y, canAfford);
	str->draw(rend, x + 128 - str->getWidth(), y + 128 + 14, col);
	drawnum(rend, cost, x - 10, y + 128 + 14, col);
}

static void drawGraphicLevel(SDL_Renderer *rend, int x, int y, int level, bool full, Mouse m, bool canAfford) {
	if (!level)
		return;
	SDL_Color col = getGraphicColor(m, x, y, canAfford);
	if (full) {
		Str::maxLevel.drawcen(rend, x + 64, y + 128 + 44, col);
		return;
	}
	int o1 = measurenum(level) / 2;
	int o2 = Str::level.drawcen(rend, x + 64 - o1, y + 128 + 44, col);
	drawnum(rend, level, x + 64 - o1 + o2, y + 128 + 34, col);
}

static bool ropeMax(const Upgrades &u) {
	return u.ropemult <= 0.3f;
}

static void drawExitText(SDL_Renderer *rend, Mouse m) {
	SDL_Color col = getGraphicColor(m, (800 - 60) / 2, 600 - 40, 60, 20, true);
	Str::exit.drawcen(rend, 400, 600 - 30, col);
}

static int ropeCost(int ropelvl) {
	return (1 + ropelvl * ropelvl * ropelvl) * 20;
}

void Upgrades::drawMenu(SDL_Renderer *rend, Mouse m, Score *s)
{
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 200);
	SDL_RenderFillRect(rend, nullptr);

	Str::upgrade.drawcen(rend, 400, 30, SDL_Color{ 255, 255, 255, 255 });

	bool ropeAffordable = s->score >= ropeCost(ropelvl);
	drawGraphicOutline(rend, menuCenX, menuCenY, m, ropeAffordable);
	drawRopeGraphic(rend, menuCenX, menuCenY);
	drawGraphicTitle(rend, menuCenX, menuCenY, &Str::ropeUpgrade, m, ropeCost(ropelvl), ropeAffordable);
	drawGraphicLevel(rend, menuCenX, menuCenY, ropelvl, ropeMax(*this), m, ropeAffordable);

	drawExitText(rend, m);
}

void Upgrades::mouseUp(Mouse m, Score *s)
{
	if (isMouseHovering(m, menuCenX, menuCenY)) {
		if (s->score < ropeCost(ropelvl))
			return;
		s->score -= ropeCost(ropelvl);
		ropelvl++;
		ropemult -= 0.1f;
		return;
	}
	if (isMouseHovering(m, (800 - 60) / 2, 600 - 40, 60, 20)) {
		closeMenu();
		return;
	}
}

void Upgrades::openMenu()
{
	menuopen = true;
	menuEverOpened = true;
}

void Upgrades::closeMenu()
{
	menuopen = false;
}

void Upgrades::drawMenuHint(SDL_Renderer *rend)
{
	if (menuEverOpened)
		return;
	Str::upgradeHint.draw(rend, 20, 600 - 40, SDL_Color{ 128, 128, 128, 255 });
}
