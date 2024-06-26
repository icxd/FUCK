#include "parser.h"
#include "arena.h"
#include "common.h"
#include <stdint.h>
#include <stdio.h>

static void n_dump_block(block_t block, uint8_t indent) {
  for (size_t i = 0; i < block.size; i++) {
    node_t *node = block.items[i];
    if (node == NULL)
      continue;
    n_dump(*node, indent + 1);
  }
}

void n_dump(node_t node, uint8_t indent) {
  for (uint8_t i = 0; i < indent; i++)
    printf("  ");

  // printf("%d ", node.tag);
  printf("%s", n_name(node.tag));

  switch (node.tag) {
  case _NODE_Package:
    printf(" ('%s')\n", node.as.NODE_Package.name);
    n_dump_block(node.as.NODE_Package.nodes, indent + 1);
    break;

  case _NODE_Fn:
    printf(" ('%s')\n", node.as.NODE_Fn.name);
    n_dump_block(node.as.NODE_Fn.body, indent + 1);
    break;

  case _NODE_Let:
    printf(" ('%s')\n", node.as.NODE_Let.name);
    n_dump(*node.as.NODE_Let.initializer, indent + 1);
    break;

  case _NODE_BinOp:
    printf(" (%s)\n", tt_name(node.as.NODE_BinOp.op));
    n_dump(*node.as.NODE_BinOp.lhs, indent + 1);
    n_dump(*node.as.NODE_BinOp.rhs, indent + 1);
    break;

  case _NODE_Id:
    printf(" ('%s')\n", node.as.NODE_Id.id);
    break;
  }
}

#define P_ADVANCE(diagnostic, parser)                                          \
  (diagnostic) = l_next((parser)->lexer, &(parser)->token)
#define P_EXPECT(diagnostic, parser, expected)                                 \
  do {                                                                         \
    if ((parser)->token.type != (expected))                                    \
      return D_ERROR((parser)->token.span,                                     \
                     format("expected `%s`, but got `%s` instead",             \
                            tt_name(expected),                                 \
                            tt_name((parser)->token.type)));                   \
    P_ADVANCE(diagnostic, parser);                                             \
  } while (0)
#define P_CALL(diagnostic, parser, fn, ...)                                    \
  do {                                                                         \
    (diagnostic) = (fn)((parser), __VA_ARGS__);                                \
    if ((diagnostic).type == DT_ERROR)                                         \
      return (diagnostic);                                                     \
  } while (0)

void p_init(parser_t *p, lexer_t *lexer) {
  p->lexer = lexer;
  (void)l_next(p->lexer, &p->token);
}

static diagnostic_t p_parse_declaration(parser_t *, node_t *);
static diagnostic_t p_parse_package(parser_t *, node_t *);
static diagnostic_t p_parse_fn(parser_t *, node_t *);

static diagnostic_t p_parse_statement(parser_t *, node_t *);
static diagnostic_t p_parse_let(parser_t *, node_t *, token_type_t);

static diagnostic_t p_parse_expression(parser_t *, node_t *);
static diagnostic_t p_parse_precedence(parser_t *, node_t *, uint8_t);
static diagnostic_t p_parse_primary(parser_t *, node_t *);

static diagnostic_t p_parse_block(parser_t *, block_t *,
                                  diagnostic_t (*fn)(parser_t *, node_t *));

diagnostic_t p_parse(parser_t *p, node_t *node) {
  return p_parse_declaration(p, node);
}

static diagnostic_t p_parse_declaration(parser_t *p, node_t *node) {
  switch (p->token.type) {
  case T_KW_PKG:
    return p_parse_package(p, node);
  case T_KW_FN:
    return p_parse_fn(p, node);

  default:
    return D_ERROR(p->token.span,
                   format("expected declaration, but got `%s` instead",
                          tt_name(p->token.type)));
  }
  return D_OK();
}

static diagnostic_t p_parse_package(parser_t *p, node_t *node) {
  node->tag = _NODE_Package;
  diagnostic_t diag;
  P_ADVANCE(diag, p);

  node->as.NODE_Package.name = p->token.id;
  P_EXPECT(diag, p, T_ID);

  slice_init(&node->as.NODE_Package.nodes);
  P_CALL(diag, p, p_parse_block, &node->as.NODE_Package.nodes,
         p_parse_declaration);

  return D_OK();
}

