#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "node.h"
#include "symbol.h"
#include "type.h"

/*************************/
static void type_assign_in_statement(struct node* node,
				     const struct type*
				     return_type);

static void type_assign_in_return(struct node* node,
				  const struct type* type ) ;
static void type_assign_in_expr(struct node *expression);
static struct node* unary_usual_conversion_helper(struct node* operand_node,
						  struct type** operand_type,
						  bool special_casting);
static struct type* usual_binary_conversion_helper
(struct node** left_node_ptr, struct node** right_node_ptr, bool lhs_special_casting);
static struct node* create_auto_cast_node
(struct type* target_type, struct node* unary_expr,
 struct type** src_type, bool convertible, bool special_casting);

/*************************/

void type_error_fcn(const char* msg, int line_no)
{
  printf(" TYPE FATAL ERROR : %d : %s\n",line_no, msg);
  symbol_table_num_errors++;
  exit(1);
}

/**************************
 * PRINT TYPE EXPRESSIONS *
 **************************/

void type_print_basic(FILE *output, struct type *basic) {
  assert(TYPE_BASIC == basic->kind);

  if (basic->data.basic.is_unsigned) {
    fputs("unsigned ", output);
  } else {
    fputs("signed ", output);
  }

  switch (basic->data.basic.width) {
    case TYPE_WIDTH_INT:
      fputs("int", output);
      break;
    default:
      assert(0);
      break;
  }
}

void type_print(FILE *output, struct type *kind) {
  assert(NULL != kind);

  switch (kind->kind) {
    case TYPE_BASIC:
      type_print_basic(output, kind);
      break;
    default:
      assert(0);
      break;
  }
}
/*Make sure the caller doesn't pass its original node - otherwise
 it will be death by recursion*/

static struct type* get_result_lazy(struct node* expr)
{
  struct type* type = node_get_result(expr)->type;

  if(NULL == type) /*In case type hasn't been initialized*/
    {
      /* These types should already be assigned before
	 type checking */
      assert((NODE_NUMBER != expr->kind) &&
	     (NODE_IDENTIFIER != expr->kind) &&
	     (NODE_STRING != expr->kind));
      type_assign_in_expr(expr);
      type = node_get_result(expr)->type;
    }

  /*???Leaving this logic to the caller side allows
    LHS checks in assignment, can clean up later on???*/
  /* if(TYPE_FUNCTION == type->kind) */
  /*   type = type->data.function.return_type; */
  return type;
}
/***************************
 * CREATE TYPE EXPRESSIONS *
 ***************************/

struct type *type_basic(bool is_unsigned, int width) {
  struct type *basic;

  basic = malloc(sizeof(struct type));
  assert(NULL != basic);

  basic->kind = TYPE_BASIC;
  basic->data.basic.is_unsigned = is_unsigned;
  basic->data.basic.width = width;
  return basic;
}

struct type *type_void()
{
  struct type* void_type = malloc(sizeof(struct type));
  assert(NULL != void_type);

  void_type->kind = TYPE_VOID;

  return void_type;
}
/*Parent and child in type tree, with information from child to parent*/
/*May need to pass in line_number for error handling*/
void type_set_for_decl(struct type** type_parent_ref,
		       struct type* type_child)
{
  struct type* type_parent = *type_parent_ref;
  assert(NULL != type_child);
    /* return; */

  if(NULL == type_parent)
    {
      *type_parent_ref = type_child;
      return;
    }
  switch(type_parent->kind)
    {
    case TYPE_POINTER:
      {
	struct type* curr_type = type_parent;
	while(curr_type->data.pointer.pointer_to != NULL &&
	      TYPE_POINTER ==
	      curr_type->data.pointer.pointer_to->kind)
	  {
	    curr_type = curr_type->data.pointer.pointer_to;
	  }
	if(curr_type->data.pointer.pointer_to != NULL)
	  type_set_for_decl(&(curr_type->data.pointer.pointer_to),
			    type_child);
	else
	  curr_type->data.pointer.pointer_to = type_child;

	break;
      }
    case TYPE_FUNCTION:
      {
	if(NULL != type_parent->data.function.return_type)
	  type_set_for_decl(&(type_parent->data.function.return_type),
			    type_child);
	else
	  type_parent->data.function.return_type = type_child;

	{
	  struct type* ret_type =
	    type_parent->data.function.return_type;
	  assert(ret_type != NULL);

	  if( (TYPE_FUNCTION == ret_type->kind) ||
	      (TYPE_ARRAY == ret_type->kind) ||
	      (TYPE_LABEL == ret_type->kind) )
	    type_error_fcn("Invalid return type for function",-1);
	}
	break;
      }
    case TYPE_ARRAY:
      {
	struct type* curr_type = type_parent;
	/*Find last element of array tree*/
	while(curr_type->data.array.array_of_type != NULL &&
	      TYPE_ARRAY ==
	      curr_type->data.array.array_of_type->kind)
	  {
	    curr_type = curr_type->data.array.array_of_type;
	  }
	if(curr_type->data.array.array_of_type != NULL)
	  type_set_for_decl(&(curr_type->data.array.array_of_type),
			    type_child);
	else
	  curr_type->data.array.array_of_type = type_child;
	break;
      }
    default:
      assert(0);break;
    }
  *type_parent_ref = type_parent;
}
struct type *type_pointer(struct type* pointer_to_type)
{
  struct type* pointer_type = malloc(sizeof(struct type));
  if(NULL != pointer_to_type &&
     (TYPE_FUNCTION == pointer_to_type->kind ||
      TYPE_VOID == pointer_to_type->kind ||
      TYPE_LABEL == pointer_to_type->kind))
    type_error_fcn("Invalid pointer types",-1);

  pointer_type->kind = TYPE_POINTER;
  pointer_type->data.pointer.pointer_to = pointer_to_type;
  return pointer_type;
}

struct type *type_array(struct type* array_of_type,
			int num_elements)
{
  struct type* array_type = malloc(sizeof(struct type));
  if(NULL != array_of_type &&
     (TYPE_FUNCTION == array_of_type->kind ||
      TYPE_VOID == array_of_type->kind ||
      TYPE_LABEL == array_of_type->kind))
    type_error_fcn("Array of invalid types ",-1);

  array_type->kind = TYPE_ARRAY;
  array_type->data.array.array_of_type = array_of_type;
  array_type->data.array.num_elements = num_elements;

  return array_type;
}
struct type *type_function(struct type* return_type,
			   struct symbol_list* params_list,
			   struct symbol_table* outermost_block_table,
			   struct symbol_table* statement_label_symbol_table,
			   struct node* function_body)
{
  struct type* function_type = malloc(sizeof(struct type));
  if(NULL != return_type &&
     (TYPE_FUNCTION == return_type->kind ||
      TYPE_ARRAY == return_type->kind ))
    type_error_fcn("Function has invalid return type",
		   (function_body)?function_body->line_number:(-1));

