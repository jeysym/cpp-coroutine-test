#include "game.hpp"
#include "co_task.hpp"
#include "timers.hpp"
#include "input.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>

size_t g_CurrentFrameIdx = 0;
float g_CurrentSeconds = 0.0f;
GameState g_State = {};

void log(std::string_view category, std::string_view message) {
  std::cout << "[" << g_CurrentSeconds << "s][" << category << "] " << message
            << std::endl;
}

void log_fade_ratio(std::string_view category) {
  std::cout << "[" << g_CurrentSeconds << "s][" << category
            << "] Current fade ratio: " << g_State.m_FadeRatio << std::endl;
}

CoTask co_fade_out(float duration) {
  const float fadeStartTime = g_CurrentSeconds;
  const float fadeEndTime = g_CurrentSeconds + duration;

  while (g_CurrentSeconds <= fadeEndTime) {
    g_State.m_FadeRatio = ((g_CurrentSeconds - fadeStartTime) / duration);
    co_await CoWaitFrame{};
  }

  g_State.m_FadeRatio = 1.0f;
  co_return;
}

CoTask co_fade_in(float duration) {
  const float fadeEndTime = g_CurrentSeconds + duration;

  while (g_CurrentSeconds <= fadeEndTime) {
    g_State.m_FadeRatio = ((fadeEndTime - g_CurrentSeconds) / duration);
    co_await CoWaitFrame{};
  }

  g_State.m_FadeRatio = 0.0f;
  co_return;
}

CoTask co_faded_teleport(float fadeSeconds) {
  log("FadedTeleport", "Fade out");
  co_await co_fade_out(fadeSeconds);
  log("FadedTeleport", "Teleport");
  g_State.m_PlayerPosition = { 0.5f, 0.5f };
  log("FadedTeleport", "Fade in");
  co_await co_fade_in(fadeSeconds);
  log("FadedTeleport", "Done");
  co_return;
}

void handle_input(float deltaSeconds) {
  using namespace Input;
  Vec2 inputVector;
  
  if (getActionActive(Action::UP)) {
    inputVector.y += 1.0f;
  }

  if (getActionActive(Action::DOWN)) {
    inputVector.y -= 1.0f;
  }

  if (getActionActive(Action::LEFT)) {
    inputVector.x -= 1.0f;
  }

  if (getActionActive(Action::RIGHT)) {
    inputVector.x += 1.0f;
  }        

  inputVector.normalize();
  constexpr float movementSpeed = 0.5f;
  g_State.m_PlayerPosition += ((movementSpeed * deltaSeconds) * inputVector);
}

void Game::update(float deltaSeconds) {
  g_CurrentFrameIdx += 1;
  g_CurrentSeconds += deltaSeconds;

  handle_input(deltaSeconds);
  Timers::update(deltaSeconds);
}

void Game::init() {
  std::cout << std::fixed << std::setprecision(2);
  
  Timers::add(2.0f, []() { log("Timer", "2 seconds"); });
  Timers::add(1.0f, []() { log("Timer", "1 second"); });
  Timers::add(8.0f, []() { log("Timer", "8 seconds"); });
  Timers::add(1.0f, []() { co_faded_teleport(4.0f); });
}
