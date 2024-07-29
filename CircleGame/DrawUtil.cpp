#include "DrawUtil.h"

#include <SDL2/SDL.h>

void drawArrow(SDL_Renderer *rend, int x1, int y1, int x2, int y2, float min) {
	int dSqr = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	float d = sqrtf((float)dSqr);
	if (!d)
		return;
	if (d < min) {
		x2 -= x1;
		y2 -= y1;
		x2 = (int)((float)x2 * min / d);
		y2 = (int)((float)y2 * min / d);
		drawArrow(rend, x1, y1, x1 + x2, y1 + y2, 0);
		return;
	}
	SDL_RenderDrawLine(rend, x1, y1, x2, y2);
	double angle = atanf((float)(y2 - y1) / (float)(x2 - x1));
	d /= 3;
	if (d > 30)
		d = 30;
	if (x2 - x1 < 0)
		angle += M_PI;
	double a1 = angle + M_PI_4 * 3;
	double a2 = angle - M_PI_4 * 3;
	SDL_RenderDrawLine(rend, x2, y2, (int)(x2 + cos(a1) * d), (int)(y2 + sin(a1) * d));
	SDL_RenderDrawLine(rend, x2, y2, (int)(x2 + cos(a2) * d), (int)(y2 + sin(a2) * d));
}

static void drawOctagonFast(SDL_Renderer *rend, int r, int x, int y) {
	double side = r * sin(M_PI_2);
	int side2 = (int)(side / 2.0);
	
	SDL_Point points[9];
	points[0] = { x - side2, y - r };
	points[1] = { x + side2, y - r };
	points[2] = { x + r, y - side2 };
	points[3] = { x + r, y + side2 };
	points[4] = { x + side2, y + r };
	points[5] = { x - side2, y + r };
	points[6] = { x - r, y + side2 };
	points[7] = { x - r, y - side2 };
	points[8] = { x - side2, y - r };

	SDL_RenderDrawLines(rend, points, 9);
}

void drawOctagonEx(SDL_Renderer *rend, int r, int x, int y, int explode) {
	if (explode == 0)
		return drawOctagonFast(rend, r, x, y);
	
	double side = r * sin(M_PI_2);
	int side2 = (int)(side / 2.0);

	SDL_RenderDrawLine(rend, x - side2, y - r - explode, x + side2, y - r - explode);
	SDL_RenderDrawLine(rend, x + side2 + explode, y - r - explode, x + r + explode, y - side2 - explode);
	SDL_RenderDrawLine(rend, x + r + explode, y - side2, x + r + explode, y + side2);
	SDL_RenderDrawLine(rend, x + r + explode, y + side2 + explode, x + side2 + explode, y + r + explode);
	SDL_RenderDrawLine(rend, x + side2, y + r + explode, x - side2, y + r + explode);
	SDL_RenderDrawLine(rend, x - side2 - explode, y + r + explode, x - r - explode, y + side2 + explode);
	SDL_RenderDrawLine(rend, x - r - explode, y + side2, x - r - explode, y - side2);
	SDL_RenderDrawLine(rend, x - r - explode, y - side2 - explode, x - side2 - explode, y - r - explode);
}

void drawOctagon(SDL_Renderer *rend, int r, int x, int y) {
	drawOctagonEx(rend, r, x, y, 0);
}
