#include "timers.hpp"
#include <algorithm>

std::vector<Timers::Timer> Timers::s_Timers{};

bool Timers::Timer::isDone() const {
  return m_RemainingSeconds <= 0.0f; 
}

void Timers::add(float durationSeconds, callback_type callback) {
  s_Timers.push_back(
      Timer{.m_RemainingSeconds = durationSeconds, .m_Callback = callback});
}

void Timers::update(float deltaSeconds) {
  for (auto &timer : s_Timers) {
    timer.m_RemainingSeconds -= deltaSeconds;

    if (timer.isDone()) {
      timer.m_Callback();
    }
  }

  s_Timers.erase(
      std::remove_if(s_Timers.begin(), s_Timers.end(),
                     [](const Timer &timer) { return timer.isDone(); }),
      s_Timers.end());
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
