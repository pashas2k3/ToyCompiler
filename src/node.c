#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "node.h"
#include "symbol.h"
#include "type.h"

extern int yylineno;
/***********
  Prototypes
************/
void node_print_expression_statement(FILE *output,
				     struct node
				     *expression_statement);
void node_print_fcn_declarator(FILE* output,
			       struct node* declarator);
void node_print_declarator  (FILE* output,
			     struct node* declarator);
void node_print_expression_list(FILE* output,
				struct node* node);
void node_print_fcn_call  (FILE* output,
			   struct node* declarator);
void node_print_expression(FILE *output, struct node *expression);
void node_print_comma_expr(FILE* output, struct node* expression);
void node_print_compound_statement(FILE* output, struct node* expression);
void node_print_decl(FILE* output, struct node* expression);
void node_print_type_node(FILE * output,
			  struct node* decl_specifier);
void node_print_initialized_declarator(FILE * output,
				       struct node* decl_specifier);
void node_print_subscript_expr(FILE* output, struct node* expression);
void node_print_cast(FILE* output, struct node* cast_expr);
void node_print_type_name_node(FILE* output, struct node* cast_expr);
/****************
 * CREATE NODES *
 ****************/

/* Allocate and initialize a generic node. */
struct node *node_create(int node_kind) {
  struct node *n;

  n = malloc(sizeof(struct node));
  assert(NULL != n);

  n->kind = node_kind;
  n->line_number = yylineno;
  n->ir = NULL;
  return n;
}

/*
 * node_identifier - allocate a node to represent an identifier
 *
 * Parameters:
 *   text - string - contains the name of the identifier
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Returns a NUL-terminated string in newly allocated memory, containing the
 *   name of the identifier. Returns NULL if memory could not be allocated.
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 *
 */
struct node *node_identifier(char *text, int length)
{
  struct node *node = node_create(NODE_IDENTIFIER);
  memset(node->data.identifier.name, 0, MAX_IDENTIFIER_LENGTH + 1);
  strncpy(node->data.identifier.name, text, length);
  node->data.identifier.symbol = NULL;
  return node;
}

/*
 * node_number - allocate a node to represent a number
 *
 * Parameters:
 *   text - string - contains the numeric constant
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Returns a node containing the value and an error flag. The value is computed
 *   using strtoul. If the error flag is set, the integer constant was too large
 *   to fit in an unsigned long. Returns NULL if memory could not be allocated.
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 */
struct node *node_number(char *text)
{
  struct node *node = node_create(NODE_NUMBER);

  errno = 0;
  node->data.number.value = strtoul(text, NULL, 10);
  if (node->data.number.value == ULONG_MAX
      && ERANGE == errno) {
    /* Strtoul indicated overflow. */
    node->data.number.overflow = true;
  } else if (node->data.number.value > 4294967295ul) {
    /* Value is too large for 32-bit unsigned long type. */
    node->data.number.overflow = true;
  } else {
    node->data.number.overflow = false;
  }
  /* Since one bit would be for sign - 2^31*/
  if (node->data.number.value < 2147483648ul)
    node->data.number.result.type =
      type_basic(false, TYPE_WIDTH_INT);
  else
    node->data.number.result.type =
      type_basic(true, TYPE_WIDTH_INT);

  node->data.number.result.ir_operand = NULL;
  return node;
}

struct node *node_string(const char *text, int length)
{
  struct node *node = node_create(NODE_STRING);
  int i = 0;
  memset(node->data.string.str, 0, MAX_STRING_LENGTH + 1);
  node->data.string.strLen = (length < MAX_STRING_LENGTH)?length:MAX_STRING_LENGTH;

  for(i = 0; i < node->data.string.strLen; i++)
    node->data.string.str[i] = text[i];

  node->data.string.result.type =
    type_array( type_basic(false, TYPE_WIDTH_CHAR), length);
  /*removed the unsignedness to allow conversion of
    string to char arrays which may be signed as they would need to
    be compatible types*/
  node->data.string.result.ir_operand = NULL;
  return node;
}

struct node *node_binary_operation(int operation,
				   struct node *left_operand,
                                   struct node *right_operand)
{
  struct node *node = node_create(NODE_BINARY_OPERATION);
  if(operation < BINOP_MULTIPLICATION ||
     operation > UNARY_NOT)
    assert(0);/*replace with error message or error recovery mechanism*/
  node->data.binary_operation.operation = operation;
  node->data.binary_operation.left_operand = left_operand;
  node->data.binary_operation.right_operand = right_operand;
  node->data.binary_operation.result.type = NULL;
  node->data.binary_operation.result.ir_operand = NULL;
  return node;
}
struct node *node_if_else_trenary(struct node *operand1,
				  struct node *operand2,
				  struct node* operand3)
{
  struct node *node = node_create(NODE_IF_ELSE_TRENARY);
  /*NEED ASSERTION HERE*/
  node->data.if_else_trenary.operand[0] = operand1;
  node->data.if_else_trenary.operand[1] = operand2;
  node->data.if_else_trenary.operand[2] = operand3;

  node->data.if_else_trenary.result.type = NULL;
  node->data.if_else_trenary.result.ir_operand = NULL;

  return node;
}

