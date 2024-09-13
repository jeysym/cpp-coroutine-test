#pragma once
#include "timers.hpp"
#include <coroutine>

// Awaiting this structure resumes the coroutine passed (by its handle) into its
// constructor.
struct CoContinue {
  CoContinue(std::coroutine_handle<> handle);
  bool await_ready() noexcept;
  void await_resume() noexcept;
  void await_suspend(std::coroutine_handle<> handle) noexcept;

  std::coroutine_handle<> m_Continue;
};

// Represents async suspendable task with result of TReturn.
template <typename TReturn> 
struct CoTask {

  struct CoPromise {
    CoTask get_return_object() {
      return {.m_Handle = CoTask::handle_type::from_promise(*this)};
    }

    std::suspend_never initial_suspend() { return {}; }

    CoContinue final_suspend() noexcept { return {m_Continue}; }

    void return_value(TReturn &&ret) { m_RetValue = std::move(ret); }

    void unhandled_exception() {}

    TReturn m_RetValue;
    std::coroutine_handle<> m_Continue = nullptr;
  };

  using promise_type = CoPromise;
  using handle_type = std::coroutine_handle<promise_type>;

  bool await_ready() { return m_Handle.done(); }

  TReturn await_resume() { return std::move(m_Handle.promise().m_RetValue); }

  void await_suspend(std::coroutine_handle<> handle) {
    m_Handle.promise().m_Continue = handle;
  }

  handle_type m_Handle = nullptr;
};

template <>
struct CoTask<void> {

  struct CoPromise {
    CoTask get_return_object() {
      return {.m_Handle = CoTask::handle_type::from_promise(*this)};
    }

    std::suspend_never initial_suspend() { return {}; }

    CoContinue final_suspend() noexcept { return {m_Continue}; }

    void return_void() {}

    void unhandled_exception() {}

    std::coroutine_handle<> m_Continue = nullptr;
  };

  using promise_type = CoPromise;
  using handle_type = std::coroutine_handle<promise_type>;

  bool await_ready() { return m_Handle.done(); }

  void await_resume() {}

  void await_suspend(std::coroutine_handle<> handle) {
    m_Handle.promise().m_Continue = handle;
  }

  handle_type m_Handle = nullptr;
};