  function_type->kind = TYPE_FUNCTION;
  function_type->data.function.return_type = return_type;
  function_type->data.function.params_list = params_list;
  /*TO DO - Replace any outer arrays with pointer types*/

  function_type->data.function.outermost_block_table = outermost_block_table;
  function_type->data.function.statement_label_symbol_table = statement_label_symbol_table;/* LABEL symbol table*/

  function_type->data.function.function_body = function_body;

  return function_type;
}
struct type *type_label(const char* label_name, int line_num)
{
  struct type* label_type = malloc(sizeof(struct type));
  assert(NULL != label_type);

  label_type->kind = TYPE_LABEL;
  assert(NULL != label_name);
  label_type->data.label.label_name = label_name;
  label_type->data.label.line_num = line_num;
  return label_type;
}

/****************************************
 * TYPE EXPRESSION INFO AND COMPARISONS *
 ****************************************/
struct type*
replace_outermost_array_with_ptr_type_if_needed(struct type* type, struct node** right_node_ref)
{
  /*Algo to implement---------------------
    Replace the array declarations with a pointer*/
  if( TYPE_ARRAY == type->kind ) {
    struct type* inner_type = type->data.array.array_of_type;
    struct node* right_node = *right_node_ref;
    struct type* target_type = type_pointer(inner_type);

    right_node = 
      create_auto_cast_node(target_type, right_node, &inner_type, 
			    true, false);
    *right_node_ref = right_node;
    /*inner_type is casted correctly at this stage*/
    return (inner_type);
  }
  return type;
}
/*Returns casted node*/
struct node* replace_array_subscripted_with_ptr_type_if_needed
(struct type** src_type, struct node *array_node)
{
  /*Algo to implement---------------------
    Replace the array declarations with a pointer
    Add a cast node at the lowest level of the array

    May have problem with array of pointer of arrays
  */
  if( TYPE_ARRAY == (*src_type)->kind ) {
    struct type *pointer_type = NULL;
    struct type *curr_type = (*src_type);
    struct type *prev_type = NULL;

    /*Go down the AST in a while loop and create a 
      pointer tree hierarchy - it doesn't matter with pointer 
      tree if created top down or bottom up as long as the right 
      side is attached to the primitive type*/

    while (TYPE_BASIC != curr_type->kind)
      {
	pointer_type = type_pointer(prev_type);
	if(TYPE_ARRAY == curr_type->kind)
	  curr_type = curr_type->data.array.array_of_type;
	else 
	  curr_type = curr_type->data.pointer.pointer_to;
	prev_type = pointer_type;/*For next iteration*/
      }

    assert(NULL != curr_type && TYPE_BASIC == curr_type->kind);
    /*Attach pointer to the the basic type as array cannot be 
      void/function type*/
    type_set_for_decl(&pointer_type, curr_type);
    
    /*Return casted node*/
    return create_auto_cast_node(pointer_type, array_node, src_type,
				 true, false );
  }

  return array_node;/*if nothing to be done*/
}

int type_is_function_equal(struct type* left,
			   struct type* right)
{
  int equal;
  assert((left->kind == right->kind) &&
	 (TYPE_FUNCTION == left->kind ));
  equal= type_is_equal(left->data.function.return_type,
		       right->data.function.return_type);

  if(equal)
    {
      /*Check params_list to see if function type is same*/
      struct symbol_list* left_params =
	left->data.function.params_list;
      struct symbol_list* right_params =
	right->data.function.params_list;
      int param_equality = 1;
      while(left_params != NULL &&
	    right_params != NULL &&
	    param_equality)
	{
	  /*Ignore name as only types matter in type matching*/
	  struct type * left_param_type = left_params->
	    symbol.result.type;
	  struct type * right_param_type = right_params->
	    symbol.result.type;

	  /* Make sure the pointer and array comparison
	     is done correctly
	     TODO - move this replacement to function construction
	     time to have only a one time cost
	  */
	  /* left_param_type = */
	  /*   replace_innermost_array_with_ptr_type_if_needed */
	  /*   (left_param_type); */
	  /* right_param_type = */
	  /*   replace_innermost_array_with_ptr_type_if_needed */
	  /*   (right_param_type); */

	  param_equality = type_is_equal(left_param_type,
					 right_param_type);
	  left_params = left_params->next;
	  right_params = right_params->next;
	}

      equal = param_equality &&
	(left_params == NULL)&& (right_params == NULL);
    }

  return equal;
}

int type_is_array_types_equal(struct type* left,
			      struct type* right)
{
  int num_left_arrays, num_right_arrays;
  int equal;
  assert((left->kind == right->kind) &&
	 (TYPE_ARRAY == left->kind ));
  /*Traverse the array tree and call type_is_equal
    on array_of type*/
  num_left_arrays = num_right_arrays = 0;

  while((TYPE_ARRAY == left->kind) &&
	(TYPE_ARRAY == right->kind) &&
	( (0 == left->data.array.num_elements)||
	  (0 == right->data.array.num_elements)||
	  (left->data.array.num_elements ==
	   right->data.array.num_elements) ))
    {
      num_left_arrays++; num_right_arrays++;
      left = left->data.array.array_of_type;
      right = right->data.array.array_of_type;
    }

  /* To make sure all the num_elements have been matched
   check that the array is not the type anymore*/
  equal = ((TYPE_ARRAY != left->kind) &&
	   (TYPE_ARRAY != right->kind) &&
	   (type_is_equal(left,right)));
  return equal;
}
int type_is_pointer_types_equal(struct type* left,
				struct type* right)
{
  int num_left_pointers, num_right_pointers;
  int equal;
  assert((left->kind == right->kind) &&
	 (TYPE_POINTER == left->kind ));

  /*Traverse the pointer tree and call type_is_equal
    on pointed to type*/
  num_left_pointers = num_right_pointers = 0;

