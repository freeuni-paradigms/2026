#ifndef PARADIGMS_ROBOTS_ROBOT_H_
#define PARADIGMS_ROBOTS_ROBOT_H_

#include <stdbool.h>

typedef struct {
  int id;
  void* fight_fn;
  void* data;
} Robot;





// აბრუნებს true-ს თუ x უგებს y-ს და false-ს წინააღმდეგ შემთხვევაში.
// გარანტირებული ხართ რომ RobotFightWith(x, y) == !RoboFightWith(y, x)
bool RobotFightWith(const Robot* x, const Robot* y);

#endif // PARADIGMS_ROBOTS_ROBOT_H_
