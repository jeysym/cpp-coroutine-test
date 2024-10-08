#pragma once
#include "math.hpp"
#include <cstdlib>
#include <vector>

namespace Game {

struct BulletState {
  Vec2 m_Position;
  Vec2 m_Velocity;
};

struct GameState {
  // 0.0 => No fade
  // 1.0 => Full fade
  float m_FadeRatio = 0.0f;

  Vec2 m_PlayerPosition;
  Vec2 m_InputVector;
  std::vector<BulletState> m_Bullets;
};

extern size_t g_CurrentFrameIdx;
extern float g_CurrentSeconds;
extern GameState g_State;

void init();
void update(float deltaSeconds);

} // namespace Game
