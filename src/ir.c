#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "symbol.h"
#include "type.h"
#include "ir.h"


int ir_generation_num_errors;
static bool main_function_found = false;/*Flag to keep track of functions encountered*/
static int register_counter;

struct string_table string_table;/*Kept here for now*/
/******************************
Prototypes
******************************/

void ir_generate_expression(struct node *expression);
static void ir_generate_statement
(struct node *expression, const char* fn_exit_label, 
 const char* innermost_scope_top, 
 const char* innermost_scope_bottom, const char* fn_name);
static int ir_type_width(struct type* type);
/******************************/
static int get_array_size(struct type* type)
{
  assert(TYPE_ARRAY == type->kind);
  return type->data.array.num_elements*
    ir_type_width(type->data.array.array_of_type);
}

static int ir_type_width(struct type* type)
{
  if(TYPE_POINTER == type->kind)
    return TYPE_WIDTH_INT;/*_INT*/
  else if(TYPE_ARRAY == type->kind)
    return get_array_size(type);
  else if (TYPE_BASIC == type->kind)
    return (type->data.basic.width < TYPE_WIDTH_INT)?
      type->data.basic.width:TYPE_WIDTH_INT;

  assert(0);/*Shouldn't get here*/
  return (-1);
}

static int offset_get_set_table(struct symbol_table* symbol_table, 
				int prev_offset ) {
  /*Traverse the list of symbols and add offset information 
    for all identifiers in the compound_statement of 
    function_symbol_table*/
  struct symbol_list* curr_var = symbol_table->variables;

  /*Passed in parameters treated like local variables here*/
  while(NULL != curr_var){

    struct type* curr_type = curr_var->symbol.result.type;
    /*All types - pointers and arrays other than basic char 
      and short type need word alignment*/
    int alignment_mult = TYPE_WIDTH_INT;

    if(TYPE_BASIC == curr_type->kind)
      alignment_mult = (curr_type->data.basic.width 
			< TYPE_WIDTH_INT)?
	curr_type->data.basic.width:TYPE_WIDTH_INT;

    /*Apply alignment operation irrespective of aligned or not. 
      If already aligned, shall result in no change in offset*/
    /*Set offset by alignment function 
      ((addr+ multiple-1)/multiple)*multiple*/

    /*Update the offset with current type width */
    if(!curr_var->symbol.is_fn_param ){
      prev_offset = 
	( (prev_offset + alignment_mult - 1)/
	  alignment_mult)*alignment_mult;
      curr_var->symbol.offset = prev_offset;
      prev_offset += ir_type_width(curr_type);
    } /* else { */
    /*   prev_offset += TYPE_WIDTH_INT; */
    /*   /\*Word width for arguments as stored on registers*\/ */
    /* } */

    curr_var = curr_var->next;
  }
  
  return prev_offset;
}

static int traverse_symbol_tabel_children_for_offset
(struct symbol_table_list* symbol_table_list, int prev_offset){
  struct symbol_table_list* curr_table = NULL;
  int max_offset = prev_offset;
  if(NULL != symbol_table_list) {
    /*Get the offsets for every symbol and 
      pick the max offset found*/
    curr_table = symbol_table_list;
    
    while(NULL != curr_table) {

      int tmp_offset = offset_get_set_table
	(curr_table->symbol_table, prev_offset);
      /*Check for nested blocks offset also*/
      tmp_offset = traverse_symbol_tabel_children_for_offset
	(curr_table->symbol_table->children, tmp_offset);
      
      if(tmp_offset > max_offset)
	max_offset = tmp_offset;
      
      /*Traverse to the next table in the list*/
      curr_table = curr_table->next;
    }
  }

  return max_offset;
}

static int traverse_fn_symbol_table_for_maximum_offset
(struct symbol_table* symbol_table){
  /* Use the list of parameters for static overhead if needed */
  int prev_offset = 0;
  int fn_offset = offset_get_set_table(symbol_table, prev_offset);

  /* Traverse the children to find the update the function offset*/
  fn_offset = traverse_symbol_tabel_children_for_offset
    (symbol_table->children, fn_offset);

  /*!!!May need an assertion to check the offsets are less 
    than equal to the maximum set on the function!!!*/

  /*Caller can set the function offset on the symbol table*/
  return fn_offset;
}
/***************************************************************/
void ir_error_fcn(const char* msg, int line_no)
{
  printf(" IR FATAL ERROR : %d : %s\n",line_no, msg);
  ir_generation_num_errors++;
  exit(1);
}
/*String to label mapping*/
void add_to_global_string_repository(const char* string_label, 
				     const char* string,
				     int strlen)
{
  struct _global_string_list* curr_string_node = NULL;
  if (NULL == global_string_repository.list) {
    curr_string_node = 
      malloc(sizeof(struct _global_string_list));
    global_string_repository.list = curr_string_node;
  } else {
    /*Traverse the list*/
    struct _global_string_list* prev_string_node;
    curr_string_node = global_string_repository.list;

    while (curr_string_node != NULL) {  
      bool same_label = (0 == strcmp (string_label, 
				      curr_string_node->label));
      bool same_string = (0 == strncmp (string, 
					curr_string_node->string,
					strlen));
      /*Return if the label with same string already 
	registered*/
      if(same_label) { assert(same_string); return; }
	
      prev_string_node = curr_string_node;
      curr_string_node = curr_string_node->next;
    }
      
    /*If end reached, allocate a new node at tail of list 
      and update curr_string_node*/
    curr_string_node = malloc(sizeof(struct _global_string_list));
    prev_string_node->next = curr_string_node;
  }
    
  /*If not found or was the first time entry was made*/
  curr_string_node->strlen = strlen;
  curr_string_node->string = string;
  curr_string_node->label = string_label;
  curr_string_node->next = NULL;
}

static void add_to_global_decl_list(const char* name, 
				    const char* type_name,
				    int space)
{
  struct _global_decl_list* curr_decl = NULL;
  if (NULL == global_decl_list.list) {
    curr_decl = malloc(sizeof(struct _global_decl_list));
    global_decl_list.list = curr_decl;
  } else {
    struct _global_decl_list* prev_decl = NULL;
    curr_decl = global_decl_list.list;

    while(curr_decl != NULL)
      {
	bool same_var = (0 == strcmp (name, curr_decl->name));
	assert(!same_var);/*Cannot have multiple declarations*/
	
	prev_decl = curr_decl;
	curr_decl = curr_decl->next;
      }
    /*If end reached, allocate a new node at tail of list 
      and update curr_decl*/
    curr_decl = malloc(sizeof(struct _global_decl_list));
    prev_decl->next = curr_decl;
  }
  
  curr_decl->name = name;
  curr_decl->type_name = type_name;
  curr_decl->space = space;
  curr_decl->next = NULL;
}


/*Register counting*/
int get_register_count()
{
  int tmp = register_counter;
  register_counter++;
  return tmp;
}

/* void reset_register_counter() */
/* { */
/*   register_counter = 0; */
/* } */
/************************
 * CREATE IR STRUCTURES *
 ************************/
static const char* generate_function_label() {
  static int count = 0;
  static char gen_label_tmpl[] = "_Generated_Function_Label";
  char* str = malloc(sizeof(char)*32);/*32 chosen by guesstimate*/

  sprintf(str,"%s_%d", gen_label_tmpl, count); count++;
  return str;
}
static const char* generate_user_goto_label(const char* fn_name, const char* label) {
  static char gen_label_tmpl[] = "_User_Label";
  char* str = malloc(sizeof(char)*96);/*32*3 chosen by guesstimate*/

  sprintf(str,"%s_%s_%s", gen_label_tmpl, fn_name, label); 
  return str;
}

static const char* generate_label() {
  static int count = 0;
  static char gen_label_tmpl[] = "_Generated_Label";
  char* str = malloc(sizeof(char)*32);/*32 chosen by guesstimate*/

  sprintf(str,"%s_%d", gen_label_tmpl, count); count++;
  return str;
}

static const char* generate_string_label() {
  static int count = 0;
  static char gen_label_tmpl[] = "_Generated_String_Label";
  char* str = malloc(sizeof(char)*32);/*32 chosen by guesstimate*/

  sprintf(str,"%s_%d", gen_label_tmpl, count); count++;
  return str;
}

/*
 * An IR section is just a list of IR instructions. Each node has an associated
 * IR section if any code is required to implement it.
 */
struct ir_section *ir_section(struct ir_instruction *first, struct ir_instruction *last) {
  struct ir_section *code;
  code = malloc(sizeof(struct ir_section));
  assert(NULL != code);

  assert(last->index > first->index || last == first);
  code->first = first;
  code->last = last;

  return code;
}

struct ir_section *ir_copy(struct ir_section *orig) {
  return ir_section(orig->first, orig->last);
}

/*
 * This joins two IR sections together into a new IR section.
 */
struct ir_section *ir_concatenate(struct ir_section *before, struct ir_section *after) {
  /* patch the two sections together */
  before->last->next = after->first;
  after->first->prev = before->last;

  assert(before->last->index < after->first->index );
  return ir_section(before->first, after->last);
}

static struct ir_section *ir_append(struct ir_section *section,
				    struct ir_instruction *instruction) {
  if (NULL == section) {
    section = ir_section(instruction, instruction);

    /*Where would we run into else if situation??*/
  /* } else if (NULL == section->first || NULL == section->last) { */
  /*   assert(NULL == section->first && NULL == section->last); */
  /*   section->first = instruction; */
  /*   section->last = instruction; */
  /*   instruction->prev = NULL; */
  /*   instruction->next = NULL; */

  }else {
    instruction->next = section->last->next;

    if (NULL != instruction->next) {
      assert(instruction->next->index > instruction->index);
      instruction->next->prev = instruction;
    }

    assert(section->last->index < instruction->index);
    section->last->next = instruction;

    instruction->prev = section->last;
    section->last = instruction;
  }
  return section;
}

/*
 * An IR instruction represents a single 3-address statement.
 */
struct ir_instruction *ir_instruction(int kind) {
  struct ir_instruction *instruction;
  static int index_count = 0;
  instruction = malloc(sizeof(struct ir_instruction));
  assert(NULL != instruction);

  instruction->kind = kind;
  instruction->index = index_count++;
  instruction->next = NULL;
  instruction->prev = NULL;

  return instruction;
}

/* static struct ir_operand* ir_operand(int kind){ */
/*   struct ir_operand* operand; */
/*   operand = malloc(sizeof(struct ir_operand)); */
/*   assert(NULL != operand); */

/*   operand->kind = kind; */
/*   operand->is_lvalue = true; */

/*   return operand; */
/* } */
/*For use by ++ and -- operators*/
static void ir_operand_value
(struct ir_instruction *instruction,
 int position, int value) {
  instruction->operands[position].kind = OPERAND_NUMBER;
  instruction->operands[position].is_lvalue = false;
  instruction->operands[position].data.number = value;
}
static void ir_operand_function
(struct ir_instruction* instruction, int position,  
 const char* name, struct type* type, int offset)
{
  instruction->operands[position].kind = OPERAND_FUNCTION;
  instruction->operands[position].is_lvalue = false;
  instruction->operands[position].data.function.name = name;
  instruction->operands[position].data.function.type = type;
  instruction->operands[position].data.function.local_offset = offset;
}

static void ir_operand_identifier
(struct ir_instruction* instruction, 
 int position,  const char* name, 
 struct symbol* symbol, int offset, bool is_lvalue)
{
  instruction->operands[position].kind = OPERAND_IDENTIFIER;
  instruction->operands[position].is_lvalue = is_lvalue;
  instruction->operands[position].data.identifier.name = name; 
  instruction->operands[position].data.identifier.symbol = symbol;
  instruction->operands[position].data.identifier.offset = offset;
}
static void ir_operand_number(struct ir_instruction *instruction,
			      int position, struct node *number) {
  instruction->operands[position].kind = OPERAND_NUMBER;
  instruction->operands[position].is_lvalue = false;
  instruction->operands[position].data.number = 
    number->data.number.value;
}

static void ir_operand_str(struct ir_instruction *instruction,
				  int position, 
				  const char *identifier)
{
  instruction->operands[position].kind = OPERAND_STR;
  instruction->operands[position].is_lvalue = true;
  instruction->operands[position].data.str = identifier; 
}
static void ir_operand_temporary
(struct ir_instruction *instruction, int position, bool is_lvalue) {
  /* static int next_temporary; */
  instruction->operands[position].kind = OPERAND_TEMPORARY;
  instruction->operands[position].is_lvalue = is_lvalue;
  /* instruction->operands[position].data.temporary = 
     next_temporary++; */
  
  instruction->operands[position].data.temporary = get_register_count();
}

static void ir_operand_copy(struct ir_instruction *instruction,
			    int position,
			    struct ir_operand *operand) {
  instruction->operands[position] = *operand;
}

static struct ir_section* generate_ir_no_operation()
{
  struct ir_instruction* 
    instruction = ir_instruction(IR_NO_OPERATION);
  return ir_section(instruction, instruction);
}


/*******************************
 * GENERATE IR FOR EXPRESSIONS *
 *******************************/
void ir_generate_number(struct node *number) {
  struct ir_instruction *instruction;
  assert(NODE_NUMBER == number->kind);

  instruction = ir_instruction(IR_CONST_INT);
  ir_operand_temporary(instruction, 0, false);
  ir_operand_number(instruction, 1, number);

  number->ir = ir_section(instruction, instruction);
 
  number->data.number.result.ir_operand = &instruction->operands[0];
}

void ir_generate_string(struct node* string_node)
{
  struct ir_instruction *instruction;
  const char* string_label = NULL;

  assert(NODE_STRING == string_node->kind);

  /*Load the reference to the string in a 
    temporary register*/
  instruction = ir_instruction(IR_ADDRESS_OF);
  ir_operand_temporary(instruction, 0, false);

  string_label = generate_string_label();
  ir_operand_str(instruction, 1, string_label);
  
  add_to_global_string_repository(string_label, 
				  string_node->data.string.str,
				  string_node->data.string.strLen);

  string_node->ir = ir_section(instruction, instruction);
  string_node->data.string.result.ir_operand = 
    &instruction->operands[0];
}

static struct ir_section* ir_append_sequence_point(struct ir_section* section)
{
  struct ir_instruction* instruction = NULL;
  assert(NULL != section);
  instruction = ir_instruction(IR_SEQUENCE_POINT);
  section = ir_append(section, instruction);
  return section;
}

