#ifndef _NODE_H
#define _NODE_H

#include <stdio.h>
#include <stdbool.h>

struct type;

#define MAX_IDENTIFIER_LENGTH               31
#define MAX_STRING_LENGTH                   509

enum Nodes{
  NODE_NUMBER,
  NODE_IDENTIFIER,
  NODE_STRING,

  NODE_BINARY_OPERATION,
  NODE_UNARY_OPERATION,
  NODE_POINTER,
  NODE_LABEL,
  /* NODE_FOR, */
  NODE_FUNCTION_DECL,
  NODE_FUNCTION_DEFN,
  NODE_FUNCTION_CALL,
  NODE_INITIALIZED_DECL,
  NODE_DECL,
  NODE_ABSTRACT_DECL,
  NODE_DIRECT_ABSTRACT_DECL,
  NODE_PARAMETER_DECL,
  NODE_CAST,
  NODE_TYPE_NAME,
  NODE_PRM_LIST,
  NODE_ARRAY_DECLARATOR,
  NODE_TYPE,
  NODE_FUNCTION_DEF_SPECIFER,
  NODE_POINTER_DECLARATOR,
  NODE_DECL_OR_STATEMENT_LIST,
  NODE_EXPRESSION_LIST,

  NODE_COMMA_EXPR,
  NODE_SUBSCRIPT_EXPR,

  NODE_TRANSLATION_UNIT,

  /*Statements of different kind*/
  NODE_EXPRESSION_STATEMENT,
  NODE_LABELED_STATEMENT,
  NODE_COMPOUND_STATEMENT,
  NODE_IF,
  NODE_IF_ELSE,
  NODE_IF_ELSE_TRENARY,
  NODE_DO_WHILE,
  NODE_WHILE,
  NODE_FOR_EXPR,
  NODE_BREAK,
  NODE_CONTINUE,
  NODE_RETURN,
  NODE_GOTO
};

struct result {
  struct type *type;
  struct ir_operand *ir_operand;
};

struct node {
  int kind;
  int line_number;
  struct ir_section *ir;
  union {
    struct {
      unsigned long value;
      bool overflow;
      struct result result;
    } number;
    struct {
      char name[MAX_IDENTIFIER_LENGTH + 1];
      struct symbol *symbol;
    } identifier;
    struct {
      int operation;
      struct node *left_operand;
      struct node *right_operand;
      struct result result;
    } binary_operation;
    struct {
      int operation;
      struct node *operand;
      struct result result;
    } unary_operation;
    struct{
      struct node* condn;
      struct node* expr[2];
    } if_else;
    struct{
      struct node* operand[3];
      struct result result;
    } if_else_trenary;
    struct {
      struct node* location;
    }jump;
    struct {
      struct node* label;
      struct type* type;
    }label;
    struct{
      struct node* condn;
      struct node* statement;
    }condn_loop;
    struct{
      struct node* initial_clause;
      struct node* term_expr;
      struct node* inc_expr;
      struct node* statement;
    }for_loop_expr;
    struct {
      struct node *expression;
    } expression_statement;
    struct {
      struct node *init;
      struct node *statement;
    } decl_or_statement_list;
    struct {
      char str[MAX_STRING_LENGTH + 1]; /*To tie in life time of string to node with minimal code*/
      int strLen; /* As string could contain '\0'*/
      struct result result;
    } string; /*As only this kind of array allowed not naming it array*/
    struct{
      struct node* pointerListPrior;
      struct type* type;
    }pointer;
    struct{
      struct node* pointer;
      struct node* direct_declarator;
    }pointer_declarator;
    struct {
      struct node* list;
      struct node* term;
    }term_list;/*Separable parts of an expression*/
    /*are the following separate node needed?*/
    /*would combining them into one cause any problems in readability?*/
    struct {
      struct node* name;
      struct node* paramsList;
      struct type* type;
    }declarator;
    struct {
      struct node* name;
      struct node* paramsList;
    }decl;
    struct{
      struct node* decl_specifier;
      struct node* declarator;
      struct type* type;
    }param_decl;
    struct {
      struct node* decl_specifier;
      struct node* declarator;
      struct type* type;
    }function_defn_specifier;
    struct {
      struct node* declarator;
      struct node* body;
      struct result result;
    }function_defn;
    struct {
      struct node* postfix_expr;
      struct node* expression_list;
      struct result result;
    }function_call;
    struct{
      struct node* optPtrType;
      struct node* abstractDecl;
    }abstract_decl;
    struct{
      struct node* list;
      struct node* term;
      int type;
    }direct_abstract_decl;
    struct{
      struct node* label;
      struct node* statement;
    }labeled_statement;
    struct{
      struct node* type;
      struct node* expr;
      struct result result; /*Do we ever use this??*/
      bool special_usage;/*Used by pretty printer*/
    }cast;
    struct{
      struct node* decl_specifier;
      struct node* abstract_decl;
    }type_name;
    struct{
      struct node* declarator;
      struct node* constant_expr;
    }array_decl;
    struct{
      struct type* type;
      int typeId;
    }type;
    struct{
      struct node* decl_or_statement_list;
      struct symbol_table* block_symbol_table;
    }compound_statement;
    struct{
      struct node* list;
      struct node* assignment_expr;
    }expr_list;
    struct{
      struct node* postfix_expr;
      struct node* expr;
      struct result result;
    }subscript_expr;
    struct {
      struct node* list;
      struct node* top_level_decl;
      struct result result;
    }translation_unit;

  } data;
};

