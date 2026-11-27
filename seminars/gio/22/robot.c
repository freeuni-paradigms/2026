#include "robot.h"

typedef bool(*FightFn)(const Robot* x, const Robot* y);

bool RobotFightWith(const Robot* x, const Robot* y) {
  return ((FightFn)x->fight_fn)(x, y);
}
