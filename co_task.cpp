#include "co_task.hpp"
#include "timers.hpp"

CoTask CoTask::CoPromise::get_return_object() {
  return {.m_Handle = CoTask::handle_type::from_promise(*this)};
}

std::suspend_never CoTask::CoPromise::initial_suspend() { return {}; }

std::suspend_always CoTask::CoPromise::final_suspend() noexcept {
  // TODO: Is this right? Maybe this should be in the returned awaiter.
  if (m_Continue)
    m_Continue.resume();

  return {};
}

void CoTask::CoPromise::return_void() {}

void CoTask::CoPromise::unhandled_exception() {
  // TODO: Do I need this if I am not using exceptions?
}

bool CoTask::await_ready() { return m_Handle.done(); }

void CoTask::await_resume() {}

void CoTask::await_suspend(handle_type handle) { m_Handle.promise().m_Continue = handle; }

CoWait::CoWait(float durationSeconds) : m_DurationSeconds(durationSeconds) {}

bool CoWait::await_ready() { return false; }

void CoWait::await_resume() {}

void CoWait::await_suspend(CoTask::handle_type handle) {
  Timers::add(m_DurationSeconds, [handle]() { handle.resume(); });
}

bool CoWaitFrame::await_ready() { return false; }

void CoWaitFrame::await_resume() {
  // TODO: Return duration of the time waited here.
}

void CoWaitFrame::await_suspend(CoTask::handle_type handle) {
  // TODO: This is just a hack, do this properly.
  Timers::add(0.00001f, [handle]() { handle.resume(); });
}
