CC = g++
CFLAGS = -c -I./src/include -I./src/tmpl -I./src/libs
LOC = build/debug

ifeq ($(BUILD), release)
	CFLAGS += -Ofast -s -DNDEBUG -march=native -mtune=native
	LOC = build/release
else
	CFLAGS += -O0 -g -Wall -Wextra
endif

.DEFAULT_GOAL := all
.PHONY: all App

all:
	@mkdir -p build/
	@mkdir -p $(LOC)
	@make App --no-print-directory

clean:
	rm -rf build/

remove: clean
	rm -f Equalate

$(LOC)/Log.o: src/Log.cpp src/tmpl/Log.tpp src/include/Log.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Opts.o: src/Opts.cpp src/include/Log.hpp src/include/Opts.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Pref.o: src/Pref.cpp src/include/Log.hpp src/include/Opts.hpp src/include/Pref.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Irc.o: src/Irc.cpp src/include/Log.hpp src/include/Pref.hpp src/include/Helpers.hpp src/include/Irc.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Module.o: src/Module.cpp src/include/Log.hpp src/include/Module.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Mods.o: src/Mods.cpp src/include/Helpers.hpp src/include/Irc.hpp src/include/Log.hpp src/include/Pref.hpp src/include/Opts.hpp src/include/Module.hpp src/include/Mods.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Helpers.o: src/Helpers.cpp src/include/Helpers.hpp
	$(CC) $< $(CFLAGS) -o $@

$(LOC)/Main.o: src/Main.cpp src/include/Log.hpp src/tmpl/Log.tpp src/include/Opts.hpp src/include/Pref.hpp src/include/Irc.hpp src/include/Mods.hpp src/include/Helpers.hpp
	$(CC) $< $(CFLAGS) -o $@

App: $(LOC)/Log.o $(LOC)/Opts.o $(LOC)/Pref.o $(LOC)/Irc.o $(LOC)/Helpers.o $(LOC)/Main.o $(LOC)/Module.o $(LOC)/Mods.o
	$(CC) $^ -o Equalate