struct result *node_get_result(struct node *expression) {
  switch (expression->kind) {
  case NODE_NUMBER:
    return &expression->data.number.result;
  case NODE_IDENTIFIER:
    return &expression->data.identifier.symbol->result;
  case NODE_BINARY_OPERATION:
    return &expression->data.binary_operation.result;
  case NODE_STRING:
    return &expression->data.string.result;
  case NODE_UNARY_OPERATION:
    return &expression->data.unary_operation.result;
  case NODE_CAST:
    return &expression->data.cast.result;
  case NODE_FUNCTION_CALL:
    return &expression->data.function_call.result;
  case NODE_COMMA_EXPR:
    return node_get_result(expression->data.expr_list.assignment_expr);
  case NODE_IF_ELSE_TRENARY:
    return &expression->data.if_else_trenary.result;
  case NODE_SUBSCRIPT_EXPR:
      return &expression->data.subscript_expr.result;

  default:
    assert(0);
    return NULL;
  }
}

/***************************************
 * PARSE TREE PRETTY PRINTER FUNCTIONS *
 ***************************************/

void node_print_binary_operation(FILE *output, struct node *binary_operation) {
  static const char *binary_operators[] = {
    "*", "/", "+", "-", "=", "%",
    "&&", "||", "|", "^", "&",
    "==", "!=", ">>","<<",
    "<", "<=", ">",">=",
    "+=", "-=", "*=", "/=", "%=",
    "<<=", ">>=", "&&=", "^=", "||=",
    NULL
  };
  assert(NULL != binary_operation);
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  fputs("(", output);
  node_print_expression(output, binary_operation->data.binary_operation.left_operand);
  fputs(" ", output);
  fputs(binary_operators[binary_operation->data.binary_operation.operation], output);
  fputs(" ", output);
  node_print_expression(output, binary_operation->data.binary_operation.right_operand);
  fputs(")", output);
}

void node_print_unary_operation(FILE *output,
				struct node *unary_operation)
{
  static const char *unary_operators[] = {
    "!", "++", "--", "&",
    "~", "+", "-", "*",
    "++", "--",
    NULL
  };
  assert(NULL != unary_operation);

  assert(NODE_UNARY_OPERATION == unary_operation->kind);

  fputs("(", output);
  /*Print the operator before or after*/
  if (UNARY_POST_INC > unary_operation->data.unary_operation.
      operation){
    fputs(unary_operators[unary_operation->
			  data.unary_operation.operation -
			  UNARY_NOT], output);
    fputs(" ", output);
    node_print_expression(output, unary_operation->
			  data.unary_operation.operand);
  } else {

    node_print_expression(output, unary_operation->data.
			  unary_operation.operand);
    fputs(" ", output);
    fputs(unary_operators[unary_operation->data.
			  unary_operation.operation - UNARY_NOT],
	  output);
  }
  fputs(")", output);
}
void node_print_trenary_operation(FILE* output,
				  struct node* trenary_statement)
{
  assert(NODE_IF_ELSE_TRENARY == trenary_statement->kind);
  fputs("(", output);
  node_print_expression(output, trenary_statement->data.
			if_else_trenary.operand[0]);
  fputs(")?", output);
  node_print_expression(output, trenary_statement->data.
			if_else_trenary.operand[1]);
  fputs(":", output);
  node_print_expression(output, trenary_statement->data.
			if_else_trenary.operand[2]);
  fputs("\n", output);
}

void node_print_number(FILE *output, struct node *number) {
  assert(NULL != number);
  assert(NODE_NUMBER == number->kind);

  fprintf(output, "%lu", number->data.number.value);
}

/*
 * After the symbol table pass, we can print out the symbol address
 * for each identifier, so that we can compare instances of the same
 * variable and ensure that they have the same symbol.
 */
void node_print_identifier(FILE *output, struct node *identifier) {
  assert(NULL != identifier);
  assert(NODE_IDENTIFIER == identifier->kind);
  fputs(identifier->data.identifier.name, output);
  fprintf(output, "$%lx", (unsigned long)identifier->data.identifier.symbol);
}

