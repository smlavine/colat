/**
 * colat - simple program to show what hexadecimal colors actually look like
 * Copyright (c) 2019-2022 Sebastian LaVine <mail@smlavine.com>
 * SPDX-License-Identifier: MIT
 * See LICENSE for details.
 */

#include <limits.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "err/err.h"

// The size in bytes of hexadecimal color strings that we can handle, not
// counting a possible leading '#' character.
enum {
	_12BITLEN = 3,
	_24BITLEN = 6,
};

// Maximum allowable color length + 1 for a possible '#' character.
enum { COLORINFO_S_MAXLEN = _24BITLEN + 1 };

// A color, along with the string that the user provided for it.
struct colorinfo {
	SDL_Color color;
	char str[COLORINFO_S_MAXLEN + 1 /* for NUL */];
};

// Possible return values for strtocolor().
enum strtocolor_value {
	// strtocolor() completed successfully.
	STRTOCOLOR_OK,
	// strtocolor() failed because the provided string contained a
	// non-hexadecimal character.
	STRTOCOLOR_STR_INVALID,
	// strtocolor() failed because the provided length was invalid. If the
	// length was valid but simply inaccurate, then strtocolor() may
	// deceptively return STRTOCOLOR_OK.
	STRTOCOLOR_LEN_INVALID,
};

// Converts a hexadecimal char to an integer. If the provided char is
// not hexadecimal, then a negative value is returned.
int8_t
hextoi(char c)
{
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 0x0A;
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 0x0a;
	} else {
		return -1;
	}
}

// Converts a hexadecimal color string str of length n to a SDL_Color.
enum strtocolor_value
strtocolor(SDL_Color *color, const char *str, size_t n)
{
	enum { NIBBLE = CHAR_BIT / 2 };
	int8_t rr, r, gg, g, bb, b;

	// Skip a leading '#' character, if one is present.
	if (str[0] == '#') {
		str++;
		n--;
	};

	switch (n) {
	case _12BITLEN:
		r = hextoi(*str++);
		g = hextoi(*str++);
		b = hextoi(*str++);
		if (r < 0 || g < 0 || b < 0) {
			return STRTOCOLOR_STR_INVALID;
		}
		color->r = (r << NIBBLE) + r;
		color->g = (g << NIBBLE) + g;
		color->b = (b << NIBBLE) + b;
		break;
	case _24BITLEN:
		rr = hextoi(*str++);
		r = hextoi(*str++);
		gg = hextoi(*str++);
		g = hextoi(*str++);
		bb = hextoi(*str++);
		b = hextoi(*str++);
		if (rr < 0 || r < 0 || gg < 0 || g < 0 || bb < 0 || b < 0) {
			return STRTOCOLOR_STR_INVALID;
		}
		color->r = (rr << NIBBLE) + r;
		color->g = (gg << NIBBLE) + g;
		color->b = (bb << NIBBLE) + b;
		break;
	default:
		return STRTOCOLOR_LEN_INVALID;
	}
	color->a = SDL_ALPHA_OPAQUE;

	return STRTOCOLOR_OK;
}

// Generates a random color.
void
randomize_colorinfo(struct colorinfo *color)
{
	enum { HEXAMT = 16 };
	static const char hex[HEXAMT] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F',
	};
	color->str[0] = '#';
	for (size_t i = 1; i < COLORINFO_S_MAXLEN; i++) {
		color->str[i] = hex[rand() % HEXAMT];
	}
	color->str[COLORINFO_S_MAXLEN] = '\0';
	strtocolor(&color->color, color->str, COLORINFO_S_MAXLEN);
}

