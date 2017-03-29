#ifndef _IR_H
#define _IR_H

#include <stdio.h>
#include <stdbool.h>

#define OPTIMIZATION 1

struct node;
struct symbol;
struct symbol_table;

/*Populated during the IR generation*/
/*This should be made static in code cleanup to avoid global exposure*/
struct _global_decl_list{
  const char* name;
  const char* type_name;
  int space;
  struct _global_decl_list* next;
};

struct{
  struct _global_decl_list* list; /*Assumption - they will be NULL initialized as global variable*/
}global_decl_list;

struct _global_string_list{
  const char* label;
  const char* string;
  int strlen;
  struct _global_string_list* next;
};

struct 
{
  struct _global_string_list* list; /*Assumption - they will be NULL initialized as global variable*/
}global_string_repository;
/*Populated during the IR generation*/

enum operand_type{ 
  OPERAND_NUMBER,
  OPERAND_TEMPORARY,
  OPERAND_IDENTIFIER,
  OPERAND_FUNCTION,
  OPERAND_STR
};
struct ir_operand {
  int kind;
  bool is_lvalue; /*Can use this flag to differentiate when dealing with temporaries*/
  union {
    unsigned long number;
    int temporary;
    const char* str; /*For string and labels*/
    struct {
      const char* name;
      struct symbol* symbol; /*Can be used to get the number 
			       of arguments in case of functions*/
      int offset; /*For variables - its the offset 
		    w.r.t stack frame, 
		    for functions it is the maximum 
		    offset they need to account for*/
    }identifier; /*For variable, functions */

    struct{
      const char* name;
      struct type* type;      
      int local_offset;
    }function;
  } data;
};
/* This needs to be cleaned up ENUM - combining
   the terms */

/*Worry only about the signedness of the operand , let the MIPS code generation decide the hi/lo and sign extension part*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                       IMPORTANT
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if you add anything here, you gotta add it to ir_print_opcode also
*/
/*MIPS INSTRUCTION SET HAS BOTH GREATER THAN AND GREATER THAN EQUAL TO (AND CORRESPONDING OPERANDS FOR OTHER RELATIONAL OPERATIONS)*/
enum ir_operation{
  IR_NO_OPERATION, 

  /*3 operand instructions*/  
  IR_MULT, IR_MULTU, IR_DIV, IR_DIVU, 
  IR_ADD, IR_ADDU, IR_SUB, IR_SUBU, 
  IR_OR, IR_NOT, IR_XOR, IR_AND, 
  IR_SET_ON_LESS_THAN, IR_SET_ON_LESS_THAN_UNSIGNED,  
  IR_SET_ON_EQUALITY, IR_SET_ON_NOT_EQUAL,
  IR_SET_ON_GREATER_THAN, 
  IR_SET_ON_GREATER_THAN_UNSIGNED, 
  IR_SET_ON_GREATER_THAN_EQUAL,
  IR_SET_ON_GREATER_THAN_EQUAL_UNSIGNED,
  IR_SET_ON_LESS_THAN_EQUAL,  
  IR_SET_ON_LESS_THAN_EQUAL_UNSIGNED,
  IR_REMAINDER, IR_REMAINDERU,/*These two will need special handling on mips side*/

  /*Shift R-type instructions*/
  IR_SHIFT_L, IR_SHIFT_R, IR_SHIFT_RU, IR_SHIFT_LU,
  
  /*Branch instructions*/  
  IR_GOTO, IR_GOTO_FALSE,IR_GOTO_TRUE,

  /*Function definition demarcation*/
  IR_PROC_BEGIN, IR_PROC_END, 
  
  /*label definition*/
  IR_LABEL,

  /*Casting*/
  IR_CAST_WORD_TO_HALF_WORD,
  IR_CAST_WORD_TO_BYTE, IR_CAST_HALF_WORD_TO_UWORD,
  IR_CAST_HALF_WORD_TO_WORD, IR_CAST_BYTE_TO_UWORD,
  IR_CAST_BYTE_TO_WORD,

  /*Load Store operations*/
  IR_LOAD_WORD, IR_LOAD_BYTE, IR_LOAD_HALF_WORD,
  IR_STORE_WORD, IR_STORE_BYTE, IR_STORE_HALF_WORD,
  IR_CONST_INT, IR_ADDRESS_OF,

  /*Function invocation*/
  IR_PARAMETER, IR_RESULT_WORD,
  IR_CALL, IR_SYS_CALL, IR_RETURN_WORD,

  /*Miscellaneous*/
  IR_NEGATION, IR_LOGICAL_NOT, IR_SEQUENCE_POINT, 
  IR_TRANSFER_VALUE
};

struct string_pair
{
  const char* label;
  const char* stringValue;
};
struct string_table{
  struct string_pair pair;
  struct string_table *next;
};


struct ir_instruction {
  int kind;
  int index;
  struct ir_instruction *prev, *next;
  struct ir_operand operands[3];
};

struct ir_section {
  struct ir_instruction *first, *last;
};

void ir_generate_translation_unit(struct node *statement_list,
				  struct symbol_table* symbol_table);

void ir_print_section(FILE *output, struct ir_section *section);

void ir_optimize(struct ir_section* root_ir);

extern FILE *error_output;
extern int ir_generation_num_errors;
#endif
