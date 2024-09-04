all: cpp-coroutine-test.elf

run:
	./cpp-coroutine-test.elf

cpp-coroutine-test.elf: game.cpp timers.hpp timers.cpp co_task.hpp co_task.cpp x11_main.cpp
	g++ -std=c++20 -fcoroutines -o cpp-coroutine-test.elf -g game.cpp timers.cpp co_task.cpp x11_main.cpp -l X11 -l GL -l GLU
