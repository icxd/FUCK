#include "lexer.h"
#include "common.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void l_init(lexer_t *lexer, const char *source) {
  lexer->source = source;
  lexer->pos = 0;
  lexer->length = strlen(source);
  lexer->continuation = false;
}

void l_free(lexer_t *lexer) {
  // free((void *)lexer->source);
  free(lexer);
}

diagnostic_t l_next(lexer_t *lexer, token_t *token) {
  token->span = (span_t){lexer->pos, lexer->pos + 1};
  char ch = lexer->source[lexer->pos];

  if (lexer->pos >= lexer->length) {
    token->type = T_EOF;
    return D_OK();
  }

  if (isspace(ch)) {
    lexer->pos++;
    return l_next(lexer, token);
  }

  if (isalpha(ch)) {
    token->type = T_ID;
    size_t start = lexer->pos;
    while (lexer->pos < lexer->length && isalnum(lexer->source[lexer->pos]))
      lexer->pos++;
    token->span.end = lexer->pos;
    size_t length = lexer->pos - start;
    if (length >= MAX_ID_LENGTH)
      return D_ERROR(token->span,
                     format("identifiers cannot be longer than %d character",
                            MAX_ID_LENGTH));
    strncpy(token->id, lexer->source + start, length);
    token->id[length] = '\0';
    if (strncmp(token->id, "pkg", length) == 0)
      token->type = T_KW_PKG;
    if (strncmp(token->id, "pub", length) == 0)
      token->type = T_KW_PUB;
    if (strncmp(token->id, "priv", length) == 0)
      token->type = T_KW_PRIV;
    if (strncmp(token->id, "prot", length) == 0)
      token->type = T_KW_PROT;
    if (strncmp(token->id, "class", length) == 0)
      token->type = T_KW_CLASS;
    if (strncmp(token->id, "fn", length) == 0)
      token->type = T_KW_FN;
    if (strncmp(token->id, "let", length) == 0)
      token->type = T_KW_LET;
    if (strncmp(token->id, "mut", length) == 0)
      token->type = T_KW_MUT;
    if (strncmp(token->id, "const", length) == 0)
      token->type = T_KW_CONST;
    return D_OK();
  }

  if (isdigit(ch)) {
    token->type = T_INT;
    size_t start = lexer->pos;
    while (lexer->pos < lexer->length && isdigit(lexer->source[lexer->pos]))
      lexer->pos++;
    if (lexer->source[lexer->pos++] == '.') {
      token->type = T_FLOAT;
      while (lexer->pos < lexer->length && isdigit(lexer->source[lexer->pos]))
        lexer->pos++;
    }
    token->span.end = lexer->pos;
    size_t length = lexer->pos - start;
    char *buffer = malloc(length + 1);
    strncpy(buffer, lexer->source + start, length);
    buffer[length] = '\0';
    if (token->type == T_INT)
      token->integer = atoi(buffer);
    else
      token->floating = atof(buffer);
    free(buffer);
    return D_OK();
  }

  switch (ch) {
  case '\n':
    if (!lexer->continuation)
      token->type = T_EOL;
    break;

  case '(':
    token->type = T_OPEN_PAREN;
    lexer->pos++;
    return D_OK();
  case ')':
    token->type = T_CLOSE_PAREN;
    lexer->pos++;
    return D_OK();
  case '{':
    token->type = T_OPEN_BRACE;
    lexer->pos++;
    return D_OK();
  case '}':
    token->type = T_CLOSE_BRACE;
    lexer->pos++;
    return D_OK();
  case '[':
    token->type = T_OPEN_BRACKET;
    lexer->pos++;
    return D_OK();
  case ']':
    token->type = T_CLOSE_BRACKET;
    lexer->pos++;
    return D_OK();

  case ',':
    token->type = T_COMMA;
    lexer->pos++;
    return D_OK();

  case ':':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == ':') {
      token->type = T_COLON_COLON;
      lexer->pos += 2;
    } else {
      token->type = T_COLON;
      lexer->pos++;
    }
    break;

  case '+':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '+') {
      token->type = T_PLUS_PLUS;
      lexer->pos += 2;
    } else if (lexer->pos + 1 < lexer->length &&
               lexer->source[lexer->pos + 1] == '=') {
      token->type = T_PLUS_EQUALS;
      lexer->pos += 2;
    } else {
      token->type = T_PLUS;
      lexer->pos++;
    }
    break;

  case '-':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '-') {
      token->type = T_MINUS_MINUS;
      lexer->pos += 2;
    } else if (lexer->pos + 1 < lexer->length &&
               lexer->source[lexer->pos + 1] == '=') {
      token->type = T_MINUS_EQUALS;
      lexer->pos += 2;
    } else {
      token->type = T_MINUS;
      lexer->pos++;
    }
    break;

  case '*':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '=') {
      token->type = T_ASTERISK_EQUALS;
      lexer->pos += 2;
    } else {
      token->type = T_ASTERISK;
      lexer->pos++;
    }
    break;

  case '/':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '/') {
      lexer->pos += 2;
      while (lexer->pos < lexer->length && lexer->source[lexer->pos] != '\n')
        lexer->pos++;
      lexer->pos++;
      return l_next(lexer, token);
    } else if (lexer->pos + 1 < lexer->length &&
               lexer->source[lexer->pos + 1] == '=') {
      token->type = T_SLASH_EQUALS;
      lexer->pos += 2;
    } else {
      token->type = T_SLASH;
      lexer->pos++;
    }
    break;

  case '%':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '=') {
      token->type = T_PERCENT_EQUALS;
      lexer->pos += 2;
    } else {
      token->type = T_PERCENT;
      lexer->pos++;
    }
    break;

  case '=':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '=') {
      token->type = T_EQUALS_EQUALS;
      lexer->pos += 2;
    } else {
      token->type = T_EQUALS;
      lexer->pos++;
    }
    break;

  case '<':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '=') {
      token->type = T_LESS_EQUALS;
      lexer->pos += 2;
    } else if (lexer->pos + 1 < lexer->length &&
               lexer->source[lexer->pos + 1] == ':') {
      token->type = T_LESS_COLON;
      lexer->pos += 2;
    } else {
      token->type = T_LESS_THAN;
      lexer->pos++;
    }
    break;

  case '>':
    if (lexer->pos + 1 < lexer->length &&
        lexer->source[lexer->pos + 1] == '=') {
      token->type = T_GREATER_EQUALS;
      lexer->pos += 2;
    } else if (lexer->pos + 1 < lexer->length &&
               lexer->source[lexer->pos + 1] == ':') {
      token->type = T_GREATER_COLON;
      lexer->pos += 2;
    } else {
      token->type = T_GREATER_THAN;
      lexer->pos++;
    }
    break;

  default:
    return D_ERROR(token->span, format("illegal character `%c`", ch));
  }

  return D_OK();
}
