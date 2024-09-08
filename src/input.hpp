namespace Input {

enum class Action { UP, DOWN, LEFT, RIGHT, SHOOT, QUIT, COUNT };

bool getActionActive(Action action);
void setActionActive(Action action, bool active);

} // namespace Input