static void print_expression_compound_stmt_call(FILE* output, struct node* stmt)
{
  if(NULL != stmt)
    {
      if(NODE_COMPOUND_STATEMENT == stmt->kind)
	node_print_compound_statement(output, stmt);
      else
	node_print_expression(output, stmt);
    }
}
void node_print_reserved_keyword_expr(FILE* output, struct node* expression)
{
  assert(NULL != expression);
  switch (expression->kind)
    {
    case NODE_IF_ELSE:
      {
	fputs("IF(",output);
	node_print_expression(output,
			      expression->data.if_else.condn);
	fputs("){\n", output);
	/* if statement*/
	print_expression_compound_stmt_call(output,
				      expression->data.
				      if_else.expr[0]);
	fputs("} ELSE {\n", output);
	/*else statement*/
	print_expression_compound_stmt_call(output,
					    expression->data.
					    if_else.expr[1]);

	fputs("}", output);
	break;
      }
    case NODE_IF:
      {
	fputs("IF(",output);
	node_print_expression(output,
			      expression->data.if_else.condn);
	fputs("){\n", output);
	/* if statement*/
	print_expression_compound_stmt_call(output,
					    expression->data.
					    if_else.expr[0]);
	fputs("}", output);

	break;
      }
    case NODE_GOTO:
      {
	fputs("GOTO (",output);
	node_print_expression(output,
			      expression->data.jump.location);
	fputs(")", output);
	break;
      }
    /* case NODE_FOR: */
    /*   { */
    /* 	assert(NODE_FOR_EXPR == expression->data.condn_loop.condn->kind); */
    /* 	node_print_expression(output, expression->data. */
    /* 			      condn_loop.condn); */
    /* 	fputs("{\n",output); */

    /* 	print_expression_compound_stmt_call(output,  */
    /* 					    expression->data. */
    /* 					    condn_loop.statement); */
    /* 	fputs("}\n",output); */
    /*   } */
    case NODE_FOR_EXPR:
      {
	fputs("FOR (", output);
	/*PRINT THE FOR LOOP !!!!*/
	if(NULL != expression->data.for_loop_expr.
	   initial_clause )
	  node_print_expression(output,
				expression->data.for_loop_expr.
				initial_clause);

	fputs(";", output);
	if(NULL != expression->data.for_loop_expr.
	   term_expr )
	  node_print_expression(output,
				expression->data.for_loop_expr.
				term_expr);
	fputs(";", output);
	if(NULL != expression->data.for_loop_expr.
	   inc_expr )
	  node_print_expression(output,
				expression->data.for_loop_expr.
				inc_expr);

	fputs(")", output);
	fputs("{\n",output);

	print_expression_compound_stmt_call(output,
					    expression->data.
					    for_loop_expr.statement);
	fputs("}\n",output);
	break;
      }
    case NODE_DO_WHILE:
      {
	fputs("DO {\n", output);

	print_expression_compound_stmt_call(output,
					    expression->data.
					    condn_loop.statement);

	fputs("} WHILE(", output);
	node_print_expression(output,expression->data.condn_loop.
			      condn);
	fputs(")\n",output);

	break;
      }
    case NODE_WHILE:
      {
	fputs("WHILE(", output);
	node_print_expression(output,expression->data.condn_loop.
			      condn);
	fputs("){\n",output);
	print_expression_compound_stmt_call(output,expression->data.
					    condn_loop.statement);
	fputs("}\n", output);
	break;
      }
    case NODE_CONTINUE:
      {
	fputs("CONTINUE\n", output);
	break;
      }
    case NODE_BREAK:
      {
	fputs("BREAK\n", output);
	break;
      }
    case NODE_LABEL:
      {
	node_print_identifier(output, expression->data.label.label);
	break;
      }
    case NODE_LABELED_STATEMENT:
      {
	struct node* label =
	  expression->data.labeled_statement.label;
	struct node* named_label =
	  label->data.label.label;

	node_print_identifier(output, named_label);
	fputs(":", output);
	node_print_expression_statement(output,
					expression->data.
					labeled_statement.
					statement);
	break;
      }

    case NODE_RETURN:
      {
	fputs("RETURN (", output);
	if( NULL != expression->data.jump.location )
	  node_print_expression(output,expression->data.
				jump.location);
	fputs(")",output);
	break;
      }
    case NODE_IF_ELSE_TRENARY:
      node_print_trenary_operation(output, expression);
      break;

    case NODE_COMPOUND_STATEMENT:
      node_print_compound_statement(output,expression);
      break;
      /*This has been put here as placeholder till
	the final refactoring*/

    default:
      assert(0);
    }
}
void node_print_string(FILE* output, struct node* node)
{
  int cnt;
  assert(NODE_STRING == node->kind);
  fputs ("STRING(", output);

  for(cnt = 0; cnt<  node->data.string.strLen; cnt++)
    {
      fprintf (output,"%d ", node->data.string.str[cnt]);
    }

  fputs(")",output);
}
void node_print_abstract_declarator(FILE* output,
				    struct node* abstract_decl)
{
  assert(NODE_ABSTRACT_DECL == abstract_decl->kind);/*Unimplmented*/

  if(abstract_decl->data.abstract_decl.optPtrType)
    {
      struct node* curr_node;
      assert(NODE_POINTER == abstract_decl->data.abstract_decl.
	     optPtrType->kind);
      curr_node = abstract_decl->data.abstract_decl.
	optPtrType->data.pointer.pointerListPrior;

      while(curr_node != NULL) {
	curr_node = curr_node->data.pointer.pointerListPrior;
	fputs("*", output);
      }
    fputs(" *", output);
  }

  if(abstract_decl->data.abstract_decl.abstractDecl)
    node_print_declarator(output,
			  abstract_decl->data.abstract_decl.
			  abstractDecl);
}

