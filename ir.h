#ifndef IR_H
#define IR_H

#include "common.h"
#include <stdbool.h>
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

struct allocated_buffer {
  temp_index_t index;
  int64_t buffer_size;
};

typedef slice(struct allocated_buffer) buffers_t;

typedef enum {
  V_INTEGER,
  V_INDEX,
} ir_value_type_t;

struct ir_value_integer {
  int64_t value;
};

struct ir_value_index {
  temp_index_t index;
  int64_t buffer_size;
};

typedef struct {
  ir_value_type_t tag;
  union {
    struct ir_value_integer integer;
    struct ir_value_index index;
  };
} ir_value_t;

typedef enum {
  OP_ALLOC,
  // OP_LOAD,
  OP_STORE,
  OP_ADD,
} ir_op_t;

static inline bool ir_allocates_new_buffer(ir_op_t op) {
  switch (op) {
  case OP_ADD:
    return true;

  default:
    return false;
  }
}

struct ir_inst_alloc {
  struct ir_value_index result_index;
  struct ir_value_integer bytes;
};
// struct ir_inst_load {};
struct ir_inst_store {
  struct ir_value_index destination;
  ir_value_t source;
};
struct ir_inst_add {
  struct ir_value_index result_index;
  ir_value_t source1, source2;
};

typedef struct {
  ir_op_t tag;

  union {
    struct ir_inst_alloc alloc;
    // struct ir_inst_load load;
    struct ir_inst_store store;
    struct ir_inst_add add;
  };
} ir_inst_t;

typedef struct {
  buffers_t allocated_buffers;
} ir_state_t;

void ir_init(ir_state_t *);

diagnostic_t ir_compile_value(ir_state_t *, ir_value_t,
                              struct allocated_buffer *);
diagnostic_t ir_compile_inst(ir_state_t *, ir_inst_t *);
int64_t ir_get_size_of_value(ir_state_t *, ir_value_t);

static inline void dump_value(ir_value_t value) {
  switch (value.tag) {
  case V_INTEGER:
    printf("%ld", value.integer.value);
    break;

  case V_INDEX:
    printf("%%%d", value.index.index);
    break;
  }
}

static inline void dump_inst(ir_inst_t inst) {
  switch (inst.tag) {
  case OP_ALLOC: {
    ir_value_t result = {.tag = V_INDEX, .index = inst.alloc.result_index};
    ir_value_t bytes = {.tag = V_INTEGER, .integer = inst.alloc.bytes};
    dump_value(result);
    printf(" := alloc ");
    dump_value(bytes);
    printf("\n");
  } break;

  case OP_STORE: {
    ir_value_t destination = {.tag = V_INDEX, .index = inst.store.destination};
    printf("store ");
    dump_value(destination);
    printf(", ");
    dump_value(inst.store.source);
    printf("\n");
  } break;

  case OP_ADD: {
    ir_value_t result = {.tag = V_INDEX, .index = inst.add.result_index};
    dump_value(result);
    printf(" := add ");
    dump_value(inst.add.source1);
    printf(", ");
    dump_value(inst.add.source2);
    printf("\n");
  } break;
  }
}

#endif // IR_H
