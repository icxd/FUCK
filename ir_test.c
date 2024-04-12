#include "ir.h"

int main(void) {
  // t0 = 4
  // t1 = 6
  // t2 = t0 + t1

  ir_inst_t t0 = {
      0,
      OP_NONE,
      some((ir_value_t){.tag = V_INTEGER, .integer = 4}),
      none,
  };
  ir_inst_t t1 = {
      1,
      OP_NONE,
      some((ir_value_t){.tag = V_INTEGER, .integer = 5}),
      none,
  };
  ir_inst_t t2 = {
      2,
      OP_ADD,
      some((ir_value_t){.tag = V_INDEX, .index = 0}),
      some((ir_value_t){.tag = V_INDEX, .index = 1}),
  };

  ir_compile_inst(t0);
  ir_compile_inst(t1);
  ir_compile_inst(t2);
}
