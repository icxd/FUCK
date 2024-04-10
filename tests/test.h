#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define VERIFY(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "\033[31;1mASSERTION FAILED!\033[0;0m " #expr "\n");     \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

typedef void (*test_fn_t)();

extern test_fn_t tests[];

#define DEF(id) void test_##id()
#define REGISTER(id) test_##id

static inline void run_tests(test_fn_t *tests) {
  for (int i = 0;; i++) {
    test_fn_t test = tests[i];
    if (!test)
      break;

    test();
  }

  printf("\033[32;1mAll tests passed!\033[0;0m\n");
}

#endif // TEST_H
