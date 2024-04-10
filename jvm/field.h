#ifndef JVM_FIELD_H
#define JVM_FIELD_H

#include "attribute.h"
#include "internal.h"

enum {
  FACC_PUBLIC = 0x0001,
  FACC_PRIVATE = 0x0002,
  FACC_PROTECTED = 0x0004,
  FACC_STATIC = 0x0008,
  FACC_FINAL = 0x0010,
  FACC_VOLATILE = 0x0040,
  FACC_TRANSIENT = 0x0080,
  FACC_SYNTHETIC = 0x1000,
  FACC_ENUM = 0x4000,
};

typedef struct {
  u2 access_flags;
  u2 name_index, descriptor_index;
  u2 attributes_count;
  attribute_info_t *attributes;
} field_info_t;

#endif // JVM_FIELD_H
