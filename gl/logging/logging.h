#ifndef GL_LOGGING_LOGGING_H_
#define GL_LOGGING_LOGGING_H_

#include <stdio.h>
#include <stdlib.h>

#define LOG(severity, ...) \
  printf("%c %s:%d: ", severity, __FILE__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); fflush(stdout);

#define LOG_INFO(...) LOG('I', __VA_ARGS__)
#define LOG_ERROR(...) LOG('E', __VA_ARGS__)
#define LOG_FATAL(...) LOG('F', __VA_ARGS__) \
  fflush(stdout); exit(EXIT_FAILURE);

#define CHECK(cond)							\
  if (!(cond)) {							\
    LOG_FATAL("CHECK FAILED: %s", #cond);				\
  }									\

#endif // GL_LOGGING_LOGGING_H_
