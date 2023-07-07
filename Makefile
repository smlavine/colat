.POSIX:

include config.mk

SRC = colat.c vendor/err/err.c
OBJ = $(SRC:.c=.o)

all: options colat

options:
	@echo Build options:
	@echo "CPPFLAGS = ${CPPFLAGS}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo

$(OBJ): config.mk

colat: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f colat $(OBJ)

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f colat $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/colat

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/colat

.PHONY: all options clean install uninstall