void node_print_type_name_node(FILE* output,
			       struct node* node)
{
  assert(NODE_TYPE_NAME == node->kind);
  /* fputs ("(", output); */
  node_print_expression(output, node->data.
			type_name.decl_specifier);
  if(node->data.type_name.abstract_decl)
    {
      struct node* abstract_decl = node->data.type_name.
	abstract_decl;
      if(NODE_ABSTRACT_DECL == abstract_decl->kind)
	node_print_abstract_declarator(output, abstract_decl);
      else{
	assert(NODE_POINTER == abstract_decl->kind);
	fputs( "*", output);
      }
    }
  /* fputs (")", output); */
}
void node_print_expression(FILE *output, struct node *expression) {
  assert(NULL != expression);
  switch (expression->kind)
    {
    case NODE_BINARY_OPERATION:
      node_print_binary_operation(output, expression);
      break;
    case NODE_UNARY_OPERATION:
      node_print_unary_operation(output, expression);
      break;
    case NODE_IDENTIFIER:
      node_print_identifier(output, expression);
      break;
    case NODE_STRING:
      node_print_string(output, expression);
      break;
    case NODE_NUMBER:
      node_print_number(output, expression);
      break;
    case NODE_TYPE:
      node_print_type_node(output, expression);
      break;
    case NODE_TYPE_NAME:
      node_print_type_name_node(output, expression);
      break;
    case NODE_DECL:
      node_print_decl(output, expression);
      break;
    case NODE_FUNCTION_CALL:
      node_print_fcn_call(output, expression);
      break;
    case NODE_EXPRESSION_LIST:
      node_print_expression_list(output, expression);
      break;
    case NODE_COMMA_EXPR:
      node_print_comma_expr (output, expression);
      break;
    case NODE_SUBSCRIPT_EXPR:
      node_print_subscript_expr(output, expression);
      break;
    case NODE_CAST:
      node_print_cast(output, expression);
      break;
    default:
      node_print_reserved_keyword_expr(output, expression);
      break;
    }
}

void node_print_cast(FILE* output, struct node* cast)
{
  assert(NODE_CAST == cast->kind);
  if(cast->data.cast.special_usage)
    fputs("/*",output);
  fputs("(", output);

  node_print_type_name_node(output,
		       cast->data.cast.type);
  fputs(")", output);

  if(cast->data.cast.special_usage)
    fputs("*/",output);

  node_print_expression(output,
			cast->data.cast.expr);
}

void node_print_subscript_expr(FILE* output, struct node* expression)
{
  assert(NODE_SUBSCRIPT_EXPR == expression->kind);
  fputs("(",output);
  node_print_expression(output,
			expression->data.subscript_expr.postfix_expr);
  fputs("[",output);

  node_print_expression(output,
			expression->data.subscript_expr.expr);
  fputs("])",output);
}

void node_print_comma_expr(FILE* output, struct node* expression)
{
  assert(NODE_COMMA_EXPR == expression->kind);
  fputs("(",output);
  node_print_expression(output, expression->data.
			expr_list.assignment_expr);
  {
    struct node* curr_node;
    curr_node = expression->data.expr_list.list;

    while(curr_node != NULL)
      {
	fputs(",", output);
	node_print_expression(output,
			      expression->data.expr_list.list);
	curr_node = curr_node->data.pointer.pointerListPrior;
      }
  }
  fputs(")",output)  ;
}

void node_print_expression_statement(FILE *output, struct node *expression_statement) {
  assert(NULL != expression_statement);
  if(NODE_EXPRESSION_STATEMENT == expression_statement->kind)
    node_print_expression(output,
			  expression_statement->data.
			  expression_statement.expression);

  assert(NODE_EXPRESSION_STATEMENT < expression_statement->kind ||
  	 NODE_GOTO >= expression_statement->kind);

  node_print_expression(output,
			expression_statement);
}

void node_print_decl_or_statement_list(FILE *output,
				       struct node *statement_list)
{
  assert(NODE_DECL_OR_STATEMENT_LIST == statement_list->kind);

  if (NULL != statement_list->data.decl_or_statement_list.init) {
    node_print_decl_or_statement_list(output,
				      statement_list->data.
				      decl_or_statement_list.init);
  }
  node_print_expression_statement(output,
				  statement_list->
				  data.decl_or_statement_list.
				  statement);
  fputs(";\n", output);
}

void node_print_type_node(FILE * output,
			  struct node* decl_specifier)
{
  /*This does not include pointer type, keeping this separate*/
  struct type* type;
  assert(NODE_TYPE == decl_specifier->kind);
  fputs("(", output);
  type = decl_specifier->data.type.type;

  if (TYPE_BASIC == decl_specifier->data.type.typeId)
    {
      if (!type_is_unsigned(type))
	fputs("SIGNED ",output);
      else
	fputs("UNSIGNED ",output);

      switch(type_size(type))
	{
	case TYPE_WIDTH_CHAR:
	  fputs("CHAR ",output);break;
	case TYPE_WIDTH_SHORT:
	  fputs("SHORT INT ",output);break;
	case TYPE_WIDTH_INT:
	case TYPE_WIDTH_LONG:
	  fputs("LONG INT ",output); break;
	default:
	  assert(0);
	}
    } else {
    assert(TYPE_VOID == decl_specifier->data.type.typeId);
    fputs("VOID ",output);
  }

  fputs(")",output);
}

/* void node_print_pointer(FILE* output,  */
/* 			struct node* pointer,  */
/* 			* */

void node_print_pointer_declarator(FILE* output,
				   struct node* pointer_declarator)
{
  struct node* pointerList;
  struct node* curr_node;
  assert(NODE_POINTER_DECLARATOR == pointer_declarator->kind);
  /*Making sure we need to get (*p) instead of (*)p */


  pointerList = pointer_declarator->data.pointer_declarator.pointer;

  assert(NODE_POINTER == pointerList->kind);
  curr_node = pointerList->data.pointer.pointerListPrior;
  {
    int num_parens = 0;
    int num;
    while(curr_node != NULL)
      {
	curr_node = curr_node->data.pointer.pointerListPrior;
	num_parens++;
	fputs("( *", output);
      }
    fputs("( *", output);
    node_print_declarator(output,
			  pointer_declarator->data.
			  pointer_declarator.direct_declarator);
    fputs(")", output);

    for(num = 0;num < num_parens; num++)
      {
	fputs(")", output);
      }
  }
}

