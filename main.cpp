#include <algorithm>
#include <assert.h>
#include <coroutine>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string_view>
#include "timers.h"

static int currentFrameIdx = 0;
static float currentTime = 0.0f;
constexpr float simStepSeconds = 0.1f;
constexpr float simDurationSeconds = 10.0f;

void log(std::string_view category, std::string_view message) {
  std::cout << "[" << currentTime << "s][" << category << "] " << message << std::endl;
}

struct CoTask {

  struct CoPromise {
    CoTask get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {
    } // TODO: Do I need this if I am not using exceptions?
  };

  using promise_type = CoPromise;
  using handle_type = std::coroutine_handle<promise_type>;
};

struct CoWait {
  float m_DurationSeconds = 0.0f;

  CoWait(float durationSeconds) : m_DurationSeconds(durationSeconds) {}

  bool await_ready() { return false; }
  bool await_resume() { return false; }

  void await_suspend(CoTask::handle_type handle) {
    Timers::add(m_DurationSeconds, [handle]() { handle.resume(); });
  }
};

CoTask co_faded_teleport(float fadeSeconds) {
  log("FadedTeleport", "Fade out");
  co_await CoWait{fadeSeconds};
  log("FadedTeleport", "Teleport");
  log("FadedTeleport", "Fade in");
  co_await CoWait{fadeSeconds};
  log("FadedTeleport", "Done");
  co_return;
}

void update(float deltaSeconds) {
  currentFrameIdx += 1;
  currentTime += deltaSeconds;

  Timers::update(deltaSeconds);
}

int main() {
  Timers::add(2.0f, []() { log("Timer", "2 seconds"); });
  Timers::add(1.0f, []() { log("Timer", "1 second"); });
  Timers::add(8.0f, []() { log("Timer", "8 seconds"); });
  Timers::add(1.0f, []() { co_faded_teleport(4.0f); });

  std::cout << "==== Loop Start ====" << std::endl;
  while (true) {
    update(simStepSeconds);

    if (currentTime >= simDurationSeconds)
      break;
  }
  std::cout << "==== Loop End ====" << std::endl;
}
