#include "input.hpp"
#include <array>
#include <coroutine>
#include <vector>

namespace Input {

std::array<bool, (int)Action::COUNT> g_ActionActive;
using Handles_t = std::vector<std::coroutine_handle<>>;
std::array<Handles_t, (int)Action::COUNT> g_ActionTriggers;

bool getActionActive(Action action) { return g_ActionActive[(int)action]; }

void setActionActive(Action action, bool active) {
  const bool prevActive = g_ActionActive[(int)action];
  g_ActionActive[(int)action] = active;

  if (active && !prevActive) {
    std::vector<std::coroutine_handle<>> handlesToRun =
        std::move(g_ActionTriggers[(int)action]);
    g_ActionTriggers[(int)action] = std::vector<std::coroutine_handle<>>();

    // The resume might modify the g_ActionTriggers so it needs to be cleared
    // before calling it.
    for (auto &handle : handlesToRun) {
      handle.resume();
    }
  }
}

CoActionTrigger::CoActionTrigger(Action action) : m_Action(action) {}

bool CoActionTrigger::await_ready() { return false; }

void CoActionTrigger::await_resume() {}

void CoActionTrigger::await_suspend(std::coroutine_handle<> handle) {
  g_ActionTriggers[(int)m_Action].push_back(handle);
}

} // namespace Input
