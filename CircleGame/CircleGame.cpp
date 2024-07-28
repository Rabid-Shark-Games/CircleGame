#include <SDL2/sdl.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#define WALLS 0
#define ROPE 1
#define DEBUG_VEL 0
#define DEBUG_VARS 0
#if !ROPE
#define INSANE_LOOP 1
#endif
#define DARK_MODE 1
#define EXPLODING_ORBS 1

TTF_Font *font;

struct RendString {
	SDL_Surface *sur;
	SDL_Texture *tex;
};

void loadstring(RendString *out, SDL_Renderer *rend, const char *str) {
	out->sur = TTF_RenderText_Solid(font, str, SDL_Color{ 255, 255, 255, 255 });
	out->tex = SDL_CreateTextureFromSurface(rend, out->sur);
	SDL_SetTextureBlendMode(out->tex, SDL_BLENDMODE_BLEND);
}

void destroystring(RendString *str) {
	SDL_DestroyTexture(str->tex);
	SDL_FreeSurface(str->sur);
}

int drawstring(SDL_Renderer *rend, RendString *str, int x, int y, SDL_Color col) {
	SDL_SetTextureColorMod(str->tex, col.r, col.g, col.b);
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = str->sur->w;
	dstrect.h = str->sur->h;
	SDL_RenderCopy(rend, str->tex, nullptr, &dstrect);
	return dstrect.w;
}

int drawstringcv(SDL_Renderer *rend, RendString *str, int x, int y, SDL_Color col) {
	SDL_SetTextureColorMod(str->tex, col.r, col.g, col.b);
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y - str->sur->h / 2;
	dstrect.w = str->sur->w;
	dstrect.h = str->sur->h;
	SDL_RenderCopy(rend, str->tex, nullptr, &dstrect);
	return dstrect.w;
}

void drawstringcen(SDL_Renderer *rend, RendString *str, int x, int y, SDL_Color col) {
	SDL_SetTextureColorMod(str->tex, col.r, col.g, col.b);
	SDL_Rect dstrect;
	dstrect.x = x - str->sur->w / 2;
	dstrect.y = y - str->sur->h / 2;
	dstrect.w = str->sur->w;
	dstrect.h = str->sur->h;
	SDL_RenderCopy(rend, str->tex, nullptr, &dstrect);
}

RendString streakStr;

RendString numStrs[13];
void initnums(SDL_Renderer *rend) {
	for (int i = 0; i < 10; i++) {
		char fmt[2] = { '0' + i, 0 };
		loadstring(&numStrs[i], rend, fmt);
	}
	loadstring(&numStrs[10], rend, ".");
	loadstring(&numStrs[11], rend, "+");
	loadstring(&numStrs[12], rend, "-");
}

int numidx(char c) {
	if (c == '.')
		return 10;
	else if (c == '+')
		return 11;
	else if (c == '-')
		return 12;
	return c - '0';
}

int drawnum(SDL_Renderer *rend, int num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%d", num);
	int off = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		off += drawstring(rend, &numStrs[numidx(buf[i])], x + off, y, col);
	}
	return off;
}

int drawnumstr(SDL_Renderer *rend, const char *num, int c, int x, int y, SDL_Color col) {
	int off = 0;
	for (int i = 0; i < c; ++i) {
		off += drawstring(rend, &numStrs[numidx(num[i])], x + off, y, col);
	}
	return off;
}

void drawnumcen(SDL_Renderer *rend, int num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%d", num);
	int off = 0;
	int len = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		len += numStrs[numidx(buf[i])].sur->w;
	}
	off = -len / 2;
	for (int i = 0; i < c; ++i) {
		off += drawstringcv(rend, &numStrs[numidx(buf[i])], x + off, y, col);
	}
}

void drawfloat(SDL_Renderer *rend, float num, int x, int y, SDL_Color col) {
	char buf[256];
	int c = snprintf(buf, 256, "%f", num);
	int off = 0;
	if (c > 255) {
		c = 255;
	}
	for (int i = 0; i < c; ++i) {
		off += drawstring(rend, &numStrs[numidx(buf[i])], x + off, y, col);
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
		len += numStrs[numidx(buf[i])].sur->w;
	}
	off = -len / 2;
	for (int i = 0; i < c; ++i) {
		off += drawstringcv(rend, &numStrs[numidx(buf[i])], x + off, y, col);
	}
}

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

constexpr float min_shoot_dist = 150;
constexpr float time_until_shoot = 0.6f;

