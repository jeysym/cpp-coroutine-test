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
