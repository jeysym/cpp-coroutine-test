#include <coroutine>

namespace Input {

enum class Action { UP, DOWN, LEFT, RIGHT, SHOOT, QUIT, COUNT };

bool getActionActive(Action action);
void setActionActive(Action action, bool active);

struct CoActionTrigger {
  CoActionTrigger(Action action);
  bool await_ready();
  void await_resume();
  void await_suspend(std::coroutine_handle<> handle);

  Action m_Action;
};

} // namespace Input
