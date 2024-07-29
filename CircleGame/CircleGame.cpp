#include <SDL2/sdl.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "String.h"
#include "NumberStrings.h"
#include "Mouse.h"
#include "Constants.h"
#include "Timer.h"
#include "Score.h"
#include "Collisions.h"
#include "StaticStrings.h"
#include "Player.h"
#include "DrawUtil.h"

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

	void process(float delta, int px, int py, Score *s, Collisions *collisions) {
		for (Faller &faller : _fallers) {
			faller.fall(delta);

			if (faller.intersects(px, py)) {
				int pts = s->countHit();
				collisions->add_collision(SDL_Point{ (int)faller.x, (int)faller.y + 20 }, pts);
				if (s->wasStreakHit())
					collisions->mark_streak(SDL_Point{ (int)faller.x, (int)faller.y + 40 }, s->streakMult());
				faller.begin_animate();
			}
		}
	}

	void draw(SDL_Renderer *rend) {
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		for (const Faller &faller : _fallers) {
			faller.draw(rend);
		}
	}
};

void run(SDL_Renderer *rend, bool *running) {
	SDL_Event ev;
	srand(8213);
	Timer t;
	Mouse m;
	Player p;
	float strength = 1;
	Score s;
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
					p.startGrab(m);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					m.dragging = false;
					p.tryShoot(m, strength * s.streakStrengthMult());
				}
				break;
			}
		}

		t.tick();

		if (p.moved) {
			strength += 0.02f * t.getDelta();
			s.tick(t.getDelta(), collisions);
		}

		if (p.tick(m, t.getDelta(), strength * s.streakStrengthMult()))
			return;

		if (p.moved) {
			collisions.process(t.getDelta());
			fallers.process(t.getDelta(), p.px, p.py, &s, &collisions);
		}

		if (p.py < 255) {
			int dark = p.py;
			dark += 128;
			if (dark > 255)
				dark = 255;
#if DARK_MODE
			dark = 255 - dark;
			dark += s.streakDarkness();
#else
			dark -= s.streakDarkness();
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		else if (p.py > 600 - 255) {
			int dark = 255 + 600 - p.py;
			dark += 128;
			if (dark > 255)
				dark = 255;
#if DARK_MODE
			dark = 255 - dark;
			dark += s.streakDarkness();
#else
			dark -= s.streakDarkness();
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		else {
			int dark = 255 - s.streakDarkness();
			if (dark < 0)
				dark = 0;
#if DARK_MODE
			dark = 255 - dark;
#endif
			SDL_SetRenderDrawColor(rend, dark, dark, dark, 255);
		}
		SDL_RenderClear(rend);

		fallers.draw(rend);

		p.draw(rend, m, strength);

		collisions.draw(rend);

		s.tickCounters(t.getDelta(), p.moved);
		s.draw(rend);
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
	
	Str::load(rend);
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