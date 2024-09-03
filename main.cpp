#include "co_task.hpp"
#include "timers.hpp"
#include <iostream>
#include <string_view>

static int currentFrameIdx = 0;
static float currentTime = 0.0f;
constexpr float simStepSeconds = 0.1f;
constexpr float simDurationSeconds = 10.0f;

void log(std::string_view category, std::string_view message) {
  std::cout << "[" << currentTime << "s][" << category << "] " << message
            << std::endl;
}

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
