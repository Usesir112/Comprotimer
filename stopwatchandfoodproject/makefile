PROG = stopwatchWithFood

CC      = gcc
LIBS    = `pkg-config --cflags --libs gtk+-3.0`
CFLAGS  = -std=c99 -Wall -Wextra

PREFIX   ?= /usr/local
BINPREFIX = $(PREFIX)/bin

all: CFLAGS += -O3 -mtune=native
all: $(PROG)

debug: CFLAGS += -O0 -g -pedantic
debug: all

$(PROG): $(PROG).c
	$(CC) $(PROG).c -o $(PROG) $(CFLAGS) $(LIBS)

install:
	mkdir -p $(DESTDIR)/$(BINPREFIX)
	mkdir -p $(DESTDIR)/usr/share/applications/

	install -m 0755 $(PROG) $(DESTDIR)/$(BINPREFIX)/
	install -m 0644 data/$(PROG).desktop $(DESTDIR)/usr/share/applications/

uninstall:
	rm -f $(BINPREFIX)/$(PROG)
	rm -f /usr/share/applications/$(PROG).desktop

clean:
	rm -f $(PROG)
