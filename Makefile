all: cpp-coroutine-test.elf

run: cpp-coroutine-test.elf
	./cpp-coroutine-test.elf

cpp-coroutine-test.elf: game.cpp timers.hpp timers.cpp co_task.hpp co_task.cpp x11_main.cpp
	clang++ -std=c++20 -o cpp-coroutine-test.elf -g -I lib game.cpp timers.cpp co_task.cpp x11_main.cpp -l X11 -l GL -l GLU
