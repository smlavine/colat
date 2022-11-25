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

#include "err/err.h"

const unsigned CHANNELS = 3; // R, G, and B
const unsigned MAX_CHAN_CHARS = 2; // Allow rgb or rrggbb

// Converts an ASCII hexadecimal string to an array of Uint8s. Returns 0 on
// success, 1 on failure.
int
fill_color(Uint8 color[CHANNELS], const char *s)
{
	const size_t len = strlen(s);

	if (len % CHANNELS != 0 || len > MAX_CHAN_CHARS * CHANNELS) {
		warn("'%s' is not a valid color.\n", s);
		return 1;
	}

	// The length of each color in the string (the "R", or "G", or "B").
	// Either 1 or 2, depending on 12-bit or 24-bit.
	const size_t channel_len = len / CHANNELS;

	for (size_t chan = 0; chan < len; chan += channel_len) {
		for (size_t j = 0; j < channel_len; j++) {
			char c = s[chan + j];
			// Assure channel is made up of hex chars.
			if (!isxdigit(c)) {
				warn("'%s' is not a valid color.\n", s);
				return 1;
			}
			// Converts a hex character to an integer value.
			// We first convert the character to the numeric value
			// it represents. Then, we multiply that by a power of
			// 16 so that it represents the right place in the
			// final value we are computing over the loop.
			// TODO: use strtol instead.
			color[chan / channel_len] +=
				(isdigit(c) ? c - '0' : toupper(c) - 55)
				* pow(16, channel_len == 1 ? 1 : j);
		}
	}
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
	// colors[n][0] = R, [1] = G, [2] = B.
	Uint8 colors[argc - 1][CHANNELS];
	memset(colors, 0, (argc - 1) * CHANNELS * sizeof(Uint8));
	// Before initalizing SDL things, make sure the arguments are valid, and
	// convert from string into Uint8 array.
	// Supports "RGB" or "RRGGBB".
	for (int i = 1; i < argc; i++) {
		if (fill_color(colors[i - 1], argv[i]) != 0)
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
		SDL_SetRenderDrawColor(renderer,
			colors[index][0], colors[index][1], colors[index][2], 0xFF);
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
