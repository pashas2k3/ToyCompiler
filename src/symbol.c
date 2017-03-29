#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "node.h"
#include "symbol.h"
#include "type.h"

int symbol_table_num_errors;
/*A local data structure to reverse the passed in param list 
  for registering them in order of declaration*/
struct param_list{
  struct symbol symbol;
  struct node* identifier;/*This was needed to do to big a refactoring of logic and code*/
  struct param_list* next;
};

struct param_symbol_list{
  struct param_list* list;
}; 

static void prepend_param(struct param_symbol_list* param_list, 
			  struct type* type,
			  struct node* name,
			  bool set_param_name)
{
  struct param_list* p_param = malloc(sizeof(struct param_list));

  assert(!set_param_name ||(TYPE_VOID == type->kind) ||
	 name != NULL);

  if(set_param_name && TYPE_VOID != type->kind){
    p_param->identifier = name; /*Using shallow copies*/
    strcpy(p_param->symbol.name, name->data.identifier.name);
  } else {
    p_param->identifier = NULL;
  }
  p_param->symbol.result.type = type;
  p_param->symbol.is_fn_param = true;

  /*Prepend the new node*/
  p_param->next = param_list->list;
  param_list->list = p_param;
}

    /* Leaving GOTO and other symbol references as is as
       per discussion at
       https://piazza.com/class/i57l56de5h24g0?cid=87*/
/* struct node_list{
  struct node* node;
  struct node_list* next;
};

struct node_table {
  struct node_list* to_update_node;
}to_update_table;

void add_node(struct node* new_node_ref)
{
  struct node_list* tmp = malloc(sizeof(struct node_list));
  tmp->node = new_node_ref;

  /\*ASSUMPTION - Global variable will be always initialized to 0*\/
  if(to_update_table.to_update_node == NULL){
    tmp->next = NULL;
  }else{
    tmp->next = to_update_table.to_update_node;
  }

  to_update_table.to_update_node = tmp;
}
*/
/**********************Prototypes******************************/
void symbol_add_from_declarators(struct symbol_table *table,
				 struct type* decl_type,
				 struct node *node);
void symbol_add_from_ptr_decl(struct symbol_table *table,
			      struct type* decl_type,
			      struct node *node);
void symbol_add_identifier(struct symbol_table *table,
			   struct symbol_table *decl_type,
			   struct symbol_table *paramNode);
void symbol_add_function_decl(struct symbol_table *table,
			      struct symbol_table *decl_type,
			      struct symbol_table *paramNode);
static void symbol_add_from_decl(struct symbol_table *table,
				 struct node *node);
static struct type* get_decl_specifier(struct node* type_node);
static struct type* get_function_decl(struct node** identifier,
				      struct type** top_type,
				      struct node *node,
				      bool param_name_expected);
static void symbol_add_compound_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* node);
static void symbol_add_from_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* statement);
void symbol_add_from_expression(struct symbol_table *table, struct node *expression);
static void symbol_add_from_reserved_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* statement);
static void symbol_add_from_function_call(struct symbol_table *table, struct node *node);
static void symbol_add_from_reserved_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* statement);
static void symbol_reference_from_label(struct symbol_table* label_table, struct node *label);
static void symbol_add_from_label(struct symbol_table* label_table,
				  struct node *label);
static void symbol_add_from_labeled_statement(struct symbol_table *table,struct symbol_table *label_table, struct node *expression_statement);
static void symbol_add_from_expression_list
(struct symbol_table *table,
 struct node *expression_statement);
struct type* get_declarator_type( struct node** identifier,
				 struct type** top_type,
				 struct node *node,
				 bool function_flag);
static void symbol_add_from_if_else(struct symbol_table* table, struct symbol_table* label_table, struct node* statement);
/***********************************/
struct type* get_type_from_decl(  struct node** identifier,
				 struct type* decl_type,
				 struct node *node,
				 bool function_flag);
int simplify(struct node* expr);
struct type* get_type_from_abstract_decl ( struct node** toBeIgnoredIdentifier,
					  struct type* decl_type,
					  struct node* node);
/**********************Prototypes******************************/

void add_symbol_table_to_children_list(struct symbol_table* parent,
				       struct symbol_table* child)
{
  struct symbol_table_list* tmp = malloc(sizeof(struct symbol_table_list));
  tmp->next = NULL;
  tmp->symbol_table = child;

  /*Add the newly created symbol_table_list to children*/
  if(NULL == parent->children) {
    parent->children  = tmp;
  } else {
    /*Iterate till the end of the list*/
    struct symbol_table_list* prev_node = NULL,
      *curr_node = parent->children;
    while(NULL != curr_node) {
      prev_node = curr_node;
      curr_node = curr_node->next;
    }
    prev_node->next = tmp;
  }
}
struct symbol_table* create_new_symbol_table()
{
  struct symbol_table *table = malloc(sizeof(struct symbol_table));
  table->variables = NULL;
  table->parent = NULL;
  table->children = NULL;

  return table;
}
void symbol_initialize_table(struct symbol_table *table) {
  table->variables = NULL;
  table->parent = NULL;
}

