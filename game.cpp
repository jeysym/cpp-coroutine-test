#include "co_task.hpp"
#include "timers.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>

static int currentFrameIdx = 0;
static float currentTime = 0.0f;
static float currentFadeRatio = 0.0f;
constexpr float simStepSeconds = 0.1f;
constexpr float simDurationSeconds = 10.0f;

void log(std::string_view category, std::string_view message) {
  std::cout << "[" << currentTime << "s][" << category << "] " << message
            << std::endl;
}

void log_fade_ratio(std::string_view category) {
  std::cout << "[" << currentTime << "s][" << category
            << "] Current fade ratio: " << currentFadeRatio << std::endl;
}

CoTask co_fade_out(float duration) {
  const float fadeStartTime = currentTime;
  const float fadeEndTime = currentTime + duration;

  while (currentTime <= fadeEndTime) {
    currentFadeRatio = (currentTime - fadeStartTime) / duration;
    log_fade_ratio("Fade Out");
    co_await CoWaitFrame{};
  }

  currentFadeRatio = 1.0f;
  log_fade_ratio("Fade Out");

  co_return;
}

CoTask co_fade_in(float duration) {
  const float fadeEndTime = currentTime + duration;

  while (currentTime <= fadeEndTime) {
    currentFadeRatio = (fadeEndTime - currentTime) / duration;
    log_fade_ratio("Fade In");
    co_await CoWaitFrame{};
  }

  currentFadeRatio = 0.0f;
  log_fade_ratio("Fade In");

  co_return;
}

CoTask co_faded_teleport(float fadeSeconds) {
  log("FadedTeleport", "Fade out");
  co_await co_fade_out(fadeSeconds);
  log("FadedTeleport", "Teleport");
  log("FadedTeleport", "Fade in");
  co_await co_fade_in(fadeSeconds);
  log("FadedTeleport", "Done");
  co_return;
}

void update(float deltaSeconds) {
  currentFrameIdx += 1;
  currentTime += deltaSeconds;

  Timers::update(deltaSeconds);
}

int old_main() {
  std::cout << std::fixed << std::setprecision(2);
  
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
  return 0;
}