static diagnostic_t p_parse_fn(parser_t *p, node_t *node) {
  node->tag = _NODE_Fn;
  diagnostic_t diag;
  P_ADVANCE(diag, p);

  node->as.NODE_Fn.name = p->token.id;
  P_EXPECT(diag, p, T_ID);

  P_EXPECT(diag, p, T_OPEN_PAREN);
  P_EXPECT(diag, p, T_CLOSE_PAREN);

  slice_init(&node->as.NODE_Fn.body);
  P_CALL(diag, p, p_parse_block, &node->as.NODE_Fn.body, p_parse_statement);

  return D_OK();
}

static diagnostic_t p_parse_statement(parser_t *p, node_t *node) {
  switch (p->token.type) {
  case T_KW_LET:
  case T_KW_MUT:
    return p_parse_let(p, node, p->token.type);

  default:
    return D_ERROR(p->token.span,
                   format("expected statement, but got `%s` instead",
                          tt_name(p->token.type)));
  }
  return D_OK();
}

static diagnostic_t p_parse_let(parser_t *p, node_t *node, token_type_t type) {
  node->tag = _NODE_Let;
  node->as.NODE_Let.mutable = type == T_KW_MUT;
  diagnostic_t diag;
  P_ADVANCE(diag, p);

  node->as.NODE_Let.name = p->token.id;
  P_EXPECT(diag, p, T_ID);

  P_EXPECT(diag, p, T_EQUALS);

  node_t *initializer = (node_t *)a_alloc(sizeof(node_t));
  P_CALL(diag, p, p_parse_expression, initializer);
  node->as.NODE_Let.initializer = initializer;

  return D_OK();
}

static diagnostic_t p_parse_expression(parser_t *p, node_t *node) {
  diagnostic_t diag;
  node_t *lhs = (node_t *)a_alloc(sizeof(node_t));
  P_CALL(diag, p, p_parse_primary, lhs);
  node = lhs;
  P_CALL(diag, p, p_parse_precedence, lhs, 0);
  node = lhs;
  return diag;
}

static diagnostic_t p_parse_precedence(parser_t *p, node_t *lhs,
                                       uint8_t min_precedence) {
  diagnostic_t diag;
  token_t lookahead = p->token;

  while (tt_precedence(lookahead.type) != PREC_NONE &&
         tt_precedence(lookahead.type) >= min_precedence) {
    token_type_t op = lookahead.type;
    P_ADVANCE(diag, p);
    node_t *rhs = (node_t *)a_alloc(sizeof(node_t));
    P_CALL(diag, p, p_parse_primary, rhs);
    lookahead = p->token;

    while (tt_precedence(lookahead.type) != PREC_NONE &&
           tt_precedence(lookahead.type) > tt_precedence(op)) {
      P_CALL(diag, p, p_parse_precedence, rhs,
             tt_precedence(op) +
                 (tt_precedence(lookahead.type) > tt_precedence(op) ? 1 : 0));
      lookahead = p->token;
    }

    node_t *tmp = (node_t *)a_alloc(sizeof(node_t));
    tmp->tag = _NODE_BinOp;
    tmp->as.NODE_BinOp.op = op;
    tmp->as.NODE_BinOp.lhs = lhs;
    tmp->as.NODE_BinOp.rhs = rhs;
    lhs = tmp;
  }

  return D_OK();
}

static diagnostic_t p_parse_primary(parser_t *p, node_t *node) {
  diagnostic_t diag = D_OK();

  switch (p->token.type) {
  case T_ID:
    node->tag = _NODE_Id;
    node->as.NODE_Id.id = p->token.id;
    P_EXPECT(diag, p, T_ID);
    return D_OK();

  default:
    return D_ERROR(p->token.span,
                   format("expected expression, but got `%s` instead",
                          tt_name(p->token.type)));
  }
  return diag;
}

static diagnostic_t p_parse_block(parser_t *p, block_t *block,
                                  diagnostic_t (*fn)(parser_t *, node_t *)) {
  diagnostic_t diag;

  P_EXPECT(diag, p, T_OPEN_BRACE);
  for (;;) {
    if (p->token.type == T_CLOSE_BRACE)
      break;

    node_t *node = (node_t *)a_alloc(sizeof(node_t));
    P_CALL(diag, p, fn, node);
    slice_push(block, node);
  }
  P_EXPECT(diag, p, T_CLOSE_BRACE);

  return D_OK();
}
