all: cpp-coroutine-test.elf

run:
	./cpp-coroutine-test.elf

cpp-coroutine-test.elf: main.cpp timers.h timers.cpp co_task.h co_task.cpp
	g++ -std=c++20 -fcoroutines -o cpp-coroutine-test.elf -g main.cpp timers.cpp co_task.cpp