void error_fcn(FILE* output,  char* msg, int line_no)
{
  fprintf(output, "ERROR at line %d: %s\n",line_no,msg);
  symbol_table_num_errors++;
  /* exit(1); */
}

/**********************************************
 * WALK PARSE TREE AND ADD SYMBOLS INTO TABLE *
 **********************************************/
/*
 * This function is used to retrieve a symbol from a table.
 */
struct symbol *symbol_get(struct symbol_table *table,  char name[]) {
  struct symbol_list *iter;
  for (iter = table->variables; NULL != iter; iter = iter->next) {
    if (!strcmp(name, iter->symbol.name)) {
      return &iter->symbol;
    }
  }
  return NULL;
}
struct symbol *symbol_put(struct symbol_table *table,  char name[]) {
  struct symbol_list *symbol_list;

  symbol_list = malloc(sizeof(struct symbol_list));
  assert(NULL != symbol_list);

  strncpy(symbol_list->symbol.name, name, MAX_IDENTIFIER_LENGTH);
  symbol_list->symbol.result.type = NULL;
  symbol_list->symbol.result.ir_operand = NULL;
  symbol_list->symbol.offset = -1; /*Will be used in IR generation*/
  symbol_list->next = NULL;

  if(NULL == table->variables)
    table->variables = symbol_list;
  else {
    /* iterate to end of list */
    struct symbol_list* prev_list, *curr_list = table->variables;
    
    while(NULL != curr_list) {
      prev_list = curr_list;
      curr_list = curr_list->next;
    }
    prev_list->next = symbol_list ;
  } 

  return &symbol_list->symbol;
}
void symbol_add_decl_to_table(struct symbol_table* table,
			      struct type* decl_type,
			      struct node* identifier,
			      bool is_fn_param)
{
  struct symbol* symbol = NULL;
  assert(identifier != NULL && NODE_IDENTIFIER == identifier->kind);
  /*Implementation here for the symbol addition to the table*/
  symbol =
    symbol_get(table, identifier->data.identifier.name);
  if(NULL == symbol) {
    symbol = symbol_put(table, identifier->data.identifier.name);
    symbol->result.type = decl_type;
    symbol->is_fn_param = is_fn_param;
    identifier->data.identifier.symbol = symbol;
  } else {
    error_fcn(stdout,"Symbol has been redeclared",
	      identifier->line_number);
  }

}
void symbol_add_from_identifier(struct symbol_table *table,
				struct node *identifier) {
  struct symbol *symbol;
  assert(NODE_IDENTIFIER == identifier->kind);

  symbol = symbol_get(table, identifier->data.identifier.name);
  if (NULL == symbol) {
    symbol = symbol_put(table, identifier->data.identifier.name);
  }
  identifier->data.identifier.symbol = symbol;
}
void symbol_reference_from_identifier(struct symbol_table *table,
				      struct node *identifier) {
  struct symbol *symbol;
  struct symbol_table* curr_table = NULL;
  assert(NODE_IDENTIFIER == identifier->kind);
  curr_table = table;

  do{
      symbol = symbol_get(curr_table,
			  identifier->data.identifier.name);
      curr_table = curr_table->parent;
    }  while(NULL == symbol && NULL != curr_table);

  if (NULL == symbol) {

    error_fcn(stdout, "Variable not defined",
	      identifier->line_number);
  }
  identifier->data.identifier.symbol = symbol;
}

void symbol_add_from_binary_operation(struct symbol_table *table,
				      struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  symbol_add_from_expression(table, binary_operation->data.binary_operation.left_operand);
  symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);

  /* type_check_with_implicit_cast */
  /*   (binary_operation->data.binary_operation.operation, */
  /*    binary_operation->data.binary_operation.left_operand, */
  /*    binary_operation->data.binary_operation.right_operand); */

}
void symbol_add_from_unary_operation(struct symbol_table *table,
				     struct node *unary_operation) {
  assert(NODE_UNARY_OPERATION == unary_operation->kind);

  symbol_add_from_expression(table, unary_operation->data.unary_operation.operand);
}

static void symbol_add_from_function_call(struct symbol_table *table,
					  struct node *node)
{
  assert(NODE_FUNCTION_CALL == node->kind);
  symbol_add_from_expression(table,node->data.
			     function_call.postfix_expr);
  /*To take care of void as input argument*/
  if(NODE_EXPRESSION_LIST ==
     node->data.function_call.expression_list->kind)
    {
      /*It is either an expression list or a statement*/
      symbol_add_from_expression_list(table,
				      node->data.
				      function_call.expression_list);
    } else {
    symbol_add_from_expression(table,
			       node->data.
			       function_call.expression_list);
  }
}


void symbol_add_from_subscript_expr(struct symbol_table *table,
				    struct node *expression) {
  assert(NODE_SUBSCRIPT_EXPR == expression->kind);
  symbol_add_from_expression(table, expression->data.
			     subscript_expr.postfix_expr);
  symbol_add_from_expression(table, expression->data.
			     subscript_expr.expr);
}

