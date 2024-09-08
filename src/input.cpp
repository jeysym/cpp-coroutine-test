#include "input.hpp"
#include <array>

namespace Input {

std::array<bool, (int)Action::COUNT> g_ActionActive;

bool getActionActive(Action action) { return g_ActionActive[(int)action]; }

void setActionActive(Action action, bool active) {
  g_ActionActive[(int)action] = active;
}

} // namespace Input
