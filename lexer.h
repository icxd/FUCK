#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_ID_LENGTH 128

enum {
  PREC_NONE = 0,
  PREC_ASSIGNMENT,     // =, +=...
  PREC_EQUALITY,       // ==, !=
  PREC_COMPARISON,     // <, <=, >, >=
  PREC_ACCESS,         // ., ::
  PREC_MULTIPLICATIVE, // *, /, %
  PREC_ADDITION,       // +, -
};

/// An X-macro defining all tokens for the tokenizer
#define TOKENS                                                                 \
  T(T_ID, "id", PREC_NONE)                                                     \
  T(T_INT, "int", PREC_NONE)                                                   \
  T(T_FLOAT, "float", PREC_NONE)                                               \
  T(T_BOOL, "bool", PREC_NONE)                                                 \
                                                                               \
  T(T_KW_PKG, "pkg", PREC_NONE)                                                \
  T(T_KW_PUB, "pub", PREC_NONE)                                                \
  T(T_KW_PRIV, "priv", PREC_NONE)                                              \
  T(T_KW_PROT, "prot", PREC_NONE)                                              \
  T(T_KW_CLASS, "class", PREC_NONE)                                            \
  T(T_KW_FN, "fn", PREC_NONE)                                                  \
  T(T_KW_LET, "let", PREC_NONE)                                                \
  T(T_KW_MUT, "mut", PREC_NONE)                                                \
  T(T_KW_CONST, "const", PREC_NONE)                                            \
                                                                               \
  T(T_OPEN_PAREN, "(", PREC_NONE)                                              \
  T(T_CLOSE_PAREN, ")", PREC_NONE)                                             \
  T(T_OPEN_BRACE, "{", PREC_NONE)                                              \
  T(T_CLOSE_BRACE, "}", PREC_NONE)                                             \
  T(T_OPEN_BRACKET, "[", PREC_NONE)                                            \
  T(T_CLOSE_BRACKET, "]", PREC_NONE)                                           \
  T(T_COMMA, ",", PREC_NONE)                                                   \
  T(T_PERIOD, ".", PREC_ACCESS)                                                \
  T(T_COLON, ":", PREC_NONE)                                                   \
  T(T_COLON_COLON, "::", PREC_ACCESS)                                          \
                                                                               \
  T(T_PLUS, "+", PREC_ADDITION)                                                \
  T(T_PLUS_PLUS, "++", PREC_NONE)                                              \
  T(T_PLUS_EQUALS, "+=", PREC_ASSIGNMENT)                                      \
  T(T_MINUS, "-", PREC_ADDITION)                                               \
  T(T_MINUS_MINUS, "--", PREC_NONE)                                            \
  T(T_MINUS_EQUALS, "-=", PREC_ASSIGNMENT)                                     \
  T(T_ASTERISK, "*", PREC_MULTIPLICATIVE)                                      \
  T(T_ASTERISK_EQUALS, "*=", PREC_ASSIGNMENT)                                  \
  T(T_SLASH, "/", PREC_MULTIPLICATIVE)                                         \
  T(T_SLASH_EQUALS, "/=", PREC_ASSIGNMENT)                                     \
  T(T_PERCENT, "%", PREC_MULTIPLICATIVE)                                       \
  T(T_PERCENT_EQUALS, "%=", PREC_ASSIGNMENT)                                   \
                                                                               \
  T(T_EQUALS, "=", PREC_ASSIGNMENT)                                            \
  T(T_EQUALS_EQUALS, "==", PREC_EQUALITY)                                      \
  T(T_BANG, "!", PREC_NONE)                                                    \
  T(T_BANG_EQUALS, "!=", PREC_EQUALITY)                                        \
  T(T_LESS_THAN, "<", PREC_COMPARISON)                                         \
  T(T_LESS_EQUALS, "<=", PREC_COMPARISON)                                      \
  T(T_LESS_COLON, "<:", PREC_NONE)                                             \
  T(T_GREATER_THAN, ">", PREC_COMPARISON)                                      \
  T(T_GREATER_EQUALS, ">=", PREC_COMPARISON)                                   \
  T(T_GREATER_COLON, ">:", PREC_NONE)                                          \
                                                                               \
  T(T_EOL, "end of line", PREC_NONE)                                           \
  T(T_EOF, "end of file", PREC_NONE)

typedef enum {
#define T(x, ...) x,
  TOKENS
#undef T
} token_type_t;

static inline const char *tt_name(token_type_t type) {
  const char *names[] = {
#define T(x, name, ...) name,
      TOKENS
#undef T
  };
  return names[type];
}

static inline uint8_t tt_precedence(token_type_t type) {
  uint8_t precedences[] = {
#define T(x, name, prec) prec,
      TOKENS
#undef T
  };
  return precedences[type];
}

typedef struct {
  token_type_t type; /*!< The type of the token */
  span_t span;       /*!< The start and end location in the source buffer */

  union {
    char id[MAX_ID_LENGTH];
    char *string;
    int integer;
    float floating;
    bool boolean;
  };
} token_t;

typedef struct {
  const char *source; /*!< The source buffer */
  size_t pos;         /*!< The current position in the source buffer */
  size_t length;      /*!< The length of the source buffer */
  bool continuation;  /*!< Should put T_EOL token if set */
} lexer_t;

/// Initialize a `lexer_t` structure
///
/// \param lexer A pointer to the lexer state
/// \param source The source buffer
void l_init(lexer_t *lexer, const char *source);

/// Get the next token_t
///
/// \param lexer A pointer to the lexer state
/// \param token A pointer to the token to write into
/// \return A diagnostic
diagnostic_t l_next(lexer_t *lexer, token_t *token);

#endif // LEXER_H
