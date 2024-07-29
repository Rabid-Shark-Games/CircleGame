#include "Player.h"

#include <cmath>

#include "DrawUtil.h"
#include "NumberStrings.h"
#include "Upgrade.h"

void Player::startGrab(Mouse m)
{
	gcx = m.x;
	gcy = m.y;
	mousedist = m.getDistance(px, py);
}

static void shoot(float *vx, float *vy, int px, int py, int mx, int my, float strength) {
	float dx = mx - px;
	float dy = my - py;
	float d = sqrtf(dx * dx + dy * dy);
	dx /= d;
	dy /= d;
	if (d < MIN_SHOOT_DIST * strength)
		d = MIN_SHOOT_DIST * strength;
	if (d > 1000)
		d = 1000;
	*vx = dx * d * strength * 1.4f;
	*vy = dy * d * strength * 1.8f;
}

void Player::tryShoot(Mouse m, float strength)
{
	moved = true;
	if (timesinceshoot >= TIME_UNTIL_SHOOT) {
		shoot(&vx, &vy, px, py, m.x, m.y, strength);
		timesinceshoot = 0;
	}
}

static void calcrope(float *px, float *py, float *vx, float *vy, int mx, int my, float dist) {
	float ox = *px;
	float oy = *py;

	float dx = *px - mx;
	float dy = *py - my;
	float d = sqrtf(dx * dx + dy * dy);
	if (d > dist) {
		*px -= mx;
		*py -= my;

		*px /= d;
		*py /= d;
		*px *= dist;
		*py *= dist;

		*px += mx;
		*py += my;

		*vx += *px - ox;
		*vy += *py - oy;

		if (oy < *vy) {
			*vy = 0;
		}
	}
}

bool Player::tick(Mouse m, float delta, float strength, Upgrades *u)
{
	if (moved) {
		timesinceshoot += delta;
		vy += 200 * delta;
	}

	px += vx * delta;
	py += vy * delta;

	if (!m.dragging) {
		if (px < 0) {
			px += 800;
		}
		if (px > 800) {
			px -= 800;
		}
	}
	if (py < -20 || py > 600 + 20)
		return true;

	if (moved && m.dragging && timesinceshoot >= TIME_UNTIL_SHOOT) {
		calcrope(&px, &py, &vx, &vy, gcx, gcy, mousedist);
		mousedist += delta * 80 * strength * u->ropemult;
	}

	return false;
}

void Player::draw(SDL_Renderer *rend, Mouse m, float strength)
{
	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	drawOctagon(rend, 20, px, py);
	drawOctagon(rend, 20, px - 800, py);
	drawOctagon(rend, 20, px + 800, py);

	if (moved && m.dragging) {
		SDL_SetRenderDrawColor(rend, 0, 0, timesinceshoot < TIME_UNTIL_SHOOT ? 128 : 255, 255);
		drawOctagon(rend, 10, gcx, gcy);

		SDL_RenderDrawLine(rend, px, py, gcx, gcy);
		SDL_RenderDrawLine(rend, px - 800, py, gcx - 800, gcy);
		SDL_RenderDrawLine(rend, px + 800, py, gcx + 800, gcy);

		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	}

#if COMP_DEBUG_VEL
	SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);

	drawArrow(rend, px, py, px + vx, py + vy, 0);

	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
#endif


	if (m.dragging) {
		if (timesinceshoot < TIME_UNTIL_SHOOT) {
			SDL_SetRenderDrawColor(rend, 128, 0, 0, 255);
			SDL_RenderDrawLine(rend, px, py, m.x, m.y);
			mousedist = m.getDistance(px, py);
		}
		else {
			drawArrow(rend, px, py, m.x, m.y, MIN_SHOOT_DIST);
			drawArrow(rend, px - 800, py, m.x - 800, m.y, MIN_SHOOT_DIST * strength);
			drawArrow(rend, px + 800, py, m.x + 800, m.y, MIN_SHOOT_DIST * strength);
		}
	}

	if (timesinceshoot < TIME_UNTIL_SHOOT) {
		drawnumcen(rend, (TIME_UNTIL_SHOOT - timesinceshoot) * 100, px, py + 30, SDL_Color{
#if COMP_DARK_MODE
			255, 255, 255,
#else
			0, 0, 0,
#endif
				255 });
	}
}