void symbol_add_from_cast(struct symbol_table *table,
			  struct node *expression)
{
  assert(NODE_CAST == expression->kind);
  /*Ignoring the type as that should not need special referencing
   */
  symbol_add_from_expression(table, expression->data.cast.expr);
}

void symbol_add_from_comma_expr(struct symbol_table *table,
			  struct node *expression)
{
  assert(NODE_COMMA_EXPR == expression->kind);
  symbol_add_from_expression(table, expression->data.expr_list.list);
  symbol_add_from_expression(table, expression->data.expr_list.assignment_expr);
}

void symbol_add_from_expression(struct symbol_table *table,
				struct node *expression) {
  if(NULL == expression)
    return;

  switch (expression->kind) {
  case NODE_BINARY_OPERATION:
    symbol_add_from_binary_operation(table, expression);
    break;
  case NODE_UNARY_OPERATION:
    symbol_add_from_unary_operation(table, expression);
    break;
  case NODE_IDENTIFIER:
    symbol_reference_from_identifier(table, expression);
    break;
  case NODE_FUNCTION_CALL:
    symbol_add_from_function_call(table, expression);
    break;
  case NODE_NUMBER:
  case NODE_STRING:
  case NODE_TYPE_NAME:
    break;
  case NODE_COMMA_EXPR:
    symbol_add_from_comma_expr(table, expression);
    break;
  case NODE_SUBSCRIPT_EXPR:
    symbol_add_from_subscript_expr(table, expression);
    break;
  case NODE_CAST:
    symbol_add_from_cast(table, expression);
    break;
  case NODE_IF_ELSE_TRENARY:
    symbol_add_from_if_else(table, NULL, expression);break;
  case NODE_TYPE:
    assert(TYPE_VOID == expression->data.type.typeId);
    break;
  /*To implement*/
  default:
    assert(0);
    break;
  }
}

static void symbol_add_from_if_else(struct symbol_table* table, struct symbol_table* label_table, struct node* statement)
{
  switch(statement->kind){
  case NODE_IF_ELSE_TRENARY:
    {
      int i = 0;
      for(i = 0; i<3; i++)
	symbol_add_from_expression(table,
				   statement->data.
				   if_else_trenary.operand[i]);
      break;
    }
  case NODE_IF_ELSE:
    symbol_add_from_expression(table,
			       statement->data.if_else.condn);
    symbol_add_from_statement(table, label_table,
			      statement->data.if_else.expr[0]);
    symbol_add_from_statement(table, label_table,
			      statement->data.if_else.expr[1]);
    break;
  case NODE_IF:
    symbol_add_from_expression(table,
			       statement->data.if_else.condn);
    symbol_add_from_statement(table, label_table,
			      statement->data.if_else.expr[0]);
    break;
  default:
    assert(0);break;
  }
}

static void symbol_add_from_iterative_statements
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* statement)
{
  switch(statement->kind)
    {
    case NODE_DO_WHILE:
    case NODE_WHILE:
      symbol_add_from_expression(table,
				 statement->data.condn_loop.condn);
      symbol_add_from_statement(table, label_table,
				statement->data.condn_loop.
				statement);
      break;
    case NODE_FOR_EXPR: {
      symbol_add_from_statement(table, label_table,
				statement->data.
				for_loop_expr.initial_clause);
      symbol_add_from_expression(table,
				 statement->data.
				 for_loop_expr.term_expr);
      symbol_add_from_expression(table,
				 statement->data.
				 for_loop_expr.inc_expr);
      symbol_add_from_statement(table, label_table, statement->data.
				for_loop_expr.statement);
      break;
    }
    default:
      assert(0); break;
    }
}

static void symbol_add_from_reserved_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* statement)
{
  switch(statement->kind)
    {
      /*Keywords*/
    case NODE_BREAK:
    case NODE_CONTINUE:
      break;
    case NODE_RETURN:
      if(NULL != statement->data.jump.location)
	symbol_add_from_expression(table,
				   statement->data.jump.location);
      break;
    case NODE_GOTO:
      symbol_reference_from_label(label_table,
				  statement->data.jump.location);
      break;

    default:
      symbol_add_from_expression(table, statement);
    }
}
static void symbol_add_from_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* statement)
{
  /*****************888*********************/
  if(statement == NULL)
    return;
  /*To implement this*/
  switch(statement->kind)
    {
    case NODE_COMPOUND_STATEMENT:
      symbol_add_compound_statement(table, label_table,
				    statement);break;
    case NODE_DECL:
      symbol_add_from_decl(table, statement);break;

    case NODE_EXPRESSION_LIST:
      symbol_add_from_expression_list(table, statement);
      break;

    case NODE_LABELED_STATEMENT:
      symbol_add_from_labeled_statement(table, label_table, statement);break;
      /*Conditional statements*/
    case NODE_IF:
    case NODE_IF_ELSE:
    case NODE_IF_ELSE_TRENARY:
      symbol_add_from_if_else(table, label_table, statement);break;
      /*Iterative statements*/
    case NODE_DO_WHILE:
    case NODE_WHILE:
    case NODE_FOR_EXPR:
      symbol_add_from_iterative_statements(table, label_table, statement);break;

    default:
      symbol_add_from_reserved_statement(table, label_table,statement);break;
    }

}
static void symbol_reference_from_label(struct symbol_table* label_table,
					struct node *identifier)
{
  struct symbol* symbol;
  assert(NODE_IDENTIFIER == identifier->kind);/*For goto*/

