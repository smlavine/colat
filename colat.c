/**
 * colat - simple program to show what hexadecimal colors actually look like
 * Copyright (c) 2019-2022 Sebastian LaVine <mail@smlavine.com>
 * SPDX-License-Identifier: MIT
 * See LICENSE for details.
 */

#include <assert.h>
#include <limits.h>
#include <SDL2/SDL.h>

#include "debug.h"
#include "err/err.h"

// Size of a half-byte; or a nibble.
const unsigned NIBBLE = CHAR_BIT / 2;

const unsigned CHANNELS = 3; // R, G, and B
const unsigned MAX_CHAN_CHARS = 2; // Allow rgb or rrggbb

struct color {
	// The order of this struct matters; it is cast from a Uint32 in
	// fill_color().
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};

// Converts a hexadecimal char to an integer. If the provided char is
// not hexadecimal, then a negative value is returned.
int
hextoi(char c)
{
	if (!isxdigit(c))
		return -1;
	return (isdigit(c) ? c - '0' : toupper(c) - 55);
}

// Parses a hexadecimal string s into a color that is stored in
// newcolor. Returns 0 on success, negative on failure.
int
fill_color(struct color *const restrict newcolor, const char *colorstr)
{
	unsigned shift;
	const unsigned MAX_SHIFT = CHANNELS * MAX_CHAN_CHARS * NIBBLE;
	const char *s = colorstr;
	union {
		struct color c;
		Uint32 i;
	} colorbits = { .c = {
		.r = 0,
		.g = 0,
		.b = 0,
		.a = SDL_ALPHA_OPAQUE
	} };

	// Allow for colors to begin like "#fff".
	if (*s == '#')
		s++;

	assert(MAX_SHIFT == 24);
	for (shift = 0; *s != '\0' && shift < MAX_SHIFT; s++, shift += NIBBLE) {
		int x = hextoi(*s);
		if (x < 0) {
			warn("'%s' contains an bad character.\n", colorstr);
			return -1;
		}
		colorbits.i |= ((Uint32)x << shift);
	}

	switch (shift) {
	case 12:
		// A 12-bit color was provided. We need to "stretch"
		// colorbits so that each nibble that was provided is doubled.
		Uint32 mask;

		// Blue channel
		mask = (colorbits.i & (0xF << 8));
		colorbits.i |= mask << 12;
		colorbits.i |= mask << 8;

		// Green channel
		mask = (colorbits.i & (0xF << 4));
		colorbits.i |= mask << 8;
		colorbits.i |= mask << 4;

		// Second half of the red channel
		colorbits.i |= (colorbits.i & 0xF) << 4;

		break;
	case 24:
		// A 24-bit color was provided. Nothing else needs to be done.
		break;
	default:
		// A string not of length 3 or 6 was provided.
		warn("'%s' is a bad length.\n", colorstr);
		return -1;
	}

	*newcolor = colorbits.c;
	return 0;
}

// Helper function to paint the screen with a given color.
void
paint(SDL_Renderer *renderer, struct color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

int
main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	
	// Quit if no color arguments.
	if (argc < 2)
		return EXIT_SUCCESS;

	program_invocation_name = argv[0];

	struct color colors[argc - 1];
	for (int i = 1; i < argc; i++) {
		if (fill_color(&colors[i - 1], argv[i]) < 0)
			return EXIT_FAILURE;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		err("Error initializing SDL: %s", SDL_GetError());
	atexit(SDL_Quit);

	window = SDL_CreateWindow("colat",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		400, 400,
		SDL_WINDOW_RESIZABLE);
	if (window == NULL)
		err("Error creating window: %s", SDL_GetError());

	renderer = SDL_CreateRenderer(
		window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
		err("Error creating renderer: %s", SDL_GetError());

	int quit = 0;
	// Where in the colors array is being displayed.
	int index = 0;
	paint(renderer, colors[index]);
	while (!quit) {
		if (SDL_WaitEvent(&event) == 0) {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			err("SDL_WaitEvent() error: %s", SDL_GetError());
		}

		switch (event.type) {
		case SDL_QUIT:
			quit = 1;
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_q:
			case SDLK_ESCAPE:
				quit = 1;
				break;
			case SDLK_SPACE:
			case SDLK_RETURN:
			case SDLK_RIGHT:
			case SDLK_j:
				// Shift to next image
				if (index < argc - 2) {
					index++;
					paint(renderer, colors[index]);
				}
				break;
			case SDLK_BACKSPACE:
			case SDLK_LEFT:
			case SDLK_k:
				// Shift to previous image
				if (index > 0) {
					index--;
					paint(renderer, colors[index]);
				}
				break;
			}
			break;
		}
	}

	// Clean up SDL objects.
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}
