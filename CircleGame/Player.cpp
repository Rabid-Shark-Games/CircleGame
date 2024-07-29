#include "Player.h"

#include <cmath>

#include "DrawUtil.h"
#include "NumberStrings.h"

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
	if (d < min_shoot_dist * strength)
		d = min_shoot_dist * strength;
	*vx = dx * d * strength * 1.4f;
	*vy = dy * d * strength * 1.8f;
}

void Player::tryShoot(Mouse m, float strength)
{
	moved = true;
	if (timesinceshoot >= time_until_shoot) {
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

bool Player::tick(Mouse m, float delta, float strength)
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

	if (moved && m.dragging && timesinceshoot >= time_until_shoot) {
		calcrope(&px, &py, &vx, &vy, gcx, gcy, mousedist);
		mousedist += delta * 80 * strength;
	}

	return false;
}

void Player::draw(SDL_Renderer *rend, Mouse m, float strength)
{
	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	drawocto(rend, 20, px, py);
	drawocto(rend, 20, px - 800, py);
	drawocto(rend, 20, px + 800, py);

	if (moved && m.dragging) {
		SDL_SetRenderDrawColor(rend, 0, 0, timesinceshoot < time_until_shoot ? 128 : 255, 255);
		drawocto(rend, 10, gcx, gcy);

		SDL_RenderDrawLine(rend, px, py, gcx, gcy);
		SDL_RenderDrawLine(rend, px - 800, py, gcx - 800, gcy);
		SDL_RenderDrawLine(rend, px + 800, py, gcx + 800, gcy);

		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	}

#if DEBUG_VEL
	SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);

	drawarrow(rend, px, py, px + vx, py + vy, 0);

	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
#endif


	if (m.dragging) {
		if (timesinceshoot < time_until_shoot) {
			SDL_SetRenderDrawColor(rend, 128, 0, 0, 255);
			SDL_RenderDrawLine(rend, px, py, m.x, m.y);
			mousedist = m.getDistance(px, py);
		}
		else {
			drawarrow(rend, px, py, m.x, m.y, min_shoot_dist);
			drawarrow(rend, px - 800, py, m.x - 800, m.y, min_shoot_dist * strength);
			drawarrow(rend, px + 800, py, m.x + 800, m.y, min_shoot_dist * strength);
		}
	}

	if (timesinceshoot < time_until_shoot) {
		drawnumcen(rend, (time_until_shoot - timesinceshoot) * 100, px, py + 30, SDL_Color{
#if DARK_MODE
			255, 255, 255,
#else
			0, 0, 0,
#endif
				255 });
	}
}