  while((TYPE_POINTER == left->kind) &&
	(TYPE_POINTER == right->kind))
    {
      num_left_pointers++; num_right_pointers++;
      left = left->data.pointer.pointer_to;
      right = right->data.pointer.pointer_to;
    }

  equal = ( (num_left_pointers == num_right_pointers) &&
	    type_is_equal(left, right) );
  return equal;
}

int type_is_equal(struct type *left, struct type *right) {
  int equal;

  equal = left->kind == right->kind;

  if (equal) {
    switch (left->kind) {
      case TYPE_BASIC:
        equal = equal &&
	  left->data.basic.is_unsigned == right->data.basic.is_unsigned;
        equal = equal &&
	  left->data.basic.width == right->data.basic.width;
        break;

    case TYPE_VOID:
      break;

    case TYPE_POINTER:
      return type_is_pointer_types_equal(left, right);
      break;
    case TYPE_ARRAY:
      return type_is_array_types_equal(left, right);
      break;

    case TYPE_FUNCTION: /*has to handle pointer to array conversion also*/
      return type_is_function_equal(left,right);
      break;
    case TYPE_LABEL:
      equal = (strcmp(left->data.label.label_name,
		      right->data.label.label_name)==0);
      break;/*line number doesn't matter for equality*/
    default:
      equal = 0;
      break;
    }
  }

  return equal;
}

int type_is_arithmetic(struct type *t) {
  return TYPE_BASIC == t->kind;
}

int type_is_unsigned(struct type *t) {
  return( (type_is_arithmetic(t) && t->data.basic.is_unsigned) );
}

int type_is_void(struct type *t) {
  return TYPE_VOID == t->kind;
}

int type_is_scalar(struct type *t) {
  return type_is_arithmetic(t) || TYPE_POINTER == t->kind;
}

int type_size(struct type *t) {
  switch (t->kind) {
    case TYPE_BASIC:
      return t->data.basic.width;
    case TYPE_POINTER:
      return TYPE_WIDTH_POINTER;
    default:
      return 0;
  }
}

/*****************
 * TYPE CHECKING *
 *****************/

int type_checking_num_errors;
static int type_conversion_rank(struct type* type)
{
  int return_val = -1;
  assert(NULL != type && type_is_arithmetic(type));

  switch(type->data.basic.width)
    {
    case TYPE_WIDTH_CHAR:
      return_val =  20;break;
    case TYPE_WIDTH_SHORT:
      return_val = 30;break;
    case TYPE_WIDTH_INT:
      return_val = 40;break;
    case TYPE_WIDTH_LONG:
      return_val = 50;break;
    default:
      assert(0);break;
    }
  return return_val;
}

/* build the tree bottom up,
   returning decl_node by value and
   type_node by address*/
/* MAKE SURE YOU GOT IT RIGHT */
static struct node* get_type_node(struct type* type,
				  struct node** type_node)
{
  struct node* returned_node = NULL;
  /*Traverse the rest of the type tree recursively
    and chaining in opposite direction*/
  switch(type->kind){

  case TYPE_ARRAY: {
    int num_el = type->data.array.num_elements;
    struct node* node_const_expr = NULL;
    returned_node = get_type_node (type->data.array.array_of_type,
				   type_node);
    if(num_el != 0) {
      /* convert this to string to avoid changing
	 exposed node function.
	 ??? May have to refactor the code for type_array to
	 use a const_expr node with/without the num_el */
      char str[15];
      sprintf(str, "%d", num_el);
      node_const_expr = node_number(str);
    }
    return node_array_declarator(returned_node, node_const_expr);
    break;}

  case TYPE_POINTER:{
    returned_node = get_type_node (type->data.pointer.pointer_to,
				   type_node);
    return node_pointer(returned_node);
    break;}

  case TYPE_BASIC:{
    *type_node = node_type(type->kind, type->data.basic.width,
			   !type->data.basic.is_unsigned);
    /*The API expects a signed flag not unsigned flag*/
    return NULL;
    break; }
  default: assert(0); break;
    }
}

static struct node* create_auto_cast_node(struct type* target_type,
					  struct node* unary_expr,
					  struct type** src_type,
					  bool convertible,
					  bool special_casting)
{
  struct node* abstract_decl = NULL;
  struct node* decl_specifier = NULL;

  if (!convertible)
    {
      type_error_fcn
	("The two types cannot be converted implicitly",
	 unary_expr->line_number);
      return NULL;
    }

  abstract_decl = get_type_node(target_type, &decl_specifier);

  /* The cast_expr is handled on the caller side */
  {
    struct node* type_name = NULL;
    *src_type = target_type;
    type_name = node_type_name (decl_specifier, abstract_decl);
    return node_cast (type_name, unary_expr, target_type,
		      special_casting);
  }
}

/* static bool usual_binary_operation_convertible */
/* (struct type *target_type, struct type *src_type) */
/* { */
/*   if(TYPE_POINTER == target_type->kind && */
/*      TYPE_POINTER == src_type->kind) */
/*     return false; */

/*   if( (type_is_arithmetic(target_type) || */
/*        TYPE_POINTER == target_type->kind) && */
/*       (type_is_arithmetic(src_type) || */
/*        TYPE_POINTER == src_type->kind) ) */
/*     { */
/*       return true; */
/*     } */

/*   /\*A void type cannot be directly used and */
/*     void* is a pointer to void, so should be covered*\/ */
/*   return false; */
/* } */

