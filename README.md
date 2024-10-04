# cpp-coroutine-test

This project tests the C++20 coroutines. It's a simple game that uses
coroutines mainly to drive the gameplay logic.
For example this is how it does teleport with fade:

    CoTask<void> coFadeOut(float duration) {
      const float fadeStartTime = g_CurrentSeconds;
      const float fadeEndTime = g_CurrentSeconds + duration;

      while (g_CurrentSeconds <= fadeEndTime) {
        g_State.m_FadeRatio = ((g_CurrentSeconds - fadeStartTime) / duration);
        co_await CoWaitFrame{};
      }

      g_State.m_FadeRatio = 1.0f;
      co_return;
    }

    CoTask<void> coFadedTeleport(float fadeSeconds) {
      co_await coFadeOut(fadeSeconds);
      g_State.m_PlayerPosition = {0.5f, 0.5f};
      co_await coFadeIn(fadeSeconds);
      co_return;
    }

These look like normal functions, but they are coroutines, and can
be suspended/resumed. Thus they can run across multiple frames.

## Build

The project uses Zig as its build system. Build & run it like this:

    $zig build
    $zig build run
