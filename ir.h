#ifndef IR_H
#define IR_H

#include "common.h"
#include <stdint.h>
#include <stdio.h>

/*
 * This is my own implementation / interpretation of the commonly used
 * three-address code (TAC or 3AC) intermediate representation.
 *
 * I really have no idea what I'm doing so this could go one of two
 * ways:
 *   1. I get a somewhat working solution that's reliable enough to be
 *      used in this compiler.
 *   2. I scrap the entire thing and just end up compiling directly from
 *      the AST to JVM bytecode. (The more probable result)
 */

typedef uint8_t temp_index_t;

enum {
  V_INTEGER,
  V_INDEX,
};
typedef struct {
  uint8_t tag;
  union {
    int64_t integer;
    temp_index_t index;
  };
} ir_value_t;

typedef enum {
  OP_NONE,
  OP_ADD,
} ir_op_t;

typedef struct {
  temp_index_t result;
  ir_op_t op;
  option(ir_value_t) arg1;
  option(ir_value_t) arg2;
} ir_inst_t;

void ir_compile_value(ir_value_t);
void ir_compile_inst(ir_inst_t);
const char *ir_get_instruction_from_op(ir_op_t);

static inline void dump_value(ir_value_t value) {
  switch (value.tag) {
  case V_INTEGER:
    printf("%ld", value.integer);
    break;

  case V_INDEX:
    printf("t%d", value.index);
    break;
  }
}

static inline void dump_inst(ir_inst_t inst) {
  printf("t%d = ", inst.result);

  if (inst.arg1 != none) {
    dump_value(*inst.arg1);
  }

  if (inst.arg2 != none) {
    switch (inst.op) {
    case OP_NONE:
      break;

    case OP_ADD:
      printf(" + ");
      break;
    }

    dump_value(*inst.arg2);
  }

  printf("\n");
}

#endif // IR_H
