CC=cc
LDFLAGS=-lXm -lXt -lX11
CFLAGS=-Os
D_CFLAGS=-g -DDEBUG
#DESTDIR=../dtbin
PREFIX=/usr/local

all:
	$(CC) $(CFLAGS) dtscan.c dtscan_stubs.c $(LDFLAGS) -o dtscan

debug:
	$(CC) $(D_CFLAGS) dtscan.c dtscan_stubs.c $(LDFLAGS)

clean:
	rm dtscan
install: all
	cp dtscan $(DESTDIR)/$(PREFIX)/bin
