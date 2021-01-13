# A simple makefile for compiling small SDL projects

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lm

# add header files here
HDRS :=

# add source files here
SRCS := colat.c

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := colat

# default recipe
all: $(EXEC)

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) makefile
#	$(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

install:
	cp $(EXEC) /usr/local/bin/$(exec)
# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
