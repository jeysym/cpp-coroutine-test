#include "timers.hpp"
#include <algorithm>

std::vector<Timers::Timer> Timers::s_Timers{};

bool Timers::Timer::is_done() const {
  return m_RemainingSeconds <= 0.0f; 
}

void Timers::add(float durationSeconds, callback_type callback) {
  s_Timers.push_back(
      Timer{.m_RemainingSeconds = durationSeconds, .m_Callback = callback});
}

void Timers::update(float deltaSeconds) {
  for (auto &timer : s_Timers) {
    timer.m_RemainingSeconds -= deltaSeconds;

    if (timer.is_done()) {
      timer.m_Callback();
    }
  }

  s_Timers.erase(
      std::remove_if(s_Timers.begin(), s_Timers.end(),
                     [](const Timer &timer) { return timer.is_done(); }),
      s_Timers.end());
}
