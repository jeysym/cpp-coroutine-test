cpp_files = src/game.cpp src/timers.cpp src/co_task.cpp src/renderer.cpp src/input.cpp src/math.cpp src/x11_main.cpp src/stb_image.cpp
hpp_files = src/game.hpp src/timers.hpp src/co_task.hpp src/renderer.hpp src/input.hpp src/math.hpp
lib_hpp_files = lib/stb/stb_image.h

all: cpp-coroutine-test.elf

run: cpp-coroutine-test.elf
	./cpp-coroutine-test.elf

cpp-coroutine-test.elf: $(cpp_files) $(hpp_files) $(lib_hpp_files)
	clang++ -std=c++20 -o cpp-coroutine-test.elf -g -I lib $(cpp_files) -l X11 -l GL -l GLU
