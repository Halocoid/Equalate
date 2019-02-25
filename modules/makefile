CC = g++
CFLAGS = -c -I./src/ -I./src/core/
LDFLAGS =

ifeq ($(BUILD), release)
	CFLAGS += -Ofast -s -DNDEBUG -march=native -mtune=native
else
	CFLAGS += -O0 -g -Wall -Wextra
endif

ifdef ECFLAGS
	CFLAGS += $(ECFLAGS)
endif

ifdef ELDFLAGS
	LDFLAGS += $(ELDFLAGS)
endif

.DEFAULT_GOAL := all
.PHONY: all

all: src/core/Helpers.o src/core/Manager.o src/$(MODULE).o
	$(CC) $^ $(LDFLAGS) -o $(MODULE)
	rm src/$(MODULE).o

src/core/Helpers.o: src/core/Helpers.cpp src/core/Helpers.hpp
	$(CC) $< $(CFLAGS) -o $@

src/core/Manager.o: src/core/Manager.cpp src/core/Manager.hpp src/core/Helpers.hpp
	$(CC) $< $(CFLAGS) -o $@

src/$(MODULE).o: src/$(MODULE).cpp
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f src/core/Helpers.o src/core/Manager.o src/*.o