  /*Not doing an error check for labels in this case*/
  symbol = symbol_get(label_table,
		      identifier->data.identifier.name);
  if(symbol == NULL) {
      /*Put the symbol now, will be resolved in add_label stage
	or throw error in function_defn end*/
    symbol = symbol_put(label_table,
			identifier->data.identifier.name);
    symbol->result.type = NULL;
    /* add_node(identifier); */
    }
}
static void symbol_add_from_label(struct symbol_table* label_table,
				  struct node *label)
{
  struct symbol* symbol;
  struct node* identifier;
  assert(NODE_LABEL == label->kind);

  identifier =label->data.label.label;
  assert(NODE_IDENTIFIER == identifier->kind);

  symbol = symbol_get(label_table,
		      identifier->data.identifier.name);
  if (NULL == symbol) {
    symbol = symbol_put(label_table,
			identifier->data.identifier.name);
  }
  if(NULL != symbol->result.type ) {
    error_fcn(stdout, "Label has been defined before",
	      identifier->line_number);
  }

  identifier->data.identifier.symbol = symbol;
  symbol->result.type= type_label(identifier->data.identifier.name,
				  identifier->line_number);
}
static void symbol_add_from_labeled_statement(struct symbol_table *table,struct symbol_table *label_table, struct node *expression_statement)
{

  assert(NODE_LABELED_STATEMENT == expression_statement->kind);
  symbol_add_from_label(label_table, expression_statement->data.
			labeled_statement.label);

  symbol_add_from_statement(table, label_table,
			    expression_statement->
			    data.labeled_statement.statement);
}

static void symbol_add_from_expression_list
(struct symbol_table *table,
 struct node *expression_statement) {
  struct node* currNode = NULL;
  assert(NODE_EXPRESSION_LIST == expression_statement->kind);

  currNode = expression_statement;
  while (NODE_EXPRESSION_LIST == currNode->kind)
    {
      symbol_add_from_expression(table,
				 currNode->
				 data.expr_list.assignment_expr);
      currNode = currNode->data.expr_list.list;
    }

  symbol_add_from_expression(table,
			     currNode);
}

void symbol_add_from_statement_list
(struct symbol_table *table, struct symbol_table *label_table,
 struct node *statement_list) {
  assert(NODE_DECL_OR_STATEMENT_LIST == statement_list->kind);

  if (NULL != statement_list->data.decl_or_statement_list.init) {
    symbol_add_from_statement_list(table, label_table,
				   statement_list->data.
				   decl_or_statement_list.init);
  }
  symbol_add_from_statement(table, label_table,
			    statement_list->data.
			    decl_or_statement_list.statement);
}
static void symbol_add_compound_statement
(struct symbol_table* table, struct symbol_table* label_table,
 struct node* node)
{
  struct symbol_table* new_table = NULL;
  assert(NODE_COMPOUND_STATEMENT == node->kind);

  /*1. Create a symbol table for this compound statement*/
  new_table = create_new_symbol_table();
  node->data.compound_statement.block_symbol_table = new_table;
  /*2. Outward and inward linkages for this*/
  new_table->parent = table;
  add_symbol_table_to_children_list(table, new_table);

  /*3. add expr from each statement to the list*/
  symbol_add_from_statement_list(new_table,
				 label_table,
				 node->data.
				 compound_statement.
				 decl_or_statement_list);
}