static struct type* usual_binary_conversion_helper
(struct node** left_node_ptr, struct node** right_node_ptr,
 bool lhs_special_casting)
{
  struct node* left_node = *left_node_ptr;
  struct node* right_node = *right_node_ptr;
  struct type* left_type = NULL;
  struct type* right_type = NULL;
  struct type* binary_type = NULL;

  /* Do unary conversion first and then binary conversion */
  left_node = unary_usual_conversion_helper(left_node,
					    &left_type,
					    lhs_special_casting);
  right_node = unary_usual_conversion_helper(right_node,
					     &right_type,
					     false);

  /* Binary type determination when types don't match
     1. any numeric op signed (higher rank) => signed with higher rank
     2. unsigned (higher rank) op any numeric => unsigned with higher rank
     3. pointer_type op numeric => pointer_type
   */


  /*Implicit casts have already taken care of it*/
  if(type_is_equal(left_type, right_type))
    {
      *left_node_ptr = left_node;
      *right_node_ptr = right_node;
      return left_type;
    }

  /* at least one side should be arithmetic types
     if unary conversion valid*/
  if(!type_is_arithmetic(left_type) &&
     !type_is_arithmetic(right_type)) {
    type_error_fcn("Cannot convert between two non-arithmetic type",
		   left_node->line_number);
    return NULL;
  }
  if(type_is_arithmetic(right_type) &&
     type_is_arithmetic(left_type))
    {
      int left_rank = type_conversion_rank(left_type);
      int right_rank = type_conversion_rank(right_type);

      /*???need a better way of handling the signed
	and unsigned cases???*/
      if(left_rank > right_rank) {
	right_node = create_auto_cast_node(left_type, right_node,
					   &right_type, true, false);
      } else if (left_rank < right_rank){
	left_node = create_auto_cast_node(right_type, left_node,
					  &left_type, true, lhs_special_casting);
      } else {
	/*Unsigned operations need to be handled here*/
	assert(left_rank == right_rank);
	assert(!type_is_equal(left_type, right_type));

	/*If one is unsigned and other is not and have same rank,
	  cast them to unsigned type*/
	if(left_type->data.basic.is_unsigned &&
	   !right_type->data.basic.is_unsigned) {
	  right_node = create_auto_cast_node(left_type, right_node,
					     &right_type, true, false);
	}else {
	  assert(!left_type->data.basic.is_unsigned &&
		 right_type->data.basic.is_unsigned);
	  left_node = create_auto_cast_node(right_type, left_node,
					    &left_type, true, lhs_special_casting);
	}

      }
      assert(type_is_equal(left_type, right_type));
      binary_type = left_type;
    } else {
    /* one of them should be pointer type at this stage. 
       Same pointer types at two end would do at type_is_equal
       The type of the output is set to this*/
    struct type* pointer_type = NULL;
    if(TYPE_POINTER == left_type->kind)
      pointer_type = left_type;
    else{
      /*??? Should there be special casing for pointers
	according to operation - left_operand only can be
	pointer or none of them can be pointer or should
	only BINOP_ADDITION allows pointer type*/
      assert(TYPE_POINTER == right_type->kind);
      pointer_type = right_type;
    }
    binary_type = pointer_type;
    /*The pointer type is used for the binary op type*/
  }
  *left_node_ptr = left_node;
  *right_node_ptr = right_node;

  return binary_type;
}

static bool allows_lhs_rhs_pointers(int operation)
{
  switch(operation) {
   case BINOP_ASSIGN:
   case BINOP_EQUALITY:
   case BINOP_INEQUALITY:
   case BINOP_LESS:
   case BINOP_LESS_EQUAL:
   case BINOP_GREATER:
   case BINOP_GREATER_EQUAL: 
     return true;break;
  default:
    return false;break;
   }
}

static bool allows_one_side_pointer(int operation)
{
  switch(operation) {
  case BINOP_ADDITION:
  case BINOP_SUBTRACTION:    
  case BINOP_ADD_ASSIGN:
  case BINOP_SUBTRACT_ASSIGN:
    return true;break;
  default:
    return false;break;
  }
}
static void type_convert_usual_binary
(struct node *binary_operation, bool lhs_special_cast) {
  struct node* left_node, * right_node;
  struct type* binary_type;
  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  assert(BINOP_ASSIGN != binary_operation->data.binary_operation.
	 operation);
  left_node = binary_operation->data.binary_operation.left_operand;
  right_node = binary_operation->data.binary_operation.right_operand;

  binary_type = usual_binary_conversion_helper(&left_node,
					       &right_node,
					       lhs_special_cast);

  /*If it is pointer type output => pointer arithmetic involved
    which is allowed only for addition operations*/
  if(TYPE_POINTER == binary_type->kind)
    {
      struct type* left_type = get_result_lazy(left_node);
      struct type* right_type = get_result_lazy(right_node);

      if(left_type->kind == right_type->kind) {
	if(!allows_lhs_rhs_pointers(binary_operation->data.
				    binary_operation.operation))
	  type_error_fcn("Both sides of this operation cannot be pointers", binary_operation->line_number);
	}else{
	if(!allows_one_side_pointer(binary_operation->data.
				    binary_operation.operation))
	  type_error_fcn("Only one operand can be pointer type for this", binary_operation->line_number);
      }	
    }
  /*As stated in Harbison and Steele, treat the pointer
    like an unsigned type*/

  /*Reassign the left and right node to make sure the
    binary operation nodes are updated*/
  binary_operation->data.binary_operation.left_operand = left_node;
  binary_operation->data.binary_operation.right_operand = right_node;
  binary_operation->data.binary_operation.result.type = binary_type;
}

/*Recursively traverse the nodes to see if the LHS is lvalue*/
static bool is_lvalue_operand(struct node* node)
{
  switch(node->kind)
    {
    case NODE_NUMBER:
    case NODE_STRING:
    case NODE_FUNCTION_CALL:
      return false; break;
    case NODE_IDENTIFIER:
      return true; break;
    case NODE_ARRAY_DECLARATOR:
      return is_lvalue_operand(node->data.array_decl.declarator);
      break;

    case NODE_POINTER:
      return is_lvalue_operand(node->data.pointer_declarator.
			       direct_declarator); break;

      /*??is this correct way to go about it??*/
    case NODE_CAST:
      return is_lvalue_operand(node->data.cast.expr);break;

    case NODE_SUBSCRIPT_EXPR: {
      /*In case the type_corresponding to this is an arithmetic
	type rather than array type */
      /*What if it is a pointer type*/
      if(type_is_arithmetic(get_result_lazy(node)))
	return true;
      else
	return false;
      break;
    }
    case NODE_UNARY_OPERATION: {
	/*If indirection  and underlying expr has type_pointer
	 return true*/
      if(UNARY_INDIRECTION ==
	 node->data.unary_operation.operation &&
	 TYPE_POINTER == get_result_lazy(node->data.unary_operation.operand)->kind ) {
	return true;
      } else {
	return false;
      }
      break;
    }

    case NODE_COMMA_EXPR:
      {
	/*Comma expr last term can be a l_value*/
	return true;/*??? Does this need more analysis???*/
      }

    default:
      break;
    }
  return false;
}

struct node* assignment_conversion_helper(struct type * left_type,
					  struct node* right_node)
{
  struct type *right_type;

  /*Cast type_function to the return type of the function*/
  right_type = get_result_lazy(right_node);
  if(TYPE_FUNCTION == right_type->kind)
    right_type = right_type->data.function.return_type;

