#include "DrawUtil.h"

void drawarrow(SDL_Renderer *rend, int x1, int y1, int x2, int y2, float min) {
	float d = sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	if (!d)
		return;
	if (d < min) {
		x2 -= x1;
		y2 -= y1;
		x2 *= min / d;
		y2 *= min / d;
		drawarrow(rend, x1, y1, x1 + x2, y1 + y2, 0);
		return;
	}
	SDL_RenderDrawLine(rend, x1, y1, x2, y2);
	float angle = atanf((float)(y2 - y1) / (float)(x2 - x1));
	d /= 3;
	if (d > 30)
		d = 30;
	if (x2 - x1 < 0)
		angle += M_PI;
	float a1 = angle + M_PI_4 * 3;
	float a2 = angle - M_PI_4 * 3;
	SDL_RenderDrawLine(rend, x2, y2, x2 + cosf(a1) * d, y2 + sinf(a1) * d);
	SDL_RenderDrawLine(rend, x2, y2, x2 + cosf(a2) * d, y2 + sinf(a2) * d);
}

void drawocto2(SDL_Renderer *rend, int r, int x, int y, int explode) {
	float side = r * sinf(M_PI_2);
	int side2 = side / 2.f;
	SDL_Point points[9];

	SDL_RenderDrawLine(rend, x - side2, y - r - explode, x + side2, y - r - explode);
	SDL_RenderDrawLine(rend, x + side2 + explode, y - r - explode, x + r + explode, y - side2 - explode);
	SDL_RenderDrawLine(rend, x + r + explode, y - side2, x + r + explode, y + side2);
	SDL_RenderDrawLine(rend, x + r + explode, y + side2 + explode, x + side2 + explode, y + r + explode);
	SDL_RenderDrawLine(rend, x + side2, y + r + explode, x - side2, y + r + explode);
	SDL_RenderDrawLine(rend, x - side2 - explode, y + r + explode, x - r - explode, y + side2 + explode);
	SDL_RenderDrawLine(rend, x - r - explode, y + side2, x - r - explode, y - side2);
	SDL_RenderDrawLine(rend, x - r - explode, y - side2 - explode, x - side2 - explode, y - r - explode);
}

void drawocto(SDL_Renderer *rend, int r, int x, int y) {
	drawocto2(rend, r, x, y, 0);
}
