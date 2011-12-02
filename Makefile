# Makefile to build dtextra tree
CC=gcc
CPP=g++
PREFIX=/usr/local
DESTDIR=$(pwd)/binary
#Also meditres
AUTOTGTS := nebula-0.5 xps-4.2 Xlt-13.0.13
IMAKETGT := mppp1.1b xmastm1.1 mxascii1.1 xplore-1.2a mxkill1.2 xtar-1.4.1

# Local installs will use static libs, to simplify life	
lib-loc:
	cd Xlt-13.0.13
	./configure --prefix=../libs --enable-static --disable-shared
	make; make install
	cd ..
	LDFLAGS=-L../libs/lib
	CFLAGS=-I../libs/inlude

#meditres-loc: lib-loc
#	cd meditres
#	LDFLAGS=-L../libs/lib
#	CFLAGS=-I../libs/inlude
#	./configure --prefix=$HOME/.local --with-xlt-includes=

config:
	echo "You must manually build meditres, after installing Xlt"
	$(foreach dir,$(AUTOTGTS),cd ${dir} && ./configure --prefix=${PREFIX}; cd ..)
	$(foreach dir,$(IMAKETGT),cd ${dir} && xmkmf -a; cd ..)
	cd xterm_177_motif
	./configure --with-motif --prefix=${PREFIX}
	cd ..

build: config
	SRCDIR := $(IMAKETGT) $(AUTOTGTS) xterm_177_motif
	$(foreach dir,$(SRCDIR),cd ${dir} && make; cd ..}

