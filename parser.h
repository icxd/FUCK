#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "lexer.h"
#include <stdbool.h>

typedef struct node_t node_t;
typedef struct type_t type_t;
typedef slice(node_t *) block_t;

#define NODES                                                                  \
  N(Package, {                                                                 \
    const char *name;                                                          \
    block_t nodes;                                                             \
  })                                                                           \
  N(Fn, {                                                                      \
    const char *name;                                                          \
    block_t body;                                                              \
  })                                                                           \
  N(Let, {                                                                     \
    bool mutable;                                                              \
    const char *name;                                                          \
    type_t *type;                                                              \
    node_t *initializer;                                                       \
  })                                                                           \
                                                                               \
  N(BinOp, {                                                                   \
    token_type_t op;                                                           \
    node_t *lhs, *rhs;                                                         \
  })                                                                           \
  N(Id, { const char *id; })

typedef enum {
#define N(name, ...) _NODE_##name,
  NODES
#undef N
} node_type_t;

struct node_t {
  node_type_t tag;
  union {
#define N(name, ...) struct __VA_ARGS__ NODE_##name;
    NODES
#undef N
  } as;
};

static inline const char *n_name(node_type_t type) {
  const char *names[] = {
#define N(name, ...) #name,
      NODES
#undef N
  };
  return names[type];
}
void n_dump(node_t, uint8_t);

struct type_t {
  int _;
};

typedef struct {
  lexer_t *lexer;
  token_t token;
} parser_t;

void p_init(parser_t *, lexer_t *);
void p_free(parser_t *);

diagnostic_t p_parse(parser_t *, node_t *);

#endif // PARSER_H
