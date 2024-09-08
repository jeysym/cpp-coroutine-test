#pragma once
#include <coroutine>

struct CoTask {
  struct CoPromise {
    CoTask get_return_object();
    std::suspend_never initial_suspend();
    std::suspend_always final_suspend() noexcept;
    void return_void();
    void unhandled_exception(); 

    std::coroutine_handle<> m_Continue = nullptr;
  };

  using promise_type = CoPromise;
  using handle_type = std::coroutine_handle<promise_type>;

  bool await_ready();
  void await_resume();
  void await_suspend(handle_type handle);

  handle_type m_Handle = nullptr;
};

struct CoWait {
  float m_DurationSeconds = 0.0f;

  CoWait(float durationSeconds);
  bool await_ready();
  void await_resume();
  void await_suspend(CoTask::handle_type handle);
};

struct CoWaitFrame {
  bool await_ready();
  void await_resume();
  void await_suspend(CoTask::handle_type handle);
};