  /*Void is valid cast but cannot be used with anything*/
  if(TYPE_VOID == right_type->kind) {
    type_error_fcn
      ("Void types cannot be part of any assignment operation",
       right_node->line_number);
  }

  
  right_type = replace_outermost_array_with_ptr_type_if_needed(right_type, &right_node);

  if (!type_is_equal(right_type, left_type)) {

    /* arithmetic type - direct conversion*/
    bool both_are_arithmetic = type_is_arithmetic(left_type) &&
      type_is_arithmetic(right_type);

    /* Sticking with ISO C implemenation rather than traditional C compiler here
       pointer to T1 from pointer of T2 -
       a. conversion if T1 and T2 are equal
       b. constant 0
       c. void**/

    bool void_pointer_lhs_valid = false;
    {
      bool lhs_is_void_pointer =
	( (TYPE_POINTER == left_type->kind) &&
	  type_is_equal(type_void(), left_type->data.
			pointer.pointer_to) );
      bool rhs_is_zero = ((NODE_NUMBER == right_node->kind) &&
			  (0 == right_node->data.number.value));

      bool rhs_is_pointer = (TYPE_POINTER == right_type->kind);

      /*pointers to compatible type should already be
	covered when type equality is checked for */
      /*unsigned and signed type pointers are not compatible
       because unsigned and signed types are not compatible0*/
      void_pointer_lhs_valid =
	(  lhs_is_void_pointer && (rhs_is_zero || rhs_is_pointer));
    }
    /* Sticking with ISO C implemenation rather
       than traditional C compiler here (H&S 6.3.2)
       void* if a pointer
       a. any pointer
       b. constant 0*/
    bool pointer_lhs_valid = false;
    {
      bool lhs_is_pointer = (TYPE_POINTER == left_type->kind);
      bool rhs_is_void_ptr = ( (TYPE_POINTER == right_type->kind) &&
			       type_is_equal(type_void(),
					     right_type->data.
					     pointer.pointer_to));
      bool rhs_is_zero = ( (NODE_NUMBER == right_node->kind) &&
			   (0 == right_node->data.number.value));
      pointer_lhs_valid = ( lhs_is_pointer &&
			    (rhs_is_void_ptr || rhs_is_zero));
    }
    /*Didn't implement int to pointer or otherway conversion
      as that is not ISO C and bool conversion is C99 not C89*/

    if(pointer_lhs_valid || void_pointer_lhs_valid ||
       both_are_arithmetic) {
      right_node = create_auto_cast_node(left_type,
					 right_node,
					 &right_type, true,
					 false);
    } else {
      type_error_fcn("RHS cannot be converted to LHS",
		     right_node->line_number);
    }
  }

  return right_node;
}


void type_convert_assignment(struct node *binary_operation) {
  struct node *right_node, *left_node;
  struct type* left_type;
  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  assert(BINOP_ASSIGN == binary_operation->data.binary_operation.
	 operation);

  left_node = binary_operation->data.binary_operation.
    left_operand;
  right_node = binary_operation->data.binary_operation.
    right_operand;

  if(!is_lvalue_operand(left_node)) {
      type_error_fcn("need lvalue on the LHS",
		     binary_operation->line_number);
      return;
    }

  left_type = get_result_lazy(left_node);
  right_node = assignment_conversion_helper(left_type, right_node);

  /*???ASSUMPTION - lhs doesn't need any casting???*/
  binary_operation->data.
    binary_operation.right_operand = right_node;

  binary_operation->data.binary_operation.result.type =
    get_result_lazy(left_node);
}

static struct node* unary_usual_conversion_helper(struct node* operand_node,
						  struct type** operand_type_ptr,
						  bool special_casting)
{
  struct type* operand_type = NULL;
  struct type* target_type = NULL;

  operand_type = get_result_lazy(operand_node);

  /*Valid conversions for type
   1. array of T => pointer of T
   2. any integer type of rank < rank of int  => int
   (ignoring representability by signed/unsigned)
   3. any integer type with rank > rank(int) => (No conversion)
  */

  /*Checking against signed int to see if any
    scalar expansion is needed*/
  target_type = type_basic(false, TYPE_WIDTH_INT);

  /* Function call identifier
     will have to be replaced by the return type*/
  if(TYPE_FUNCTION == operand_type->kind)
      operand_type = operand_type->data.function.return_type;

  operand_node =
    replace_array_subscripted_with_ptr_type_if_needed(&operand_type,
						      operand_node);

  if(TYPE_VOID == operand_type->kind) {
    type_error_fcn("Void types cannot be part of any operation",
		   operand_node->line_number);
  }

  if(type_is_arithmetic(target_type) &&
     type_is_arithmetic(operand_type)){
    /*Check the rank value*/

    int operand_rank = type_conversion_rank(operand_type);
    int target_rank = type_conversion_rank(target_type);

    /*Only cast if arithmetic type is smaller than int type*/
    if(operand_rank < target_rank) {
      /*Unsigned /signed choices??*/
      operand_node = create_auto_cast_node
	(target_type, operand_node, &operand_type, true,
	 special_casting);
    }
  }

  *operand_type_ptr = operand_type;
  return operand_node;
}

void type_assign_in_unary_operation(struct node *unary_operation)
{
  bool special_casting = false;
  struct node* operand =
    unary_operation->data.unary_operation.operand;
  struct type* operand_type = NULL;
  /*Convert all types to int*/
  type_assign_in_expr(operand);

  assert(NODE_UNARY_OPERATION == unary_operation->kind);

  switch(unary_operation->data.
	 unary_operation.operation) {
  case UNARY_INDIRECTION: {
    if(TYPE_POINTER != get_result_lazy(operand)->kind) {
      type_error_fcn
	("Indirection can only be called on pointer types",
	 unary_operation->line_number);
    }
    break;
  }
  case UNARY_ADDRESS: {
    if(!is_lvalue_operand(operand)) {
	type_error_fcn("Operand should be lvalue",
		       unary_operation->line_number);
    }	
    break;
  }

  case UNARY_PRE_INC:
  case UNARY_PRE_DEC:
  case UNARY_POST_INC:
  case UNARY_POST_DEC:
    special_casting = true;
    break;
  default:
    special_casting =false;
    break;
  }

  operand =
    unary_usual_conversion_helper(operand, &operand_type,
				  special_casting);
  {
    struct type* unary_type = operand_type;
    switch(unary_operation->data.unary_operation.operation)
      {
	/*Types are changed in the following*/
      case UNARY_INDIRECTION: {
	if(TYPE_POINTER != operand_type->kind)
	  type_error_fcn("Operand should be pointer type",
			 unary_operation->line_number);
	unary_type = operand_type->data.pointer.pointer_to;
	break;
      }
      case UNARY_ADDRESS: {
	unary_type = type_pointer(operand_type);
	break;
      }
      default:
      break;
      }
   
  /*If it is pointer type output => pointer arithmetic involved
    which is allowed only for addition operations*/
  /* if(TYPE_POINTER == unary_type->kind) */
  /*   { */
  /*     if( (UNARY_POST_INC != */
  /* 	   unary_operation->data.unary_operation.operation) && */
  /* 	  (UNARY_PRE_INC != */
  /* 	   unary_operation->data.unary_operation.operation) ) */
  /* 	type_error_fcn */
  /* 	  ("Only post and preincrement allowed in pointer arithmetic", */
  /* 	   right_node->line_number); */
  /*   } */

    unary_operation->data.unary_operation.result.type = unary_type;
    unary_operation->data.unary_operation.operand = operand;
  }
}

