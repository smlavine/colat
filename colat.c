/**
 * colat - simple program to show what hexadecimal colors actually look like
 * Copyright (c) 2019-2022 Sebastian LaVine <mail@smlavine.com>
 * SPDX-License-Identifier: MIT
 * See LICENSE for details.
 */

#include <assert.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "err/err.h"

struct color {
	// The order of this struct matters; it is cast from a Uint32 in
	// fill_color().
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};

struct colorinfo {
	struct color c;
	const char *s;
};

// Possible return values for fill_color().
enum fill_color_status {
	// fill_color() completed successfully.
	FILL_COLOR_OK,
	// fill_color() failed because the provided string contained a
	// non-hexadecimal character.
	FILL_COLOR_NOT_HEX,
	// fill_color() failed because the provided string was not the correct
	// size for a 12-bit or 24-bit color.
	FILL_COLOR_BAD_SIZE,
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

// Fills newcolor with the color represented by the provided string.
// See the definition of fill_color_status for the meaning of return values.
enum fill_color_status
fill_color(struct colorinfo *const restrict newcolor, const char *name)
{
	enum {
		_12BITLEN = 3,
		_24BITLEN = 6,
		CHANNELS  = 3,
		NIBBLE    = CHAR_BIT / 2,
	};
	const char *s = name;
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

	assert(_12BITLEN == 3 && _24BITLEN == 6 && CHANNELS == 3);
	switch (strnlen(s, _24BITLEN + 1)) {
	case _12BITLEN:
		for (unsigned shift = 0; *s != '\0'; s++, shift += CHAR_BIT) {
			int x = hextoi(*s);
			if (x < 0) {
				return FILL_COLOR_NOT_HEX;
			}
			colorbits.i |= ((Uint32)x << shift)
				| ((Uint32)x << (shift + NIBBLE));
		}
		break;
	case _24BITLEN:
		for (unsigned shift = NIBBLE; *s != '\0';
				s += _24BITLEN / CHANNELS, shift += CHAR_BIT) {
			int x1 = hextoi(*s), x2 = hextoi(*(s + 1));
			if (x1 < 0 || x2 < 0) {
				return FILL_COLOR_NOT_HEX;
			}
			colorbits.i |= ((Uint32)x1 << shift)
				| ((Uint32)x2 << (shift - NIBBLE));
		}
		break;
	default:
		return FILL_COLOR_BAD_SIZE;
	}

	newcolor->c = colorbits.c;
	newcolor->s = name;
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

// Run the main loop. On success, 0 is returned. On error, an error message is
// printed and -1 is returned.
int
run(SDL_Renderer *r, const struct colorinfo *colors, size_t n)
{
	SDL_Event event;
	bool quit = false;
	size_t index = 0;

	paint(r, colors[index].c);
	puts(colors[index].s);
	while (!quit) {
		if (SDL_WaitEvent(&event) == 0) {
			ewarn("SDL_WaitEvent() error: %s", SDL_GetError());
			return -1;
		}

		switch (event.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_EXPOSED:
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_MOVED:
				paint(r, colors[index].c);
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_q:
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_SPACE:
			case SDLK_RETURN:
			case SDLK_RIGHT:
			case SDLK_j:
				// Shift to next image
				if (index < n - 1) {
					index++;
					paint(r, colors[index].c);
					puts(colors[index].s);
				}
				break;
			case SDLK_BACKSPACE:
			case SDLK_LEFT:
			case SDLK_k:
				// Shift to previous image
				if (index > 0) {
					index--;
					paint(r, colors[index].c);
					puts(colors[index].s);
				}
				break;
			}
			break;
		}
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	int ret;
	
	// Quit if no color arguments.
	if (argc < 2)
		return EXIT_SUCCESS;

	program_invocation_name = argv[0];

	struct colorinfo colors[argc - 1];
	for (int i = 1; i < argc; i++) {
		switch (fill_color(&colors[i - 1], argv[i])) {
		case FILL_COLOR_OK:
			break;
		case FILL_COLOR_NOT_HEX:
			warn("%s contains a bad character.\n", argv[i]);
			return EXIT_FAILURE;
		case FILL_COLOR_BAD_SIZE:
			warn("%s is not a valid length.\n", argv[i]);
			return EXIT_FAILURE;
		}
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

	renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		err("Error creating renderer: %s", SDL_GetError());
	}

	if (run(renderer, colors, argc - 1) == 0) {
		ret = EXIT_SUCCESS;
	} else {
		ret = EXIT_FAILURE;
	}

	// Clean up SDL objects.
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return ret;
}
