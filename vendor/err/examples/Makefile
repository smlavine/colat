# paths
PREFIX = /usr

#includes and libs
INCS = -I$(PREFIX)/include
LIBS =

# flags
CPPFLAGS = $(INCS)
CFLAGS = -std=c89 -Wall -Wextra -pedantic
LDFLAGS = -L$(PREFIX)/lib $(LIBS)

# compiler and linker
CC = cc


examples = files hello weekday

all: $(examples)

options:
	@echo 'examples build options:'
	@echo "CPPFLAGS = $(CPPFLAGS)"
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

files: files.c ../err.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $?

hello: hello.c ../err.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $?

weekday: weekday.c ../err.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $?

clean:
	rm -f $(examples)

.PHONY: all options $(examples) clean
