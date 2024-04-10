#ifndef JVM_CLASS_H
#define JVM_CLASS_H

#include "attribute.h"
#include "constant.h"
#include "field.h"
#include "internal.h"
#include "method.h"
#include <stdio.h>

typedef struct {
  slice(cp_info_t) constant_pool;
  slice(u2) interfaces;
  slice(field_info_t) fields;
  slice(method_info_t) methods;
  slice(attribute_info_t) attributes;

  cp_info_t this_class, super_class;
} class_file_t;

void cf_init(class_file_t *);
size_t cf_add_constant(class_file_t *, cp_info_t);

typedef struct {
  u4 magic;
  u2 minor_version, major_version;
  u2 constant_pool_count;
  cp_info_t *constant_pool;
  u2 access_flags;
  u2 this_class, super_class;
  u2 interfaces_count;
  u2 *interfaces;
  u2 fields_count;
  field_info_t *fields;
  u2 methods_count;
  method_info_t *methods;
  u2 attributes_count;
  attribute_info_t *attributes;
} class_writer_t;

void cw_write_class_file(class_writer_t *, class_file_t);
void cw_write_to_file(class_writer_t, FILE *);

#endif // JVM_CLASS_H