void node_print_array_declarator(FILE* output,
				 struct node* array_decl)
{
  assert(NODE_ARRAY_DECLARATOR == array_decl->kind);
  fputs("(",output);
  node_print_declarator(output, array_decl->data.array_decl.declarator);
  fputs("[",output);
  if (array_decl->data.array_decl.constant_expr != NULL)
    node_print_expression(output, array_decl->data.array_decl.constant_expr);
  /**COULD CALLING PRINT_EXPR CAUSE ISSUES??****/
  fputs("]",output);
  fputs(")",output);
}

void node_print_declarator(FILE* output,
			   struct node* declarator)
{
  /*Gateway for printing out declared variables*/
  switch(declarator->kind)
    {
    case NODE_POINTER_DECLARATOR:
      node_print_pointer_declarator(output,
				    declarator);
      break;/*direct/pointer declarator*/
    case NODE_IDENTIFIER:
      node_print_identifier(output, declarator);
      break;
    case NODE_INITIALIZED_DECL:
      node_print_initialized_declarator(output, declarator);
      break;
    case NODE_ABSTRACT_DECL:
      node_print_abstract_declarator(output, declarator);
      break;
    case NODE_ARRAY_DECLARATOR:
      node_print_array_declarator(output, declarator);
      break;
    case NODE_FUNCTION_DECL:
      node_print_fcn_declarator(output, declarator);
      break;
    default:
      assert(0);
      break;
    }

}

void node_print_param_decl(FILE* output, struct node* paramsList)
{
  assert(NODE_PARAMETER_DECL == paramsList->kind);
  fputs("(",output);
  node_print_type_node(output,
		       paramsList->data.param_decl.decl_specifier);

  /*Declarator print*/
  if (paramsList->data.param_decl.declarator != NULL)
    {
      node_print_declarator(output,
			    paramsList->data.param_decl.declarator);
    }
  fputs(")",output);
}

void node_print_param_list (FILE* output,
			    struct node* paramsList)
{
  fputs("(", output);
  /*should be a type [identifier]*/
  assert(NODE_PARAMETER_DECL == paramsList->kind ||
	 NODE_TYPE == paramsList->kind ||
	 NODE_PRM_LIST == paramsList->kind);

  if(NODE_PARAMETER_DECL == paramsList->kind)
    {
      node_print_param_decl(output, paramsList);
    }
  else if(NODE_TYPE == paramsList->kind)
    {
      node_print_type_node(output, paramsList);
    } else {
      node_print_param_decl(output,
			    paramsList->data.term_list.term);
      fputs(",", output);
      node_print_param_list(output,
			    paramsList->data.term_list.list);
    }

  fputs(")", output);
}
void node_print_expression_list(FILE* output,
				struct node* node)
{
  assert(NODE_EXPRESSION_LIST == node->kind);
  node_print_expression(output, node->data.expr_list.list);
  fputs(",",output);
  node_print_expression(output, node->data.expr_list.assignment_expr);

}
void node_print_fcn_call(FILE* output,
			 struct node* node)
{
  struct node* expList, *curr_node;
  assert(NODE_FUNCTION_CALL == node->kind);
  fputs("(", output);
  node_print_expression(output, node->data.function_call.postfix_expr);

  /*Print the list of inputs*/
  fputs("(", output);
  expList = node->data.function_call.expression_list;
  curr_node = expList;

  if(NULL != curr_node)
    node_print_expression(output, curr_node);
  /* while(NULL != curr_node && */
  /* 	NODE_EXPRESSION_LIST == curr_node->kind) */
  /*   { */
  /*     node_print_expression(output, curr_node->data. */
  /* 			    expr_list.assignment_expr); */
  /*     curr_node = expList->data.expr_list.list; */
  /*     if(curr_node != NULL) */
  /* 	fputs(",",output); */
  /*   } */
  /* /\* THe current node is not a list but some identifier */
  /*    (most probable case), so print it separately*\/ */
  /* if(curr_node != NULL) { */
  /*   node_print_expression(output, curr_node); */
  /* } */


  fputs("))", output);

}
void node_print_fcn_declarator(FILE* output,
			       struct node* declarator)
{
  assert(NODE_FUNCTION_DECL == declarator->kind);

  node_print_identifier(output, declarator->data.declarator.name);
  node_print_param_list(output, declarator->data.declarator.paramsList);
}

void node_print_initialized_declarator(FILE* output, struct node* declarator)
{
  assert(NODE_INITIALIZED_DECL == declarator->kind);
  fputs("(", output);
  node_print_declarator(output, declarator->data.term_list.list);
  fputs("),(",output);
  node_print_declarator(output, declarator->data.term_list.term);
  fputs(")", output);
}

void node_print_fcn_def_specifier(FILE* output, struct node* fn_defn_sp)
{
  assert(NODE_FUNCTION_DEF_SPECIFER == fn_defn_sp->kind);

  node_print_type_node(output,
		       fn_defn_sp->data.function_defn_specifier.
		       decl_specifier);

  node_print_declarator(output,
			fn_defn_sp->data.
			function_defn_specifier.declarator);

  if(fn_defn_sp->data.function_defn_specifier.type != NULL)
    {
      fprintf(output,"\n   /*-----------Function Table-----\n");
      fprintf(output,"\n   -----Type-----\n");
      symbol_print_table(output,
			 fn_defn_sp->data.function_defn_specifier.
			 type->data.function.outermost_block_table);
      fprintf(output,"   -----Label-----\n");
      symbol_print_table(output,
			 fn_defn_sp->data.function_defn_specifier.
			 type->data.function.
			 statement_label_symbol_table);
      fprintf(output,"   ------Table ends-------*/\n");
    }

}

