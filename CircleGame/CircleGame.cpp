#include <SDL2/sdl.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "String.h"
#include "NumberStrings.h"
#include "Mouse.h"
#include "Constants.h"
#include "Timer.h"

StaticString streakStr("Streak! x");

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
			0.3f
			) {
			respawn();
		}
	}

	void respawn(float y = -10) {
		animating = false;
		this->y = y;
		x = 10 + rand() % (800 - 20);
		v = 0;
	}

	void draw(SDL_Renderer *rend) const {
		drawocto2(rend, 5, x, y, animating ? animtime * animtime * 30 : 0);
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
		int amt;
		bool operator ==(Collision *c) {
			return this == c;
		}
	};
	std::vector<Collision> collisions;
	std::vector<Collision> streaks;
	bool diddraw = false;

	void add_collision(SDL_Point p, int amt) {
		for (Collision &c : collisions) {
			if (c.rem <= 0) {
				c.p = p;
				c.rem = 1.f;
				c.amt = amt;
				return;
			}
		}
		Collision c;
		c.p = p;
		c.rem = 1.f;
		c.amt = amt;
		collisions.push_back(c);
	}

	void mark_streak(SDL_Point where, int num) {
		for (Collision &c : streaks) {
			if (c.rem <= 0) {
				c.p = where;
				c.rem = sqrtf(num) * 2;
				c.amt = num;
				return;
			}
		}
		Collision c;
		c.p = where;
		c.rem = 2.f;
		c.amt = num;
		streaks.push_back(c);
	}

	void process(float delta) {
		for (Collision &c : collisions) {
			c.rem -= delta;
		}
		for (Collision &c : streaks) {
			c.rem -= delta;
		}
	}

	unsigned char getopac(float lifetime) {
		int ret = lifetime * 500;
		if (ret > 255)
			return 255;
		return ret;
	}

	void draw(SDL_Renderer *rend) {
		diddraw = false;
		for (const Collision &c : collisions) {
			if (c.rem <= 0)
				continue;
			drawnumcen(rend, c.amt, c.p.x, c.p.y, SDL_Color{0, 0, 255, getopac(c.rem)});
			diddraw = true;
		}

		for (const Collision &c : streaks) {
			if (c.rem <= 0)
				continue;
			SDL_Color col = SDL_Color{ 128, 0, 255, getopac(c.rem) };
			int o1 = measurenum(c.amt) / 2;
			int o2 = streakStr.drawcen(rend, c.p.x - o1, c.p.y, col);
			drawnum(rend, c.amt, c.p.x - o1 + o2, c.p.y - 10, col);
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

struct Fallers {
	std::vector<Faller> _fallers;

	Fallers() {
		for (int i = 0; i < num_fallers; i++) {
			_fallers.push_back(Faller{ (float)i * (800 / num_fallers) + (400 / num_fallers) - 5, 100, 0, ((float)rand() / RAND_MAX) * 1.3f });
			Faller f;
			f.respawn(-50);
			f.activationtime = ((float)rand() / RAND_MAX) * 2.3f;
			_fallers.push_back(f);
		}
	}

	void process(float delta, int px, int py, int *streakn, int *streakc, float *score, Collisions *collisions) {
		for (Faller &faller : _fallers) {
			faller.fall(delta);

			if (faller.intersects(px, py)) {
				++*streakn;
				int s = 10
					* (1 + *streakn / 4)
					;
				*score += s;
				*streakc += s;
				collisions->add_collision(SDL_Point{ (int)faller.x, (int)faller.y + 20 }, s);
				if (*streakn % 4 == 0)
					collisions->mark_streak(SDL_Point{ (int)faller.x, (int)faller.y + 40 }, *streakn / 4);
				faller.begin_animate();
			}
		}
	}

	void draw(SDL_Renderer *rend) {
		for (const Faller &faller : _fallers) {
			faller.draw(rend);
		}
	}
};

float highscore = 0.0f;
float highscorecount = 0.0f;

void run(SDL_Renderer *rend, bool *running) {
	SDL_Event ev;
	srand(8213);
	Timer t;
	float px = 400;
	float py = 500;
	Mouse m;
	float vx = 0;
	float vy = 0;
	bool moved = false;
	float strength = 1;
	float score = 0.0f;
	float scorecount = 0.0f;
	float streakccount = 0.0f;
	float streaktimerem = 0.0f;
	bool pb = false;
	int streakn = 0;
	int streakc = 0;
	float timesinceshoot = time_until_shoot;
	float mousedist = 0;
	int gcx = 0;
	int gcy = 0;
	Fallers fallers;
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
				m.x = ev.motion.x;
				m.y = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					m.x = ev.button.x;
					m.y = ev.button.y;
					m.dragging = true;
					gcx = m.x;
					gcy = m.y;
					mousedist = m.getDistance(px, py);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					m.dragging = false;
					moved = true;
					if (timesinceshoot >= time_until_shoot) {
						shoot(&vx, &vy, px, py, m.x, m.y, strength * (1 + sqrtf(streakn) / 4));
						timesinceshoot = 0;
					}
				}
				break;
			}
		}

		t.tick();

		if (moved) {
			timesinceshoot += t.getDelta();
			vy += 200 * t.getDelta();
			strength += 0.02f * t.getDelta();
			if (score > highscore) {
				pb = true;
				highscore = score;
			}

			streaktimerem -= t.getDelta();
			if (collisions.diddraw || streakc != streakccount) {
				streaktimerem = 1.f;
			}

			if (streaktimerem <= 0) {
				streakn = 0;
				streakc = 0;
			}
		}

		px += vx * t.getDelta();
		py += vy * t.getDelta();
		
		if (!m.dragging) {
			if (px < 0) {
				px += 800;
			}
			if (px > 800) {
				px -= 800;
			}
		}
		if (py < -20 || py > 600 + 20)
			return;

		if (moved && m.dragging && timesinceshoot >= time_until_shoot) {
			calcrope(&px, &py, &vx, &vy, gcx, gcy, mousedist);
			mousedist += t.getDelta() * 80 * strength * (1 + sqrtf(streakn) / 4);
		}

		if (moved) {
			collisions.process(t.getDelta());
			fallers.process(t.getDelta(), px, py, &streakn, &streakc, &score, &collisions );
		}

		if (py < 255) {
			int dark = py;
			dark += 128;
			if (dark > 255)
				dark = 255;
#if DARK_MODE
			dark = 255 - dark;
			dark += (int)(sqrtf(streakn) * 5);
#else
			dark -= (int)(sqrtf(streakn) * 5);
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
			dark += (int)(sqrtf(streakn) * 5);
#else
			dark -= (int)(sqrtf(streakn) * 5);
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		else {
			int dark = 255 - (int)(sqrtf(streakn) * 5);
			if (dark < 0)
				dark = 0;
#if DARK_MODE
			dark = 255 - dark;
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		SDL_RenderClear(rend);

		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		fallers.draw(rend);

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

		highscorecount += t.getDelta() * (moved ? 90 : 300);
		scorecount += t.getDelta() * 90;
		streakccount += t.getDelta() * 90;
		if (highscorecount > highscore)
			highscorecount = highscore;
		if (scorecount > score)
			scorecount = score;
		if (streakccount > streakc)
			streakccount = streakc;

		drawnum(rend,
			highscorecount,
			10, 10, SDL_Color{ 0, 255, 0, 255 });
		int noff = drawnum(rend,
			scorecount,
			10, 30, pb ? SDL_Color{ 255, 128, 0, 255 } : SDL_Color{ 128, 128, 128, 255 });
		if (streaktimerem > 0.0f) {
			noff += drawnumstr(rend, "+", 1, 20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
			drawnum(rend,
				streakccount,
				20 + noff, 30, SDL_Color{ 128, 128, 255, 255 });
		}
#if DEBUG_VARS
		drawfloat(rend, strength, 10, 50, SDL_Color{
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
	if (!loadFont())
		return 1;

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
	
	streakStr.load(rend);
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