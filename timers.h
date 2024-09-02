#pragma once
#include <functional>
#include <vector>

struct Timers {
  using callback_type = std::function<void()>;

  struct Timer {
    float m_RemainingSeconds = 0.0f;
    callback_type m_Callback = nullptr;

    bool is_done() const;
  };

  static std::vector<Timer> s_Timers;

  static void add(float durationSeconds, callback_type callback);
  static void update(float deltaSeconds);
};
