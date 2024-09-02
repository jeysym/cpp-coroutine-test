#include "co_task.h"
#include "timers.h"

CoTask CoTask::CoPromise::get_return_object() { return {}; }

std::suspend_never CoTask::CoPromise::initial_suspend() { return {}; }

std::suspend_always CoTask::CoPromise::final_suspend() noexcept { return {}; }

void CoTask::CoPromise::return_void() {}

void CoTask::CoPromise::unhandled_exception() {
  // TODO: Do I need this if I am not using exceptions?
}

CoWait::CoWait(float durationSeconds) : m_DurationSeconds(durationSeconds) {}

bool CoWait::await_ready() { return false; }

bool CoWait::await_resume() { return false; }

void CoWait::await_suspend(CoTask::handle_type handle) {
  Timers::add(m_DurationSeconds, [handle]() { handle.resume(); });
}
