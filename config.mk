# colat version
VERSION = 0.1

# Paths
PREFIX = /usr/local

# Flags
CPPFLAGS =
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Werror `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

# Compiler and linker
CC = cc
