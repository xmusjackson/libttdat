OBJS	= obj/ttdat.o util/obj/file.o util/obj/util.o test/obj/ttdat_test.o
SOURCE	= ttdat.cpp util/file.cpp util/util.cpp test/ttdat_test.cpp
HEADER	= ttdat.hpp util/file.hpp util.hpp test/ttdat_test.hpp
OUT	    = build build/bin build/lib build/lib/libttdat.so build/bin/ttdat_test
CC	    = g++
FLAGS	= -std=c++23 -c -O2 -Werror -Wall -Wextra -Wpedantic -Wshadow
LFLAGS	= -lttdat -L./build/lib -I.
SOFLAGS = -fPIC -shared
DFLAGS  = -DTTDAT_DEBUG
OBJDIRS = test/obj util/obj obj

all: $(OUT)

build/bin/ttdat_test: test/obj/ttdat_test.o build/lib/libttdat.so build/bin
	$(CC) $(DFLAGS) $(LFLAGS) -o build/bin/libttdat_test test/obj/ttdat_test.o

build/lib/libttdat.so: obj/ttdat.o util/obj/file.o util/obj/util.o build/lib
	$(CC) $(SOFLAGS) -o build/lib/libttdat.so obj/ttdat.o util/obj/file.o util/obj/util.o

test/obj/ttdat_test.o: test/ttdat_test.cpp obj
	$(CC) $(FLAGS) $(DFLAGS) test/ttdat_test.cpp -o test/obj/ttdat_test.o

obj/ttdat.o: ttdat.cpp obj
	$(CC) $(FLAGS) $(DFLAGS) -fPIC ttdat.cpp -o obj/ttdat.o

util/obj/file.o: util/file.cpp obj
	$(CC) $(FLAGS) $(DFLAGS) util/file.cpp -o util/obj/file.o

util/obj/util.o: util/util.cpp obj
	$(CC) $(FLAGS) $(DFLAGS) util/util.cpp -o util/obj/util.o

build:
	mkdir build

build/bin: build
	mkdir build/bin

build/lib: build
	mkdir build/lib

obj:
	mkdir $(OBJDIRS)

clean:
	rm -rf $(OBJDIRS) build