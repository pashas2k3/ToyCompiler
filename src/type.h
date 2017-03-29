#ifndef _TYPE_H
#define _TYPE_H

#include <stdio.h>
#include <stdbool.h>

struct node;

#define TYPE_BASIC    1
#define TYPE_VOID     2
#define TYPE_POINTER  3
#define TYPE_ARRAY    4
#define TYPE_FUNCTION 5
#define TYPE_LABEL    6

#define TYPE_WIDTH_CHAR     1
#define TYPE_WIDTH_SHORT    2
#define TYPE_WIDTH_INT      4
#define TYPE_WIDTH_LONG     5
#define TYPE_WIDTH_POINTER  6
struct type {
  int kind;
  union {
    struct {
      bool is_unsigned;
      int width;
    } basic;
      /*Check the Void type is assigned to a function return type or a pointer type*/
    struct{
      struct type* pointer_to;
      /*Check the pointed to is a pointer, basic or array*/
    }pointer;
    struct{
      /* Check the arrayOf is a pointer, basic or array*/
      struct type* array_of_type;
      int num_elements;
    }array;
    struct{
      /* Check that the return type is pointer, basic or void*/
      /* Take care of conversions also*/
      struct type* return_type;
      struct symbol_list* params_list;

      struct symbol_table* outermost_block_table;
      struct symbol_table* statement_label_symbol_table; /*Symbol table for labels in the function*/

      struct node* function_body;
    }function;
    struct{
      const char* label_name;
      int line_num;
    }label;
  } data;
};

struct type *type_basic(bool is_unsigned, int width);
struct type *type_void();
struct type *type_pointer(struct type* pointer_to_type);
struct type *type_array(struct type* array_of_type,
			int num_elements);
struct type *type_function(struct type* return_type,
			   struct symbol_list* params_list,
			   struct symbol_table* outermost_block_table,
			   struct symbol_table* statement_label_symbol_table,
			   struct node* function_body);
struct type *type_label(const char* label_name, int line_num);


int type_size(struct type *t);

int type_is_equal(struct type *left, struct type *right);

int type_is_arithmetic(struct type *t);
int type_is_unsigned(struct type *t);

/* void type_assign_in_statement_list(struct node *statement_list); */
void type_assign_in_translation_unit(struct node *translation_unit);

void type_print(FILE *output, struct type *type);
void type_set_for_decl(struct type** type,
		       struct type* decl_type);

extern FILE *error_output;
extern int type_checking_num_errors;

#endif /* _TYPE_H */