void ir_generate_identifier(struct node *identifier) {
  struct ir_instruction *instruction;
  bool variable_is_lvalue = true;
  assert(NODE_IDENTIFIER == identifier->kind);

  /*if array type, set it to be rvalue so that it doesn't get loaded unecessarily*/
  if(TYPE_ARRAY == node_get_result(identifier)->type->kind)
    variable_is_lvalue = false;
 
  /*Load the reference to the identifier in a 
    temporary register*/
  instruction = ir_instruction(IR_ADDRESS_OF);



  ir_operand_temporary(instruction, 0, variable_is_lvalue);
  ir_operand_identifier
    (instruction, 1, identifier->data.identifier.name,
     identifier->data.identifier.symbol,
     identifier->data.identifier.symbol->offset, true);

  identifier->ir = ir_section(instruction,instruction);

  /*Populate the identifier*/
  identifier->data.identifier.symbol->
    result.ir_operand = &instruction->operands[0];

  /*Deep copying the identifier name into the operand also
    Do we need it?*/
  
  assert(NULL != identifier->data.identifier.
	 symbol->result.ir_operand);
}

struct ir_instruction* generate_load_instruction(struct type* type)
{
  struct ir_instruction* instruction = NULL;
  switch (type->kind) {
  case TYPE_BASIC: {
    switch(type->data.basic.width) {
    case TYPE_WIDTH_CHAR:
      instruction =  ir_instruction(IR_LOAD_BYTE);
      break;
    case TYPE_WIDTH_SHORT: 
      instruction = ir_instruction(IR_LOAD_HALF_WORD);
      break;
    case TYPE_WIDTH_INT:
    case TYPE_WIDTH_LONG:
    /* case TYPE_WIDTH_POINTER: /\*Where is this dude being used?*\/ */
      instruction = ir_instruction(IR_LOAD_WORD);
      break;
    default: assert(0);/*Shouldn't get here*/
    }
    break;
  }
  case TYPE_POINTER:
  case TYPE_ARRAY: /*Added on Trial basis as array types are referenced and casted from to pointer type*/
    instruction = ir_instruction(IR_LOAD_WORD);break;
	
  default:
    assert(0);break;
  }
  return instruction;
}
struct ir_operand* ir_transfer_if_lvalue
(struct ir_section** section, struct node* expr)
{
  struct ir_instruction* instruction = NULL;

  /*Could this cause problems with nested types??
   Probably not but keep it in mind*/
  /*In case of a Rvalue already, just return*/
  if(!node_get_result(expr)->ir_operand->is_lvalue) {
    return node_get_result(expr)->ir_operand;
  }
  instruction = ir_instruction(IR_TRANSFER_VALUE);
  
  ir_operand_copy(instruction,1,node_get_result(expr)->ir_operand);
  ir_operand_temporary(instruction, 0, false);

  *section = ir_append(*section, instruction);

  return &instruction->operands[0];

}

struct ir_operand* prepare_pointer_arithmetic_helper
(struct ir_section** section_ptr, struct type * ptr_type, 
 struct ir_operand* other_operand)
{
  int mult_inc = 1;
  struct ir_section* section = *section_ptr;
  assert(TYPE_POINTER == ptr_type->kind);
  /*Get the size of the underlying pointed to type*/
  mult_inc = ir_type_width(ptr_type->data.pointer.pointer_to);
 
  /*multiply the non-pointer part of the arithmetic operation
   with ir_type_width output for pointer type*/
  /*???Would this need sign handling for the non-pointer operand??*/
  {  
    struct ir_instruction* instruction = 
      ir_instruction(IR_CONST_INT);
    ir_operand_value(instruction, 1, mult_inc);
    ir_operand_temporary(instruction, 0, false);
    section = ir_append(section,instruction);
  {
    struct ir_instruction* 
      mult_instruction = ir_instruction(IR_MULT);
    ir_operand_copy(mult_instruction, 1, other_operand);
    ir_operand_copy(mult_instruction, 2, &instruction->operands[0]);
    ir_operand_temporary(mult_instruction, 0, false);
    section = ir_append(section,mult_instruction);
    *section_ptr = section;
    return &mult_instruction->operands[0];
  }
  }
}

void prepare_pointer_arithmetic
(struct ir_section** section_ptr, struct type * left_type,
 struct ir_operand** left_operand_ptr, struct type* right_type,
 struct ir_operand** right_operand_ptr)

{
    if(TYPE_POINTER == left_type->kind)
      *right_operand_ptr = prepare_pointer_arithmetic_helper
	(section_ptr, left_type, *right_operand_ptr);
    else {
      assert(TYPE_POINTER == right_type->kind);
      *left_operand_ptr = prepare_pointer_arithmetic_helper
	(section_ptr, right_type, *left_operand_ptr);
    }
}

struct ir_operand* ir_append_load_if_lvalue
(struct ir_section** section, struct node* expr)
{

  struct type* type = node_get_result(expr)->type;
  struct ir_instruction* instruction = NULL;

  /*Could this cause problems with nested types??
   Probably not but keep it in mind*/
  /*In case of a Rvalue already, just return*/
  if(!node_get_result(expr)->ir_operand->is_lvalue) {
    return node_get_result(expr)->ir_operand;
  }
  instruction = generate_load_instruction(type);
  
  ir_operand_copy(instruction, 1, 
		  node_get_result(expr)->ir_operand);
  ir_operand_temporary(instruction, 0, false);

  *section = ir_append(*section, instruction);

  return &instruction->operands[0];
}

bool needs_lvalue(int operation)
{
  /*Only for unary operations*/
  assert((BINOP_OR_ASSIGN < operation) && 
	 (operation <= UNARY_POST_DEC));

  switch(operation) {
  case UNARY_PRE_INC:
  case UNARY_PRE_DEC:
  case UNARY_ADDRESS:
  case UNARY_POST_INC:
  case UNARY_POST_DEC:
    return true;

  default:
    return false;
  }

}
struct ir_instruction* generate_store_instruction_helper
(struct type* type)
{
  struct ir_instruction* instruction = NULL;
  switch (type->kind)
    {
    case TYPE_BASIC: {
	switch(type->data.basic.width) {
	case TYPE_WIDTH_CHAR:
	  instruction = ir_instruction(IR_STORE_BYTE);
	  break;
	case TYPE_WIDTH_SHORT: 
	  instruction = ir_instruction(IR_STORE_HALF_WORD);
	  break;
	case TYPE_WIDTH_INT:
	case TYPE_WIDTH_LONG:
	  instruction = ir_instruction(IR_STORE_WORD);
	  break;
	default: assert(0);/*Shouldn't get here*/
	}
	break;
      }
    case TYPE_POINTER:
      instruction = ir_instruction(IR_STORE_WORD);break;

    default:
      assert(0);break;
    }
return instruction;
}
struct ir_instruction* generate_store_instruction
(/* struct node* expr, */ 
 struct type* type,
 struct ir_operand* dest_operand,
 struct ir_operand* src_operand)
{
  struct ir_instruction* instruction = NULL;
  /*It should be assigned by this time*/
  assert(NULL != type);
  instruction = generate_store_instruction_helper(type);

  ir_operand_copy(instruction, 0, src_operand);
  ir_operand_copy(instruction, 1, dest_operand);
  return instruction;
}
struct ir_instruction* ir_generate_temporary_copy (struct node* operand)
{
  struct ir_instruction* instruction = ir_instruction(IR_TRANSFER_VALUE);
  ir_operand_copy(instruction, 1, 
		  node_get_result(operand)->ir_operand);
  ir_operand_temporary(instruction, 0, node_get_result(operand)->
		       ir_operand->is_lvalue);
  /*Should be tree for increment and decrement operations*/

  return instruction;
}

struct ir_section* ir_generate_unary_increment_operation
(struct node* unary_operand, bool positive_operation)
{
  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Have to handle the case where the nodes can have cast too??
    It will have to be applied before the addition
    !!!!!!!!!!!!!!!!!!!!!!!!!!
  */
  struct ir_instruction* instruction = NULL, 
    *const_instruction = NULL;
  struct ir_section* section = NULL;
  struct ir_operand* operand = NULL;
  int inc_size = 1;

  /*1. Constant evaluation instruction*/
  const_instruction = ir_instruction(IR_CONST_INT);
  if(TYPE_POINTER == node_get_result(unary_operand)->type->kind) {
    struct type* inner_type = node_get_result(unary_operand)->type->
      data.pointer.pointer_to;
    inc_size = ir_type_width(inner_type);
  }
  
  ir_operand_value(const_instruction,1, inc_size);
  ir_operand_temporary(const_instruction, 0, false);

  section = ir_section(const_instruction, const_instruction);

  /*Addition instruction*/
  operand = ir_append_load_if_lvalue(&section, unary_operand);
  
  if( (NODE_POINTER == unary_operand->kind)
     && 
      (node_get_result(unary_operand)->type->
       data.basic.is_unsigned) ){
    if(positive_operation)
      instruction = ir_instruction(IR_ADDU);
    else
      instruction = ir_instruction(IR_SUBU);
  }else{
    if(positive_operation)
      instruction = ir_instruction(IR_ADD);
    else
      instruction = ir_instruction(IR_SUB);
  }
  ir_operand_temporary(instruction, 0, false);
  ir_operand_copy(instruction, 1, operand); 
  ir_operand_copy(instruction, 2, 
		  &const_instruction->operands[0]); 

  section = ir_append(section, instruction);

  /*4. Generate IR to assign the value of operand 
    to a temporary*/
  /*Assign the value back to the variable*/
  {
    struct ir_instruction* assign_instruction = 
      generate_store_instruction
      (node_get_result(unary_operand)->type, 
       node_get_result(unary_operand)->ir_operand, 
       &instruction->operands[0]);
    section = ir_append(section, assign_instruction);
    instruction = assign_instruction;
  }

  return section;
}

void ir_generate_unary_operation(struct node *unary_operation) {
  struct ir_instruction *instruction = NULL;
  struct ir_section* section = NULL;
  struct ir_operand* ir_operand = NULL;
  struct node* operand = NULL;
  int operation = -1;
  assert(NODE_UNARY_OPERATION == unary_operation->kind);

  operand = unary_operation->data.unary_operation.operand;
  ir_generate_expression(operand);
  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    1. If it is pointer type, with pointer arithmetic, 
    we need to scale the factor by type of the operand node 
    before addition

    2. Make sure the values are converted to rvalue if needed 
    by the operator
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

  operation = unary_operation->data.unary_operation.operation;

  section = ir_copy(unary_operation->data.unary_operation.
		    operand->ir);
  /*Get equivalent instructions*/
  switch(operation)
    {
    case UNARY_NOT:
      /*If not equal to zero, set the output value to zero*/
      ir_operand = ir_append_load_if_lvalue/*ir_append_transfer_if_lvalue*/(&section, operand);
      instruction = ir_instruction(IR_LOGICAL_NOT);
      ir_operand_temporary(instruction, 0, false);
      ir_operand_copy(instruction, 1, ir_operand);
      break;

    case UNARY_BITWISE_NEGATION:
      ir_operand = ir_append_load_if_lvalue/*ir_append_transfer_if_lvalue*/(&section, operand);
      instruction = ir_instruction(IR_NOT);
      ir_operand_temporary(instruction, 0, false);
      ir_operand_copy(instruction, 1, ir_operand);
      break;

      /*not handling cast operations with pre and 
	post increment yet*/

    case UNARY_POST_INC: 
      {
	/*1. Load the value in a separate register before 
	  incrementing it*/
	{
	  struct ir_instruction* 
	    load_instruction = generate_load_instruction
	    (node_get_result(operand)->type);

	  ir_operand_copy(load_instruction, 1, 
			  node_get_result(operand)->ir_operand);
	  ir_operand_temporary(load_instruction, 0, false);

	  section = ir_append(section, load_instruction);

	  /* load_instruction */
	  instruction = load_instruction;
	}
	/*1. Generate IR section for unary increment*/
	section = 
	  ir_concatenate (section, 
			  ir_generate_unary_increment_operation
			  (operand, true));
	break;
      }
      /*Call binary operation for assignment and increment*/
    case UNARY_POST_DEC:
      {
	assert(NODE_CAST != operand->kind);
	/*1. Load the value in a separate register before 
	  incrementing it*/
	{
	  struct ir_instruction* 
	    load_instruction = generate_load_instruction
	    (node_get_result(operand)->type);

	  ir_operand_copy(load_instruction, 1, 
			  node_get_result(operand)->ir_operand);
	  ir_operand_temporary(load_instruction, 0, false);

	  section = ir_append(section, load_instruction);

	  /* load_instruction */
	  instruction = load_instruction;
	}

	section = ir_concatenate(section, ir_generate_unary_increment_operation (operand, false));

	break;
      }
    case UNARY_PRE_INC: {
      assert(NODE_CAST != operand->kind);
      section = ir_concatenate(section,ir_generate_unary_increment_operation(operand, true));

      /*1. Load the value in a separate register after
	incrementing it*/
      {
	struct ir_instruction* 
	  load_instruction = generate_load_instruction
	  (node_get_result(operand)->type);

	ir_operand_copy(load_instruction, 1, 
			node_get_result(operand)->ir_operand);
	ir_operand_temporary(load_instruction, 0, false);

	section = ir_append(section, load_instruction);

	/* load_instruction */
	instruction = load_instruction;
      }

      break;
    }
    case UNARY_PRE_DEC: {
      section = ir_concatenate(section, ir_generate_unary_increment_operation(operand, false));

      assert(NODE_CAST != operand->kind);
      /*1. Load the value in a separate register after
	incrementing it*/
      {
	struct ir_instruction* 
	  load_instruction = generate_load_instruction
	  (node_get_result(operand)->type);

	ir_operand_copy(load_instruction, 1, 
			node_get_result(operand)->ir_operand);
	ir_operand_temporary(load_instruction, 0, false);

	section = ir_append(section, load_instruction);

	/* load_instruction */
	instruction = load_instruction;
      }
      break;
    }
    case UNARY_ADDRESS:
      /*Since the operand has to be an lvalue already, just 
	move it to a rvalue register*/
      instruction = ir_instruction(IR_TRANSFER_VALUE);
      ir_operand = node_get_result(operand)->ir_operand;
      ir_operand_copy(instruction, 1, ir_operand);
      ir_operand_temporary(instruction, 0, false);
      section = ir_append(section, instruction);
      break; /* we need the addressOf variable, 
		which should be in temporary already */

    case UNARY_INDIRECTION:
      /*Pointer type - load word*/
      assert(TYPE_POINTER == node_get_result(operand)->type->kind);
      if(node_get_result(operand)->ir_operand->is_lvalue)
	{
	instruction = ir_instruction(IR_LOAD_WORD);
	ir_operand_temporary(instruction, 0, true);
	ir_operand_copy(instruction, 1, 
			node_get_result(operand)->ir_operand);
	section = ir_append(section, instruction);
      } else {
      	/*If not an lvalue, just continue with existing
      	  register assignment for storing the value on the word*/
      	/*Move value done to avoid refactoring existing code base*/
      	instruction = ir_instruction(IR_TRANSFER_VALUE);
      	ir_operand_temporary(instruction, 0, true);
      	ir_operand_copy(instruction, 1,
      			node_get_result(operand)->ir_operand);
      	section = ir_append(section, instruction);
      }
      break;

    case UNARY_PLUS:
      instruction = ir_instruction(IR_NO_OPERATION);
      section = ir_append(section, instruction);
      break;/*Unary plus does no work*/
      
    case UNARY_MINUS:
      ir_operand = ir_append_load_if_lvalue(&section, operand);

      instruction = ir_instruction(IR_NEGATION);
      ir_operand_temporary(instruction, 0, false);
      ir_operand_copy(instruction, 1, ir_operand);
      section = ir_append(section, instruction);
      break;

    default:assert(0);break;
    }

  /*Store the result at temporary registers*/
    /*May need operation specific work here -as apart from
    unary_address*/

  /* ir_append(unary_operation->ir, instruction); */
  unary_operation->ir = section;
  unary_operation->data.
    unary_operation.result.ir_operand = &instruction->operands[0];
}

