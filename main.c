#include "common.h"
#include "lexer.h"
#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_SIZE (64 * 1024)

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path> [OPTS]\n", argv[0]);
    return -1;
  }

  char *path = argv[1];

  char buffer[MAX_BUF_SIZE];
  int ret = read_entire_file(path, buffer);
  assert(ret >= 0);

  diagnostic_t diag;

  lexer_t *l = malloc(sizeof(lexer_t));
  l_init(l, buffer);

  lexer_t *l_copy = malloc(sizeof(lexer_t));
  l_init(l_copy, buffer);

  token_t token;
  for (;;) {
    diag = l_next(l_copy, &token);
    if (diag.type != DT_OK) {
      print_diagnostic(diag, buffer, path);
      if (diag.type == DT_ERROR)
        return -1;
    }

    printf("%s ", tt_name(token.type));
    if (token.type == T_ID)
      printf("(%s)", token.id);
    printf("\n");

    if (token.type == T_EOF)
      break;
  }

  l_free(l_copy);

  parser_t *parser = malloc(sizeof(parser_t));
  p_init(parser, l);

  node_t *node = malloc(sizeof(node_t));
  diag = p_parse(parser, node);
  if (diag.type != DT_OK) {
    print_diagnostic(diag, buffer, path);
    if (diag.type == DT_ERROR)
      return -1;
  }

  n_dump(*node, 0);

  p_free(parser);
  // l_free(l);

  return 0;
}
