#ifndef JVM_ATTRIBUTE_T
#define JVM_ATTRIBUTE_T

#include "internal.h"

typedef struct attribute_info_t attribute_info_t;
typedef struct annotation_t annotation_t;

typedef struct element_value {
  u1 tag;
  union {
    u2 const_value_index;

    struct {
      u2 type_name_index, const_name_index;
    } enum_const_value;

    u2 class_info_index;

    annotation_t *annotation_value;

    struct {
      u2 num_values;
      struct element_value *values;
    } array_value;
  } as;
} element_value_t;

struct annotation_t {
  u2 type_index;
  u2 num_element_value_pairs;
  struct {
    u2 element_name_index;
    element_value_t value;
  } * element_value_pairs;
};

struct exception_table_entry {
  u2 start_pc, end_pc, handler_pc;
  u2 catch_type;
};

struct stack_map_table_entry {
  // TODO: add all the stack frame types
  int _;
};

struct inner_class {
  u2 inner_class_info_index, outer_class_info_index;
  u2 inner_name_index, inner_class_access_flags;
};

struct line_number_table_entry {
  u2 start_pc, line_number;
};

struct local_variable_table_entry {
  u2 start_pc, length;
  u2 name_index, descriptor_index, index;
};

struct local_variable_type_table_entry {
  u2 start_pc, length;
  u2 name_index, signature_index, index;
};

struct bootstrap_method {
  u2 bootstrap_method_ref;
  u2 num_bootstrap_arguments;
  u2 *bootstrap_arguments;
};

#define ATTRIBUTES                                                             \
  A(ConstantValue, { u2 constantvalue_index; })                                \
  A(Code, {                                                                    \
    u2 max_stack, max_locals;                                                  \
    u4 code_length;                                                            \
    u1 *code;                                                                  \
    u2 exception_table_length;                                                 \
    struct exception_table_entry *exception_table;                             \
    u2 attributes_count;                                                       \
    attribute_info_t *attributes;                                              \
  })                                                                           \
  A(StackMapTable, {                                                           \
    u2 number_of_entries;                                                      \
    struct stack_map_table_entry *entries;                                     \
  })                                                                           \
  A(Exceptions, { u2 number_of_exceptions, *exception_index_table; })          \
  A(InnerClasses, {                                                            \
    u2 number_of_classes;                                                      \
    struct inner_class *classes;                                               \
  })                                                                           \
  A(EnclosingMethod, { u2 class_index, method_index; })                        \
  A(Synthetic, { int _; })                                                     \
  A(Signature, { u2 signature_index; })                                        \
  A(SourceFile, { u2 sourcefile_index; })                                      \
  A(SourceDebugExtension, { u1 *debug_extensions; })                           \
  A(LineNumberTable, {                                                         \
    u2 line_number_table_length;                                               \
    struct line_number_table_entry *line_number_table;                         \
  })                                                                           \
  A(LocalVariableTable, {                                                      \
    u2 local_variable_table_length;                                            \
    struct local_variable_table_entry *local_variable_table;                   \
  })                                                                           \
  A(LocalVariableTypeTable, {                                                  \
    u2 local_variable_type_table_length;                                       \
    struct local_variable_type_table_entry *local_variable_type_table;         \
  })                                                                           \
  A(Deprecated, { int _; })                                                    \
  A(RuntimeVisibleAnnotations, {                                               \
    u2 num_annotations;                                                        \
    annotation_t *annotations;                                                 \
  })                                                                           \
  A(RuntimeInvisibleAnnotations, {                                             \
    u2 num_annotations;                                                        \
    annotation_t *annotations;                                                 \
  })                                                                           \
  A(RuntimeVisibleParameterAnnotations, {                                      \
    u1 num_parameters;                                                         \
    struct {                                                                   \
      u2 num_annotations;                                                      \
      annotation_t *annotations;                                               \
    } * parameter_annotations;                                                 \
  })                                                                           \
  A(RuntimeInvisibleParameterAnnotations, {                                    \
    u1 num_parameters;                                                         \
    struct {                                                                   \
      u2 num_annotations;                                                      \
      annotation_t *annotations;                                               \
    } * parameter_annotations;                                                 \
  })                                                                           \
  A(AnnotationDefault, { element_value_t default_value; })                     \
  A(BootstrapMethods, {                                                        \
    u2 num_bootstrap_methods;                                                  \
    struct bootstrap_method *bootstrap_methods;                                \
  })

struct attribute_info_t {
  u2 attribute_name_index;
  u4 attribute_length;
  union {
#define A(name, ...) struct __VA_ARGS__ name;
    ATTRIBUTES
#undef A
  } as;
};

#endif // JVM_ATTRIBUTE_T