static void symbol_add_fcn_defn(struct symbol_table * table,
				struct node * node){
  struct node* function_def_specifier = NULL;
  struct symbol* fn_symbol_on_table = NULL;
  struct node* identifier = NULL;
  struct type* decl_type = NULL;
  /*1. Check if function is defined in the current table,
    otherwise use function_def_specifier to add a new declaration*/
  assert(NODE_FUNCTION_DEFN == node->kind);

  function_def_specifier = node->data.function_defn.declarator;

  /*Get the type and identifier string*/
  decl_type = get_decl_specifier
    (function_def_specifier->data.function_defn_specifier.
     decl_specifier);
  decl_type = get_type_from_decl
    (&identifier, decl_type, function_def_specifier->
     data.function_defn_specifier.declarator, true);

  /*Set the parents of the symbol table and label table*/
  decl_type->data.function.outermost_block_table->parent
    = table;

  assert(NULL != identifier);
  /*and call
    statment_list evaluator on the compound_statement  to avoid
    unecessary addition of another symbol_table for the compound
    statement in function body*/

  /*Should label table have parent as well??*/
  assert(NULL == node->data.function_defn.body ||
	 NODE_COMPOUND_STATEMENT ==
	 node->data.function_defn.body->kind);
    /*Would we need the following??*/
    decl_type->data.function.function_body = node->data.function_defn.body;

    /*3. Add the function symbol type and name to table*/
    symbol_add_from_identifier(table, identifier);
    fn_symbol_on_table = symbol_get(table,
				    identifier->data.identifier.
				    name);
    assert (NULL != fn_symbol_on_table );

    /*For definitions without a preceding prototype*/
    if(NULL == fn_symbol_on_table->result.type ) {
      fn_symbol_on_table->result.type = decl_type;
    } else if(!type_is_equal
	      (fn_symbol_on_table->result.type,decl_type)){
      error_fcn(stdout,
		"Function name conflict with other declaration",
		node->line_number);
    }

    /*Since shallow copies of tables passed around, therefore
      using the pointers directly*/
    fn_symbol_on_table->result.type = decl_type;
    node->data.function_defn.declarator
      ->data.function_defn_specifier.type = decl_type;

    /*5. call symbol_add_for_statement for the body  and
      Add the symbols from the code body of function recursively*/
    if(NULL != node->data.function_defn.body) {
      assert(NODE_COMPOUND_STATEMENT ==
	     node->data.function_defn.body->kind);
      symbol_add_from_statement_list
	(decl_type->data.function.outermost_block_table,
	 decl_type->data.function.statement_label_symbol_table,
	 node->data.function_defn.body->data.compound_statement.
	 decl_or_statement_list);
    }

    /*6. Check if any labels left unresolved in the function by
      traversing through the label symbol table*/
    /* Leaving GOTO and other symbol references as is as
       per discussion at
       https://piazza.com/class/i57l56de5h24g0?cid=87*/
}

static struct type* get_ptr_type_tree(struct node *pointer_list)
{
  struct type* pointer_type = NULL;
  struct node* curr_pointer_list = NULL;
  struct type* curr_ptr;
  struct type* prev_ptr = NULL;

  assert(NODE_POINTER == pointer_list->kind);
  pointer_type  = type_pointer(NULL);

  prev_ptr = pointer_type;
  curr_pointer_list = pointer_list->data.pointer.pointerListPrior;
  while(NULL != curr_pointer_list)
    {
      curr_ptr = type_pointer(prev_ptr);
      curr_pointer_list = curr_pointer_list->data.
	pointer.pointerListPrior;
      prev_ptr = curr_ptr;
    }

  return prev_ptr;
  /*return the root of the tree, the tail is still dangling*/
}

static struct type* get_ptr_decl( struct node** identifier,
				 struct type** top_type,
				 struct node *node,
				 bool function_flag)
{
  struct type* ptr_decl_type = NULL;
  struct type* decl_type = NULL;
  assert(NODE_POINTER_DECLARATOR == node->kind);

  ptr_decl_type = get_ptr_type_tree(node->data.
				    pointer_declarator.pointer);

  decl_type = get_declarator_type( identifier,
				   top_type,
				   node->data.pointer_declarator.
				   direct_declarator,
				   function_flag);
  type_set_for_decl(&decl_type,ptr_decl_type);
  return decl_type;
}


static struct type* get_type_from_direct_abstract_decl
( struct node** ignoredString,/*Probably don't need this*/
 struct type* decl_type,
 struct node* node)
{
  struct type* new_type = NULL;
  assert(NODE_DIRECT_ABSTRACT_DECL == node->kind);
  switch(node->data.direct_abstract_decl.type)
    {
    case DIRECT_ABS_DECL_PARENTHESIZED:
      new_type = get_type_from_abstract_decl(ignoredString,
					     new_type,
					     node->data.
					     direct_abstract_decl.
					     list);
      break;
    case DIRECT_ABS_DECL_SQUARE:
      {
	if(NULL != node->data.direct_abstract_decl.list)
	  new_type = get_type_from_direct_abstract_decl(ignoredString,
							new_type,
							node->data.
							direct_abstract_decl.
							list);
	/*Get the constant expression and assign it to array or pointer types - ignoring the array*/
	{
	  int num_elems = -1;
	  if(NULL != node->data.direct_abstract_decl.term)
	    num_elems =
	      simplify(node->data.direct_abstract_decl.term);
	  return type_array(new_type, num_elems);
	}
	break;
    }
    default:
      assert(0);break;
    }

  type_set_for_decl(&decl_type,new_type);
  return NULL;
}

struct type* get_type_from_abstract_decl
( struct node** stringToBeIgnored,
 struct type* decl_type,
 struct node* node)
{
  struct type* new_type = decl_type, *ptr_decl_type = NULL;
  assert(NODE_ABSTRACT_DECL == node->kind);
  /*both cannot be NULL, abstractDecl could be NULL in
    case of type_name from cast expression*/
  assert(NULL != node->data.abstract_decl.optPtrType ||
	 NULL != node->data.abstract_decl.abstractDecl);

  if(NULL != node->data.abstract_decl.optPtrType)
    ptr_decl_type = get_ptr_type_tree( node->data.
				  abstract_decl.optPtrType);
  if(NULL != node->data.abstract_decl.abstractDecl)
    new_type = get_type_from_direct_abstract_decl(stringToBeIgnored,
						  new_type,
						  node->data.
						  abstract_decl.
						  optPtrType);

