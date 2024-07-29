#include "NumberStrings.h"

#include "String.h"

static RendString numStrs[13];

static int numidx(char c) {
	if (c == '.')
		return 10;
	else if (c == '+')
		return 11;
	else if (c == '-')
		return 12;
	return c - '0';
}

void initnums(SDL_Renderer *rend) {
	for (int i = 0; i < 10; i++) {
		char fmt[2] = { '0' + i, 0 };
		numStrs[i].load(rend, fmt);
	}
	numStrs[10].load(rend, ".");
	numStrs[11].load(rend, "+");
	numStrs[12].load(rend, "-");
}

int measurenum(int num) {
	char buf[256];
	int c = snprintf(buf, 256, "%d", num);
	int off = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		off += numStrs[numidx(buf[i])].getWidth();
	}
	return off;
}

int drawnum(SDL_Renderer *rend, int num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%d", num);
	int off = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		off += numStrs[numidx(buf[i])].draw(rend, x + off, y, col);
	}
	return off;
}

int drawnumstr(SDL_Renderer *rend, const char *num, int c, int x, int y, SDL_Color col) {
	int off = 0;
	for (int i = 0; i < c; ++i) {
		off += numStrs[numidx(num[i])].draw(rend, x + off, y, col);
	}
	return off;
}

int drawnumcen(SDL_Renderer *rend, int num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%d", num);
	int off = 0;
	int len = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		len += numStrs[numidx(buf[i])].getWidth();
	}
	off = -len / 2;
	for (int i = 0; i < c; ++i) {
		off += numStrs[numidx(buf[i])].drawcv(rend, x + off, y, col);
	}
	return off;
}

void drawfloat(SDL_Renderer *rend, float num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%f", num);
	int off = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		off += numStrs[numidx(buf[i])].draw(rend, x + off, y, col);
	}
}

void drawfloatcen(SDL_Renderer *rend, float num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%f", num);
	int off = 0;
	int len = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		len += numStrs[numidx(buf[i])].getWidth();
	}
	off = -len / 2;
	for (int i = 0; i < c; ++i) {
		off += numStrs[numidx(buf[i])].drawcv(rend, x + off, y, col);
	}
}