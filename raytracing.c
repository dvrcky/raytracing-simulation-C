#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_video.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <math.h>

/* a few basic macros */
#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_GRAY 0xefefefef

const static SDL_Rect erase_rect = (SDL_Rect) {0,0,WIDTH, HEIGHT};

/* Struct to describe circle */
typedef struct Circle {
	double x;
	double y;
	double radius;
} Circle;

typedef struct Area {
	Circle* circles;
	size_t size;
} Area;

/* Struct to describe ray */
typedef struct Ray {
	double x_start, y_start;
	double angle;
} Ray;

/* Draw a circle. */
void FillCircle(SDL_Surface* surface, Circle* circle, uint32_t color) {
	double radius_squared = pow(circle->radius, 2);

	for (double x = circle->x - circle->radius; x <= circle->x + circle->radius; ++x) {
		for (double y = circle->y - circle->radius; y <= circle->y + circle->radius; ++y) {
			double distance_squared = (pow((x - circle->x), 2) + pow((y - circle->y), 2));
			if (distance_squared <= radius_squared) {
				SDL_Rect pixel = (SDL_Rect){x,y,1,1};
				SDL_FillRect(surface, &pixel, color);
			}
		}
	}
}

// Formulas of lines those are borders of window.

 /*****************
 *								*
 * 	upper line:	  *
 * 		y = 0;			*
 * 		x = diff;		*
 *								*
 * 	bottom line		*
 * 		y = HEIGHT;	*
 * 		x = diff;		*
 *								*
 * 	left line			*
 * 		y = diff;		*
 * 		x = 0				*
 *								*
 * 	right line 		*
 * 		y = diff;		*
 * 		x = WIDTH;	*
 *								*
 *****************/

/* Takes empty array of rays and fills it in */
void MakeCircleEmitRays(Circle* circle, Ray* rays, uint64_t rays_number) {
	for (size_t i = 0; i < rays_number; ++i) {
		double angle = ((double)i / (double)rays_number) * 2 * M_PI;

		Ray ray = {
			.x_start = circle->x,
			.y_start = circle->y,
			.angle = angle,
		};

		rays[i] = ray;

		printf("angle: %f\n", angle);
	}
}

void FillRays(SDL_Surface* surface, Ray* rays, size_t rays_number,
							Uint32 color, Area* area) {
	for (size_t i = 0; i < rays_number; ++i) {
		Ray ray = rays[i];

		bool end_of_screen = false;
		bool object_hit = false;

		double step = 1;
		double x_draw = ray.x_start;
		double y_draw = ray.y_start;
		while (!end_of_screen && !object_hit) {
			x_draw += step * cos(ray.angle);
			y_draw += step * sin(ray.angle);

			SDL_Rect pixel = (SDL_Rect) {
				.x = x_draw,
				.y = y_draw,
				.h = 1,
				.w = 1,
			};
			SDL_FillRect(surface, &pixel, color);

			if (x_draw < 0 || x_draw > WIDTH || y_draw < 0 || y_draw > HEIGHT) {
				end_of_screen = true;
			}

			for (size_t i = 0; i < area->size; ++i) {
				double circle_x = area->circles[i].x;
				double circle_y = area->circles[i].y;
				double radius = area->circles[i].radius;


				if (pow(x_draw - circle_x, 2) + pow(y_draw - circle_y, 2) <= pow(radius, 2)) {
					object_hit = true;
					break;
				}
			}
		}
	}
}

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Raytracing",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	SDL_Event event;
	SDL_Rect rect = (SDL_Rect) {
		.x = 200,
		.y = 200,
		.h = 200,
		.w = 200
	};

	SDL_Surface* surface = SDL_GetWindowSurface(window);

	Area area = (Area) {
		.size = 2,
		.circles = NULL,
	};

	Circle source_of_light = {
		.x = 200,
		.y = 150,
		.radius = 80
	};

	Circle shadow_circle = {
		.x = 700,
		.y = 400,
		.radius = 100
	};

	Circle another_circle = {
		.x = 100,
		.y = 100,
		.radius = 50,
	};

	area.circles = calloc(2, sizeof(Circle));

	area.circles[0] = shadow_circle;
	area.circles[1] = another_circle;

	printf("%f", area.circles[0].x);

	Ray* rays = calloc(100, sizeof(Ray));
	MakeCircleEmitRays(&source_of_light, rays, 100);

	int simulation_running = true;

	/* main loop */
	while (simulation_running) {
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { simulation_running = false; }
			if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
				source_of_light.x = event.motion.x;
				source_of_light.y = event.motion.y;
				MakeCircleEmitRays(&source_of_light, rays, 100);
			}
		}

		SDL_FillRect(surface, &erase_rect, COLOR_BLACK);

		for (size_t i = 0; i < area.size; ++i) {
			FillCircle(surface, &area.circles[i], COLOR_WHITE);
		}

		FillCircle(surface, &source_of_light, COLOR_WHITE);
		FillRays(surface, rays, 100, COLOR_GRAY, &area);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(10);
	}

	SDL_FreeSurface(surface);
	SDL_DestroyWindowSurface(window);
	SDL_DestroyWindow(window);
	SDL_Quit();
	free(rays);

	return EXIT_SUCCESS;
}