bool lhs_need_rvalue(int operation)/*For binary operations*/
{
  assert(operation < UNARY_NOT); /*only binary operations*/
  switch(operation) {
    /*assignment and compound assignment don't 
      need lhs to be rvalue*/
  case BINOP_ASSIGN:
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
    return false;
  default:
        return true;
  }
}

void ir_generate_arithmetic_binary_operation(int kind, struct node *binary_operation) {
  struct ir_instruction *instruction = NULL;
  struct ir_section* section = NULL;
  struct node* left_node = NULL, *right_node = NULL;
  struct ir_operand *ir_left_operand = NULL, 
    *ir_right_operand = NULL;

  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  left_node = binary_operation->data.binary_operation.left_operand;
  right_node = binary_operation->data.binary_operation.right_operand;
  /*AST doesn't change at this stage so no need to reassign it 
    back to binary expression*/
  ir_generate_expression(left_node);
  ir_generate_expression(right_node);
  
  section = left_node->ir;
  section = ir_concatenate(section, right_node->ir);

  /*Assume the values passed in are LValues always
   Change them to rvalues*/
  /*ASSUMPTION, both operands in expression have a value 
    stored in operand[0]. That will be used to create the rvalues */
  /*Right to left associativity*/
  ir_left_operand = ir_append_load_if_lvalue(&section, left_node);
  ir_right_operand = ir_append_load_if_lvalue(&section, right_node);

  if (TYPE_POINTER ==
      node_get_result(binary_operation)->type->kind){
    prepare_pointer_arithmetic
      (&section, node_get_result(left_node)->type,
       &ir_left_operand, node_get_result(right_node)->type,
       &ir_right_operand);
  }

  /*2. Instruction created here*/
  instruction = ir_instruction(kind);
  ir_operand_copy(instruction, 1, ir_left_operand);
  ir_operand_copy(instruction, 2, ir_right_operand);
  ir_operand_temporary(instruction, 0, false);
  section = ir_append(section, instruction);

  binary_operation->ir = section;

  binary_operation->data.binary_operation.
    result.ir_operand = &instruction->operands[0];
}
void ir_generate_simple_assignment(struct node *expr) {
  struct ir_instruction *instruction = NULL;
  struct ir_section* section = NULL;
  struct node *left, *right;
  struct ir_operand* rhs_operand = NULL;
  assert(NODE_BINARY_OPERATION == expr->kind);

  ir_generate_expression(expr->data.binary_operation.right_operand);
  ir_generate_expression(expr->data.binary_operation.left_operand);

  left = expr->data.binary_operation.left_operand;
  right = expr->data.binary_operation.right_operand;

  /*1. Instructions to get the two operands*/
  /*Should we concatenate instead of copy??*/
  section = ir_copy(right->ir);
  section = ir_concatenate(section, left->ir);
   
  /*2. Assign the value*/
  rhs_operand = ir_append_load_if_lvalue(&section, right);

  instruction = generate_store_instruction
    (node_get_result(left)->type, 
     node_get_result(left)->ir_operand, rhs_operand);

  section = ir_append(section, instruction);

  /*4. Load it back into the expr IR*/
  expr->ir = section;
  expr->data.binary_operation.result.ir_operand = 
    &instruction->operands[0];
}

static struct ir_operand* ir_short_circuiting
(struct node* first_condn, bool jump_if_false,
 struct node* second_condn , struct ir_section** sectionPtr)
{
  /*Implement short circuiting behavior*/
  struct ir_section* section = NULL;
  const char* label = generate_label();
  struct ir_operand* first_operand = NULL, 
    *second_operand = NULL;

  ir_generate_expression(first_condn);

  /*0. Evaluate the first condition and check the operand*/
  section = first_condn->ir;
  first_operand = ir_append_load_if_lvalue(&section, first_condn);

  /*1. If condition is true, jump to end label*/
  {
    struct ir_instruction* instruction =
    ir_instruction( (jump_if_false)? IR_GOTO_FALSE:
  		    IR_GOTO_TRUE);
    ir_operand_copy(instruction, 0, first_operand);
    ir_operand_str(instruction, 1, label);
    section = ir_append(section, instruction);
  }
    
  /*2. Following condition contains follow-up instruction*/
  /*3. Follow up instruction stores the value at the
    same operand as first instruction */
  ir_generate_expression(second_condn);
  section = ir_concatenate(section, second_condn->ir);
  second_operand = ir_append_load_if_lvalue(&section,
  					    second_condn);

  /*Load the value from the second operand to the same location
    as the first operand*/
  {
    struct ir_instruction*
      store_instruction = ir_instruction(IR_STORE_WORD);
    ir_operand_copy(store_instruction, 0, second_operand);
    ir_operand_copy(store_instruction, 1, first_operand);
    section = ir_append(section, store_instruction);
  }
  
  /*4. Attach the label instruction*/
  {
    struct ir_instruction* label_ir =
      ir_instruction(IR_LABEL);
    ir_operand_str(label_ir, 0, label);
    section = ir_append(section, label_ir);
  }
  /*5. Append everything in section and return it */
  * sectionPtr = section;    
  return first_operand;
}

static void ir_gen_compound_assignment(struct node* binary_operation)
{
  struct ir_operand* left_operand = NULL, *right_operand = NULL;
  struct ir_section* section = NULL;
  struct ir_instruction* instruction = NULL, 
    *assign_instruction = NULL;
  struct node* left_node = NULL; struct node* right_node = NULL;
  struct node* left_node_underlying = NULL;

  assert(NODE_BINARY_OPERATION == binary_operation->kind);


  /* Get the left operand words and the right operand */
  left_node = binary_operation->data.binary_operation.left_operand;
  right_node = binary_operation->data.binary_operation.right_operand;  
  /*Generate the child nodes expression IR tree*/
  ir_generate_expression(left_node);
  ir_generate_expression(right_node);

  /*Special casting done here*/
  if(NODE_CAST == left_node->kind) {
    left_node_underlying = left_node->data.cast.expr;
  } else {
    left_node_underlying = left_node;
  }

  section = ir_copy(left_node->ir);
  section = ir_concatenate(section, right_node->ir);
  left_operand = ir_append_load_if_lvalue(&section, left_node);
  right_operand = ir_append_load_if_lvalue(&section, right_node);

  if (TYPE_POINTER == node_get_result(binary_operation)->
      type->kind){
    prepare_pointer_arithmetic
      (&section, node_get_result(left_node)->type,
       &left_operand, node_get_result(right_node)->type,
       &right_operand);
  }
  
  /* Pick the arithmetic operation to perform from a switch case */
  switch(binary_operation->data.
	 binary_operation.operation) {
  case BINOP_ADD_ASSIGN:
    {
      if( (TYPE_POINTER == node_get_result(binary_operation)->
	   type->kind) ||
	  (node_get_result(binary_operation)->type->data.basic.
	   is_unsigned) ) {
	instruction = ir_instruction(IR_ADDU);	
      } else {
	instruction = ir_instruction(IR_ADD);
      }
      break;
    }
  case BINOP_SUBTRACT_ASSIGN:
    {
      if( (TYPE_POINTER == node_get_result(binary_operation)->
	   type->kind) ||
	  (node_get_result(binary_operation)->type->data.basic.
	   is_unsigned) ) {
	instruction = ir_instruction(IR_SUBU);
      } else {
	instruction = ir_instruction(IR_SUB);
      }
      break;
    }
  case BINOP_MULTIPLY_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      if( node_get_result(binary_operation)->type->data.basic.
	   is_unsigned ) {
	instruction = ir_instruction(IR_MULTU);
      } else {
	instruction = ir_instruction(IR_MULT);
      }
      break;
    }
  case BINOP_DIVIDE_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      if( node_get_result(binary_operation)->type->data.basic.
	  is_unsigned ) {
	instruction = ir_instruction(IR_DIVU);
      } else {
	instruction = ir_instruction(IR_DIV);
      }
      break;
    }
  case BINOP_REMAINDER_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      if( node_get_result(binary_operation)->type->data.basic.
	  is_unsigned ) {
	instruction = ir_instruction(IR_REMAINDERU);
      } else {
	instruction = ir_instruction(IR_REMAINDER);
      }
      break;
    }
  case BINOP_BITSHIFT_LEFT_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      if( node_get_result(binary_operation)->type->data.basic.
	  is_unsigned ) {
	instruction = ir_instruction(IR_SHIFT_LU);
      } else {
	instruction = ir_instruction(IR_SHIFT_L);
      }
      break;
    }
  case BINOP_BITSHIFT_RIGHT_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      if( node_get_result(binary_operation)->type->data.basic.
	  is_unsigned ) {
	instruction = ir_instruction(IR_SHIFT_LU);
      } else {
	instruction = ir_instruction(IR_SHIFT_L);
      }
      break;
    }
  case BINOP_AND_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      instruction = ir_instruction(IR_AND);
      break;
    }
  case BINOP_XOR_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      instruction = ir_instruction(IR_XOR);
      break;
    }
  case BINOP_OR_ASSIGN:
    {
      assert(TYPE_BASIC == node_get_result(binary_operation)->
	     type->kind);
      instruction = ir_instruction(IR_OR);
      break;
    }
  default:assert(0);break;
  }
  
  /*Check if there is a cast on LHS of the assignment, 
    the special cast needs handling*/
  /*Generate instruction to update the left_operand used
    to point to operand out of casting operation*/

  /* Use a copy of the words obtained and store it back to 
     the original left_operand */
  ir_operand_copy(instruction, 1, left_operand);
  ir_operand_copy(instruction, 2, right_operand);
  ir_operand_temporary(instruction, 0, false);

  section = ir_append(section, instruction);
  
  /*Assign the value to a new temporary operand and use that 
    as IR operand for this operation*/
  assign_instruction = generate_store_instruction
    (node_get_result(left_node_underlying)->type, 
     node_get_result(left_node_underlying)->ir_operand,
     &instruction->operands[0]);

  section = ir_append(section, assign_instruction);

  {
    /*4. Return rvalue of the value assigned*/
    struct ir_instruction *return_instruction = 
      ir_instruction(IR_TRANSFER_VALUE);

    /*Store ordering is reversed*/
    ir_operand_copy(return_instruction, 1, 
		    &assign_instruction->operands[1]);
    ir_operand_temporary(return_instruction, 0, false);

    section = ir_append(section, return_instruction);

    /*Assign it to the node*/
    binary_operation->ir = section;
    binary_operation->data.
      binary_operation.result.ir_operand = &return_instruction->
      operands[0];
  }
}

void ir_generate_binary_operation(struct node *binary_operation) {
  bool is_pointer_arithmetic = (TYPE_POINTER == node_get_result(binary_operation)->type->kind);
  bool is_simple_arithmetic = (TYPE_BASIC == node_get_result(binary_operation)->type->kind);

  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  assert( is_pointer_arithmetic || is_simple_arithmetic );    

  switch (binary_operation->data.binary_operation.operation) {
  case BINOP_MULTIPLICATION:
    assert(!is_pointer_arithmetic);
    if(node_get_result(binary_operation)->type->data.basic.
       is_unsigned) {
      ir_generate_arithmetic_binary_operation
	(IR_MULTU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_MULT, binary_operation);
    }
    break;

  case BINOP_DIVISION:
    assert(!is_pointer_arithmetic);
    if(node_get_result(binary_operation)->type->data.basic.
       is_unsigned) {
      ir_generate_arithmetic_binary_operation
	(IR_DIVU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_DIV, binary_operation);
    }
    break;

  case BINOP_ADDITION:
    if( !is_pointer_arithmetic &&
	(node_get_result(binary_operation)->type->data.basic.
	 is_unsigned) ) {
      ir_generate_arithmetic_binary_operation
	(IR_ADDU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_ADD, binary_operation);
    } 
    break;
      
  case BINOP_SUBTRACTION:
    if( !is_pointer_arithmetic &&
	(node_get_result(binary_operation)->type->data.basic.
	 is_unsigned) ) {
      ir_generate_arithmetic_binary_operation
	(IR_SUBU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_SUB, binary_operation);
    } 
    break;

  case BINOP_ASSIGN:
    ir_generate_simple_assignment(binary_operation);
    break;

  case BINOP_REMAINDER:{
    assert(!is_pointer_arithmetic );
    
    if( node_get_result(binary_operation)->type->data.basic.
	is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_REMAINDERU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_REMAINDER, binary_operation);
    } 
  }
    break;

    /*Logical operations*/
    /*Exhibit short circuiting behavior also*/
    
  case BINOP_AND: {
    struct ir_section *section = NULL;
    struct node* left, *right;
    struct ir_operand* operand;
    /*Check the value*/
    assert(!is_pointer_arithmetic);

    left = binary_operation->data.binary_operation.left_operand;
    right = binary_operation->data.binary_operation.right_operand;

    operand = ir_short_circuiting(left, true, right, &section);
    binary_operation->ir = section;
    binary_operation->data.
      binary_operation.result.ir_operand = operand;
  }

  case BINOP_OR: {
    struct ir_section *section = NULL;
    struct node* left, *right;
    struct ir_operand* operand;
    /*Check the value*/
    assert(!is_pointer_arithmetic);

    left = binary_operation->data.binary_operation.left_operand;
    right = binary_operation->data.binary_operation.right_operand;
    operand = ir_short_circuiting(left, false, right, &section);
    binary_operation->ir = section;
    binary_operation->data.
      binary_operation.result.ir_operand = operand;
    break;
  }
    /*Binary operations*/
  case BINOP_BITWISE_OR: {
    assert(!is_pointer_arithmetic);

    ir_generate_arithmetic_binary_operation(IR_OR, 
					    binary_operation);
    break;
  }

  case BINOP_BITWISE_XOR: {
    assert(!is_pointer_arithmetic);
    ir_generate_arithmetic_binary_operation(IR_XOR, 
					    binary_operation);
    break;
  }
  case BINOP_BITWISE_AND: {
    assert(!is_pointer_arithmetic);
    ir_generate_arithmetic_binary_operation(IR_AND, 
					    binary_operation);
    break;
  }
  case BINOP_BITSHIFT_RIGHT: {
    assert(!is_pointer_arithmetic);
    if( node_get_result(binary_operation)->type->data.basic.
	 is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_SHIFT_RU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_SHIFT_R, binary_operation);
    } 
    break;
  }
   
  case BINOP_BITSHIFT_LEFT: {
    assert(!is_pointer_arithmetic);
    if( 
       node_get_result(binary_operation)->type->data.basic.
	is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_SHIFT_LU, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_SHIFT_L, binary_operation);
    } 
    break;
  }

    /*Relational operation*/   
  case BINOP_EQUALITY: {
    ir_generate_arithmetic_binary_operation
      (IR_SET_ON_EQUALITY, binary_operation);
    break;
  }

  case BINOP_LESS: {
    if( !is_pointer_arithmetic &&
	node_get_result(binary_operation)->type->data.basic.
	is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_LESS_THAN_UNSIGNED, binary_operation);
    } else {
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_LESS_THAN, binary_operation);
    }
    break;
  }

    /*Derivative Relations*/
  case BINOP_INEQUALITY: {
    ir_generate_arithmetic_binary_operation
      (IR_SET_ON_NOT_EQUAL, binary_operation);
    break;
  }

  case BINOP_LESS_EQUAL:{
    if( !is_pointer_arithmetic &&
	node_get_result(binary_operation)->type->data.basic.
	is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_LESS_THAN_EQUAL_UNSIGNED, binary_operation);
    }else{
      ir_generate_arithmetic_binary_operation
      (IR_SET_ON_LESS_THAN_EQUAL, binary_operation);
      break;
    }
  }
  case BINOP_GREATER:{
    if( !is_pointer_arithmetic &&
	node_get_result(binary_operation)->type->data.basic.
	is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_GREATER_THAN_UNSIGNED, binary_operation);
    }else{
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_GREATER_THAN, binary_operation);
      break;
    }
  }

  case BINOP_GREATER_EQUAL:{
    if( !is_pointer_arithmetic && 
	node_get_result(binary_operation)->type->data.basic.
	is_unsigned ) {
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_GREATER_THAN_EQUAL_UNSIGNED, binary_operation);
    }else{
      ir_generate_arithmetic_binary_operation
	(IR_SET_ON_GREATER_THAN_EQUAL, binary_operation);
      break;
    }
  }


    /*Compound assignment*/
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
    ir_gen_compound_assignment(binary_operation); break;
  default:
    assert(0);
    break;
  }
}