  type_set_for_decl(&ptr_decl_type, new_type);
  return ptr_decl_type;
}
static struct type* get_decl_specifier(struct node* type_node)
{
  struct type* decl_type = NULL;
  assert(NODE_TYPE == type_node->kind);

  switch(type_node->data.type.typeId) {
  case TYPE_VOID:
    {
      decl_type = type_void();
      break;
    }
  case TYPE_BASIC:
    {
      /*Till other types are implemented,
	these two have same process*/
      decl_type = type_node->data.type.type;
      /*Should we make a deep copy???*/
      break;
    }
  default:
    {
      assert(0);/*ERROR HERE - Cannot have any other initial var*/
      break;
    }
  }
  return decl_type;
}

static struct type* replace_outermost_array_with_ptr
(struct type* type)
{
  /*Find last element of array tree*/
  if( TYPE_ARRAY == type->kind ) {
    return type_pointer(type->data.array.array_of_type);
  }
  return type;
}

struct type* get_type_from_type_name(struct node* node)
{
  struct node* abstract_decl = NULL;
  struct type* new_type = NULL;
  struct node* identifier_to_ignore = NULL;
  assert(NODE_TYPE_NAME == node->kind);

  /*look at the type_name for defining type of the
    cast expr and set the typ*/
  new_type = get_decl_specifier(node->data.type_name.
				decl_specifier);

  abstract_decl = node->data.type_name.abstract_decl;
  /*?? Would this cause problem with any assertion of identifier strings being NULL??*/
  if(NULL != abstract_decl)
    new_type = get_type_from_abstract_decl(&identifier_to_ignore,
					   new_type,
					   abstract_decl);
  return new_type;
}

/*Kept separate from get_type_decl as this can only be reached by
 function_def_specifier or function_decl_specifier*/
static struct type* get_parameter_decl
( struct node** identifier, struct node *node)
{
  struct type* decl_type = NULL;
  struct type* derived_decl_type = NULL;
  assert(NODE_PARAMETER_DECL == node->kind);
  decl_type = get_decl_specifier(node->data.
				 param_decl.decl_specifier);
  if(NULL == node->data.param_decl.declarator )
    return decl_type;


  else if(NODE_ABSTRACT_DECL ==
	  node->data.param_decl.declarator->kind)
    derived_decl_type = get_type_from_abstract_decl
      (identifier, decl_type, node->data.param_decl.
       declarator);
  else
    derived_decl_type = get_type_from_decl(identifier, decl_type,
			      node->data.param_decl.declarator,
			      false);

  derived_decl_type = replace_outermost_array_with_ptr(derived_decl_type);

  return derived_decl_type;
}

static struct symbol_list* create_symbol_list(struct symbol* symbol)
{
  struct symbol_list* tmp = malloc(sizeof(struct symbol_list));
  
  tmp->symbol = *symbol; /*Relying on default struct copy*/
  tmp->next = NULL;

  return tmp;
}

static struct symbol_list* add_symbol_from_prm_decl_list
(struct node* params_node_list, struct symbol_table* symbol_table,
 bool param_name_expected)
{
  /*Parameter are stored bottom up, so the final
    root_node should point to beginning of prameters*/
  struct node* curr_node = params_node_list;
  struct symbol_list* param_list = NULL;
  /*In case there are any parameters that need to be added to
    the table, they will be reversed to right ordering and
    added with this approach*/
  struct param_symbol_list param_symbol_list;
  param_symbol_list.list = NULL;

  /*Iterate over the parameters and store them in order*/
  while(NODE_PRM_LIST == curr_node->kind){
    struct node* param_name = NULL;     
    struct type* type = get_parameter_decl
      (&param_name, curr_node->data.term_list.term);

    prepend_param(&param_symbol_list, type, param_name, 
		  param_name_expected);

    curr_node = curr_node->data.term_list.list;
  }
  /*The last param node would be handled here.
    In case it was just one param node, it will skip over
    the while loop*/
  assert (NODE_PARAMETER_DECL == curr_node->kind) ;
  {
    struct node* param_name = NULL;     
    struct type* type = get_parameter_decl
      (&param_name, curr_node);
    prepend_param(&param_symbol_list, type, param_name, 
		  param_name_expected);
    curr_node = curr_node->data.term_list.list;
  } 

  /*Iterate over the list accumulated in the correct order and 
    set it on the symbol table*/
  {
    struct symbol_list* param_prev_list = NULL;
    struct param_list* curr_param = param_symbol_list.list;
    while(NULL != curr_param) {      
      /*Add the parameters to the new symbol table for
	the function*/
      struct symbol_list* curr_param_symbol = 
	create_symbol_list(&curr_param->symbol);

      if(TYPE_VOID != curr_param->symbol.result.type->kind){
	if(NULL == param_list){
	  param_list = curr_param_symbol;
	} else {
	  param_prev_list->next = curr_param_symbol;
	}
      }
      param_prev_list = curr_param_symbol;
      if(param_name_expected && 
	 TYPE_VOID != curr_param->symbol.result.type->kind) {
	symbol_add_decl_to_table
	  (symbol_table, curr_param->symbol.result.type,
	   curr_param->identifier, true);
      }
      curr_param = curr_param->next;
    }
  }    
  return param_list;
}

