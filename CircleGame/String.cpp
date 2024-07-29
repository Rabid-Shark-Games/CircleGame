#include "String.h"

TTF_Font *font;

bool loadFont()
{
	TTF_Init();

	font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 20);
	if (!font) {
		SDL_MessageBoxButtonData btns[2] = { { 0 }, { 0 } };
		btns[0].buttonid = 0;
		btns[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		btns[0].text = "FUCK YOU";
		btns[1].buttonid = 1;
		btns[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		btns[1].text = "My bad";
		SDL_MessageBoxData msg = { 0 };
		msg.flags = SDL_MESSAGEBOX_ERROR | SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
		msg.buttons = btns;
		msg.numbuttons = 2;
		msg.title = "Look what you did.";
		msg.message = SDL_GetError();
		int res;
		SDL_ShowMessageBox(&msg, &res);
		return false;
	}

	return true;
}

void RendString::load(SDL_Renderer *rend, const char *str) {
	this->_sur = TTF_RenderText_Solid(font, str, SDL_Color{ 255, 255, 255, 255 });
	this->_tex = SDL_CreateTextureFromSurface(rend, this->_sur);
	SDL_SetTextureBlendMode(this->_tex, SDL_BLENDMODE_BLEND);
}

void RendString::destroy() {
	SDL_DestroyTexture(this->_tex);
	SDL_FreeSurface(this->_sur);
}

int RendString::draw(SDL_Renderer *rend, int x, int y, SDL_Color col) {
	SDL_SetTextureColorMod(this->_tex, col.r, col.g, col.b);
	SDL_SetTextureAlphaMod(this->_tex, col.a);
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = this->_sur->w;
	dstrect.h = this->_sur->h;
	SDL_RenderCopy(rend, this->_tex, nullptr, &dstrect);
	return dstrect.w;
}

int RendString::drawcv(SDL_Renderer *rend, int x, int y, SDL_Color col) {
	SDL_SetTextureColorMod(this->_tex, col.r, col.g, col.b);
	SDL_SetTextureAlphaMod(this->_tex, col.a);
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y - this->_sur->h / 2;
	dstrect.w = this->_sur->w;
	dstrect.h = this->_sur->h;
	SDL_RenderCopy(rend, this->_tex, nullptr, &dstrect);
	return dstrect.w;
}

int RendString::drawcen(SDL_Renderer *rend, int x, int y, SDL_Color col) {
	SDL_SetTextureColorMod(this->_tex, col.r, col.g, col.b);
	SDL_SetTextureAlphaMod(this->_tex, col.a);
	SDL_Rect dstrect;
	dstrect.x = x - this->_sur->w / 2;
	dstrect.y = y - this->_sur->h / 2;
	dstrect.w = this->_sur->w;
	dstrect.h = this->_sur->h;
	SDL_RenderCopy(rend, this->_tex, nullptr, &dstrect);
	return dstrect.w / 2;
}

int RendString::getWidth() const {
	return this->_sur->w;
}

StaticString::StaticString(const char *str) : _str(str) {}

void StaticString::load(SDL_Renderer *rend) {
	this->RendString::load(rend, this->_str);
}