/*If expression statement is added back in, it will automatically start working*/
/* void ir_generate_expression_statement(struct node *expression_statement) { */
  /* struct ir_instruction *instruction; */
  /* struct node *expression = expression_statement->data.expression_statement.expression; */
  /* assert(NODE_EXPRESSION_STATEMENT == expression_statement->kind); */
  /* ir_generate_expression(expression); */

  /* instruction = ir_instruction(IR_PRINT_NUMBER); */
  /* ir_operand_copy(instruction, 0, node_get_result(expression)->ir_operand); */

  /* expression_statement->ir = ir_copy(expression_statement->data.expression_statement.expression->ir); */
  /* ir_append(expression_statement->ir, instruction); */
/* } */

/* static void ir_generate_decl(struct node *node){ */
/*   (void) node; assert(0); */
/* } */

static void ir_generate_labeled_statement
(struct node* node, const char* fn_exit_label, 
 const char* innermost_scope_top, 
 const char* innermost_scope_bottom, const char* fn_name){
  struct ir_instruction* instruction = NULL;
  struct node* label = NULL;
  struct ir_section* section = NULL;
  const char* char_label = NULL;
  assert(NODE_LABELED_STATEMENT == node->kind);

  /*1. Generate instruction*/
  label = node->data.labeled_statement.label;
  assert(NODE_LABEL == label->kind);

  instruction = ir_instruction(IR_LABEL);
  char_label = generate_user_goto_label
    (fn_name, label->data.label.label->data.identifier.name);
  ir_operand_str(instruction, 0, char_label);
  section = ir_section(instruction, instruction);

  /*2. Generate the statement following the label*/
  ir_generate_statement(node->data.labeled_statement.statement,
			fn_exit_label, innermost_scope_top,
			innermost_scope_bottom, fn_name);

  /*3.Add instruction and the statement section to node ir section*/
  node->ir = ir_concatenate(section, node->data.labeled_statement.
			    statement->ir);
}

static void ir_generate_if_else_statement
(struct node* node, const char* fn_exit_label, 
 const char* innermost_scope_top, 
 const char* innermost_scope_bottom, const char* fn_name){
  struct ir_section* section = NULL;
  struct node* condn = NULL;
  const char* label = NULL;
  /*1. If statement*/
  assert(NODE_IF == node->kind || NODE_IF_ELSE == node->kind);

  /*Generate IR code for the expression and 
    conditions to be handled*/
  condn = node->data.if_else.condn;
  ir_generate_expression(condn);

  section = ir_copy(condn->ir);
  {
    struct ir_instruction* instruction = 
      ir_instruction(IR_GOTO_FALSE);
    
    label = generate_label();

    ir_operand_copy(instruction, 0,
		    node_get_result(condn)->ir_operand);
    ir_operand_str(instruction, 1, label);
    section = ir_append(section, instruction);
  }

  ir_generate_statement(node->data.if_else.expr[0], 
			fn_exit_label, 
			innermost_scope_top,
			innermost_scope_bottom, fn_name);
  section = ir_concatenate(section, node->data.if_else.expr[0]->ir);

  /*2. Create label - if case false*/
  {
    struct ir_instruction* label_ir = ir_instruction(IR_LABEL);

    ir_operand_str(label_ir, 0, label);
    section = ir_append(section, label_ir);
  }

  /*3. Generate IR for else condn and append to the section*/
  if(NODE_IF_ELSE == node->kind) {
    ir_generate_statement(node->data.if_else.expr[1], 
			  fn_exit_label, 
			  innermost_scope_top,
			  innermost_scope_bottom, fn_name);
    section = ir_concatenate(section, node->data.if_else.expr[1]->ir);
  }

  /*4. Assign section to node*/
  node->ir = section;
}


static void ir_generate_while_statement
(struct node* node, const char* fn_exit_label, 
 const char* fn_name){

  struct node* condn = NULL;
  const char* start_label= NULL, *end_label= NULL;
  struct ir_section* section = NULL;

  assert(NODE_WHILE == node->kind);
  /*1. Generate two labels - start of statements, 
    end of statements*/
  start_label = generate_label();
  end_label = generate_label();

  /*2. Add start label before it*/
  {
    struct ir_instruction* start_label_ir = 
      ir_instruction(IR_LABEL);
    
    ir_operand_str(start_label_ir, 0, start_label);
    section = ir_section(start_label_ir, start_label_ir);
  }


  /*3. Generate IR for condn*/
  {
    condn = node->data.condn_loop.condn;
    ir_generate_expression(condn);
    section = ir_concatenate(section,condn->ir);
  }

  /*4. If the condn is false, jump to end label*/
    {
    struct ir_instruction* instruction = 
      ir_instruction(IR_GOTO_FALSE);
    
    ir_operand_copy(instruction, 0,
		    node_get_result(condn)->ir_operand);
    ir_operand_str(instruction, 1, end_label);
    section = ir_append(section, instruction);
  }

  /*5. Generate IR for statement following the condn*/
    {
      struct node* statements = node->data.condn_loop.statement;
      if(NULL!= statements){
	ir_generate_statement(statements,fn_exit_label, 
			      start_label, end_label, fn_name);
	section = ir_concatenate(section, statements->ir);
      } else {
	section = ir_append(section, 
			    ir_instruction(IR_NO_OPERATION));
      }
    }
  /*6.  Add an uncondition jump to start label */
    {
      struct ir_instruction* instruction = 
	ir_instruction(IR_GOTO);
      ir_operand_str(instruction, 0, start_label);
      section = ir_append(section, instruction);
    }

  /*7.  Add the end label */
    {
      struct ir_instruction* end_label_ir = 
	ir_instruction(IR_LABEL);
      
      ir_operand_str(end_label_ir, 0, end_label);
    section = ir_append(section, end_label_ir);
    }  
    /*8. Append the whole thing to section*/
    node->ir = section;
}

static void ir_generate_do_while_statement
(struct node* node, const char* fn_exit_label, 
 const char* fn_name){
 
  struct node* condn = NULL;
  const char* start_label= NULL, *end_label= NULL;
  struct ir_section* section = NULL;

  assert(NODE_DO_WHILE == node->kind);

  /*1. Generate two labels - start of statements, 
    end of statements*/
  start_label = generate_label();
  end_label = generate_label();

  /*2. Add start label before it*/
  {
    struct ir_instruction* start_label_ir = 
      ir_instruction(IR_LABEL);
    
    ir_operand_str(start_label_ir, 0, start_label);
    section = ir_section(start_label_ir, start_label_ir);
  }

  /* 2. Generate IR for statement following the condn*/
  {
    struct node* statements = node->data.condn_loop.statement;
    if(NULL != statements){
      ir_generate_statement(statements,fn_exit_label, 
			    start_label, end_label, fn_name);
      section = ir_concatenate(section, statements->ir);
    } else {
      section = ir_append(section, 
			  ir_instruction(IR_NO_OPERATION));
    }
  }

  /*3. Generate IR for condn*/
  {
    condn = node->data.condn_loop.condn;
    ir_generate_expression(condn);
    section = ir_concatenate(section,condn->ir);
  }
  /* 4. If the condn is false, jump to end label*/
  {
    struct ir_instruction* instruction = 
      ir_instruction(IR_GOTO_FALSE);
    
    ir_operand_copy(instruction, 0,
		    node_get_result(condn)->ir_operand);
    ir_operand_str(instruction, 1, end_label);
    section = ir_append(section, instruction);
  }
  /* 5.  Add an uncondition jump to start label */
  {
    struct ir_instruction* instruction = 
      ir_instruction(IR_GOTO);
    ir_operand_str(instruction, 0, start_label);
    section = ir_append(section, instruction);
  }

  /* 6.  Add the end label */
  {
    struct ir_instruction* end_label_ir = 
      ir_instruction(IR_LABEL);
      
    ir_operand_str(end_label_ir, 0, end_label);
    section = ir_append(section, end_label_ir);
  }  
  /* 7. Append the whole thing to section*/
  node->ir = section;
}

static void ir_generate_for_expr_statement
(struct node* node, const char* fn_exit_label, const char* fn_name) 
{
  const char* start_label = NULL,*end_label = NULL;
  struct ir_section* section = NULL;
  assert(NODE_FOR_EXPR == node->kind);
  /*1. generate the IR for node conditions*/

  /*2. Generate start and end label*/
  start_label = generate_label();
  end_label = generate_label();

  /*3. print initialization clause and then Print start label*/
  {
    struct node* for_clause = 
      node->data.for_loop_expr.initial_clause;
  
    struct ir_instruction* start_label_ir = NULL;

    /*Label added after the code*/
    if(NULL != for_clause){
      ir_generate_expression(for_clause);
      section = ir_copy(for_clause->ir);
      start_label_ir = ir_instruction(IR_LABEL);
      ir_operand_str(start_label_ir, 0, start_label);
      section = ir_append(section, start_label_ir);
    } else {
      start_label_ir = ir_instruction(IR_LABEL);
      section = ir_section(start_label_ir, start_label_ir);
    }
  }

  /*5. Print the code body IR*/
  {
    struct node* for_stmt = node->data.for_loop_expr.statement;
    if(NULL != for_stmt){
      ir_generate_statement(for_stmt,fn_exit_label, 
			    start_label, end_label, fn_name);
      section = ir_concatenate(section, for_stmt->ir);
    }
  }

  /*6. print the  increment term   */
  {
    struct node* for_inc = node->data.for_loop_expr.inc_expr;
    if(NULL != for_inc){
      
      ir_generate_expression(for_inc);
      section = ir_concatenate(section, for_inc->ir);
    }
  }
  
  {
    /* 4. Conditional goto to end label*/
    struct node* for_term = node->data.for_loop_expr.term_expr;
    struct ir_instruction* end_instruction = NULL;
    if(NULL != for_term){
      ir_generate_expression(for_term);
      section = ir_concatenate(section, for_term->ir);
      
      end_instruction = ir_instruction(IR_GOTO_FALSE);
      ir_operand_str(end_instruction, 1, end_label);
      ir_operand_copy(end_instruction, 0, 
		      node_get_result(for_term)->ir_operand);
      section = ir_append(section, end_instruction); 
    }
  }
  /*5. the unconditional goto to start label*/
  {
    struct ir_instruction* 
      goto_instruction = ir_instruction(IR_GOTO);
    ir_operand_str(goto_instruction, 0, start_label);
    section = ir_append(section, goto_instruction);
  }
  /* 7.end label*/
  {
    struct ir_instruction* 
      label_instruction = ir_instruction(IR_LABEL);
    ir_operand_str(label_instruction, 0, end_label);
    section = ir_append(section, label_instruction);
    
  }

  node->ir = section;
}

static void ir_generate_break_statement
(struct node* node, const char* innermost_scope_bottom) {
  struct ir_section* section = NULL;

  assert(NODE_BREAK == node->kind);
  {
    /*1. Goto instruction for the innermost scope bottom*/
    struct ir_instruction* instruction = 
      ir_instruction(IR_GOTO);
    ir_operand_str(instruction,0, 
			  innermost_scope_bottom);

    /*2. Append IR instruction to section*/
    section = ir_section(instruction, instruction);

  }
  node->ir = section;
}

static void ir_generate_continue_statement
(struct node* node,  const char* innermost_scope_top) {
  struct ir_section* section = NULL;
  assert(NODE_CONTINUE == node->kind);
  {
    /*1. Goto instruction for the innermost scope bottom*/
    struct ir_instruction* instruction = 
      ir_instruction(IR_GOTO);
    ir_operand_str(instruction,0, 
			  innermost_scope_top);

    /*2. Append IR instruction to section*/
    section = ir_section(instruction, instruction);

  }
  node->ir = section;
}