static struct type* get_function_decl(struct node** identifier,
				      struct type** top_type,
				      struct node *node,
				      bool param_name_expected)
{
  struct type* return_type = NULL;
  struct symbol_list* params_symbol_list = NULL;
  struct symbol_table* symbol_table = NULL;
  struct node* params_node_list = NULL;
  assert(NODE_FUNCTION_DECL == node->kind);
  /*Can't have function function kind of syntax*/
  assert(NODE_FUNCTION_DECL != node->data.declarator.name->kind);

  /*Pass the declarator portion to the get_type_declarator
    - could be pointer type also in return*/
  return_type = get_declarator_type(identifier,
				    top_type,
				    node->data.
				    declarator.name,
				    param_name_expected);
  /*only the above return types allowed*/

  /*For loop over parameter list to get the types */
  params_node_list = node->data.declarator.paramsList;

  /*Create new symbol table for the function outer body */
  symbol_table = create_new_symbol_table();
  params_symbol_list = add_symbol_from_prm_decl_list
    (params_node_list, symbol_table, param_name_expected);

  /*create the function label and symbol table*/
  /*Prototype adds the function into symbol table with
    its sub symbol table and function body for ease of
    implementation*/
  return type_function(return_type, params_symbol_list,
		       symbol_table ,create_new_symbol_table(),
		       NULL);
}

static int perform_binary_operation(int operation,
				    struct node* operand1,
				    struct node* operand2)
{
  /*Certain operations requiring lvalues won't be allowed*/
  switch(operation)
    {
    case BINOP_MULTIPLICATION:
      {
	return simplify(operand1)*simplify(operand2);
	break;
      }
    case BINOP_DIVISION:
      {
	return simplify(operand1)/simplify(operand2);
	break;
      }
    case BINOP_ADDITION:
      {
	return simplify(operand1)+simplify(operand2);
	break;
      }
    case BINOP_SUBTRACTION:
      {
	return simplify(operand1)-simplify(operand2);
	break;
      }
    /* case BINOP_ASSIGN: */
    /*   { */
    /* 	return simplify(operand1)simplify(operand2); */
    /* 	break; */
    /*   } */
    case BINOP_REMAINDER:
      {
	return simplify(operand1)%simplify(operand2);
	break;
      }
    case BINOP_AND:
      {
	return simplify(operand1) && simplify(operand2);
	break;
      }
    case BINOP_OR:
      {
	return simplify(operand1)||simplify(operand2);
	break;
      }
    case BINOP_BITWISE_OR:
      {
	return simplify(operand1)|simplify(operand2);
	break;
      }
    case BINOP_BITWISE_XOR:
      {
	return simplify(operand1)^simplify(operand2);
	break;
      }
    case BINOP_BITWISE_AND:
      {
	return simplify(operand1)&simplify(operand2);
	break;
      }
    case BINOP_EQUALITY:
      {
	return simplify(operand1)==simplify(operand2);
	break;
      }
    case BINOP_INEQUALITY:
      {
	return simplify(operand1)!=simplify(operand2);
	break;
      }
    case BINOP_BITSHIFT_RIGHT:
      {
	return simplify(operand1)>>simplify(operand2);
	break;
      }
    case BINOP_BITSHIFT_LEFT:
      {
	return simplify(operand1)<<simplify(operand2);
	break;
      }
    default:
      assert(0);/*Unsupported binary operation*/
    }
}

static int perform_unary_operation(int operation,
				   struct node* operand)
{
  /*Certain operations requiring lvalues won't be allowed*/
  switch(operation)
    {
    case UNARY_NOT:
      {
	return !simplify(operand);
	break;
      }
    /* case UNARY_ADDRESS: */
    /*   { */
    /* 	return &simplify(operand); */
    /* 	break; */
    /*   } */
    case UNARY_BITWISE_NEGATION:
      {
	return ~simplify(operand);
	break;
      }
    case UNARY_PLUS:
      {
	return +simplify(operand);
	break;
      }
    case UNARY_MINUS:
      {
	return -simplify(operand);
	break;
      }
    /* case UNARY_INDIRECTION: */
    /*   { */
    /* 	return *simplify(operand); */
    /* 	break; */
    /*   } */
    default:
      assert(0);/*Unsupported unary operation*/
    }
}