/* Binary operations */
/*USE ENUMS INSTEAD OF DEFINES*/
enum OperationType {
  /*Binary operations*/
  BINOP_MULTIPLICATION, BINOP_DIVISION, BINOP_ADDITION,
  BINOP_SUBTRACTION, BINOP_ASSIGN, BINOP_REMAINDER,
  BINOP_AND, BINOP_OR, BINOP_BITWISE_OR,
  BINOP_BITWISE_XOR, BINOP_BITWISE_AND, BINOP_EQUALITY,
  BINOP_INEQUALITY, BINOP_BITSHIFT_RIGHT, BINOP_BITSHIFT_LEFT,

  BINOP_LESS, BINOP_LESS_EQUAL, BINOP_GREATER, BINOP_GREATER_EQUAL,
  BINOP_ADD_ASSIGN, BINOP_SUBTRACT_ASSIGN,
  BINOP_MULTIPLY_ASSIGN, BINOP_DIVIDE_ASSIGN,
  BINOP_REMAINDER_ASSIGN, BINOP_BITSHIFT_LEFT_ASSIGN,
  BINOP_BITSHIFT_RIGHT_ASSIGN, BINOP_AND_ASSIGN,
  BINOP_XOR_ASSIGN, BINOP_OR_ASSIGN,

  /*Pre unary operations*/
  UNARY_NOT, UNARY_PRE_INC, UNARY_PRE_DEC,
  UNARY_ADDRESS, UNARY_BITWISE_NEGATION,UNARY_PLUS,
  UNARY_MINUS, UNARY_INDIRECTION,
  /*Post unary operations*/
  UNARY_POST_INC, UNARY_POST_DEC};
/* Constructors */
struct node *node_number(char *text);
struct node *node_identifier(char *text, int length);
struct node *node_string(const char *text, int length);

struct node *node_pointer(struct node* prior_node_pointed_from);

struct node *node_statement_list(struct node *list, struct node *item);
struct node *node_binary_operation(int operation,
				   struct node *left_operand,
				   struct node* right_operand);

struct node *node_unary_operation(int operation,
				  struct node *operand);

struct result *node_get_result(struct node *expression);

/* void node_print_statement_list(FILE *output, struct node *statement_list); */
void node_print_translation_unit(FILE *output,
				 struct node *statement_list);
/**LOOP BASED NODE FUNCTIONS**Start**/

struct node* node_do_while(struct node* statement,
			   struct node* condn);
struct node* node_while(struct node* expr,
			struct node* statement);
struct node* node_continue();
struct node* node_break();
struct node* node_if(struct node* expr, struct node* statement);
struct node* node_if_else(struct node* expr,
			  struct node* statement1,
			  struct node* statement2);
struct node* node_if_else_trenary(struct node* expr,
				  struct node* statement1,
				  struct node* statement2);
struct node* node_goto(struct node* named_label);

struct node* node_for_expr_loop(struct node* initial_clause,
				struct node* term_expr,
				struct node* inc_expr,
				struct node* statement);
/* struct node* node_for_loop(struct node* for_expr,  */
/* 			   struct node* statement); */

/**LOOP BASED NODE FUNCTIONS**End**/
/*FUNCTION NODES*Start*/

struct node* node_function_declarator(struct node* name,
				      struct node* params);
struct node* node_function_def_specifier(struct node* name,
					 struct node* params);

struct node* node_function_definition(struct node* declarator,
				      struct node* body);
struct node* node_function_call(struct node* postfix_expr,
				struct node* expression_list);
/*FUNCTION NODES*End*/
/*Initialize *Start*/
struct node* node_initialized_declarator(struct node* preceding_initialized_list,
					 struct node* initialzed_declarator);
struct node* node_decl(struct node* decl_spec,
		       struct node* init_decl_list);
/*Initialize *End*/

struct node* node_abstract_declarator(struct node* optPointerType,
				      struct node* abstractDecl);

enum DirectAbstractDeclaratorOptions{
  DIRECT_ABS_DECL_PARENTHESIZED=100,
  DIRECT_ABS_DECL_SQUARE};
struct node* node_direct_abstract_declarator(
					     struct node* declType,
					     struct node* declExpr,
					     int type);

struct node* node_parameter_decl(struct node* decl_specifier,
				 struct node* declarator);
struct node* node_label(struct node* label);
struct node* node_labeled_statement(struct node* label,
				    struct node* statement);
struct node* node_return(struct node* returnInfo);

struct node* node_cast(struct node* type,
		       struct node* expr, 
		       struct type* type_type, 
		       bool special_usage);
/*Can this also be sent to just a simple binary operation*/

struct node* node_type_name (struct node* type,
			     struct node* abstract_decl);

struct node* node_prm_list(struct node* prm_list_prior,
			   struct node* prm);

struct node* node_array_declarator(struct node* declarator,
				   struct node* expr);
struct node* node_comma_expr(struct node* comma_expr,
			     struct node* assignment_expr);
/*Set int type*/
struct node* node_type(int typeId, int width, bool isSigned);

struct node* node_conditional_expr(struct node* logicalCondn,
				   struct node* expr1,
				   struct node* expr2);
struct node* node_pointer_declarator(struct node* pointer,
				     struct node* pointer_declarator);


struct node* node_compound_statement(struct node* decl_or_statement_list);
struct node *node_decl_or_statement_list(struct node *init,
					 struct node *statement);
struct node* node_expression_list(struct node* init,
				  struct node* statement);

struct node* node_while_statement(struct node* expr,
				  struct node* statement);

struct node* node_subscript_expr(struct node* postfix_expr,
				 struct node* expr);
struct node* node_translation_unit(struct node* init,
				   struct node* expr);

#endif