void node_print_compound_statement(FILE* output,
			     struct node* compound_stmts)
{
  if(NULL == compound_stmts )
    return;/*Can be NULL under some conditions - empty braces*/
  fputs("(", output);
  assert(NODE_COMPOUND_STATEMENT == compound_stmts->kind);
  node_print_decl_or_statement_list(output,
				    compound_stmts->data.
				    compound_statement.
				    decl_or_statement_list);
  fputs(")\n", output);

  if(NULL != compound_stmts->data.compound_statement.block_symbol_table )
    {
      fprintf(output,"   /*------Compound symbol Table------\n");
      symbol_print_table(output,
			 compound_stmts->data.compound_statement.
			 block_symbol_table);

      fprintf(output,"  -----Symbol Table ends-----*/\n");
    }

}
void node_print_fcn_defn(FILE* output, struct node* fn_defn)
{
  assert(NODE_FUNCTION_DEFN == fn_defn->kind);

  node_print_fcn_def_specifier(output,
			       fn_defn->data.function_defn.
			       declarator);
  fputs("{\n", output);
  node_print_compound_statement(output,
				  fn_defn->data.function_defn.
				  body);
  fputs("}", output);

}

void node_print_decl(FILE* output, struct node* decl)
{
  assert(NODE_DECL == decl->kind);

  /*Get the type embedded in decl */
  fputs("(", output);
  node_print_type_node(output, decl->data.decl.name);

  node_print_declarator(output,
			decl->data.decl.paramsList);
  fputs(");\n", output);

}

void node_print_translation_unit(FILE* output,
				 struct node* translation_unit)
{
  int nodeType;
  assert(NODE_TRANSLATION_UNIT == translation_unit->kind &&
	 translation_unit != NULL);

  if (NULL != translation_unit->data.translation_unit.list) {
    node_print_translation_unit(output,
				translation_unit->
				data.translation_unit.list);
  }
  nodeType = translation_unit->
    data.translation_unit.top_level_decl->kind;

  assert(NODE_DECL == nodeType ||
	 NODE_FUNCTION_DEFN == nodeType);

  fputs("(", output);
  if(NODE_DECL == nodeType)
    {
    node_print_decl(output,
		    translation_unit->
		    data.translation_unit.top_level_decl);

    }
  else
    node_print_fcn_defn(output, translation_unit->
			data.translation_unit.top_level_decl);

  fputs(")\n", output);
  /* fputs("\n",output); */
}

/**********************************************/
struct node *node_pointer(struct node* prior_node_pointed_from)
{
  /*Replace with error message*/
  struct node* node;
  if (NULL != prior_node_pointed_from &&
      NODE_POINTER != prior_node_pointed_from->kind)
    {
      printf("\nERROR:Pointer type needs to be passed in\n");
      return NULL;
    }
  node =  node_create(NODE_POINTER) ;
  node->data.pointer.pointerListPrior = prior_node_pointed_from;
  node->data.pointer.type = NULL;/*To be updated in symbol table assignment*/
  return node;
}
struct node *node_unary_operation(int operation,
				  struct node *operand)
{
  struct node *node;
  if(operand == NULL )
    {
    printf("\n ERROR: Operand cannot be NULL\n");
    return NULL;
    }
  node = node_create(NODE_UNARY_OPERATION);
  if(operation < UNARY_NOT ||
     operation > UNARY_POST_DEC)
    assert(0);/*replace with error message or error recovery mechanism*/
  node->data.unary_operation.operation = operation;
  node->data.unary_operation.operand = operand;
  node->data.unary_operation.result.type = NULL;
  node->data.unary_operation.result.ir_operand = NULL;
  return node;
}

/**LOOP BASED NODE FUNCTIONS**Start**/
struct node* node_do_while(struct node* statement,
			   struct node* condn)
{
  struct node* node; node =  node_create(NODE_DO_WHILE);
  node->data.condn_loop.condn = condn;
  node->data.condn_loop.statement = statement;

  return node;
}
struct node* node_while_statement(struct node* expr, struct node* statement)
{
  struct node* node; node =  node_create(NODE_WHILE);
  node->data.condn_loop.condn = expr;
  node->data.condn_loop.statement = statement;

  return node;
}

struct node* node_continue()
{
  struct node* node; node =  node_create(NODE_CONTINUE);
  node->data.jump.location= NULL;

  return node;
}

struct node* node_break()
{
  struct node* node; node =  node_create(NODE_BREAK);
  node->data.jump.location = NULL;

  return node;
}

struct node* node_if(struct node* condn, struct node* statement)
{
  struct node* node; node =  node_create(NODE_IF);
  node->data.if_else.condn = condn;
  node->data.if_else.expr[0] = statement;
  node->data.if_else.expr[1] = NULL;

  return node;
}

