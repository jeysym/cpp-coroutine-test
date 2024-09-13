#pragma once
#include <functional>
#include <vector>
#include <coroutine>

struct Timers {
  using callback_type = std::function<void()>;

  struct Timer {
    bool isDone() const;

    float m_RemainingSeconds = 0.0f;
    callback_type m_Callback = nullptr;
  };

  static std::vector<Timer> s_Timers;

  static void add(float durationSeconds, callback_type callback);
  static void update(float deltaSeconds);
};

struct CoWait {
  CoWait(float durationSeconds);
  bool await_ready();
  void await_resume();
  void await_suspend(std::coroutine_handle<> handle);

  float m_DurationSeconds = 0.0f;
};

struct CoWaitFrame {
  bool await_ready();
  void await_resume();
  void await_suspend(std::coroutine_handle<> handle);
};