static void ir_generate_if_else_trenary(struct node* node)
{
  struct ir_section *section = generate_ir_no_operation();
  struct node* condn, *stmt1, *stmt2;
  struct ir_operand* operand;
  const char* label;
  assert(NODE_IF_ELSE_TRENARY == node->kind);

  /*Check the value*/

  condn = node->data.if_else_trenary.operand[0];
  stmt1 = node->data.if_else_trenary.operand[1];
  stmt2 = node->data.if_else_trenary.operand[2];

  /*Generate instructions to store*/
  ir_generate_expression(condn);
  ir_generate_expression(stmt1);

  /*Attach label to jump to end of the instruction
   after executing stmt1 and stmt2*/
  {
    struct ir_instruction *goto_ir = ir_instruction(IR_GOTO);
    struct ir_instruction *load_ir = NULL;
    label = generate_label();
    ir_operand_str(goto_ir, 0, label);

    stmt1->ir = ir_append(stmt1->ir, goto_ir);

    /*hack here to get common && , || code working with this*/
    /*the second expr will store its output in operand*/
    operand = ir_short_circuiting (condn, true, stmt1, &section);
    /*Append left over instructions into the section*/
    ir_generate_expression(stmt2);
    section = ir_concatenate(section, stmt2->ir);
    
    load_ir = ir_instruction(IR_STORE_WORD);/*Ignoring type information here*/
    ir_operand_copy(load_ir, 0, node_get_result(stmt2)->ir_operand);
    ir_operand_copy(load_ir, 1, operand);
    
    section = ir_append(section, load_ir);

    /*Create instruction to load information into operand returned*/
    {
      struct ir_instruction *label_ir =ir_instruction(IR_LABEL);
      ir_operand_str(label_ir, 0, label);
      section = ir_append(section, label_ir);
    }

    node->data.if_else_trenary.result.ir_operand = operand;
  }
  node->ir = section;
}

static void ir_generate_return_statement(struct node* node, const char* fn_exit_label){
  struct ir_instruction* instruction = NULL;
  struct ir_section* section = NULL;
  /* struct ir_operand* operand = NULL; */

  /*0. Assert it is return statement*/
  assert(NODE_RETURN == node->kind);

  /*1. Store the value in the result word, get the operand 
    of expression following return keyword*/
  if(NULL != node->data.jump.location){
    /*Check if return value is provided*/
    struct node* return_expr = node->data.jump.location;
    struct ir_instruction* return_instruction = NULL;
    struct ir_operand* return_operand= NULL;
    ir_generate_expression(return_expr);
    section = ir_copy(return_expr->ir);
    return_operand = 
      ir_append_load_if_lvalue(&section, return_expr);

    return_instruction = ir_instruction(IR_RETURN_WORD);
    ir_operand_copy(return_instruction, 0 ,
		    return_operand);
    section = ir_append(section, return_instruction);
  }
  
  /*2. Use the fn_exit_label to define a goto statement 
    to that label*/
  instruction = ir_instruction(IR_GOTO);
  ir_operand_str(instruction, 0,fn_exit_label);

  node->ir = ir_append(section, instruction);
}

static void ir_generate_goto_statement(struct node* node,
				       const char* fn_name){
  /* Assert it is goto statement */
    const char* char_label = NULL;
    struct node* label = NULL;
    struct ir_instruction* instruction = NULL;

    assert(NODE_GOTO == node->kind);

    label = node->data.jump.location;
    
  /*1. Generate user label prefix with fn_name/exit 
    label (that is unique too) for the label  */
    char_label = generate_user_goto_label
      (fn_name, label->data.identifier.name);
    
    instruction = ir_instruction(IR_GOTO);
    ir_operand_str(instruction, 0, char_label);

    /* Check if it is in the symbol table ??*/
    /*2. Add it to the ir section for this instruction*/
    node->ir = ir_section(instruction,instruction);
}

static void ir_generate_comma_expr(struct node* node)
{
  struct ir_section* section = NULL;
  assert(NODE_COMMA_EXPR == node->kind);

  ir_generate_expression(node->data.expr_list.list);
  ir_generate_expression(node->data.expr_list.assignment_expr);
  
  section = ir_copy(node->data.expr_list.list->ir);
  section = ir_concatenate
    (section, node->data.expr_list.assignment_expr->ir);
  
  node->ir = ir_append_sequence_point(section);
}

static struct ir_section* gen_casting_instruction(struct type* dest_type, struct type* src_type, struct node* src_node, struct ir_operand** operand_ptr)
{
  /*Casting split into two parts - one to word width and 
    other from word width to any non-word width specified 
    by destination node*/
  int src_width = ir_type_width(src_type);
  /* bool dest_usigned = (TYPE_POINTER == dest_type->kind) || */
  /*   (dest_type->data.basic.is_unsigned); */
  struct ir_section* section = NULL;
  int dest_width = ir_type_width(dest_type);  
  struct ir_instruction* instruction = NULL;
  
  ir_generate_expression(src_node);
  section = ir_copy(src_node->ir);

  if(src_width != dest_width) {
    struct ir_operand* operand = ir_append_load_if_lvalue
      (&section, src_node);

    /*Cast from src type to word length if needed*/
    if(TYPE_WIDTH_SHORT == src_width){  
      if(type_is_unsigned(src_type))
	instruction = ir_instruction(IR_CAST_HALF_WORD_TO_UWORD);
      else
	instruction = ir_instruction(IR_CAST_HALF_WORD_TO_WORD);
      
      ir_operand_copy(instruction, 1, operand);
      ir_operand_temporary(instruction, 0, false);
      section = ir_append(section,instruction);
      operand = &instruction->operands[0];

    } else if(TYPE_WIDTH_CHAR == src_width){
      if(type_is_unsigned(src_type))
	instruction = ir_instruction(IR_CAST_BYTE_TO_UWORD);
      else
	instruction = ir_instruction(IR_CAST_BYTE_TO_WORD);

      ir_operand_copy(instruction, 1, operand);
      ir_operand_temporary(instruction, 0, false);
      section = ir_append(section,instruction);
      operand = &instruction->operands[0];
    }

    
    /*Cast from word length to dest type if needed*/
    /*If further casting needed for destination, do that*/
    if(TYPE_WIDTH_SHORT == dest_width) {
      instruction = ir_instruction(IR_CAST_WORD_TO_HALF_WORD);
      ir_operand_copy(instruction, 1, operand);
      ir_operand_temporary(instruction, 0, false);
      section = ir_append(section,instruction);

    } else if(TYPE_WIDTH_CHAR == dest_width){
      instruction = ir_instruction(IR_CAST_WORD_TO_BYTE);

      ir_operand_copy(instruction, 1, operand);
      ir_operand_temporary(instruction, 0, false);
      section = ir_append(section,instruction);
    }   
  }
  /*In case the cast is unecessary use the src node ir_operand
    and return the section accumulated so far*/
  if(NULL == instruction){
    *operand_ptr = node_get_result(src_node)->ir_operand;
  } else {
    *operand_ptr = &instruction->operands[0];
  }
  return section;
}
static void ir_generate_cast_expr(struct node* node)
{
  struct ir_section* section = NULL;
  struct ir_operand* operand = NULL;
  struct node* src_node = NULL;
  struct type* src_type=NULL, *dest_type = NULL;
  assert(NODE_CAST == node->kind);

  /* Check for special casing also in the cast, 
     That will need to be handled in case by case basis*/
  
  /* If not special case*/
  src_node = node->data.cast.expr;
  /*1. Check source type width*/
  src_type = node_get_result(src_node)->type;
  
  /*2. Check type width of the node - therefore the output*/
  dest_type = node_get_result(node)->type;
  
  /*3. Create the appropriate instruction*/
  /* bool src_usigned = (TYPE_POINTER == src_type->kind) || */
  /*   (src_type->data.basic.is_unsigned); */
  
  section = gen_casting_instruction(dest_type, src_type,
				    src_node, &operand);
  node->data.cast.result.ir_operand = operand;
  node->ir = section;

  /* } else { */
  /*   /\*just generate the ir for the underlying operands   */
  /*     and leave it to the operations doing the special cast*\/ */
  /*   ir_generate_expression(src_node); */
  /* }   */
}


/*The matched index will be used to get the function pointer 
  code in IR generation stage*/
static bool is_syscall(const char* name, int* matched_index)
{
  int idx = 0;
  static char *sys_fcn_names[6]= {
    "print_int",
    "print_string",
    "read_int",
    "read_string",
    "sbrk",
    "exit"
  };
  for(; idx < 6; idx++) { /*array length is 6*/
    if ( 0 == strcmp(name, sys_fcn_names[idx])){
      *matched_index = idx;
      return true;
    }
  }
  
  return false;
}

static struct ir_section* ir_generate_param_section
(struct node* exp_list)
{
  struct ir_section* param_section = generate_ir_no_operation();
  struct ir_operand* param_operand = NULL;
  struct ir_instruction* param_instruction = NULL;
  struct node* curr_node = NULL;
  int param_idx = -1 ;

  /*0. Get size of expression_list from function type*/
  {
    int param_size = -1;
    struct node* counter_node = exp_list;

    /* node_get_result(expr_list) */
    while(NULL != counter_node &&
	  NODE_EXPRESSION_LIST == counter_node->kind) {
      param_size++;
      counter_node = counter_node->data.expr_list.list;
    }
    if (NULL != counter_node) {
      param_size++;
    }
    param_idx = param_size;
    
  }
  /*1. Load word after address of any parameters*/
  /* Get the operand of each expression in params list*/
  curr_node = exp_list;
    
  /* Pay attention to what order the parameters are identified, 
     it may be reversed*/
  while(NULL != curr_node &&
	NODE_EXPRESSION_LIST == curr_node->kind) {
    struct ir_operand* param_operand = NULL;
    struct ir_instruction* param_instruction = NULL;
    struct node* expr_node = curr_node->data.expr_list.
      assignment_expr;

    /*1. Get each operand in the list and generate their IR*/
    ir_generate_expression(expr_node);
    param_section = ir_concatenate(param_section, expr_node->ir);

    /*2. Store it on a temporary register if its not an rvalue*/
    param_operand = ir_append_load_if_lvalue(&param_section, 
					     expr_node);
      
    /*3. Use the rvalue as parameter with index*/
    param_instruction = ir_instruction(IR_PARAMETER);
    ir_operand_value(param_instruction, 0, param_idx--);
    ir_operand_copy(param_instruction, 1, param_operand);
      
    /*Load the parameter instruction onto the section*/
    param_section = ir_append(param_section, param_instruction);

    /*4. move to next iterm on the list*/
    curr_node = curr_node->data.expr_list.list;
  }
  /*5. If curr_node != NULL, there is one element left, 
    get that parameter also in*/
  if(NULL != curr_node)
  {
    struct node* expr_node = curr_node;

    /*1. Get each operand in the list and generate their IR*/
    ir_generate_expression(expr_node);
    param_section = ir_concatenate(param_section, expr_node->ir);

    /*2. Store it on a temporary register if its not an rvalue*/
    param_operand = ir_append_load_if_lvalue(&param_section, 
					     expr_node);
      
    /*3. Use the rvalue as parameter with index*/
    param_instruction = ir_instruction(IR_PARAMETER);
    ir_operand_value(param_instruction, 0, param_idx--);
    ir_operand_copy(param_instruction, 1, param_operand);
      
    /*Load the parameter instruction onto the section*/
    param_section = ir_append(param_section, param_instruction);
  }  
  assert(-1 == param_idx);
  return param_section;
}

static void ir_generate_function_call_expr(struct node* node)
{
  struct node* fn_name_node = NULL;
  const char* fn_name = NULL;
  struct ir_section* section = NULL;
  int matched_index = -1;
  /*To avoid NULL checks in other places with section concatenation and appending*/

  assert(NODE_FUNCTION_CALL == node->kind);
  /*Copy section, kept separate for easier debugging*/
  section = ir_copy(ir_generate_param_section
		    (node->data.function_call.expression_list));

  /*II. Call the function*/
  {  
    struct ir_instruction* instruction = NULL;

    /*ASSUMPTION - the function call postfix_expr will consist 
      only of identifier*/
    fn_name_node = node->data.function_call.postfix_expr;
    assert(NODE_IDENTIFIER == fn_name_node->kind);
    fn_name = fn_name_node->data.identifier.name;

    /*Check if its a syscall or a regular function call*/
    if (is_syscall(fn_name, &matched_index)) {
      instruction = ir_instruction(IR_SYS_CALL);
    } else {
      instruction = ir_instruction(IR_CALL);
    }

    /*3. call with the function name*/
    ir_operand_str(instruction, 0, fn_name);

    /*Assign the IR section to the node*/
    section = ir_append(section, instruction);
  }

  /*III. Return types to resultword*/
  {
    struct ir_instruction* 
      return_instruction = ir_instruction(IR_RESULT_WORD);
    ir_operand_temporary(return_instruction, 0, false);
    section = ir_append(section, return_instruction);
    node->data.function_call.
      result.ir_operand = &return_instruction->operands[0];
  }
  /*Assign IR operand to the function call also*/
  node->ir = section;


}

void ir_generate_expression(struct node *expression) {
  switch (expression->kind) {
  case NODE_IDENTIFIER:
    ir_generate_identifier(expression); break;
    
  case NODE_STRING:
    ir_generate_string(expression);break;

  case NODE_NUMBER:
    ir_generate_number(expression); break;

  case NODE_BINARY_OPERATION:
    ir_generate_binary_operation(expression); break;

  case NODE_UNARY_OPERATION:
    ir_generate_unary_operation(expression); break;

  case NODE_COMMA_EXPR: 
    ir_generate_comma_expr(expression); break;
    
  case NODE_CAST:
    ir_generate_cast_expr(expression); break;
    
  case NODE_DECL:
    break;
    
  case NODE_IF_ELSE_TRENARY:
    ir_generate_if_else_trenary(expression);break;

    break;

  case NODE_FUNCTION_CALL:
    ir_generate_function_call_expr(expression); break;
    
  default:
    assert(0); break;
  }
}

static void ir_generate_decl_or_statement_list
(struct node* node, const char* fn_exit_label, 
 const char* innermost_scope_top, 
 const char* innermost_scope_bottom, const char* fn_name)
{
  struct ir_section* instruction_section = NULL, 
    * init_section = NULL, *statement_section = NULL;
  assert(NODE_DECL_OR_STATEMENT_LIST == node->kind);
  node->ir = NULL;/*Make sure of initialization*/

  if(NULL != node->data.decl_or_statement_list.init) {
    ir_generate_decl_or_statement_list
      (node->data.decl_or_statement_list.init, fn_exit_label,
       innermost_scope_top, innermost_scope_bottom, fn_name);
    init_section = node->data.decl_or_statement_list.init->ir;
  } 

  if (NULL == init_section) {
    init_section = generate_ir_no_operation();
  }
  
  /*Should we be creating no operation for declaration 
    instead of leaving them as is??*/
  if(NODE_DECL != node->data.decl_or_statement_list.
     statement->kind) {
    ir_generate_statement(node->data.decl_or_statement_list.
			  statement, fn_exit_label, 
			  innermost_scope_top, 
			  innermost_scope_bottom, fn_name);   
    statement_section = node->data.decl_or_statement_list.
      statement->ir;
  } 
  if (NULL == statement_section) {
    statement_section = generate_ir_no_operation();
  }  

  /* if(NULL != init_section) { */
    instruction_section = ir_concatenate
      (init_section, statement_section);
  /* } else { */
  /*   instruction_section = node->data. */
  /*     decl_or_statement_list.statement->ir; */
  /* } */
  
  node->ir = instruction_section;  
}

