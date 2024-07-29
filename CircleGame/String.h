#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

extern TTF_Font *font;
bool loadFont();

struct RendString {
	SDL_Surface *_sur;
	SDL_Texture *_tex;

	void load(SDL_Renderer *rend, const char *str);

	void destroy();

	int draw(SDL_Renderer *rend, int x, int y, SDL_Color col);

	int drawcv(SDL_Renderer *rend, int x, int y, SDL_Color col);

	int drawcen(SDL_Renderer *rend, int x, int y, SDL_Color col);

	int getWidth() const;
};

struct StaticString : public RendString {
	const char *_str;

	StaticString(const char *str);

	void load(SDL_Renderer *rend);
};