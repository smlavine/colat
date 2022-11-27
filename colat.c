/**
 * colat - simple program to show what hexadecimal colors actually look like
 * Copyright (c) 2019-2022 Sebastian LaVine <mail@smlavine.com>
 * SPDX-License-Identifier: MIT
 * See LICENSE for details.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>

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

	printf("colorbits to start:\n");
	print32asbinary(colorbits.i);

	// Allow for colors to begin like "#fff".
	if (*s == '#')
		s++;

	for (shift = 0; *s != '\0' && shift <= MAX_SHIFT; s++, shift += NIBBLE) {
		int x = hextoi(*s);
		if (x < 0) {
			warn("'%s' is not a valid color.\n", colorstr);
			return 1;
		}
		printf("x=%d\n", x);
		printf("x\t");
		print32asbinary(x);
		printf("x shift\t");
		print32asbinary((Uint32)x << shift);
		printf("before\t");
		print32asbinary(colorbits.i);
		colorbits.i |= ((Uint32)x << shift);
		printf("after\t");
		print32asbinary(colorbits.i);
	}

	// TODO: "doubling" nibbles on 12-bit colors
	// TODO: error handling if the value of shift is not 12 or 24
	// characters (12-bit/24-bit) were read.

	printf("finally\t");
	print32asbinary(colorbits.i);

	*newcolor = colorbits.c;
	return 0;
}

int
main(int argc, char *argv[])
{
	// SDL objects.
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	
	// Quit if no color arguments.
	if (argc < 2)
		return EXIT_SUCCESS;

	program_invocation_name = argv[0];

	// RGB values that will be used to color the screen.
	struct color colors[argc - 1];
	for (int i = 1; i < argc; i++) {
		if (fill_color(&colors[i - 1], argv[i]) < 0)
			return EXIT_FAILURE;
	}

	// Initialize all the SDL things.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		err("Error initializing SDL: %s", SDL_GetError());
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

	// Loop through the program arguments, displaying them as hex-codes for
	// colors. Quit on q or ESC; SPACE, ENTER or RIGHT ARROW moves to the
	// next color; BACKSPACE or LEFT ARROW moves to the previous color.
	int quit = 0;
	// Where in the colors array is being displayed.
	int index = 0;
	while (!quit) {
		// Update color being shown
		SDL_SetRenderDrawColor(renderer, colors[index].r,
			colors[index].g, colors[index].b, colors[index].a
		);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&event)) {
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
					if (index < argc - 2)
						index++;
					break;
				case SDLK_BACKSPACE:
				case SDLK_LEFT:
				case SDLK_k:
					// Shift to previous image
					if (index > 0)
						index--;
					break;
				}
				break;
			}
		}
	}

	// Exit properly.
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}
