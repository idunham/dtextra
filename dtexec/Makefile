CC=cc
CFLAGS=-c
LDFLAGS=-lXm -lXt -lX11
SOURCES=main.c main_stubs.c
OBJ=main.o main_stubs.o
PREFIX=/usr/local

all:
	$(CC) $(CFLAGS) $(SOURCES)
	$(CC) -o dtexec $(OBJ) $(LDFLAGS)

clean:
	rm *.o dtexec

install: all
	mkdir -p $(DESTDIR)/$(PREFIX)/bin
	cp dtexec $(DESTDIR)/$(PREFIX)/bin/
