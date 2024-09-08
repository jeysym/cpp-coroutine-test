cpp_files = game.cpp timers.cpp co_task.cpp renderer.cpp x11_main.cpp stb_image.cpp
hpp_files = game.hpp timers.hpp co_task.hpp renderer.hpp
lib_hpp_files = lib/stb/stb_image.h

all: cpp-coroutine-test.elf

run: cpp-coroutine-test.elf
	./cpp-coroutine-test.elf

cpp-coroutine-test.elf: $(cpp_files) $(hpp_files) $(lib_hpp_files)
	clang++ -std=c++20 -o cpp-coroutine-test.elf -g -I lib $(cpp_files) -l X11 -l GL -l GLU