void shoot(float *vx, float *vy, int px, int py, int mx, int my, float strength) {
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

struct Faller {
	float x;
	float y;
	float v = 0;
	float activationtime = 0;
	bool animating;
	float animtime = 0;

	void fall(float delta) {
		if (animating) {
			animate(delta);
			return;
		}
		if (activationtime > 0) {
			activationtime -= delta;
			return;
		}
		v += 60.f * delta;
		y += v * delta;

		if (y > 600 + 10)
			respawn();
	}

	void begin_animate() {
		animating = true;
		animtime = 0;
	}

	void animate(float delta) {
		animtime += delta;
		if (animtime >=
#if EXPLODING_ORBS
			0.3f
#else
			0.1f
#endif
			) {
			respawn();
		}
	}

	void respawn() {
		animating = false;
		y = -10;
		x = 10 + rand() % (800 - 20);
		v = 0;
	}

	void draw(SDL_Renderer *rend) const {
#if EXPLODING_ORBS
		drawocto2(rend, 5, x, y, animating ? animtime * animtime * 30 : 0);
#else
		drawocto(rend, animating ? 5 + animtime * animtime * 15 : 5, x, y);
#endif
	}

	bool intersects(int px, int py) const {
		if (animating)
			return false;
		float a = x - px;
		float b = y - py;
		return sqrtf(a * a + b * b) < 20 + 8;
	}
};

struct Collisions {
	struct Collision {
		SDL_Point p;
		float rem;
		bool operator ==(Collision *c) {
			return this == c;
		}
	};
	std::vector<Collision> collisions;
	bool diddraw = false;
	int framedrew = 0;
	float streakrem = 0;
	SDL_Point streakPos;

	void add_collision(SDL_Point p) {
		for (Collision &c : collisions) {
			if (c.rem <= 0) {
				c.p = p;
				c.rem = 1.f;
				return;
			}
		}
		Collision c;
		c.p = p;
		c.rem = 1.f;
		collisions.push_back(c);
	}

	void mark_streak(SDL_Point where) {
		streakPos = where;
		streakrem = 2.f;
	}

	void process(float delta) {
		for (Collision &c : collisions) {
			c.rem -= delta;
		}
		streakrem -= delta;
	}

	unsigned char getopac(float lifetime) {
		int ret = lifetime * 500;
		if (ret > 255)
			return 255;
		return ret;
	}

	void draw(SDL_Renderer *rend) {
		diddraw = false;
		framedrew = 0;
		for (const Collision &c : collisions) {
			if (c.rem <= 0)
				continue;
			drawnumcen(rend, 10, c.p.x, c.p.y, SDL_Color{0, 0, 255, getopac(1 - c.rem)});
			diddraw = true;
			++framedrew;
		}

		if (streakrem > 0) {
			drawstringcen(rend, &streakStr, streakPos.x, streakPos.y, SDL_Color{ 128, 0, 255, getopac(2 - streakrem)});
		}
	}
};

void calcrope(float *px, float *py, float *vx, float *vy, int mx, int my, float dist) {
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

float highscore = 0.0f;

void run(SDL_Renderer *rend, bool *running) {
	SDL_Event ev;
	srand(8213);
	unsigned long long lastcount = SDL_GetPerformanceCounter();
	bool dragging = false;
	float px = 400;
	float py = 500;
	int mx = 0;
	int my = 0;
	float vx = 0;
	float vy = 0;
	bool moved = false;
	float strength = 1;
	float wallreimburse = 2.f / 3.f;
	float score = 0.0f;
	float timesincespawn = 0;
	bool pb = false;
	int streak = 0;
	float timesinceshoot = time_until_shoot;
#if ROPE
	float mousedist = 0;
	int gcx = 0;
	int gcy = 0;
#endif
	std::vector<Faller> fallers;
	constexpr int num_fallers = 8;
	for (int i = 0; i < num_fallers; i++) {
		fallers.push_back(Faller{ (float)i * (800 / num_fallers) + (400 / num_fallers) - 5, 100, 0, ((float)rand() / RAND_MAX) * 1.3f});
		Faller f;
		f.respawn();
		f.y = -50;
		f.activationtime = ((float)rand() / RAND_MAX) * 2.3f;
		fallers.push_back(f);
	}
	Collisions collisions;
	while (*running) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_WINDOWEVENT:
				switch (ev.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					*running = false;
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				mx = ev.motion.x;
				my = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					mx = ev.button.x;
					my = ev.button.y;
					dragging = true;
#if ROPE
					gcx = mx;
					gcy = my;
					mousedist = sqrtf((mx - px) * (mx - px) + (my - py) * (my - py));
#endif
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					dragging = false;
					moved = true;
					if (timesinceshoot >= time_until_shoot) {
						shoot(&vx, &vy, px, py, mx, my, strength);
						timesinceshoot = 0;
					}
				}
				break;
			}
		}

		unsigned long long curcount = SDL_GetPerformanceCounter();
		float delta = (float)(curcount - lastcount) / SDL_GetPerformanceFrequency();
		lastcount = curcount;

		if (moved) {
			timesinceshoot += delta;
			vy += 200 * delta;
			strength += 0.02f * delta;
			wallreimburse -= 0.015f * delta;
			if (score > highscore) {
				pb = true;
				highscore = score;
			}

			timesincespawn += delta;
			if (timesincespawn > 5) {
				timesincespawn -= 5;
				//Faller f;
				//f.respawn();
				//fallers.push_back(f);
			}

			if (!collisions.diddraw)
				streak = 0;
		}

		px += vx * delta;
		py += vy * delta;
		
		bool wall = false;
#if WALLS
		if (px <= 20) {
			vx = 0;
			px = 20;
			wall = true;
		}
		if (px >= 800 - 20) {
			vx = 0;
			px = 800 - 20;
			wall = true;
		}
#else
#if ROPE || INSANE_LOOP
		if (!dragging) {
#endif
			if (px < 0) {
				px += 800;
			}
			if (px > 800) {
				px -= 800;
			}
#if ROPE || INSANE_LOOP
		}
#endif
#endif
		if (wall && wallreimburse > 0)
			py -= vy * delta * wallreimburse;
		if (py < -20 || py > 600 + 20)
			return;

#if ROPE
		if (moved && dragging) {
			calcrope(&px, &py, &vx, &vy, gcx, gcy, mousedist);
			mousedist += delta * 80 * strength;
		}
#endif

		if (moved) {
			collisions.process(delta);
			for (Faller &faller : fallers) {
				faller.fall(delta);

				if (faller.intersects(px, py)) {
					score += 10;
					++streak;
					collisions.add_collision(SDL_Point{ (int)faller.x, (int)faller.y + 20 });
					if (streak == 4)
						collisions.mark_streak(SDL_Point{ (int)faller.x, (int)faller.y + 40 });
					faller.begin_animate();
				}
			}
		}

		if (py < 255) {
			int dark = py;
			dark += 128;
			if (dark > 255)
				dark = 255;
#if DARK_MODE
			dark = 255 - dark;
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		else if (py > 600 - 255) {
			int dark = 255 + 600 - py;
			dark += 128;
			if (dark > 255)
				dark = 255;
#if DARK_MODE
			dark = 255 - dark;
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		else
#if DARK_MODE
			SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
#else
			SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
#endif
		SDL_RenderClear(rend);

		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		for (const Faller &faller : fallers) {
			faller.draw(rend);
		}

		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		drawocto(rend, 20, px, py);
#if !WALLS
		drawocto(rend, 20, px - 800, py);
		drawocto(rend, 20, px + 800, py);
#endif

#if ROPE
		if (moved && dragging) {
			SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
			drawocto(rend, 10, gcx, gcy);
			SDL_RenderDrawLine(rend, px, py, gcx, gcy);
			
#if !WALLS
			SDL_RenderDrawLine(rend, px - 800, py, gcx - 800, gcy);
			SDL_RenderDrawLine(rend, px + 800, py, gcx + 800, gcy);
#endif

			SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		}
#endif

#if DEBUG_VEL
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);

		drawarrow(rend, px, py, px + vx, py + vy, 0);

		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
#endif


		if (dragging) {
			if (timesinceshoot < time_until_shoot) {
				SDL_SetRenderDrawColor(rend, 128, 0, 0, 255);
				SDL_RenderDrawLine(rend, px, py, mx, my);
			}
			else {
				drawarrow(rend, px, py, mx, my, min_shoot_dist);
#if !WALLS
				drawarrow(rend, px - 800, py, mx - 800, my, min_shoot_dist * strength);
				drawarrow(rend, px + 800, py, mx + 800, my, min_shoot_dist * strength);
#endif
			}
		}

		collisions.draw(rend);

		if (timesinceshoot < time_until_shoot) {
			drawnumcen(rend, (time_until_shoot - timesinceshoot) * 100, px, py + 30, SDL_Color{
#if DARK_MODE
			255, 255, 255,
#else
			0, 0, 0,
#endif
				255 });
		}

		drawnum(rend, highscore, 10, 10, SDL_Color{ 0, 255, 0, 255 });
		int noff = drawnum(rend, score, 10, 30, pb ? SDL_Color{ 255, 128, 0, 255 } : SDL_Color{ 128, 128, 128, 255 });
		if (collisions.diddraw) {
			noff += drawnumstr(rend, "+", 1, 20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
			drawnum(rend, streak * 10, 20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
		}
#if DEBUG_VARS
		drawfloat(rend, strength, 10, 50, SDL_Color{
#if DARK_MODE
			255, 255, 255,
#else
			0, 0, 0,
#endif
			255 });
		if (wallreimburse > 0)
			drawfloat(rend, wallreimburse, 10, 70, SDL_Color{
#if DARK_MODE
			255, 255, 255,
#else
			0, 0, 0,
#endif
			255 });
#endif

		SDL_RenderPresent(rend);
	}
}

int SDL_main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
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
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
	if (!win) {
		SDL_Log("Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!rend) {
		SDL_Log("Error: %s\n", SDL_GetError());
		return 1;
	}

	loadstring(&streakStr, rend, "Streak!");
	initnums(rend);

	bool running = true;
	while (running) {
		run(rend, &running);
	}
	
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	
	TTF_CloseFont(font);
	
	TTF_Quit();
	SDL_Quit();
	return 0;
}