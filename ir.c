#include "ir.h"
#include "common.h"
#include <stdbool.h>

void ir_init(ir_state_t *state) { slice_init(&state->allocated_buffers); }

diagnostic_t ir_compile_value(ir_state_t *state, ir_value_t value,
                              struct allocated_buffer *allocated_buffer) {
  switch (value.tag) {
  case V_INTEGER:
    printf("%ld", value.integer.value);
    break;
  case V_INDEX: {
    struct allocated_buffer *buffer = NULL;
    if (allocated_buffer != NULL)
      buffer = allocated_buffer;
    else {
      for (size_t i = 0; i < state->allocated_buffers.size; i++) {
        if (state->allocated_buffers.items[i].index == value.index.index) {
          buffer = &state->allocated_buffers.items[i];
        }
      }
    }
    if (buffer == NULL) {
      span_t span = {1, 2};
      return D_ERROR(span, "attempted to use an unallocated buffer");
    }

    printf("dword [rsp - %ld]", buffer->buffer_size * (buffer->index + 1));
  } break;
  }

  return D_OK();
}

diagnostic_t ir_compile_inst(ir_state_t *state, ir_inst_t *inst) {
  struct allocated_buffer *allocated_buffer = NULL;

  switch (inst->tag) {
  case OP_ALLOC: {
    inst->alloc.result_index.buffer_size = inst->alloc.bytes.value;
    slice_push(&state->allocated_buffers,
               (struct allocated_buffer){inst->alloc.result_index.index,
                                         inst->alloc.result_index.buffer_size});
  } break;

  case OP_STORE: {
    struct ir_value_index index = inst->store.destination;

    printf("  mov ");
    ir_compile_value(state, (ir_value_t){.tag = V_INDEX, .index = index},
                     allocated_buffer);
    printf(", ");
    ir_compile_value(state, inst->store.source, allocated_buffer);
    printf("\n");

  } break;

  case OP_ADD: {
    struct ir_value_index index = inst->add.result_index;

    int64_t first_size = ir_get_size_of_value(state, inst->add.source1);
    int64_t second_size = ir_get_size_of_value(state, inst->add.source2);
    int64_t buffer_size = first_size < second_size ? second_size : first_size;
    inst->add.result_index.buffer_size = buffer_size;
    slice_push(&state->allocated_buffers,
               (struct allocated_buffer){inst->add.result_index.index,
                                         inst->add.result_index.buffer_size});

    printf("  mov eax, ");
    ir_compile_value(state, inst->add.source1, allocated_buffer);
    printf("\n");

    printf("  add eax, ");
    ir_compile_value(state, inst->add.source2, allocated_buffer);
    printf("\n");

    printf("  mov ");
    ir_compile_value(state, (ir_value_t){.tag = V_INDEX, .index = index},
                     allocated_buffer);
    printf(", eax\n");
  } break;
  }

  return D_OK();
}

int64_t ir_get_size_of_value(ir_state_t *state, ir_value_t value) {
  switch (value.tag) {
  case V_INTEGER:
    return 4;
  case V_INDEX: {
    struct allocated_buffer *buffer = NULL;
    for (size_t i = 0; i < state->allocated_buffers.size; i++) {
      if (state->allocated_buffers.items[i].index == value.index.index) {
        buffer = &state->allocated_buffers.items[i];
      }
    }
    if (buffer == NULL)
      break;
    return buffer->buffer_size;
  } break;
  }

  return 4;
}