/* void type_assign_in_type_name(struct node* node, */
/* 			      struct type* type ) */
/*  {  */
/*    assert(0);/\*Not implemented yet*\/  */
/*  } */

/* void type_assign_in_type(struct node* node, */
/* 				  struct type* type ) */
/*  {  */
/*    assert(0);/\*Not implemented yet*\/  */
/*  } */

/* void type_assign_in_subscript_expr(struct node* node) */
/*  { */
/*    struct type* array_type, *ptr_type, *index_type; */
/*    assert(NODE_SUBSCRIPT_EXPR == node->kind); */
/*    /\*Make sure you dereference the node */
/*      and therefore set the type to the */
/*      one pointed to by the *\/ */
/*    array_type = get_result_lazy(node->data.subscript_expr. */
/* 				postfix_expr); */
   
/*      replace_array_subscripted_with_ptr_type_if_needed(array_type, */
/* 						     &node); */

/*    if(TYPE_POINTER != ptr_type->kind) { */
/*      type_error_fcn("Subscripting done only pointer type", */
/* 		    node->line_number); */
/*    } */

/*    index_type = get_result_lazy(node->data.subscript_expr.expr); */
/*    /\*make sure the expr evaluates to a number type *\/ */
/*    if(!type_is_arithmetic(index_type)){ */
/*     type_error_fcn("indexing of array needs to be a number type", */
/* 		    node->line_number); */
/*    } */

/*    /\*equivalent to casting to pointer, offseting and dereferencing*\/ */
/*    node->data.subscript_expr.result.type = */
/*      ptr_type->data.pointer.pointer_to; */
/*  } */

void type_assign_in_function_call(struct node* node )
 {
   struct node* function_name = NULL;
   struct node* args_list = NULL;
   struct type* fn_type = NULL;
   struct symbol_list* params_list = NULL;

   assert(NODE_FUNCTION_CALL == node->kind);

   function_name = node->data.function_call.postfix_expr;
   assert(NODE_IDENTIFIER == function_name->kind &&
	  NULL != function_name->data.identifier.
	  symbol->result.type);

   /* Check the parameters against the for each parameter */
   args_list = node->data.function_call.expression_list;
   fn_type = function_name->data.identifier.symbol->result.type;
   params_list = fn_type->data.function.params_list;

   if(NULL != args_list) {
     /*This is to keep the linked list needed for the
       argument list*/
     struct node* top_of_mod_args_list = NULL;

     /* Params list */
     /* Traverse through the list and match one by one */
     while(NULL != params_list  &&
	   NODE_EXPRESSION_LIST == args_list->kind) {
       struct node* expr = args_list->data.expr_list.
	 assignment_expr;

       /*??? Should this be using binary approach instead ???*/
       expr = assignment_conversion_helper (params_list->symbol.
					    result.type, expr);
       /**/
       if(!top_of_mod_args_list)
	 top_of_mod_args_list = args_list;

       /*Traverse to next node*/
       args_list->data.expr_list.assignment_expr = expr;
       args_list = args_list->data.expr_list.list;
       params_list = params_list->next;
     }

     assert(NULL != args_list);
     /*Run for the last argument in list or the only argument*/

     if(NULL != params_list) {
       struct node* expr = args_list;
       assert(NODE_EXPRESSION_LIST != args_list->kind);


       /*??? Should this be using binary approach instead ???*/
       expr = assignment_conversion_helper (params_list->symbol.
					    result.type, expr);

       /*If it is the only argument in the list*/
       if(!top_of_mod_args_list)
	 top_of_mod_args_list = args_list;
       args_list = NULL; /*Terminate the list - will
			   be used for param length matching*/
       params_list = NULL;
     }

     if(NULL != params_list || NULL != args_list) {

       if(NULL != args_list && NODE_TYPE != args_list->kind &&
	  TYPE_VOID != args_list->data.type.typeId)
	 type_error_fcn
	   ("Mismatch in number of parameters declared and used",
	    node->line_number);
     }
     /*Assign the nodes back to function call node*/
     node->data.function_call.
       expression_list = top_of_mod_args_list;
   }

   /*assign return type information to the node so that
     it can be used by the AST in type checking*/
   if(NULL == node->data.function_call.result.type) {
     node->data.function_call.result.type = fn_type;
   }
 }

void type_assign_in_comma_expr(struct node* node )
 {
   /*Use only the last expr in comma_expr*/
   assert(NODE_COMMA_EXPR == node->kind);

   type_assign_in_expr(node->data.expr_list.assignment_expr);
 }

void type_assign_in_cast_expr(struct node* node )
 {
   struct type* type = NULL, *expr_type = NULL;
   bool valid_cast = false;

   assert(NODE_CAST == node->kind);
   type = get_type_from_type_name(node->data.cast.type);

   /*Valid type-casts
     1. All Arithmetic to arithmetic
     2. pointer to integer
     3. pointer  (from void*, integer, other pointers)
     4. void  (from any type)

     cannot cast to array or function
    */
   expr_type = get_result_lazy(node->data.cast.expr);

   {
     bool both_src_dest_arithmetic =
       (type_is_arithmetic(expr_type) && type_is_arithmetic(type));

     bool src_pointer_dest_int =
       (type_is_arithmetic(type) &&
	(TYPE_POINTER == expr_type->kind));

     bool src_many_dest_pointer =
       ( (TYPE_POINTER == type->kind) &&
	 ( (TYPE_POINTER == expr_type->kind) ||
	  type_is_arithmetic(expr_type))
	 );

     bool void_dest = (TYPE_VOID == type->kind);

     valid_cast = void_dest || src_pointer_dest_int ||
       both_src_dest_arithmetic || src_many_dest_pointer;
   }
   if(!valid_cast)
     {
       type_error_fcn("Invalid cast usage", node->line_number);
       return;
     }

   node->data.cast.result.type = type;
}

