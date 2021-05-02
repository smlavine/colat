/**
 * colat - simple program to show what hexadecimal colors actually look like
 * Copyright (c) 2019-2021 Sebastian LaVine <mail@smlavine.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define CHANNEL_AMT 3

int fill_colors(Uint8 colors[][CHANNEL_AMT], const int argc, const char *argv[])
{
#define BADCOLOR fprintf(stderr, "\nError reading args: %s is not a valid color.\n", argv[i]); \
					fflush(stderr);
	for (int i = 1; i < argc; i++) {

		const int len = strlen(argv[i]);
		// If length does not match either "RGB" or "RRGGBB"
		if (len != CHANNEL_AMT && len != CHANNEL_AMT * 2) {
			BADCOLOR;
			return 1;
		}
		// Convert channel from hex characters to integer values.

		// The length of each color in the string (the "R", or "G", or "B").
		// Either 1 or 2, depending on 12-bit or 24-bit.
		const int channel_len = len / CHANNEL_AMT;

		// Loop through the channels, and the chars in each channel.
		for (int chan = 0; chan < len; chan += channel_len) {
			// Assure channel is made up of hex chars.
			for (int j = 0; j < channel_len; j++) {
				char c = argv[i][chan + j];
				if (!isxdigit(c)) {
					BADCOLOR;
					return 1;
				}
				// Converts hex digit to decimal number equivilant,
				// multiplies it by a power of 16 to make the number places
				// correct, and adds it to the value of the array.
				colors[i - 1][chan / channel_len] +=
					(isdigit(c) ? c - '0' : toupper(c) - 55)
					* pow(16, channel_len == 1 ? 1 : j);
			}
		}
	}
	return 0;
#undef BADCOLOR
}

int main(const int argc, const char *argv[])
{
	// SDL objects.
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	
	// Quit if no color arguments.
	if (argc < 2)
		return 0;

	// RGB values that will be used to color the screen.
	// colors[n][0] = R, [1] = G, [2] = B.
	Uint8 colors[argc - 1][CHANNEL_AMT];
	memset(colors, 0, (argc - 1) * CHANNEL_AMT * sizeof(Uint8));
	fputs("Loading...", stdout);
	fflush(stdout);
	// Before initalizing SDL things, make sure the arguments are valid, and
	// convert from string into Uint8 array.
	// Supports "RGB" or "RRGGBB".
	if (fill_colors(&colors, argc, argv) != 0)
		return 1;
	// In reference to previous "Loading..." message.
	puts("done.");
	fflush(stdout);

	// Initialize all the SDL things.
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}
	window = SDL_CreateWindow("colat",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, 0);
	if (window == NULL) {
		fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
		return 1;
	}

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
					// Shift to right image
					if (index < argc - 2)
						index++;
					break;
				case SDLK_BACKSPACE:
				case SDLK_LEFT:
					// Shift to left image
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
	return 0;
}