int simplify(struct node* expr)
{
  if(NODE_BINARY_OPERATION == expr->kind) {
    return perform_binary_operation(expr->data.binary_operation.
				    operation,
				    expr->data.binary_operation.
				    left_operand,
				    expr->data.binary_operation.
				    right_operand);
  } else if(NODE_UNARY_OPERATION == expr->kind){
    return perform_unary_operation(expr->data.unary_operation.
				   operation,
				   expr->data.unary_operation.
				   operand);
    /*operand2 would be NULL in that case*/
  } else if(NODE_NUMBER == expr->kind){
    return expr->data.number.value;
  }else{
    error_fcn(stdout, "Unsupported type used for array indexing ", expr->line_number);
    /* assert(0);/\*It has to be a ant expression*\/ */
  }
  return 0;
}
static struct type* get_array_decl( struct node** identifier,
				   struct type** top_type,
				   struct node *node,
				   bool function_flag)
{
  assert(NODE_ARRAY_DECLARATOR == node->kind);

  {
    struct type* array_type = NULL;
    struct type* arr_decl_type =
      get_declarator_type(identifier,
			  top_type,
			  node->data.array_decl.declarator,
			  function_flag);
    int num_elems = -1;
    if(NULL != node->data.array_decl.constant_expr)
      num_elems= simplify( node->data.array_decl.constant_expr);

    /*The type tree is inverted AST*/
    array_type = type_array(NULL, num_elems);

    type_set_for_decl(&arr_decl_type,array_type);

    return arr_decl_type;
  }
}

/*Don't need any table passed around as no linkaging required with
  declarator as long as identifier string can be acquired*/
struct type* get_declarator_type( struct node** identifier,
				 struct type** top_type,
				 struct node *node,
				 bool function_flag)
{
  struct type* declarator_top = *top_type;
    switch(node->kind){
    case NODE_IDENTIFIER:
      (*identifier) = node; break;

    case NODE_POINTER_DECLARATOR:
      declarator_top = get_ptr_decl(identifier,
				    top_type,node,
				    function_flag);
      break;
    case NODE_ARRAY_DECLARATOR:
      declarator_top = get_array_decl(identifier,
				      top_type,node,
				      function_flag);
      break;
    case NODE_FUNCTION_DECL:
      declarator_top = get_function_decl(identifier,
					 top_type, node,
					 function_flag);
      break;
    default:
      assert(0); break; /*CFG shouldn't allow this*/
  }
    if(NULL == *top_type)
      {
	*top_type = declarator_top;
      } /* else { */
    /*   /\*Connect the tree in reverse to get AST inverse  */
    /* 	for type tree*\/ */
    /*   type_set_for_decl(top_type, declarator_top); */
    /* } */
    return declarator_top;
}

struct type* get_type_from_decl( struct node** identifier,
				 struct type* decl_type,
				 struct node *node,
				 bool function_flag)
{
  struct type* top_type = NULL;
  struct type* declarator_type_to_ignore =
    get_declarator_type(identifier, &top_type,
			node, function_flag);

  (void)declarator_type_to_ignore;
  /*As the top level of the type tree is in the top_type itself*/

  /*Do error checking here */
  type_set_for_decl(&top_type, decl_type);

  return top_type;
}

static void symbol_add_from_decl(struct symbol_table *table,
				 struct node *node)
{
  /*Consider function declaration separately from other cases*/
  /*Rely on iterative callbacks to get the whole function type*/
  struct type* decl_type;
  assert(NODE_DECL == node->kind);
  decl_type = get_decl_specifier(node->data.decl.name);
  /*Don't have to worry about nested symbol tables decl parse tree does not include function definitions and compound_statements  */
  {
    /* struct type* type_tree = NULL; */
    struct node* paramNode = node->data.decl.paramsList;
    /*Iteratively get the declarator node and
      call on each of them*/
    struct node* currNode = paramNode;
    while(NODE_INITIALIZED_DECL ==
	  currNode->kind) {
	struct node* identifier = NULL;
	struct type* type = get_type_from_decl(&identifier,
					       decl_type,
					       currNode->data.
					       term_list.
					       term, false);
	assert(NULL != identifier);
	symbol_add_decl_to_table(table, type, identifier, false);
	currNode = currNode->data.term_list.list;
      }
    { /*Should be a decl type in that case*/
      /*Get the type and identifier from individual
	declarator and add them to symbol table*/
      struct node* identifier = NULL;
      struct type* type = get_type_from_decl( &identifier,
					      decl_type,
					      currNode,false );
      assert(NULL != identifier);
      symbol_add_decl_to_table(table, type, identifier,false);
    }
  }
}

void symbol_add_from_translation_unit(struct symbol_table *table,
				      struct node *root_node) {
  assert(NODE_TRANSLATION_UNIT == root_node->kind);

  if(NULL != root_node->data.translation_unit.list)
    {
      symbol_add_from_translation_unit(table,
				       root_node->data.
				       translation_unit.list);
    }
  {
    struct node* top_level_decl =root_node->data.translation_unit.top_level_decl;
    switch(top_level_decl->kind)
      {
      case NODE_DECL:
	/*Decl types have a type specified by data.decl.name*/
	symbol_add_from_decl(table, top_level_decl);break;
      case NODE_FUNCTION_DEFN:
	symbol_add_fcn_defn(table, top_level_decl);break;
      default: assert(0);
      }
  }
}
/***********************
 * PRINT SYMBOL TABLES *
 ***********************/

void symbol_print_table(FILE *output, struct symbol_table *table) {
  struct symbol_list *iter;

  fputs("symbol table:\n", output);

  for (iter = table->variables; NULL != iter; iter = iter->next) {
    fprintf(output, "  variable: %s $ %p\n", iter->symbol.name, (void*)&iter->symbol);
  }
  fputs("\n", output);
}