struct node* node_if_else(struct node* expr,
			  struct node* statement1,
			  struct node* statement2)
{
  struct node* node; node =  node_create(NODE_IF_ELSE);
  node->data.if_else.condn = expr;
  node->data.if_else.expr[0] = statement1;
  node->data.if_else.expr[1] = statement2;

  return node;
}

struct node* node_goto(struct node* named_label)
{
    struct node* node; node =  node_create(NODE_GOTO);
    node->data.jump.location = named_label;

    return node;
}
struct node* node_label(struct node* label)
{
  struct node* node; node =  node_create(NODE_LABEL);
  node->data.label.label = label;
  node->data.label.type = NULL;/*type_basic(,width);*/

  return node;
}

struct node* node_return(struct node* returnInfo)
{
  struct node* node; node =  node_create(NODE_RETURN);
  node->data.jump.location = returnInfo;

  return node;
}

struct node* node_for_expr_loop(struct node* initial_clause,
				struct node* term_expr,
				struct node* inc_expr,
				struct node* statement)
{
  struct node* node; node =  node_create(NODE_FOR_EXPR);
  node->data.for_loop_expr.initial_clause = initial_clause;
  node->data.for_loop_expr.term_expr = term_expr;
  node->data.for_loop_expr.inc_expr = inc_expr;

  node->data.for_loop_expr.statement = statement;

  return node;
}

/* struct node* node_for_loop(struct node* for_expr,  */
/* 			   struct node* statement) */
/* { */
/*   struct node* node; node =  node_create(NODE_FOR); */
/*   node->data.condn_loop.condn = for_expr; */
/*   node->data.condn_loop.statement = statement; */

/*   return node; */
/* } */


/**LOOP BASED NODE FUNCTIONS**End**/
/*FUNCTION NODES*Start*/

struct node* node_function_declarator(struct node* name,
				      struct node* params)
{
  struct node* node;
  assert(params != NULL && name != NULL);
  node =  node_create(NODE_FUNCTION_DECL);
  node->data.declarator.name = name;
  node->data.declarator.paramsList = params;
  node->data.declarator.type = NULL;/*Type to be declared for function*/

  return node;
}
struct node* node_function_definition(struct node* declarator,
				      struct node* body)
{
  struct node* node; node =  node_create(NODE_FUNCTION_DEFN);
  node->data.function_defn.declarator = declarator;
  node->data.function_defn.body = body;

  return node;
}

struct node* node_function_call(struct node* postfix_expr,
				struct node* expression_list)
{
  struct node* node;
  assert(postfix_expr != NULL && expression_list != NULL);
  node =  node_create(NODE_FUNCTION_CALL);
  node->data.function_call.postfix_expr = postfix_expr;
  node->data.function_call.expression_list = expression_list;
  node->data.function_call.result.type = NULL;
  node->data.function_call.result.ir_operand = NULL;
  return node;
}

struct node* node_initialized_declarator(struct node*
					 preceding_initialized_list,
					 struct node*
					 initialzed_declarator)
{
  struct node* node; node =  node_create(NODE_INITIALIZED_DECL);
  node->data.term_list.list = preceding_initialized_list;
  node->data.term_list.term = initialzed_declarator;
  return node;
}

struct node* node_decl(struct node* declaration_specifiers,
		       struct node* init_decl_list)
{
  /*Kept a separate node for declaration*/
  struct node* node; node =  node_create(NODE_DECL);
  node->data.decl.name = declaration_specifiers;
  node->data.decl.paramsList = init_decl_list;
  return node;
}

struct node* node_abstract_declarator(struct node* optPointerType,
				      struct node* abstractDecl)
{
  struct node* node; node =  node_create(NODE_ABSTRACT_DECL);
  node->data.abstract_decl.optPtrType = optPointerType;
  node->data.abstract_decl.abstractDecl = abstractDecl;

  return node;
}

struct node* node_direct_abstract_declarator(struct node* list,
					     struct node* term,
					     int type)
{
  struct node* node; node =  node_create(NODE_DIRECT_ABSTRACT_DECL);
  node->data.direct_abstract_decl.list = list;
  node->data.direct_abstract_decl.term = term;
  node->data.direct_abstract_decl.type = type;

  return node;
}

struct node* node_parameter_decl(struct node* decl_specifier,
				 struct node* declarator)
{
  struct node* node; node =  node_create(NODE_PARAMETER_DECL);
  node->data.param_decl.decl_specifier = decl_specifier;
  node->data.param_decl.declarator = declarator;
  node->data.declarator.type = NULL;

  return node;
}

struct node* node_labeled_statement(struct node* label,
				    struct node* statement)
{
  struct node* node; node =  node_create(NODE_LABELED_STATEMENT);
  node->data.labeled_statement.label = label;
  node->data.labeled_statement.statement = statement;

  return node;}

/*Can this also be sent to just a simple binary operation*/
struct node* node_cast(struct node* type,
		       struct node* expr,
		       struct type* type_type,
		       bool special_usage)
{
  struct node* node;
  node =  node_create(NODE_CAST);
  node->data.cast.type = type;
  node->data.cast.expr = expr;
  node->data.cast.result.type = type_type;
  node->data.cast.result.ir_operand = NULL;
  node->data.cast.special_usage = special_usage;
  return node;
}
struct node* node_type_name (struct node* decl_specifier,
			     struct node* abstract_decl)
{
  struct node* node;
  node =  node_create(NODE_TYPE_NAME);
  node->data.type_name.decl_specifier = decl_specifier;
  node->data.type_name.abstract_decl = abstract_decl;
  return node;
}

