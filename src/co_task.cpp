#include "co_task.hpp"
#include "timers.hpp"
#include <coroutine>

CoContinue::CoContinue(std::coroutine_handle<> handle) : m_Continue(handle) {}

bool CoContinue::await_ready() noexcept { 
  return false;
}

void CoContinue::await_resume() noexcept {}

void CoContinue::await_suspend(std::coroutine_handle<> handle) noexcept {
  if (m_Continue)
    m_Continue.resume();
}

CoWait::CoWait(float durationSeconds) : m_DurationSeconds(durationSeconds) {}

bool CoWait::await_ready() { return false; }

void CoWait::await_resume() {}

void CoWait::await_suspend(std::coroutine_handle<> handle) {
  Timers::add(m_DurationSeconds, [handle]() { handle.resume(); });
}

bool CoWaitFrame::await_ready() { return false; }

void CoWaitFrame::await_resume() {
  // TODO: Return duration of the time waited here.
}

void CoWaitFrame::await_suspend(std::coroutine_handle<> handle) {
  // TODO: This is just a hack, do this properly.
  Timers::add(0.00001f, [handle]() { handle.resume(); });
}
