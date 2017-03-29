#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdio.h>

struct node;
struct type;

struct symbol {
  char name[MAX_IDENTIFIER_LENGTH + 1];
  struct result result;
  bool is_fn_param;/*Flag used to set the offsets correctly*/
  int offset;
};

struct symbol_list {
  struct symbol symbol;
  struct symbol_list *next;
};

/*Outward linkages needed here*/
struct symbol_table_list;
struct symbol_table {
  struct symbol_list *variables;
  struct symbol_table* parent;
  struct symbol_table_list* children;
};

/*For inward linkaging*/
struct symbol_table_list {
  struct symbol_table* symbol_table;
  struct symbol_table_list *next;
};


/*The children information should be accessible by type information*/

void symbol_initialize_table(struct symbol_table *table);
void symbol_add_from_translation_unit(struct symbol_table *table,
				      struct node *statement_list);
void symbol_print_table(FILE *output, struct symbol_table *table);

struct type* get_type_from_type_name(struct node* node);

  
extern FILE *error_output;
extern int symbol_table_num_errors;

#endif /* _SYMBOL_H */