static void ir_generate_compound_statement
(struct node* node, const char* fn_exit_label, 
 const char* innermost_scope_top, 
 const char* innermost_scope_bottom, const char* fn_name){
  struct node* decl_or_statement_list;
  assert(NODE_COMPOUND_STATEMENT == node->kind);

  /*Call ir_generate_statement, 
    ignoring any NODE_DECL involved*/
  decl_or_statement_list = node->data.compound_statement.
    decl_or_statement_list;

  /*no code body case should already be handled on caller side*/
  assert(NULL != decl_or_statement_list);
  ir_generate_decl_or_statement_list(decl_or_statement_list, 
				     fn_exit_label,
				     innermost_scope_top,
				     innermost_scope_bottom, 
				     fn_name);

  if(NULL == decl_or_statement_list->ir) {
    decl_or_statement_list->ir = generate_ir_no_operation();
  } 
  node->ir = decl_or_statement_list->ir;

}

static void ir_generate_statement(struct node* node,
				  const char* fn_exit_label, 
				  const char* innermost_scope_top, 
				  const char* innermost_scope_bottom,
				  const char* fn_name){

  /*Allows resetting register count for next 
    statement to generate IR for*/
  /* reset_register_counter(); */

  switch(node->kind) {
  case NODE_COMPOUND_STATEMENT:
    ir_generate_compound_statement(node, fn_exit_label, 
				   innermost_scope_top,
				   innermost_scope_bottom, 
				   fn_name);break;
  case NODE_LABELED_STATEMENT:
    ir_generate_labeled_statement(node, fn_exit_label, 
				  innermost_scope_top,
				  innermost_scope_bottom, 
				  fn_name);break;

    /*Conditional statement*/
  case NODE_IF:
  case NODE_IF_ELSE:
    ir_generate_if_else_statement(node, fn_exit_label, 
				  innermost_scope_top,
				  innermost_scope_bottom, 
				  fn_name);break;

    /*Iterative statement*/
  case NODE_WHILE:
    ir_generate_while_statement(node, fn_exit_label, 
				fn_name);break;
  case NODE_DO_WHILE:
    ir_generate_do_while_statement(node, fn_exit_label, 
				   fn_name);break;
  case NODE_FOR_EXPR:
    ir_generate_for_expr_statement(node, fn_exit_label, 
				   fn_name);break;

    /*Break*/
  case NODE_BREAK:
    ir_generate_break_statement(node, innermost_scope_bottom);break;

    /*Continue*/
  case NODE_CONTINUE:
    ir_generate_continue_statement(node, innermost_scope_top);break;

    /*return*/
  case NODE_RETURN:
    ir_generate_return_statement(node, fn_exit_label);break;
      
      /*goto*/
  case NODE_GOTO:
    ir_generate_goto_statement(node, fn_name);break;

    /*
      case NODE_EXPRESSION_STATEMENT:
      ir_generate_expression statement(node); break;
      default:
      assert(0);break;
    */
  default:
    ir_generate_expression(node);break;
  }

  node->ir = ir_append_sequence_point(node->ir);
}
/*AST construction assumed here*/
static const char* get_fcn_name(struct node *fn_def_specifier){
  struct node* curr_node = NULL;

  assert(NODE_FUNCTION_DEF_SPECIFER == fn_def_specifier->kind);

  curr_node = fn_def_specifier->data.function_defn_specifier.
    declarator;

  /*Separate the pointer tree (if there)*/
  if(NODE_POINTER == curr_node->kind)
    curr_node = curr_node->data.pointer_declarator.
      direct_declarator;
  if(NODE_POINTER_DECLARATOR == curr_node->kind)
    {
      curr_node = curr_node->data.pointer_declarator.direct_declarator;
    }
  assert(NODE_FUNCTION_DECL == curr_node->kind &&
	 NODE_IDENTIFIER == curr_node->data.declarator.name->kind);

  curr_node = curr_node->data.declarator.name;
  return curr_node->data.identifier.name;  
}

static void ir_generate_fcn_defn(struct node *node){
  struct ir_instruction* proc_start, *proc_end;
  struct ir_section* proc_start_section, *proc_body;
  const char* fn_name = NULL;
  int max_fn_offset = -1;
  struct node* fn_defn_specifier = NULL;
  struct type* function_type = NULL;
  assert( NULL != node &&
	  NODE_FUNCTION_DEFN == node->kind);
  
  proc_start = ir_instruction(IR_PROC_BEGIN);
  fn_defn_specifier = node->data.function_defn.declarator;

  assert(NODE_FUNCTION_DEF_SPECIFER == fn_defn_specifier->kind);
  {
    int word_alignment = TYPE_WIDTH_INT;  
    function_type = fn_defn_specifier->data.
      function_defn_specifier.type;
  max_fn_offset =
    traverse_fn_symbol_table_for_maximum_offset
    (function_type->data.function.outermost_block_table);
  
  /*Align the function offset to words in case the last variables 
    are a char or short*/
  max_fn_offset = ( (max_fn_offset + word_alignment - 1)/
		    word_alignment)*word_alignment;
  assert(max_fn_offset >= 0);
  }
  /*Proc start*/
  fn_name = get_fcn_name(node->data.function_defn.declarator);  

  /*Do we need the type information on ir node for function??*/
  ir_operand_function(proc_start, 0, fn_name, 
		      function_type, max_fn_offset);

  /*Check if it is same as main function*/
  if (0 == strcmp("main",fn_name)) {
      assert(!main_function_found);
      main_function_found = true;
    }

  proc_start_section = ir_section(proc_start, proc_start);

  /*Proc body*/
  {
    struct ir_instruction* instruction = NULL;
    const char* label = generate_function_label();
    if(NULL != node->data.function_defn.body){
      ir_generate_compound_statement
	(node->data.function_defn.body, label , NULL, NULL, fn_name);
      proc_body = node->data.function_defn.body->ir;
    } else {
      proc_body = generate_ir_no_operation();
    }

    /*Label for exit*/
    instruction = ir_instruction(IR_LABEL);
    ir_operand_str(instruction, 0, label);
    proc_body = ir_append(proc_body, instruction);
  }
  /*Proc end*/
  proc_end = ir_instruction(IR_PROC_END);
  ir_operand_function(proc_end, 0, fn_name, 
		      function_type, max_fn_offset);
  proc_start_section = ir_concatenate(proc_start_section, proc_body);
  node->ir = ir_append(proc_start_section, proc_end);

}

void ir_generate_translation_unit_recursive(struct node *root_node) {
  struct node *child_node = NULL, *parent_node = NULL;
  struct ir_section* child_instruction = NULL;
  struct ir_section* parent_instruction = NULL;

  assert(root_node != NULL &&
	 NODE_TRANSLATION_UNIT == root_node->kind);
  /* 1. Generate the IR for the Root node*/

  /* Get the table corresponding to the translation_unit */
  parent_node = root_node->data.translation_unit.list;
  if (NULL != parent_node){
    ir_generate_translation_unit_recursive(parent_node);
    parent_instruction = parent_node->ir;
  } else {
    parent_instruction = generate_ir_no_operation();
  }
  
  child_node = root_node->data.translation_unit.top_level_decl;  
  /*Declarators don't need to be handled for type definition*/
  if (NULL != child_node && NODE_DECL != child_node->kind) {
    /* Skip over any declarations, 
       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       How do we handle global declarations??
       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
    assert(NODE_FUNCTION_DEFN == child_node->kind);
    ir_generate_fcn_defn(child_node);
    child_instruction = child_node->ir;
  } else {
    child_instruction = generate_ir_no_operation();
  }

  /* 2. Concatenate the first and last ir instructions for the ir*/
  root_node->ir = ir_concatenate( parent_instruction,
				  child_instruction );  
}

/**********************
 * PRINT INSTRUCTIONS *
 **********************/

static void ir_print_opcode(FILE *output, int kind) {
  /*Since this is non-spare enumeration, 
    using simple array indexing*/
  static char *instruction_names[] = {
    "NOP",
 
    /*3 operand instructions*/  
    "MULT", "MULTU", "DIV", "DIVU", 
    "ADD", "ADDU", "SUB", "SUBU", 
    "OR", "NOT", "XOR", "AND", 
    "SET_ON_LESS_THAN", "SET_ON_LESS_THAN_UNSIGNED",  
    "SET_ON_EQUALITY", "SET_ON_NOT_EQUAL", "SET_ON_GREATER_THAN", 
    "SET_ON_GREATER_THAN_UNSIGNED", 
    "SET_ON_GREATER_THAN_EQUAL",
    "SET_ON_GREATER_THAN_EQUAL_UNSIGNED",
    "SET_ON_LESS_THAN_EQUAL",  
    "SET_ON_LESS_THAN_EQUAL_UNSIGNED",
    "REMAINDER","REMAINDERU",

    /*Shift R-type instructions*/
    "SHIFT_LEFT", "SHIFT_RIGHT", "SHIFT_RIGHT_UNSIGNED", 
    "SHIFT_LU",
  
    /*Branch instructions*/  
    "GOTO", "GOTO_FALSE", "GOTO_TRUE",

    /*Function definition demarcation*/
    "PROC_BEGIN", "PROC_END", 
  
    /*label definition*/
    "LABEL",

    /*Casting*/
    "CAST_WORD_TO_HALF_WORD",
    "CAST_WORD_TO_BYTE", "CAST_HALF_WORD_TO_UWORD",
    "CAST_HALF_WORD_TO_WORD", "CAST_BYTE_TO_UWORD",
    "CAST_BYTE_TO_WORD",

    /*Load Store operations*/
    "LOAD_WORD", "LOAD_BYTE", "LOAD_HALF_WORD",
    "STORE_WORD", "STORE_BYTE", "STORE_HALF_WORD",
    "CONST_INT", "ADDRESS_OF",

    /*Function invocation*/
    "PARAMETER", "RESULT_WORD",
    "CALL", "SYS_CALL","RETURN_WORD",

    /*Miscellaneous*/
    "NEGATION", "LOGICAL_NOT", "SEQUENCE_POINT","TRANSFER_VALUE",
    NULL
  };

  fprintf(output, "%-8s", instruction_names[kind]);
}

static void ir_print_operand(FILE *output, struct ir_operand *operand) {
  /*Will have to handle global variables and functions in
    the symbol table
    
    For each function, also need to resolve the symbols
  */

  switch (operand->kind) {
  case OPERAND_NUMBER:
    fprintf(output, "%10hu", (unsigned short)operand->data.number);
    break;

  case OPERAND_TEMPORARY:
    fprintf(output, "     t%04d", operand->data.temporary);
    break;

  case OPERAND_STR:
    fprintf(output, "     %s", operand->data.str);
    break;
  case OPERAND_IDENTIFIER:
    fprintf(output, "     (%s$%p)$%03d", operand->data.identifier.name,
	    (void*)operand->data.identifier.symbol,
	    operand->data.identifier.offset);
    break;
  case OPERAND_FUNCTION:
    fprintf(output, "     (%s$%p)$%03d", operand->data.function.name,
	    (void*)operand->data.function.type,
	    operand->data.function.local_offset);
    break;

  }
}
void ir_print_instruction(FILE *output, struct ir_instruction *instruction) {
  ir_print_opcode(output, instruction->kind);

  switch (instruction->kind) {
    /*binary operations*/
  case IR_MULT: 
  case IR_MULTU: 
  case IR_DIV: 
  case IR_DIVU:
  case IR_ADD: 
  case IR_ADDU: 
  case IR_SUB: 
  case IR_SUBU: 
  case IR_OR: 
  case IR_NOT: 
  case IR_LOGICAL_NOT:
  /* case IR_NOR:  */
  case IR_XOR: 
  case IR_AND:
  case IR_SET_ON_LESS_THAN:
  case IR_SET_ON_LESS_THAN_UNSIGNED:
  case IR_SET_ON_EQUALITY:
  case IR_SET_ON_NOT_EQUAL:
  case IR_SET_ON_GREATER_THAN:
  case IR_SET_ON_GREATER_THAN_UNSIGNED:
  case IR_SET_ON_GREATER_THAN_EQUAL:
  case IR_SET_ON_GREATER_THAN_EQUAL_UNSIGNED:
  case IR_SET_ON_LESS_THAN_EQUAL: 
  case IR_SET_ON_LESS_THAN_EQUAL_UNSIGNED:
  case IR_SHIFT_L:
  case IR_SHIFT_R: 
  case IR_SHIFT_RU:
  case IR_SHIFT_LU:
  case IR_REMAINDER:
  case IR_REMAINDERU:
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[2]);
    break;

  case IR_GOTO:
    ir_print_operand(output, &instruction->operands[0]);
    break;
  case IR_GOTO_FALSE:
  case IR_GOTO_TRUE: 
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    break;

  case IR_PROC_BEGIN:
  case IR_PROC_END:
    ir_print_operand(output, &instruction->operands[0]);
    break;

  case IR_LABEL:
    ir_print_operand(output, &instruction->operands[0]);
    break;
    
  case IR_CAST_WORD_TO_HALF_WORD:
  case IR_CAST_WORD_TO_BYTE:
  case IR_CAST_HALF_WORD_TO_UWORD:
  case IR_CAST_HALF_WORD_TO_WORD:
  case IR_CAST_BYTE_TO_UWORD:
  case IR_CAST_BYTE_TO_WORD:
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    break;
    
  case IR_LOAD_WORD:
  case IR_LOAD_BYTE:
  case IR_LOAD_HALF_WORD:
  case IR_STORE_WORD:
  case IR_STORE_BYTE:
  case IR_STORE_HALF_WORD:
  case IR_CONST_INT:
  case IR_ADDRESS_OF:
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    break;
    
  case IR_PARAMETER:
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    break;
  case IR_RESULT_WORD:
  case IR_RETURN_WORD:
  case IR_CALL:
  case IR_SYS_CALL:
    ir_print_operand(output, &instruction->operands[0]);
    break;

  case IR_NEGATION:
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    break;
    
  case IR_NO_OPERATION:
  case IR_SEQUENCE_POINT:
    break;

  case IR_TRANSFER_VALUE:
    ir_print_operand(output, &instruction->operands[0]);
    fprintf(output, ", ");
    ir_print_operand(output, &instruction->operands[1]);
    break;    

  default:
    assert(0);
    break;
  }
}

