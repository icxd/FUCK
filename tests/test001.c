#include "test.h"

DEF(test) { VERIFY(1 == 1); }

test_fn_t tests[] = {
    REGISTER(test),
};
