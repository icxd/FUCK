#ifndef JVM_CONSTANT_H
#define JVM_CONSTANT_H

#include "internal.h"

#define CONSTANTS                                                              \
  C(Class, 7, { u2 name_index; })                                              \
  C(Fieldref, 9, { u2 class_index, name_and_type_index; })                     \
  C(Methodref, 10, { u2 class_index, name_and_type_index; })                   \
  C(InterfaceMethodRef, 11, { u2 class_index, name_and_type_index; })          \
  C(String, 8, { u2 string_index; })                                           \
  C(Integer, 3, { u4 bytes; })                                                 \
  C(Float, 4, { u4 bytes; })                                                   \
  C(Long, 5, { u4 high_bytes, low_bytes; })                                    \
  C(Double, 6, { u4 high_bytes, low_bytes; })                                  \
  C(NameAndType, 12, { u2 name_index, descriptor_index; })                     \
  C(Utf8, 1, {                                                                 \
    u2 length;                                                                 \
    u1 *bytes;                                                                 \
  })                                                                           \
  C(MethodHandle, 15, {                                                        \
    u1 reference_kind;                                                         \
    u2 reference_index;                                                        \
  })                                                                           \
  C(MethodType, 16, { u2 descriptor_index; })                                  \
  C(InvokeDynamic, 18, { u2 bootstrap_method_attr_index, name_and_type_index; })

typedef enum {
#define C(name, index, ...) CONSTANT_##name = index,
  CONSTANTS
#undef C
} cp_info_type_t;

typedef struct {
  cp_info_type_t tag;
  union {
#define C(name, index, ...) struct __VA_ARGS__ CONSTANT_##name;
    CONSTANTS
#undef C
  } as;
} cp_info_t;

#endif // JVM_CONSTANT_H
