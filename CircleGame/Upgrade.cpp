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
	drawOctagon(rend, 16, x + 128 - 16, y + 16);

	SDL_RenderDrawLine(rend, x + 32, y + 128 - 32, x + 128 - 16, y + 16);

	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	drawOctagon(rend, 32, x + 32, y + 128 - 32);
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

static void drawGraphicDescription(SDL_Renderer *rend, int x, int y, RendString *str, Mouse m) {
	if (!isMouseHovering(m, x, y))
		return;
	str->drawcen(rend, 400, 600 - 30 - 30, SDL_Color{ 255, 255, 255, 255 });
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

static void drawGraphicInfo(SDL_Renderer *rend, Mouse m, int x, int y, const Score &s, int cost, int level, bool maxLevel, RendString *title, RendString *desc) {
	bool affordable = s.score >= cost;
	drawGraphicOutline(rend, x, y, m, affordable);
	drawGraphicTitle(rend, x, y, title, m, cost, affordable);
	drawGraphicLevel(rend, x, y, level, maxLevel, m, affordable);
	drawGraphicDescription(rend, x, y, desc, m);
}

static void drawFSGraphic(SDL_Renderer *rend, int x, int y) {
	SDL_SetRenderDrawColor(rend, 0, 255, 0, 100);
	drawOctagon(rend, 16, x + 64, y + 64 - 16);
	drawOctagon(rend, 16, x + 64 + 42, y + 64 - 16 - 24);
	drawOctagon(rend, 16, x + 64 - 42, y + 64 - 16 + 24);
	SDL_SetRenderDrawColor(rend, 0, 255, 0, 150);
	drawOctagon(rend, 16, x + 64, y + 64 - 8);
	drawOctagon(rend, 16, x + 64 + 42, y + 64 - 8 - 24);
	drawOctagon(rend, 16, x + 64 - 42, y + 64 - 8 + 24);
	SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
	drawOctagon(rend, 16, x + 64, y + 64);
	drawOctagon(rend, 16, x + 64 + 42, y + 64 - 24);
	drawOctagon(rend, 16, x + 64 - 42, y + 64 + 24);
}

static int fsCost(int level) {
	int l4 = level * level * level * level;
	return 30 * (1 + l4);
}

static int fsMax(const Upgrades &u) {
	return u.fsmult <= 0.4f;
}

void Upgrades::drawMenu(SDL_Renderer *rend, Mouse m, Score *s)
{
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 200);
	SDL_RenderFillRect(rend, nullptr);

	Str::upgrade.drawcen(rend, 400, 30, SDL_Color{ 255, 255, 255, 255 });

	bool ropeAffordable = s->score >= ropeCost(ropelvl);

	drawGraphicInfo(
		rend,
		m,
		menuCenX - 120, menuCenY,
		*s, ropeCost(ropelvl), ropelvl, ropeMax(*this),
		&Str::ropeUpgrade, &Str::ropeUpgradeDesc);
	drawRopeGraphic(rend, menuCenX - 120, menuCenY);

	drawGraphicInfo(
		rend,
		m,
		menuCenX + 120, menuCenY,
		*s, fsCost(fslvl), fslvl, fsMax(*this),
		&Str::fallerSpeedUpgrade, &Str::fallerSpeedUpgradeDesc);
	drawFSGraphic(rend, menuCenX + 120, menuCenY);

	drawExitText(rend, m);
}

void Upgrades::mouseUp(Mouse m, Score *s)
{
	if (isMouseHovering(m, menuCenX - 120, menuCenY)) {
		if (ropeMax(*this) || s->score < ropeCost(ropelvl))
			return;
		s->score -= ropeCost(ropelvl);
		ropelvl++;
		ropemult -= 0.1f;
		return;
	}
	if (isMouseHovering(m, menuCenX + 120, menuCenY)) {
		if (fsMax(*this) || s->score < fsCost(fslvl))
			return;
		s->score -= fsCost(fslvl);
		fslvl++;
		fsmult -= 0.1f;
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