void ir_print_section(FILE *output, struct ir_section *section) {
  int i = 0;
  struct ir_instruction *iter = section->first;
  struct ir_instruction *prev = NULL;
  while (NULL != iter && section->last != prev) {
    fprintf(output, "%5d     (", i++);
    ir_print_instruction(output, iter);
    fprintf(output, ")\n");

    iter = iter->next;
  }
}

static const char* ir_type_name(struct type* type)
{
  switch(type->kind) {
  case TYPE_BASIC:
  case TYPE_POINTER:
    return ".word";
    break;
  case TYPE_ARRAY:
    return ".space";
    break;
  default:
    assert(0);break;
  }
}
void ir_generate_translation_unit(struct node *root_node, 
				  struct symbol_table* table)
{
  /*0. add the global variable declarations to global*/

  /*Iterate over the root table and add all types which 
    are not functions to the global list*/
  {
    struct symbol_list *variables = table->variables;
    struct symbol_list *curr_list = variables; 
    while(NULL != curr_list) {
      /* 1. extract the name and symbol*/	  
      const char* name = curr_list->symbol.name;
      struct type* curr_type = curr_list->symbol.result.type;
      
      /* 2. Find non -function types in declaration*/
      if(TYPE_FUNCTION != curr_type->kind &&
	 TYPE_VOID != curr_type->kind &&
	 TYPE_LABEL != curr_type->kind) {
	/* 3. get the size and type name using type info*/
	int var_size = ir_type_width(curr_type);
	const char* type_name = ir_type_name(curr_type);
	
	/* 4. add them to the global declaration*/
	add_to_global_decl_list(name, type_name, var_size);
      }
      
      curr_list= curr_list->next;
    }
  }

  /*1. generate the translation unit IR*/
  ir_generate_translation_unit_recursive(root_node);

  /*2. If main function not found at the end of 
    this function, error out*/
  if(!main_function_found)
    ir_error_fcn("Main function not found in the generated code", 
		 root_node->line_number);
}

/*******************************************************/
/*!!!KEYS TO CONTROLLING OPTIMIZATION OF SOURCE CODE !!!!*/
#define CONSTANT_PROPAGATION_FOLDING 1 
#define DEAD_REGISTERS_REMOVAL 1

#if OPTIMIZATION 
/*Do we need it to be doubly linked list*/
struct code_block{
  /* int block_index; */
  struct ir_instruction* start;
  struct ir_instruction* end;
  struct code_block* next;
};

struct {
  struct code_block* code_block;
}code_block_list;

struct code_block* create_block(struct ir_instruction* start, struct ir_instruction* end)
{
  struct code_block* blk = NULL;
  assert(NULL != start && NULL != end);
  blk = malloc(sizeof(struct code_block));
  assert(start->index <= end->index);
  blk->start = start;
  blk->end = end;
  blk->next = NULL;
  return blk;
}

void check_if_blocks_overlap(struct code_block* code_block1,
			     struct code_block* code_block2)
{
  int start1 = code_block1->start->index;
  int start2 = code_block2->end->index;
  int end1 = code_block1->end->index;
  int end2 = code_block2->end->index;

  assert( (start1 < start2 && end1 < start2) ||
	  (start1 > end2 && end1 > end2));
}

void append_code_block(struct code_block* code_block)
{
  assert(NULL != code_block);
  if(NULL == code_block_list.code_block) {
    code_block_list.code_block = code_block;
  } else {
    struct code_block* curr_code_block = code_block_list.code_block;
    while(NULL != curr_code_block->next) {
      check_if_blocks_overlap(curr_code_block, code_block);
      curr_code_block = curr_code_block->next;
    }
    check_if_blocks_overlap(curr_code_block, code_block);
    curr_code_block->next = code_block;
  }
}

struct code_block* get_first_code_block()
{
  return code_block_list.code_block;
}
static void mark_blocks(struct ir_section* root_ir)
{  
  struct ir_instruction* curr_ir = NULL;
  struct ir_instruction* block_start = NULL,
    * block_end = NULL;
  
  curr_ir = root_ir->first;
  /*Labels are used for function definition, loops, etc. 
    so we should be fine just picking labels as entry points*/
  
  while(NULL != curr_ir){
    switch(curr_ir->kind)
      {
	/*Mark every label as beginning of the block*/
      case IR_PROC_BEGIN:
      case IR_LABEL:{
	/*The first time a node is encountered, 
	  block_start could be uninitialized*/
	/*GOTO Statements following a label can cause the block_start and current IR to be at the same point*/
	if(NULL != block_start && block_start != curr_ir){
	  /* 1. The block end should be uninitialized and 
	     should be the prior instruction
	     The first could be initialized already. */
	  assert(NULL == block_end);
	  block_end = curr_ir->prev;
	    
	  /* 2. Store the beginning and the end of the block in a 
	     local structure which holds pointers for 
	     each data structure*/
	  if(NULL != block_end){
	    struct code_block* block = 
	      create_block(block_start, block_end);
	    append_code_block(block);
	  }

	}
	/* 3.reinitialize block begin to point to the current 
	   instruction reinitialize block end to be NULL*/
	block_start = curr_ir;
	block_end = NULL;
      }
	break;

	/* Mark every goto as end of the block or 
	   the line preceding a label */
      case IR_PROC_END:
      case IR_GOTO:
      case IR_GOTO_FALSE:
      case IR_GOTO_TRUE:

	/* Should syscall be in a block by itself rather 
	   than part of separate block */
      case IR_CALL:
      case IR_SYS_CALL:
      {
	/* 1. Use this instruction as the end of the block.
	   The end should be uninitialized and 
	   FIRST should be initialized */
	assert(NULL == block_end && NULL != block_start);
	
	/*2. Store the beginning and the end of the block in a 
	  local structure which holds pointers for 
	  each data structure*/
	block_end = curr_ir;
	{
	  struct code_block* block = 
	    create_block(block_start, block_end);
	  append_code_block(block);
	}
	
	/* 3. reinitialize the block end to be NULL 
	   begin to be next instruction*/
	block_start = curr_ir->next;
	block_end = NULL;
      }
      break;
      
      default:
	break;
      }
    /*Update the current ir*/
    curr_ir = curr_ir->next;
  }

  /*Nothing should be left over after the blocks 
    have been assigned*/
  assert(NULL == block_start 
	 && NULL == block_end);
  
}

struct reg_info{
  int index;/*register index*/
  unsigned long value;/*Value stored in the reg*/
  struct reg_info* next;
};

struct reg_info* reg_create(struct ir_instruction* ir_instruction)
{
  int index = -1;
  unsigned long value = -1;
  struct reg_info* tmp = NULL;
  assert(IR_CONST_INT == ir_instruction->kind);
  
  assert(OPERAND_TEMPORARY == 
	 ir_instruction->operands[0].kind);
  index = ir_instruction->operands[0].data.temporary;
  
  assert(OPERAND_NUMBER == 
	 ir_instruction->operands[1].kind);
  value = ir_instruction->operands[1].data.number;
  
  tmp = malloc(sizeof(struct reg_info));
  tmp->index = index;
  tmp->value = value;
  tmp->next = NULL;
  return tmp;
}

struct reg_info* reg_append(struct reg_info* list, struct reg_info* tmp)
{
  if (NULL == list) {
      list = tmp;
  } else {
    struct reg_info* curr = list;

    while(NULL != curr->next){
      
      if (curr->index == tmp->index){
	/***********!!!!!!!HACK HERE!!!!!!!!!!!*****************/
	/*replace curr with tmp
	 OR 
	 we could just switch values*/
	curr->value = tmp->value; /*New value of the register*/
	return list;
      }
      curr = curr->next;
    }
    curr->next = tmp;
  }
  return list;
}

bool reg_val_known_constant(struct reg_info* list,
			    int index, unsigned long* value)
{
  if(NULL != list) {
    struct reg_info* curr = list;
      while(NULL != curr){
	if(curr->index == index) {
	  *value = curr->value;
	  return true;
	}
	curr = curr->next;
      }
  }
    return false;
}

static int get_temp_reg_operand_index(struct ir_operand* operand)
{
  assert(OPERAND_TEMPORARY == operand->kind);
  return operand->data.temporary;
}

unsigned long simplify_unary(int operation, unsigned long value)
{
  unsigned long result;

  switch(operation) {
  case IR_NOT: 
    result = ~value; break;
  case IR_LOGICAL_NOT:
    result = !value; break;

  default:
    assert(0);break;
  }
  return result;
}
unsigned long simplify_binary(int operation, unsigned long value1, 
		       unsigned long value2)
{
  int val1 = (int) value1;
  int val2 = (int) value2;
  
  unsigned long result;
  switch(operation){
  case IR_MULT: 
    result = (val1*val2); break;
  case IR_DIV: 
    result = (val1/val2); break;
  case IR_ADD: 
    result = (val1+val2); break;
  case IR_SUB: 
    result = (val1-val2); break;

  case IR_MULTU: 
    result = (value1*value2); break;
  case IR_DIVU:
    result = (value1/value2); break;
  case IR_ADDU: 
    result = (value1+value2); break;
  case IR_SUBU: 
    result = (value1-value2); break;

  case IR_OR: 
    result = (value1 | value2); break;
    /* case IR_NOR:  */
  case IR_XOR:
    result = (value1 ^ value2); break;
  case IR_AND:
    result = (value1 & value2); break;

  case IR_SET_ON_LESS_THAN:
    result = (val1 < val2); break;

  case IR_SET_ON_LESS_THAN_UNSIGNED:
    result = (value1 < value2); break;

  case IR_SET_ON_EQUALITY:
    result = (value1 == value2); break;

  case IR_SET_ON_NOT_EQUAL:
    result = (value1 != value2); break;

  case IR_SET_ON_GREATER_THAN:
    result = (val1 > val2); break;

  case IR_SET_ON_GREATER_THAN_UNSIGNED:
    result = (value1 > value2); break;

  case IR_SET_ON_GREATER_THAN_EQUAL:
    result = (val1 >= val2);break;

  case IR_SET_ON_GREATER_THAN_EQUAL_UNSIGNED:
    result = (value1 >= value2);break;

  case IR_SET_ON_LESS_THAN_EQUAL: 
    result = (val1 <= val2);break;

  case IR_SET_ON_LESS_THAN_EQUAL_UNSIGNED:
    result = (value1 <= value2);break;

  case IR_SHIFT_L:
    result = (val1 << val2);break;

  case IR_SHIFT_R: 
    result = (val1 >> val2);break;

  case IR_SHIFT_RU:
    result = (value1 >> value2);break;

  case IR_SHIFT_LU:
    result = (value1 << value2);break;

  case IR_REMAINDER:
    result = (val1 % val2);break;

  case IR_REMAINDERU:
    result = (value1 % value2);break;

  default:
    assert(0);break;
  }  
  return result;
}

static struct ir_instruction* ir_optimizer_instruction(int op_code,
					       int index)
{
  struct ir_instruction* instruction = 
    malloc(sizeof(struct ir_instruction));
  assert(NULL != instruction);

  instruction->kind = op_code;
  instruction->index = index;
  instruction->next = NULL;
  instruction->prev = NULL;
  
  return instruction;  
}


static void replace_ir_tree_node(struct ir_instruction* orig,
				 struct ir_instruction* new)
{
  struct ir_instruction* prev = orig->prev;
  struct ir_instruction* next = orig->next;
  prev->next = new; new->prev = prev;
  new->next = next; next->prev = new;  
  new->index = orig->index;
  
  orig->next = NULL; orig->prev = NULL;
}

static void remove_ir_tree_node(struct ir_instruction* dead)
{
  struct ir_instruction* prev = dead->prev;
  struct ir_instruction* next = dead->next;
  prev->next = next; next->prev = prev;

  dead->next = NULL; dead->prev = NULL;

}