struct node* node_prm_list(struct node* prm_list_prior,
			   struct node* prm)
{
  struct node* node;
  assert( prm_list_prior!= NULL && prm != NULL );
  node =  node_create(NODE_PRM_LIST);
  node->data.term_list.list = prm_list_prior;
  node->data.term_list.term = prm;
  return node;
}

struct node* node_array_declarator(struct node* declarator,
				   struct node* expr)
{
  struct node* node;
  /* assert(declarator != NULL ); */
  node =  node_create(NODE_ARRAY_DECLARATOR);
  node->data.array_decl.declarator = declarator;
  node->data.array_decl.constant_expr = expr;
  return node;
}

/*Set int type*/
struct node* node_type(int typeId, int width, bool isSigned)
{
  struct node* node;
  assert(typeId >=0 && width >=0 );
  node =  node_create(NODE_TYPE);
  /*Will have to modify this when symbol table part is implemented*/
  switch(typeId)
    {
    case TYPE_BASIC:
      {
	/*Till other types are implemented,
	  these two have same process*/
	node->data.type.type = type_basic(!isSigned, width);
	node->data.type.typeId = typeId;
	  break;
      }

    case TYPE_VOID:
      {
	node->data.type.type = type_void();
	node->data.type.typeId = typeId;
	break;
      }
    default:
      {
	printf("\nERROR:Other types should be handled elsewhere\n");
	node->data.type.type = NULL; node->data.type.typeId = -1;
	assert(0); break;
      }
    }
  return node;
}

struct node* node_function_def_specifier
(struct node* decl_specifier, struct node* declarator)
{
  struct node* node;
  assert ((decl_specifier != NULL) &&
	  (declarator != NULL) /* && */
	  /* (NODE_POINTER_DECLARATOR != declarator->kind) */);

  node =  node_create(NODE_FUNCTION_DEF_SPECIFER);
  node->data.function_defn_specifier.decl_specifier = decl_specifier;
  node->data.function_defn_specifier.declarator = declarator;
  node->data.function_defn_specifier.type = NULL;
  return node;
}
struct node* node_pointer_declarator(struct node* pointer,
				     struct node* pointer_declarator)
{
  struct node* node;
  assert((pointer != NULL) &&
	 (pointer_declarator != NULL));
  node =  node_create(NODE_POINTER_DECLARATOR);
  node->data.pointer_declarator.pointer = pointer;
  node->data.pointer_declarator.direct_declarator = pointer_declarator;

  return node;
}
struct node* node_compound_statement(struct node* decl_or_statement_list)
{
  struct node* node = NULL;
  if(NULL != decl_or_statement_list)
    {
      assert(decl_or_statement_list->kind ==
	     NODE_DECL_OR_STATEMENT_LIST );
      node =  node_create(NODE_COMPOUND_STATEMENT);
      node->data.compound_statement.decl_or_statement_list =
	decl_or_statement_list;
      node->data.compound_statement.block_symbol_table = NULL;
    }
  return node;
}

struct node *node_decl_or_statement_list(struct node *init,
					 struct node *statement)
{
  struct node* node;
  assert( (init == NULL ||
	   init->kind == NODE_DECL_OR_STATEMENT_LIST) &&
	  statement != NULL);
  node =  node_create(NODE_DECL_OR_STATEMENT_LIST);
  node->data.decl_or_statement_list.init = init;
  node->data.decl_or_statement_list.statement = statement;

  return node;
}

struct node* node_expression_list(struct node* init,
				  struct node* statement)
{
  struct node* node;
  assert(init != NULL && statement != NULL);
  node =  node_create(NODE_EXPRESSION_LIST);
  node->data.expr_list.list = init;
  node->data.expr_list.assignment_expr = statement;

  return node;
}

struct node* node_comma_expr(struct node* comma_expr,
			     struct node* assignment_expr)
{
  struct node* node;

  assert(comma_expr != NULL && assignment_expr != NULL);
  node =  node_create(NODE_COMMA_EXPR);
  /*Trying out with expr_list, to avoid create a separate node*/
  node->data.expr_list.list = comma_expr;
  node->data.expr_list.assignment_expr = assignment_expr;

  return node;
}

struct node* node_subscript_expr(struct node* postfix_expr,
				 struct node* expr)
{
  struct node* node, *inner_node;
  assert(postfix_expr != NULL && expr != NULL);

  /*Replace all the subscript expr with indirection of pointer arithmetic*/
  inner_node = node_binary_operation(BINOP_ADDITION, 
				      postfix_expr, expr);
  node = node_unary_operation(UNARY_INDIRECTION, inner_node);


  /* node = node_create(NODE_SUBSCRIPT_EXPR); */
  /* node->data.subscript_expr.postfix_expr = postfix_expr; */
  /* node->data.subscript_expr.expr = expr; */

  /* node->data.subscript_expr.result.type = NULL; */
  /* node->data.subscript_expr.result.ir_operand = NULL; */
  return node;
}
struct node* node_translation_unit(struct node* init,
				   struct node* expr)
{
  struct node* node;
  assert(expr != NULL);

  node =  node_create(NODE_TRANSLATION_UNIT);
  node->data.translation_unit.list = init;
  node->data.translation_unit.top_level_decl = expr;
  return node;
}

/**********************************************/
