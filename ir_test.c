#include "ir.h"

int main(void) {
  /* entry:
   *   @0 = alloc $4
   *   store @0, $69
   *   @1 = alloc $4
   *   store @1, $420
   *
   *   @2 = add @0, @1
   */

  ir_inst_t t0 = {
      .tag = OP_ALLOC,
      .alloc = (struct ir_inst_alloc){.result_index = {0}, .bytes = {4}}};
  ir_inst_t t0_store = {.tag = OP_STORE,
                        .store = {{0}, {.tag = V_INTEGER, .integer = {69}}}};
  ir_inst_t t1 = {
      .tag = OP_ALLOC,
      .alloc = (struct ir_inst_alloc){.result_index = {1}, .bytes = {4}}};
  ir_inst_t t1_store = {.tag = OP_STORE,
                        .store = {{1}, {.tag = V_INTEGER, .integer = {420}}}};
  ir_inst_t t2 = {.tag = OP_ADD,
                  .add = {{2},
                          {.tag = V_INDEX, .index = {0}},
                          {.tag = V_INDEX, .index = {1}}}};

  // dump_inst(t0);
  // dump_inst(t0_store);
  // dump_inst(t1);
  // dump_inst(t1_store);
  // dump_inst(t2);
  // printf("\n");

  ir_state_t state;
  ir_init(&state);

  diagnostic_t diag;

#define TRY(diagnostic, ...)                                                   \
  do {                                                                         \
    (diagnostic) = (__VA_ARGS__);                                              \
    if (diag.type != DT_OK) {                                                  \
      print_diagnostic(diag, "homicide", (char *)"ir_test.c");                 \
      if (diag.type == DT_ERROR)                                               \
        exit(-1);                                                              \
    }                                                                          \
  } while (0)

  printf("section .text\n");
  printf("main:\n");

  TRY(diag, ir_compile_inst(&state, &t0));
  TRY(diag, ir_compile_inst(&state, &t0_store));
  TRY(diag, ir_compile_inst(&state, &t1));
  TRY(diag, ir_compile_inst(&state, &t1_store));
  TRY(diag, ir_compile_inst(&state, &t2));

  printf("  xor eax, eax\n");
  printf("  ret\n");

  printf("global _start\n");
  printf("_start:\n");
  printf("  call main\n");
  printf("  mov rax, 60\n");
  printf("  mov rdi, 0\n");
  printf("  syscall\n");
}
