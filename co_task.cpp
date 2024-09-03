#include "co_task.hpp"
#include "timers.hpp"

CoTask CoTask::CoPromise::get_return_object() { return {}; }

std::suspend_never CoTask::CoPromise::initial_suspend() { return {}; }

std::suspend_always CoTask::CoPromise::final_suspend() noexcept { return {}; }

void CoTask::CoPromise::return_void() {}

void CoTask::CoPromise::unhandled_exception() {
  // TODO: Do I need this if I am not using exceptions?
}

CoWait::CoWait(float durationSeconds) : m_DurationSeconds(durationSeconds) {}

bool CoWait::await_ready() { return false; }

void CoWait::await_resume() { }

void CoWait::await_suspend(CoTask::handle_type handle) {
  Timers::add(m_DurationSeconds, [handle]() { handle.resume(); });
}
