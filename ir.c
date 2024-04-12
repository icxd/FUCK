#include "ir.h"

void ir_compile_value(ir_value_t value) {
  switch (value.tag) {
  case V_INTEGER:
    printf("$%ld", value.integer);
    break;

  case V_INDEX:
    printf("%d(%%rsp)", -8 * (value.index + 1));
    break;
  }
}

void ir_compile_inst(ir_inst_t inst) {
  if (inst.op == OP_NONE) {
    printf("  mov ");
    if (inst.arg1 != none)
      ir_compile_value(*inst.arg1);
    printf(", %d(%%rsp)", -8 * (inst.result + 1));
  }

  if (inst.op == OP_ADD) {
    printf("  add ");
    if (inst.arg1 != none)
      ir_compile_value(*inst.arg1);
    printf(", ");
    if (inst.arg2 != none)
      ir_compile_value(*inst.arg2);
    printf("\n");
    printf("  mov %d(%%rsp), ", -8 * (inst.result + 1));
    ir_compile_value(*inst.arg1);
  }

  printf("\n");
}

const char *ir_get_instruction_from_op(ir_op_t op) {
  switch (op) {
  case OP_NONE:
    return "mov";

  case OP_ADD:
    return "add";
  }
}