/* void type_assign_in_void (struct node* node, */
/* 			  struct type* type) */
/*  { assert(0);/\*Not implemented yet*\/ } */

/* void type_assign_in_identifier(struct node* node, */
/* 			       struct type* type ) */
/*  { assert(0);/\*Not implemented yet*\/ } */

/* void type_assign_in_number(struct node* node, */
/* 			   struct type* type ) */
/*  { assert(0);/\*Not implemented yet*\/ } */

/* void type_assign_in_string(struct node* node, */
/* 			   struct type* type ) */
/*  { assert(0);/\*Not implemented yet*\/ } */
/* void type_check(struct type* type1, */
/* 		struct type* type2 ) */
/*  { assert(0);/\*Not implemented yet*\/ } */

void type_assign_in_binary_operation(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  type_assign_in_expr(binary_operation->data.binary_operation.left_operand);
  type_assign_in_expr(binary_operation->data.binary_operation.right_operand);

  switch (binary_operation->data.binary_operation.operation) {
  case BINOP_MULTIPLICATION:
  case BINOP_DIVISION:
  case BINOP_ADDITION:
  case BINOP_SUBTRACTION:
  case BINOP_REMAINDER:
  case BINOP_AND:
  case BINOP_OR:
  case BINOP_BITWISE_OR:
  case BINOP_BITWISE_XOR:
  case BINOP_BITWISE_AND:
  case BINOP_EQUALITY:
  case BINOP_INEQUALITY:
  case BINOP_BITSHIFT_RIGHT:
  case BINOP_BITSHIFT_LEFT:
  case BINOP_LESS:
  case BINOP_LESS_EQUAL:
  case BINOP_GREATER:
  case BINOP_GREATER_EQUAL:
    type_convert_usual_binary(binary_operation, false);
    break;

  case BINOP_ADD_ASSIGN:
  case BINOP_SUBTRACT_ASSIGN:
  case BINOP_MULTIPLY_ASSIGN:
  case BINOP_DIVIDE_ASSIGN:
  case BINOP_REMAINDER_ASSIGN:
  case BINOP_BITSHIFT_LEFT_ASSIGN:
  case BINOP_BITSHIFT_RIGHT_ASSIGN:
  case BINOP_AND_ASSIGN:
  case BINOP_XOR_ASSIGN:
  case BINOP_OR_ASSIGN:
    type_convert_usual_binary(binary_operation, true);
    break;/*Special casting for LHS*/

  case BINOP_ASSIGN:
    type_convert_assignment(binary_operation);
    break;

  default:
    assert(0);
    break;
  }

}

/* void type_assign_in_cast_operation(expression) */
/* { */
/*   assert(0);/\*Unimplemented*\/ */

/* } */

static void type_assign_in_expression_statement(struct node *expression_statement) {
  assert(NODE_EXPRESSION_STATEMENT == expression_statement->kind);
  type_assign_in_expr(expression_statement->data.expression_statement.expression);
}

void type_assign_in_statement_list(struct node *statement_list) {
  assert(NODE_DECL_OR_STATEMENT_LIST == statement_list->kind);
  if (NULL != statement_list->data.decl_or_statement_list.init) {
    type_assign_in_statement_list
      (statement_list->data.decl_or_statement_list.init);
  }
  type_assign_in_expression_statement
    (statement_list->data.decl_or_statement_list.statement);
}
/**********************************************************/

static void type_assign_in_return(struct node* node,
				  const struct type* return_type)
{
  struct type* operand_type = NULL;
  struct node* operand_node = NULL;
  assert(NODE_RETURN == node->kind);

  operand_node = node->data.jump.location;
  if( NULL != node->data.jump.location )
    operand_type = get_result_lazy(operand_node);
  else
    operand_type = type_void();

  /*All array types should be converted to pointer type
   and subscript expr should also be handled by this point*/
  assert(TYPE_ARRAY != operand_type->kind &&
	 TYPE_FUNCTION != operand_type->kind);

  /*If void types, match exactly*/
  /*otherwise apply assignment_conversion function*/
  /*???To avoid warnings, removed const qualifier???*/
  if(!type_is_equal(operand_type, (struct type*)return_type))  {
    /*Try conversion to target type by assignment rules*/
    struct type* void_type = type_void();
    if(!type_is_equal((struct type*)return_type, void_type)) {
      operand_node = assignment_conversion_helper((struct type*)return_type,
					   operand_node);
    } else {
      type_error_fcn("Return type should be void",
		   operand_node->line_number);
    }
  }
}

static void type_assign_in_iterative(struct node* node,
				     const struct type*
				     return_type){
  switch(node->kind){
  case NODE_FOR_EXPR:
    if( NULL != node->data.for_loop_expr.initial_clause )
      type_assign_in_expr(node->data.for_loop_expr.initial_clause);
    if( NULL != node->data.for_loop_expr.term_expr)
      type_assign_in_expr(node->data.for_loop_expr.term_expr);
    if( NULL != node->data.for_loop_expr.inc_expr)
      type_assign_in_expr(node->data.for_loop_expr.inc_expr);
    if(NULL != node->data.for_loop_expr.statement)
      type_assign_in_statement(node->data.for_loop_expr.statement,
			       return_type);
    break;

  case NODE_WHILE:
    type_assign_in_expr(node->data.condn_loop.condn);
    if(NULL != node->data.condn_loop.statement)
      type_assign_in_statement(node->data.condn_loop.statement,
			       return_type);
    break;

  case NODE_DO_WHILE:
    type_assign_in_expr(node->data.condn_loop.condn);
    if(NULL != node->data.condn_loop.statement)
      type_assign_in_statement(node->data.condn_loop.statement,
			       return_type);
    break;

  default:
    assert(0); break;
  }
}

