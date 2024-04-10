#include "class.h"
#include "constant.h"
#include "internal.h"
#include <stdlib.h>

int main(void) {
  class_file_t cf;
  cf_init(&cf);

  size_t super_utf8_index = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_Utf8,
                       .as.CONSTANT_Utf8 = {16, (u1 *)L"java/lang/Object"}});
  size_t super_class_index = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_Class,
                       .as.CONSTANT_Class = {super_utf8_index}});

  size_t this_utf8_index =
      cf_add_constant(&cf, (cp_info_t){.tag = CONSTANT_Utf8,
                                       .as.CONSTANT_Utf8 = {4, (u1 *)L"test"}});
  size_t this_class_index =
      cf_add_constant(&cf, (cp_info_t){.tag = CONSTANT_Class,
                                       .as.CONSTANT_Class = {this_utf8_index}});

  size_t name = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_Utf8,
                       .as.CONSTANT_Utf8 = {6, (u1 *)L"<init>"}});
  size_t type =
      cf_add_constant(&cf, (cp_info_t){.tag = CONSTANT_Utf8,
                                       .as.CONSTANT_Utf8 = {3, (u1 *)L"()V"}});
  size_t name_and_type = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_NameAndType,
                       .as.CONSTANT_NameAndType = {name, type}});
  size_t object_constructor = cf_add_constant(
      &cf,
      (cp_info_t){.tag = CONSTANT_Methodref,
                  .as.CONSTANT_Methodref = {super_class_index, name_and_type}});

  size_t Code =
      cf_add_constant(&cf, (cp_info_t){.tag = CONSTANT_Utf8,
                                       .as.CONSTANT_Utf8 = {4, (u1 *)L"Code"}});
  size_t LineNumberTable = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_Utf8,
                       .as.CONSTANT_Utf8 = {15, (u1 *)L"LineNumberTable"}});
  size_t SourceFile = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_Utf8,
                       .as.CONSTANT_Utf8 = {10, (u1 *)L"SourceFile"}});
  size_t source_file = cf_add_constant(
      &cf, (cp_info_t){.tag = CONSTANT_Utf8,
                       .as.CONSTANT_Utf8 = {9, (u1 *)L"test.java"}});

  slice_push(&cf.attributes, (attribute_info_t){
                                 .attribute_name_index = Code,
                                 .attribute_length = 1,
    .as.Code = {
      .
    }
                             });

  class_writer_t cw;
  cw_write_class_file(&cw, cf);

  cw.super_class = super_class_index;
  cw.this_class = this_class_index;

  FILE *fp = fopen("test.class", "wb");
  if (fp == NULL) {
    perror("fopen() failed");
    exit(1);
  }
  cw_write_to_file(cw, fp);
  fclose(fp);
}