// Initializes SDL objects. Returns 0 on success, negative on failure.
// On success, SDL_Quit is registered with atexit(3).
int
init_sdl(SDL_Window **windowp, SDL_Renderer **rendererp)
{
	const int WIDTH = 400, HEIGHT = 400;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		ewarn("Error initializing SDL: %s", SDL_GetError());
		return -1;
	}
	atexit(SDL_Quit);

	*windowp = SDL_CreateWindow("colat",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT,
		SDL_WINDOW_RESIZABLE);
	if (*windowp == NULL) {
		ewarn("Error creating window: %s", SDL_GetError());
		return -1;
	}

	*rendererp = SDL_CreateRenderer(*windowp, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*rendererp == NULL) {
		SDL_DestroyWindow(*windowp);
		ewarn("Error creating renderer: %s", SDL_GetError());
		return -1;
	}

	return 0;
}

// Helper function to paint the screen with a given color.
void
paint(SDL_Renderer *renderer, SDL_Color color)
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

	paint(r, colors[index].color);
	puts(colors[index].str);
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
				paint(r, colors[index].color);
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
					paint(r, colors[index].color);
					puts(colors[index].str);
				}
				break;
			case SDLK_BACKSPACE:
			case SDLK_LEFT:
			case SDLK_k:
				// Shift to previous image
				if (index > 0) {
					index--;
					paint(r, colors[index].color);
					puts(colors[index].str);
				}
				break;
			}
			break;
		}
	}

	return 0;
}

// Prints usage information to the provided file.
void
usage(FILE *out)
{
	fprintf(out,
		"usage: %s [-h] [-r amt] [colors...]\n"
		"-h\tPrints this usage information.\n"
		"-r amt\tDisplays `amt` randomly generated 24-bit colors in addition to\n"
		"      \tcolors provided on the command line.\n"
		"\n"
		"Colors can be specified as 12- or 24-bit hexadecimal,\n"
		"and can optionally begin with a '#' character.\n"
		"If -r isn't used, at least one color must be provided.\n"
		"More information can be found at <https://sr.ht/~smlavine/colat>.\n",
		program_invocation_name);
}

int
main(int argc, char *argv[])
{
	int opt, ret;
	long l;
	size_t r, colors_cap, ci;
	SDL_Window *window;
	SDL_Renderer *renderer;

	program_invocation_name = argv[0];

	r = 0;

	while ((opt = getopt(argc, argv, "hr:")) != -1) {
		switch (opt) {
		case 'h':
			usage(stdout);
			exit(EXIT_SUCCESS);
			break;
		case 'r':
			l = strtol(optarg, NULL, 10);
			if (l < 0) {
				err("-r: amt cannot be negative");
			} else if (l == 0) {
				// Also covers the case where the string
				// cannot be converted.
				err("-r: amt cannot be zero");
			} else if (l == LONG_MAX) {
				err("-r");
			}
			r = strtoul(optarg, NULL, 10);
			break;
		case '?':
			exit(EXIT_FAILURE);
			break;
		default:
			abort();
			break;
		}
	}

	if ((colors_cap = argc - optind + r) == 0) {
		err("no colors provided");
	}

	struct colorinfo colors[colors_cap];

	// Fill random colors
	srand(time(NULL));
	for (ci = 0; ci < r; ci++) {
		randomize_colorinfo(&colors[ci]);
	}

	// Fill command-line colors
	for (int ai = optind; ai < argc; ci++, ai++) {
		int n = snprintf(colors[ci].str, sizeof(colors[ci].str),
			"%s", argv[ai]);
		switch (strtocolor(&colors[ci].color, colors[ci].str, n)) {
		case STRTOCOLOR_OK:
			break;
		case STRTOCOLOR_STR_INVALID:
			warn("%s contains a bad character.\n", colors[ci].str);
			return EXIT_FAILURE;
		case STRTOCOLOR_LEN_INVALID:
			warn("%s is not a valid length.\n", argv[ai]);
			return EXIT_FAILURE;
		default:
			abort();
		}
	}

	if (init_sdl(&window, &renderer) < 0)
		return EXIT_FAILURE;

	if (run(renderer, colors, colors_cap) == 0) {
		ret = EXIT_SUCCESS;
	} else {
		ret = EXIT_FAILURE;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return ret;
}
