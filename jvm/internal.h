#ifndef JVM_INTERNAL_H
#define JVM_INTERNAL_H

#include <stddef.h>

#define JVM_CLASS_FILE_MAGIC 0xCAFEBABE
#define JVM_CLASS_FILE_MINOR_VERSION 0
#define JVM_CLASS_FILE_MAJOR_VERSION 63

typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;
typedef unsigned long long u8;

#define INIT_SLICE_CAP 16

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

#endif // JVM_INTERNAL_H
