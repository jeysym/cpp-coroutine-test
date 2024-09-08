#pragma once
#include <cstdlib>
#include "math.hpp"

struct GameState {
  // 0.0 => No fade
  // 1.0 => Full fade
  float m_FadeRatio = 0.0f;

  Vec2 m_PlayerPosition;
};

extern size_t g_CurrentFrameIdx;
extern float g_CurrentSeconds;
extern GameState g_State;

struct Game {
  static void init();
  static void update(float deltaSeconds);
};