bool do_constant_propagation()
{
  struct code_block* first = get_first_code_block();
  struct code_block* curr = first;
  bool const_propagation_done = false;
  
  /*Iterate over the blocks of code
   and go through each instruction in a second loop inside*/
  while(NULL != curr){
    struct reg_info* list = NULL;    
    struct ir_instruction* curr_ir = curr->start,
      *prev_ir = NULL;

    /*Iterate over instructions in the block*/
    while(prev_ir != curr->end) {
      if(IR_CONST_INT == curr_ir->kind){
	/*Find constant values assigned in 
	  a block (constInt) and store them in the 
	  cache you maintain for each block*/
	struct reg_info* reg = reg_create(curr_ir);
	list = reg_append(list, reg);
      } else {
	switch(curr_ir->kind) {
	case IR_MULT: 
	case IR_MULTU: 
	case IR_DIV: 
	case IR_DIVU:
	case IR_ADD: 
	case IR_ADDU: 
	case IR_SUB: 
	case IR_SUBU: 
	case IR_OR: 
	  /* case IR_NOR:  */
	case IR_XOR: 
	case IR_AND:
	case IR_SET_ON_LESS_THAN:
	case IR_SET_ON_LESS_THAN_UNSIGNED:
	case IR_SET_ON_EQUALITY:
	case IR_SET_ON_NOT_EQUAL:
	case IR_SET_ON_GREATER_THAN:
	case IR_SET_ON_GREATER_THAN_UNSIGNED:
	case IR_SET_ON_GREATER_THAN_EQUAL:
	case IR_SET_ON_GREATER_THAN_EQUAL_UNSIGNED:
	case IR_SET_ON_LESS_THAN_EQUAL: 
	case IR_SET_ON_LESS_THAN_EQUAL_UNSIGNED:
	case IR_SHIFT_L:
	case IR_SHIFT_R: 
	case IR_SHIFT_RU:
	case IR_SHIFT_LU:
	case IR_REMAINDER:
	case IR_REMAINDERU:
	  {
	    unsigned long value1 = 0, value2 = 0;
	    int reg_index1, reg_index2;
	      
	    reg_index1 = get_temp_reg_operand_index
	      (&curr_ir->operands[1]);
	    reg_index2 = get_temp_reg_operand_index
	      (&curr_ir->operands[2]);
	      
	    /*check operand 1 and operand 2 if 
	      they are any of the values in the 
	      block constant prop D.S. so far*/

	    /*1. If both operands are known constants, 
	      replace the operation with register assignment*/
	    /*Next time you go through this */

	    if(reg_val_known_constant
	       (list, reg_index1, &value1) &&
	       reg_val_known_constant
	       (list, reg_index2, &value2) ) {
	      /*Simplification operation called*/
	      unsigned long value = 
		simplify_binary(curr_ir->kind, value1, value2);

	      /*need to pay attention to the signs*/
	      struct ir_instruction* instruction = 
		ir_optimizer_instruction(IR_CONST_INT, 
					 curr_ir->index);
	      /*Set operands for the instruction*/
	      ir_operand_value(instruction, 1, value);
	      ir_operand_copy(instruction, 0, 
			      &curr_ir->operands[0]);
		  
	      /*insert it into the IR tree in place of 
		the original instruction and update the 
		current IR to point to the new instruction*/
	      replace_ir_tree_node(curr_ir, instruction);
	      curr_ir = instruction;

	      /*Set the modification done flag*/
	      const_propagation_done = true;
	    }

	    /*If only one assigned, can change it to immediate 
	      version instructions if required*/
	  }

	case IR_NOT: 
	case IR_LOGICAL_NOT:
	  {
	    break;
	  }
	case IR_GOTO:
	  break;
	case IR_GOTO_FALSE:
	case IR_GOTO_TRUE: 
	  /*Check the operand 1 for any issues and 
	    change to unconditional goto*/
	  break;
	   
	case IR_CAST_WORD_TO_HALF_WORD:
	case IR_CAST_WORD_TO_BYTE:
	case IR_CAST_HALF_WORD_TO_UWORD:
	case IR_CAST_HALF_WORD_TO_WORD:
	case IR_CAST_BYTE_TO_UWORD:
	case IR_CAST_BYTE_TO_WORD:
	  /*store the value directly into register if the 
	    right operand is constant*/
	  break;

	case IR_LOAD_WORD:
	case IR_LOAD_BYTE:
	case IR_LOAD_HALF_WORD:
	case IR_STORE_WORD:
	case IR_STORE_BYTE:
	case IR_STORE_HALF_WORD:
	  /*in this case can change to constint, if the 
	    register is known to be a constant in this code 
	    block*/
	  break;

	case IR_ADDRESS_OF:
	case IR_PARAMETER:
	case IR_RESULT_WORD:
	case IR_RETURN_WORD:
	case IR_CALL:
	case IR_SYS_CALL:
	  break;
	    
	case IR_NEGATION:
	  /* Do what other binary/unary operands are doing, 
	     replace with constInt if the value is known*/
	  break;
    
	case IR_NO_OPERATION:
	case IR_SEQUENCE_POINT:
	  break;/*Don't do anything*/

	case IR_PROC_BEGIN:
	case IR_PROC_END:
	case IR_LABEL:
	  break;/*Don't do anything*/


	  /*replace with load immediate if the RHS operand is constant*/
	case IR_TRANSFER_VALUE:
	  break;
	    
	  /*If immediate versions added, add them here*/
	  /*case IR_ADDI:*/
	default: assert(0);
	  /*UNhandled IR op code*/
	}
	/*load address + load word could also be 
	  considered, but skip them for now*/
      
	/*Store them on a data structure maintained locally - 
	  registers and their value assigned
	  Since the registers are all assigned unique ID, 
	  we don't have to worry about index reuse issues*/


	/*All binary and unary operations that have constant 
	  value for all operands, change the instruction to be */
      
	/*All transfer value operators also*/

	/*Whenever change made to any of the IR nodes, change 
	  the flag also*/
      }
      /*Even if node is replaced, the curr_ir should be 
	the same value*/
      prev_ir = curr_ir;
      curr_ir = curr_ir->next;
    }
    curr = curr->next;
  }

  return const_propagation_done;
}

struct ir_dead_instruction{
  struct ir_instruction* instruction;
  struct ir_dead_instruction* next;
};

struct ir_dead_instruction* 
create_ir_dead_instruction(struct ir_instruction* instruction)
{
  struct ir_dead_instruction* dead_instruction = 
    malloc(sizeof(struct ir_dead_instruction));
  dead_instruction->instruction = instruction;
  dead_instruction->next = NULL;

  return dead_instruction;
}

struct ir_dead_instruction* 
append_ir_dead_instruction
(struct ir_dead_instruction* dead_instruction,
 struct ir_instruction* instruction)
{
  struct ir_dead_instruction* tmp = create_ir_dead_instruction(instruction);
  if(NULL == dead_instruction)
    dead_instruction = tmp;
  else{
    struct ir_dead_instruction* curr = dead_instruction;
    while(NULL != curr->next) {
      curr = curr->next;
    }

    curr->next = tmp;
  }
  return dead_instruction;
}

/*This will contain the registers used as output operands.
 Since there is no reuse of registers in IR. We 
 can easily ignore that possibility. It contains the usage 
 ir_instruction so we know what to delete from IR tree*/
struct dead_regs{
  int index;
  struct ir_instruction* usage;
  struct dead_regs* next;
};

struct dead_regs* create_dead_regs(int index, 
				   struct ir_instruction* usage)
{
  struct dead_regs* tmp = malloc(sizeof(struct dead_regs));
  tmp->next = NULL;
  tmp->usage = usage;
  tmp->index = index;

  return tmp;
}

struct dead_regs* add_to_dead_regs(struct dead_regs* list,int index,
				  struct ir_instruction* usage)
{
  if(list == NULL) {
    list = create_dead_regs(index, usage);
  } else {
    /*if the index */
    struct dead_regs* curr = list;
    struct dead_regs* prev;
    while(NULL != curr){
      assert(curr->index != index);/*No reuse of registers assigned*/
      /* if(curr->index == index){ */
      /* 	prev->next = curr->next; */
      /* 	curr->next = NULL; */
      /* } */
      prev = curr;
      curr = curr->next;
    }
    /*In case of no repitition, add it to dead register list*/
    prev->next = create_dead_regs(index, usage);
  }
  return list;
}

struct dead_regs* remove_dead_registers_from_list
(struct dead_regs* list,int index,
 struct ir_instruction* usage)
{
  /*if the dead register as output comes before it is used 
    as input here. Remove only that one.
    
    This takes care of live-dead analysis to a certain degree
    Except this would be done at a global level rather 
    than a block level
    
    In case the register is found to be assigned to later on, 
    in the IR, it will get added back to the list
  */
  
 if(list == NULL) {
   return list;
  } else {
    /*if the index was already there, remove that*/
    struct dead_regs* curr = list;
    struct dead_regs* prev = NULL;
    while(NULL != curr){
      if(curr->index == index){
	/*It should be assigned before the usage
	 Since there is no register re-use this is an assertion*/
	assert(usage->index >= curr->usage->index);
	if(NULL == prev)
	  list = curr->next;
	else
	  prev->next = curr->next;
	curr->next = NULL;
	return list;
      }
      prev = curr;
      curr = curr->next;
    }
    /*No Op in case nothing found*/
   }
  return list;
}

struct dead_regs* mark_dead_registers(struct ir_instruction* start)
{
  struct ir_instruction* curr = start;
  
  /*Reusing reg_info to get a count on usage of each register*/
  struct dead_regs* list = NULL;
  while(NULL != curr) {
      switch(curr->kind){
      case IR_MULT: 
      case IR_MULTU: 
      case IR_DIV: 
      case IR_DIVU:
      case IR_ADD: 
      case IR_ADDU: 
      case IR_SUB: 
      case IR_SUBU: 
      case IR_OR: 
      case IR_NOT: 
      case IR_LOGICAL_NOT:
	/* case IR_NOR:  */
      case IR_XOR: 
      case IR_AND:
      case IR_SET_ON_LESS_THAN:
      case IR_SET_ON_LESS_THAN_UNSIGNED:
      case IR_SET_ON_EQUALITY:
      case IR_SET_ON_NOT_EQUAL:
      case IR_SET_ON_GREATER_THAN:
      case IR_SET_ON_GREATER_THAN_UNSIGNED:
      case IR_SET_ON_GREATER_THAN_EQUAL:
      case IR_SET_ON_GREATER_THAN_EQUAL_UNSIGNED:
      case IR_SET_ON_LESS_THAN_EQUAL: 
      case IR_SET_ON_LESS_THAN_EQUAL_UNSIGNED:
      case IR_SHIFT_L:
      case IR_SHIFT_R: 
      case IR_SHIFT_RU:
      case IR_SHIFT_LU:
      case IR_REMAINDER:
      case IR_REMAINDERU:{
	int idx0, idx1, idx2;
	idx0 = 
	  get_temp_reg_operand_index(&curr->operands[0]);
	list = add_to_dead_regs(list, idx0, curr);
	
	idx1 = 
	  get_temp_reg_operand_index(&curr->operands[1]);
	list = 
	  remove_dead_registers_from_list(list, idx1, curr);
	
	idx2 = 
	  get_temp_reg_operand_index(&curr->operands[2]);
	list = 
	  remove_dead_registers_from_list(list, idx2, curr);
	break;
      }
      case IR_GOTO:
	break;
      case IR_GOTO_FALSE:
      case IR_GOTO_TRUE: {
	int idx = 
	  get_temp_reg_operand_index(&curr->operands[0]);
	list = remove_dead_registers_from_list(list,idx, curr);
	/*Optimization should make this unconditional jump 
	  or remove it*/
	break;
      }  
      case IR_PARAMETER:{
	int idx = 
	  get_temp_reg_operand_index(&curr->operands[1]);
	list = remove_dead_registers_from_list(list,idx, curr);
	/*Optimization should make this unconditional jump 
	  or remove it*/
	break;

      }
 
      case IR_CAST_WORD_TO_HALF_WORD:
      case IR_CAST_WORD_TO_BYTE:
      case IR_CAST_HALF_WORD_TO_UWORD:
      case IR_CAST_HALF_WORD_TO_WORD:
      case IR_CAST_BYTE_TO_UWORD:
      case IR_CAST_BYTE_TO_WORD:{
	int idx0, idx1;
	idx0 = 
	  get_temp_reg_operand_index(&curr->operands[0]);
	list = add_to_dead_regs(list,idx0, curr);
	idx1 = 
	  get_temp_reg_operand_index(&curr->operands[1]);
	list = remove_dead_registers_from_list(list,idx1, curr);
	break;
      }
      case IR_LOAD_WORD:
      case IR_LOAD_BYTE:
      case IR_LOAD_HALF_WORD:{
	int idx0, idx1;
	idx0= 
	  get_temp_reg_operand_index(&curr->operands[0]);
	list = add_to_dead_regs(list,idx0, curr);
	idx1 = 
	  get_temp_reg_operand_index(&curr->operands[1]);
	list = remove_dead_registers_from_list(list,idx1, curr);
	break;
      }
      case IR_STORE_WORD:
      case IR_STORE_BYTE:
      case IR_STORE_HALF_WORD:{
	/*Ideally only the input register 
	  should be removed, but since 
	  the output could be used for the value assignment
	  Being conservative about it at this stage*/
	int idx0, idx1;
	idx1 = 
	  get_temp_reg_operand_index(&curr->operands[1]);
	list = remove_dead_registers_from_list(list,idx1, curr);
	idx0= 
	  get_temp_reg_operand_index(&curr->operands[0]);
	list = remove_dead_registers_from_list(list,idx0, curr);
	break;
      }
      case IR_CONST_INT:{
	int idx0;
	  idx0= 
	    get_temp_reg_operand_index(&curr->operands[0]);
	  list = add_to_dead_regs(list, idx0, curr);
	  break;
      }
      case IR_ADDRESS_OF:
      /* case IR_NEGATION: */	{
	int idx0,idx1;
	  idx0= 
	    get_temp_reg_operand_index(&curr->operands[0]);
	  list = add_to_dead_regs(list, idx0, curr);
	  idx1= 
	    get_temp_reg_operand_index(&curr->operands[0]);
	  list = remove_dead_registers_from_list(list, idx1, curr);

	  break;
	}	    
      case IR_RESULT_WORD:
      case IR_RETURN_WORD:
	{
	  int idx;
	  idx= 
	    get_temp_reg_operand_index(&curr->operands[0]);
	  list = remove_dead_registers_from_list(list, idx, curr);
	  break;
	}
      case IR_CALL:
      case IR_SYS_CALL:
	break;
   
      case IR_NO_OPERATION:
      case IR_SEQUENCE_POINT:
      case IR_PROC_BEGIN:
      case IR_PROC_END:
      case IR_LABEL:
	break;/*Don't do anything*/

	/*replace with load immediate if the RHS 
	  operand is constant*/
      case IR_TRANSFER_VALUE:{
	int idx0, idx1;
	idx0= 
	  get_temp_reg_operand_index(&curr->operands[0]);
	list = add_to_dead_regs(list,idx0, curr);
	idx1 = 
	  get_temp_reg_operand_index(&curr->operands[1]);
	list = remove_dead_registers_from_list(list,idx1, curr);
	break;
      }

      default:
	assert(0);
	break;
      }
      curr = curr->next;
  }
  return list;
}

bool do_dead_code_removal(struct ir_section* start)
{
  bool dead_code_removed = false;
  struct dead_regs* list = NULL, *curr = NULL;

  struct ir_instruction* start_ir = start->first;

  list = mark_dead_registers(start_ir);
  curr = list;

  /*Remove instructions corresponding to dead register outputs*/
  while( NULL != curr) {
    remove_ir_tree_node(curr->usage);
    curr = curr->next;
    dead_code_removed = true;
  }

  return dead_code_removed;
}
#endif /*Optimization*/

/***!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Do not use ir_instruction() in optimization 
    as that would assign 
    a different index number , have a ir_instruction_optimization()
    which would take the index number as well
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!****/
#if OPTIMIZATION 
void ir_optimize(struct ir_section* root_ir)
{
  /* Remove the No-operation nodes */
  if(CONSTANT_PROPAGATION_FOLDING) {
    bool const_propagation_done = false;
    mark_blocks(root_ir);
    do{
      const_propagation_done = do_constant_propagation();
    }while(const_propagation_done);
  }
  
  

  if(DEAD_REGISTERS_REMOVAL) {
    bool dead_code_removed = false;
    do{
      dead_code_removed = do_dead_code_removal(root_ir);
    }while(dead_code_removed);

    /*1. Collect list of all register indices not used 
      anywhere. i.e. only once instance of it where it 
      is initialized*/

    /*2. delete the instruction for initializing the instruction*/

    /*3. Mark instructions outputting to this 
      instruction for deletion*/

    /*Any registers not being used anywhere in the whole ir tree- 
      remove their declaration*/

    /*This would also remove the registers in which value is 
      assigned to but no one uses by user as well*/

    /*Will also need to be done iteratively as there could be a 
      hierarchy in dead registers*/

  }

  
}

#else
void ir_optimize(struct ir_section* root_ir)
{
  /*Do nothing if optimization is off*/
  (void) root_ir;
}
#endif