static void type_assign_in_conditional(struct node* node,
				       const struct type*
				       return_type){
  switch(node->kind){
  case NODE_IF:
    type_assign_in_expr(node->data.if_else.condn);
    type_assign_in_statement(node->data.if_else.expr[0],
			     return_type);
    break;

  case NODE_IF_ELSE:
    type_assign_in_expr(node->data.if_else.condn);
    type_assign_in_statement(node->data.if_else.expr[0],
			     return_type);
    type_assign_in_statement(node->data.if_else.expr[1],
			     return_type);
    break;

  case NODE_IF_ELSE_TRENARY:
    {
      struct node* expr1 = NULL, *expr2 = NULL;
      struct type* return1 = NULL, *return2 = NULL;
      struct type* final_type = NULL;
      type_assign_in_expr(node->data.if_else_trenary.
				operand[2]);
      expr1 = node->data.if_else_trenary.operand[0];
      expr2 = node->data.if_else_trenary.operand[1];

      return1 = get_result_lazy(expr1);
      return2 = get_result_lazy(expr2);

      final_type = return1;
      /*Will be reassigned if the two types are not equivalent*/
      if(!type_is_equal(return1, return2)){
	/*If unequal pointer types used - it will error out in
	  usual_binary_conversion currently because it
	  can't cast between two non-arithmetic types*/
	final_type =
	  usual_binary_conversion_helper(&expr1, &expr2, false);
      }

      node->data.if_else_trenary.result.type = final_type;
      node->data.if_else_trenary.operand[0] = expr1;
      node->data.if_else_trenary.operand[1] = expr2;
      break;
    }
    default:
      assert(0); break;
  }
}

static void type_assign_in_expr(struct node* node){
  switch(node->kind){
  case NODE_BINARY_OPERATION:
    type_assign_in_binary_operation(node);
    break;
  case NODE_UNARY_OPERATION:
    type_assign_in_unary_operation(node);
    break;
  case NODE_SUBSCRIPT_EXPR: /*Replaced with pointer arithmetic*/
    assert(0);/* type_assign_in_subscript_expr(node); */
    break;
  case NODE_FUNCTION_CALL:
    type_assign_in_function_call(node);
    break;
  case NODE_COMMA_EXPR:
    type_assign_in_comma_expr(node);
    break;
  case NODE_CAST:
    type_assign_in_cast_expr(node);
    break;

  case NODE_IF_ELSE_TRENARY:
    type_assign_in_conditional(node, NULL);break;
    /*ASSUMPTION- node_label not considered here and rest
      should already have types assigned to them
      from symbol_table*/
  case NODE_IDENTIFIER:
    assert(NULL != node->data.identifier.symbol->result.type);
    break;
    /*In my implementation, type is already assigned
      for number and string in lexical analysis stage*/
  case NODE_NUMBER:
    assert(NULL != node->data.number.result.type);
    break;
  case NODE_STRING:
    assert(NULL != node->data.string.result.type);
    break;

    /*Do nothing*/
  case NODE_DECL:
  case NODE_PRM_LIST:
  case NODE_PARAMETER_DECL:
    break;

  /* case NODE_TYPE_NAME:/\*It shouldn't get here if node_cast handles it *\/ */
  /*   type_assign_in_type_name(node); */
  /*   break; */

  /* case NODE_TYPE: */
  /*   type_assign_in_type(node); */
  /*   break; */

  default: assert(0); break;
  }
}
static void type_assign_in_labeled_statement(struct node* node,
					     const struct
					     type* return_type)
{
  struct node* statement = NULL;
  assert(NODE_LABELED_STATEMENT == node->kind);
  if (NULL != node->data.labeled_statement.label)
    type_assign_in_statement(node->data.labeled_statement.statement, 
			     return_type);

}
static void type_assign_in_compound_statement(struct node* node,
					      const struct
					      type* return_type){
  struct node* decl_or_statement_list = NULL;
  assert(NODE_COMPOUND_STATEMENT == node->kind);

  /* Recursive type assignment in the statements */
  decl_or_statement_list = node->data.compound_statement.
    decl_or_statement_list;

  if (NULL != decl_or_statement_list) {
    struct node* list = decl_or_statement_list;

    while(list != NULL){
      struct node* stmt = list->data.decl_or_statement_list.
	statement;
      type_assign_in_statement(stmt, return_type);
      list = list->data.decl_or_statement_list.init;
    }
  }
}

/* Return type passed in as there could be a return from
   any statement */
static void type_assign_in_statement(struct node* node,
				     const struct type*
				     return_type){
  assert(NULL != return_type);

  /*ASSUMPTION - Caller takes care of NULL statement*/
  switch(node->kind){
  case NODE_LABELED_STATEMENT: 
    type_assign_in_labeled_statement(node, return_type);break;
  case NODE_COMPOUND_STATEMENT:
    type_assign_in_compound_statement(node, return_type); break;

    /*Conditional expression*/
  case NODE_IF:
  case NODE_IF_ELSE:
    type_assign_in_conditional(node, return_type); break;

    /*iterative statements*/
  case NODE_WHILE:
  case NODE_DO_WHILE:
  case NODE_FOR_EXPR:
    type_assign_in_iterative(node, return_type); break;

    /*keyword statements*/
  case NODE_BREAK:
  case NODE_CONTINUE:
  case NODE_GOTO: break;

  case NODE_RETURN:
    type_assign_in_return(node, return_type);break;

  default:
    type_assign_in_expr(node);break;
    /*To cover the outcome of expression_statement->comma_expr/assignment_expr*/
  }
}

void type_assign_in_translation_unit(struct node* root_node){
  struct node* child_node;
  assert(root_node != NULL &&
	 NODE_TRANSLATION_UNIT == root_node->kind);

  if (NULL != root_node->data.translation_unit.list){
    type_assign_in_translation_unit
      (root_node->data.translation_unit.list);
  }

  child_node = root_node->data.translation_unit.top_level_decl;
  /*Declarators don't need to be handled for type definition*/
  if (NULL != child_node &&
      NODE_FUNCTION_DEFN == child_node->kind){
    if (NULL != child_node->data.function_defn.body){

      struct type* fn_type = child_node->data.function_defn.
	declarator->data.function_defn_specifier.type;
      struct type* return_type = NULL;

      assert(TYPE_FUNCTION == fn_type->kind);
      return_type = fn_type->data.function.return_type;

      /*Pass the function return type along with function body*/
      type_assign_in_statement(child_node->data.function_defn.body,
			       return_type);
    }
  }
}
