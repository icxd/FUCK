#include "class.h"
#include "attribute.h"
#include "constant.h"
#include "field.h"
#include "internal.h"
#include "method.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void cf_init(class_file_t *cf) {
  slice_init(&cf->constant_pool);
  slice_init(&cf->interfaces);
}

size_t cf_add_constant(class_file_t *cf, cp_info_t constant) {
  slice_push(&cf->constant_pool, constant);
  return cf->constant_pool.size;
}

void cw_write_class_file(class_writer_t *cw, class_file_t cf) {
  cw->magic = JVM_CLASS_FILE_MAGIC;
  cw->minor_version = JVM_CLASS_FILE_MINOR_VERSION;
  cw->major_version = JVM_CLASS_FILE_MAJOR_VERSION;
  // why the fuck is only this one -1 when all other arrays aren't
  // TODO(satire): figure out why only constant_pool_count is -1
  cw->constant_pool_count = cf.constant_pool.size;
  cw->constant_pool = cf.constant_pool.items;
  cw->access_flags = 0;
  cw->this_class = 0;
  cw->super_class = 0;
  // like here, why isn't this -1, only the constant pool. like what
  cw->interfaces_count = cf.interfaces.size;
  cw->interfaces = cf.interfaces.items;
  cw->fields_count = cf.fields.size;
  cw->fields = cf.fields.items;
  cw->methods_count = cf.methods.size;
  cw->methods = cf.methods.items;
  cw->attributes_count = cf.attributes.size;
  cw->attributes = cf.attributes.items;
}

static void write_u1(u1 value, FILE *fp) { fputc(value & 0xFF, fp); }

static void write_u2(u2 value, FILE *fp) {
  fputc((value >> 8) & 0xFF, fp);
  fputc(value & 0xFF, fp);
}

static void write_u4(u4 value, FILE *fp) {
  fputc((value >> 24) & 0xFF, fp);
  fputc((value >> 16) & 0xFF, fp);
  fputc((value >> 8) & 0xFF, fp);
  fputc(value & 0xFF, fp);
}

static void write_attributes(u2 count, attribute_info_t *attributes, FILE *fp) {
  write_u2(count, fp);
  for (u2 i = 0; i < count; i++) {
    attribute_info_t attribute = attributes[i];
    write_u2(attribute.attribute_name_index, fp);
    write_u4(attribute.attribute_length, fp);
  }
}

void cw_write_to_file(class_writer_t cw, FILE *fp) {
  write_u4(cw.magic, fp);
  write_u2(cw.minor_version, fp);
  write_u2(cw.major_version, fp);
  write_u2(cw.constant_pool_count, fp);
  for (u2 i = 0; i < cw.constant_pool_count; i++) {
    cp_info_t cp_info = cw.constant_pool[i];
    write_u1(cp_info.tag, fp);
    switch (cp_info.tag) {
    case CONSTANT_Class:
      write_u2(cp_info.as.CONSTANT_Class.name_index, fp);
      break;

    case CONSTANT_Methodref:
      write_u2(cp_info.as.CONSTANT_Methodref.class_index, fp);
      write_u2(cp_info.as.CONSTANT_Methodref.name_and_type_index, fp);
      break;

    case CONSTANT_NameAndType:
      write_u2(cp_info.as.CONSTANT_NameAndType.name_index, fp);
      write_u2(cp_info.as.CONSTANT_NameAndType.descriptor_index, fp);
      break;

    case CONSTANT_Utf8:
      write_u2(cp_info.as.CONSTANT_Utf8.length, fp);
      for (u2 j = 0; j < cp_info.as.CONSTANT_Utf8.length * 4; j++) {
        if (j % 4 != 0)
          continue;
        write_u1(cp_info.as.CONSTANT_Utf8.bytes[j] & 0xff, fp);
      }
      break;

    default:
      assert(0 && "unimplemented");
      break;
    }
  }
  write_u2(cw.access_flags, fp);
  write_u2(cw.this_class, fp);
  write_u2(cw.super_class, fp);
  write_u2(cw.interfaces_count, fp);
  for (u2 i = 0; i < cw.interfaces_count; i++)
    write_u2(cw.interfaces[i], fp);
  write_u2(cw.fields_count, fp);
  for (u2 i = 0; i < cw.fields_count; i++) {
    field_info_t field = cw.fields[i];
    write_u2(field.access_flags, fp);
    write_u2(field.name_index, fp);
    write_u2(field.descriptor_index, fp);
    write_attributes(field.attributes_count, field.attributes, fp);
  }
  write_u2(cw.methods_count, fp);
  for (u2 i = 0; i < cw.methods_count; i++) {
    method_info_t method = cw.methods[i];
    write_u2(method.access_flags, fp);
    write_u2(method.name_index, fp);
    write_u2(method.descriptor_index, fp);
    write_attributes(method.attributes_count, method.attributes, fp);
  }

  write_attributes(cw.attributes_count, cw.attributes, fp);
}
