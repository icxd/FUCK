#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SLICE_CAP 16

/// A dynamically sized array-like data structure.
#define slice(T)                                                               \
  struct {                                                                     \
    T *items;                                                                  \
    size_t capacity, size;                                                     \
  }
#define slice_init(s)                                                          \
  do {                                                                         \
    (s)->items = malloc(sizeof(*(s)->items) * INIT_SLICE_CAP);                 \
    (s)->capacity = INIT_SLICE_CAP;                                            \
    (s)->size = 0;                                                             \
  } while (0)
#define slice_push(s, ...)                                                    \
  do {                                                                         \
    if ((s)->size >= (s)->capacity) {                                          \
      (s)->capacity = (s)->capacity == 0 ? INIT_SLICE_CAP : (s)->capacity * 2; \
      (s)->items = realloc((s)->items, sizeof(*(s)->items) * (s)->capacity);   \
    }                                                                          \
    (s)->items[(s)->size++] = (__VA_ARGS__);                                          \
  } while (0)


static inline char *format(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  // Determine the size of the formatted string
  va_list args_copy;
  va_copy(args_copy, args);
  int size = vsnprintf(NULL, 0, fmt, args_copy);
  va_end(args_copy);

  // Allocate memory for the formatted string
  char *buffer = (char *)malloc(size + 1); // +1 for the null terminator
  if (buffer == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  // Format the string
  vsnprintf(buffer, size + 1, fmt, args);
  va_end(args);

  return buffer;
}

typedef struct {
  size_t start, end;
} span_t;

#define MAX_DIAGNOSTIC_MESSAGE_LENGTH 256

typedef enum {
  DT_OK,
  DT_NOTE,
  DT_WARNING,
  DT_ERROR,
} diagnostic_type_t;

typedef struct {
  diagnostic_type_t type;
  span_t span;
  char *message;
} diagnostic_t;

#define D_OK() ((diagnostic_t){DT_OK})
#define D_NOTE(s, ...) ((diagnostic_t){DT_NOTE, (s), (__VA_ARGS__)})
#define D_WARNING(s, ...) ((diagnostic_t){DT_WARNING, (s), (__VA_ARGS__)})
#define D_ERROR(s, ...) ((diagnostic_t){DT_ERROR, (s), (__VA_ARGS__)})

static char *trim_space(char *str) {
  char *end;
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return str;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
  return str;
}

static inline void print_diagnostic(diagnostic_t diagnostic, const char *source,
                                    char *path) {
  if (diagnostic.type == DT_OK)
    return;

  size_t line = 1, column = 1;
  for (size_t i = 0; i < diagnostic.span.start; i++) {
    if (source[i] == '\n') {
      line++;
      column = 0;
    } else
      column++;
  }

  fprintf(stderr, "\033[0;1m%s:%zu:%zu: ", path, line, column);
  switch (diagnostic.type) {
  case DT_OK:
    break;
  case DT_NOTE:
    fprintf(stderr, "\033[36;1mnote: ");
    break;
  case DT_WARNING:
    fprintf(stderr, "\033[33;1mwarning: ");
    break;
  case DT_ERROR:
    fprintf(stderr, "\033[31;1merror: ");
    break;
  }
  fprintf(stderr, "\033[0;0m%s\n", diagnostic.message);

  size_t line_start = 0, line_end = 0;
  for (size_t i = 0; i < strlen(source); i++) {
    if (i < diagnostic.span.start && source[i] == '\n') {
      line_start = i;
    } else if (i > diagnostic.span.start && source[i] == '\n') {
      line_end = i;
      break;
    }
  }

  size_t length = line_end - line_start;
  char line_buf[length + 1];
  strncpy(line_buf, source + line_start, length);
  line_buf[length] = '\0';
  char *buf = (char *)line_buf;
  buf = trim_space(line_buf);
  size_t spaces_trimmed = strlen(line_buf) - strlen(buf);
  fprintf(stderr, "%s\n", buf);

  size_t spacing = diagnostic.span.start - line_start - spaces_trimmed;
  for (size_t i = 0; i < spacing; i++)
    fprintf(stderr, " ");
  fprintf(stderr, "\033[32;1m^");
  for (size_t i = 0; i < diagnostic.span.end - diagnostic.span.start - 1; i++)
    fprintf(stderr, "~");
  fprintf(stderr, "\033[0;0m\n");
}

static inline int read_entire_file(char *path, char *buffer) {
  FILE *fp = fopen(path, "rb");
  if (fp == NULL)
    return -1;

  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  rewind(fp);

  fread(buffer, size, sizeof(char), fp);

  fclose(fp);
  return 0;
}

#endif // COMMON_H
