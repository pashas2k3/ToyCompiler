#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "type.h"
#include "symbol.h"
#include "ir.h"
#include "mips.h"

#define REG_EXHAUSTED   -1

/*Should this be defined here ?*/
#define WORD_WIDTH       4 
#define STACK_ALIGNMENT_FACTOR 8

#define FIRST_USABLE_REGISTER  8
#define LAST_USABLE_REGISTER  23
#define NUM_REGISTERS         32
/*$a0-$a3*/
#define NUM_ARG_REGISTER     4 
#define NUM_SAVED_REGISTER   8
#define NUM_TEMP_REGISTER    8
/*14 words*/
#define FIXED_FUNCTION_FRAME_OVERHEAD 56 

/*This structure is created to share offset information and size information across Proc labels as the offset calculations is affected by the target architecture also*/
struct _fn_stack_info
{
  int stack_frame_size;
  int s_reg_end;
  int num_tmp_regs;
  int t_reg_start;
  const char* fn_name;/*For debugging*/
  struct _fn_stack_info* next;
};
bool run_once = false;/*To avoid any static member re-initialization*/
struct 
{
  struct _fn_stack_info* list;
}fn_stack_info;

struct _fn_stack_info* create_fn_stack_info(int sz, 
					    int local_var_overhead, 
					    int num_tmp_regs,
					    const char* name) {
  struct _fn_stack_info* tmp = 
    malloc(sizeof(struct _fn_stack_info));
  tmp->stack_frame_size = sz;
  tmp->num_tmp_regs = num_tmp_regs;
  tmp->t_reg_start = local_var_overhead - 
    (num_tmp_regs * WORD_WIDTH);
  tmp->s_reg_end = tmp->t_reg_start - (2 * WORD_WIDTH);

  tmp->fn_name = name;
  return tmp;
}

void push(struct _fn_stack_info* a_info)
{
  a_info->next = fn_stack_info.list;
  fn_stack_info.list = a_info;
}

/*Prototypes*/
void mips_print_instruction(FILE *output, 
			    struct ir_instruction *instruction);
/****************************
 * REGISTER MANAGEMENT *
 ****************************/
struct {
  int saved_reg_count;
  int tmp_reg_count;
}reg_count;

static bool saved_register_used()
{
  return (reg_count.saved_reg_count == NUM_SAVED_REGISTER);
}

static int get_saved_register()
{
  assert(!saved_register_used());
  return reg_count.saved_reg_count++;
}

static int get_temp_register()
{
  assert(reg_count.tmp_reg_count != NUM_TEMP_REGISTER);
  return reg_count.tmp_reg_count++;
}
static void reset_register_counter()
{
  reg_count.saved_reg_count = 0;
  reg_count.tmp_reg_count = 0;
}

struct _fn_stack_info* pop()
{
  struct _fn_stack_info* a_info = fn_stack_info.list;
  fn_stack_info.list = a_info->next;
  a_info->next = NULL;
  return a_info;
}
/*Register pairing and mapping*/
struct temp_reg_pair{
  int temp_reg_ir_idx;
  int reg_idx;
  bool saved_regs;
  struct temp_reg_pair* next;
};

struct {
  struct temp_reg_pair* reg_pair;
}temp_reg_mapping;

static struct temp_reg_pair* get_reg(const int ir_index)
{
  struct temp_reg_pair* prev_pair = NULL;
  /*Check if the list has something with the same temp_reg_ir_idx*/
  /*ASSUMPTION - no resetting of temp IR regs*/
  struct temp_reg_pair* curr_pair = temp_reg_mapping.reg_pair;

  /*Check if the index is in the list*/
  /*If found, return the saved pairing*/
  while(NULL != curr_pair) {
    if (ir_index == curr_pair->temp_reg_ir_idx) {
      return curr_pair;
    }
    prev_pair = curr_pair;
    curr_pair = curr_pair->next;
  }
  
  /*Otherwise, add a register*/
  curr_pair = malloc(sizeof(struct temp_reg_pair));
  curr_pair->next = NULL;
  curr_pair->temp_reg_ir_idx = ir_index;
  /*Try using saved registers till you run out of them*/
  if(!saved_register_used()) {
    curr_pair->reg_idx = get_saved_register();
    curr_pair->saved_regs = true;
  } else {
    curr_pair->reg_idx =get_temp_register();
    curr_pair->saved_regs = false;
  }

  if(NULL != prev_pair)
    prev_pair->next = curr_pair;
  else 
    temp_reg_mapping.reg_pair = curr_pair;
  assert(NULL != curr_pair);
  return curr_pair;
}



/****************************
 * MIPS TEXT SECTION OUTPUT *
 ****************************/
static void mips_print_operand(FILE* output, struct ir_operand* operand, bool needs_parenthesis)
{
  switch (operand->kind) {
  case OPERAND_NUMBER:
    fprintf(output, "%10lu", operand->data.number);
    break;
    
  case OPERAND_TEMPORARY:
    {
      struct temp_reg_pair* pair = get_reg(operand->data.temporary);
      if(!needs_parenthesis)
	fprintf(output, "%10s%d", (pair->saved_regs)?"$s":"$t",
		pair->reg_idx);
      else
	fprintf(output, "(%s%d)", (pair->saved_regs)?"$s":"$t",
		pair->reg_idx);
      break;
    }
  case OPERAND_STR:
    fprintf(output, "%10s", operand->data.str);
    break;

  case OPERAND_IDENTIFIER:
    /*Offset should be determined by this stage and -1 is for 
      unset values*/

    /*Argument build area would be shifted by words so it 
      would still work once argument build area is implemented*/
    assert(-1 != operand->data.identifier.symbol->offset);
    fprintf(output, "     %d($fp)", 
	    operand->data.identifier.symbol->offset);
    break;
  default:
    assert(0);break;
  }
}

void mips_print_arithmetic(FILE *output, struct ir_instruction *instruction) {
  /*The ordering here is in direct reference to the ordering in 
    mips_print_instruction*/
  static char *opcodes[] = {
    NULL,
    "mult", "multu", "div", "divu", "add", "addu", "sub", "subu",
    "or", "not", "xor", "and", "slt", "sltu", "seq","sne",
    "sgt","sgtu","sge","sgeu","sle","sleu", "rem","remu",NULL
  };

  /*What about immediate instructions??*/
  fprintf(output, "# arithmetic operation %s\n",opcodes[instruction->kind]);
  fprintf(output, "%10s ", opcodes[instruction->kind]);
  switch(instruction->kind)
    {
    case IR_MULT:
    case IR_MULTU: 
    /* case IR_DIV: */
    /* case IR_DIVU: */
      mips_print_operand(output, &instruction->operands[1], false);
      fputs(", ", output);
      mips_print_operand(output, &instruction->operands[2], false);
      fputs ("\n", output);

      /*add MFLO instruction*/
      fprintf(output, "%10s ", "mflo");
      mips_print_operand(output, &instruction->operands[0], false);
      fputs ("\n", output);
      /*Ignoring the MFHI instruction*/

      break;

    default:
      mips_print_operand(output, &instruction->operands[0], false);
      fputs(", ", output);
      mips_print_operand(output, &instruction->operands[1], false);
      fputs(", ", output);
      mips_print_operand(output, &instruction->operands[2], false);
      fputs("\n", output);
      break;
    }
}

void mips_print_copy(FILE *output, struct ir_instruction *instruction) {
  fprintf(output, "%10s ", "ori");
  mips_print_operand(output, &instruction->operands[0], false);
  fputs(", ", output);
  mips_print_operand(output, &instruction->operands[1], false);
  fprintf(output, ", %10s\n", "$0");
}

void mips_print_load_immediate(FILE *output, struct ir_instruction *instruction) {
  fprintf(output, "%10s ", "li");
  mips_print_operand(output, &instruction->operands[0], false);
  fputs(", ", output);
  mips_print_operand(output, &instruction->operands[1], false);
  fputs("\n", output);
}

void mips_print_print_number(FILE *output, struct ir_instruction *instruction) {
  fprintf(output, "%10s %10s, %10s, %10d\n", "ori", "$v0", "$0", 1);
  fprintf(output, "%10s %10s, %10s, ", "ori", "$a0", "$0");
  mips_print_operand(output, &instruction->operands[0], false);
  fprintf(output, "\n%10s\n", "syscall");
}
void mips_print_binary_instruction(FILE *output, 
				   const char* op_code,
				   const char* operand1, 
				   const char* operand2)
{
  fprintf(output,"%10s %10s, %10s\n", 
	  op_code, operand1, operand2);
}
static void mips_print_function_end(FILE* output, struct ir_instruction* instruction)
{
  /*Pop the stack frame and restore the values of a and s registers*/
  struct _fn_stack_info* tmp = pop();  
  int saved_reg_offset = -1;
  int old_fp_offset = -1;
  int cnt = -1;
  assert(NULL != tmp );
  fprintf(output, "#Function ends %s\n",
	  instruction->operands[0].data.identifier.name);

  /*Remove offset for old $fp and $ra*/
  saved_reg_offset = tmp->s_reg_end - (NUM_SAVED_REGISTER *WORD_WIDTH);  

  /*Restore $s registers*/  
  for(cnt = (NUM_SAVED_REGISTER-1); cnt >= 0; cnt--){
    fprintf(output,"%10s %10s%d, %d(%s)\n",
	    "lw","$s", cnt, cnt*WORD_WIDTH + 
	    saved_reg_offset,"$fp");
  }

  /*Restore $t registers*/  
  for(cnt = (tmp->num_tmp_regs-1); cnt >= 0; cnt--){
    fprintf(output,"%10s %10s%d, %d(%s)\n",
	    "lw","$t", cnt, (tmp->t_reg_start + 
			     cnt*WORD_WIDTH),"$fp");
  }
  /*Restore $ra and $fp register*/
  old_fp_offset = tmp->s_reg_end;

  fprintf(output,"%10s %10s, %d(%s)\n",
	  "lw","$ra", old_fp_offset + WORD_WIDTH ,"$fp");

  fprintf(output,"%10s %10s, %d(%s)\n",
	  "lw","$fp", old_fp_offset ,"$sp");
  /*Pop the function stack frame*/  
  fprintf(output,"%10s %10s, %s, %d\n",
	  "addiu","$sp", "$sp", tmp->stack_frame_size);
  /*return to caller*/
  fprintf(output,"%10s %10s\n",
	  "jr","$ra");  
}
static void mips_print_function_begin(FILE* output, 
				      struct ir_instruction* instruction)
{
  int stack_frame_size = -1;
  int num_extra_arg_space = 0;
  int num_params = 0;
  int local_variable_offset = -1;

  /*Saving all temp registers for now
   This can be changed once we have an argument build area 
   full functional*/
  int num_tmp_regs_to_be_saved = NUM_TEMP_REGISTER;
  int space_for_temp_regs = num_tmp_regs_to_be_saved * WORD_WIDTH;

  assert(IR_PROC_BEGIN == instruction->kind);

  /*Print the comments */
  fprintf(output, "#Function begins %s\n",
	  instruction->operands[0].data.identifier.name);
  

  /*Print function label*/
  fprintf(output, "%s: \n ",instruction->operands[0].data.
  	  identifier.name);
  
  /*Calculate the alignment including cost*/

  /*Get the number of arguments - if greater than 4,
    we need argument build area*/
  {
    struct type* fn_type = instruction->operands[0].data.
      function.type;
    struct symbol_list* params_list = fn_type->data.function.
      params_list;
    struct symbol_list* curr_list = params_list;
    while(NULL != curr_list) {
      num_params++;
      curr_list = curr_list->next;
    }
    assert(num_params >= 0);
    num_extra_arg_space = num_params - NUM_ARG_REGISTER;
    
    /*!!!!!!UNIMPLEMENTED - Caller build area!!!!!*/
    assert(num_extra_arg_space < 1);
  }
  /*Align by a factor of 8*/

  /*1. If the not fixed overhead part is not divisible 
       by 8, add a flag for it,
    2. Add the unused word
    3. If the function offset is not divisible by 4, 
       round it up to the next number divisible by it
       Its because the last number is probably a char or an int
    4. For now copy over the parameters to local variables
  */

  /*Add up the overhead accumulated*/
  {
    local_variable_offset =
      instruction->operands[0].data.function.local_offset;
    bool unused_word = false;

    struct type* type = 
      instruction->operands[0].data.function.type;

    struct symbol_list* var_list = 
      type->data.function.outermost_block_table->variables;

    struct symbol_list* curr_var = var_list;

    /*Rounding to next word Width */
    stack_frame_size =  ( (num_extra_arg_space>0)?
			  num_extra_arg_space*WORD_WIDTH : 0) +
      FIXED_FUNCTION_FRAME_OVERHEAD + space_for_temp_regs + 
      local_variable_offset;
    
    /*ASSUMPTION - the offsets are at least word aligned*/
    if(0 != stack_frame_size % STACK_ALIGNMENT_FACTOR) {
      assert(WORD_WIDTH == stack_frame_size % 
	     STACK_ALIGNMENT_FACTOR);
      /*Align to the next higher multiple of WORD*/
      stack_frame_size += WORD_WIDTH; /*Unused word*/
      unused_word = true;
    }
    /*
      1. Set the offset for all param variables
      2. Set the offsets of the variables again
      Just need to increment the offset of local variables by 
      the fixed overhead*/
    if(!run_once){
      int fixed_overhead = stack_frame_size - local_variable_offset;
      int prm_idx = 0;
      int arg_reg_offset = (unused_word)? WORD_WIDTH:0;

      /*Update the offset values for the variables*/
      while(curr_var != NULL) {
	if(!curr_var->symbol.is_fn_param ) {
	  curr_var->symbol.offset += fixed_overhead;
	  /*Overhead already includes space for everything but the local variables*/
	} else {
	  curr_var->symbol.offset = arg_reg_offset + 
	    prm_idx * WORD_WIDTH;
	  prm_idx++;
	}	
	curr_var = curr_var->next;
      }
      assert(prm_idx == num_params);
    }
    {
      int ra_offset = stack_frame_size - space_for_temp_regs -
  	local_variable_offset - WORD_WIDTH;
      int old_fp_offset = ra_offset - WORD_WIDTH;
      /*Print the regular overhead function*/
      /*1. Make space for new frame*/
      fprintf(output,"%10s %10s, %s, %d\n",
  	      "addiu","$sp","$sp", -stack_frame_size);
      /*2. Save the old fp*/
      fprintf(output,"%10s %10s, %d(%s) \n",
  	      "sw","$fp", old_fp_offset, "$sp");
      /* 3. $fp -> stack frame */
      fprintf(output,"%10s %10s, %s, %s \n",
  	      "or","$fp", "$sp", "$0");
      /* 4. save the return address */
      fprintf(output,"%10s %10s, %d(%s) \n",
  	      "sw","$ra", ra_offset,"$fp");

      {
  	int arg_cnt = 0;
  	int num_regs = 0;
  	int saved_cnt = 0;
	int temp_cnt = 0;

  	if(unused_word) {
  	  num_regs += 1;
  	}

	/* 5. Store the registers for temporary*/
	for (;arg_cnt < NUM_ARG_REGISTER; arg_cnt++){
  	  fprintf(output,"%10s %10s%d, %d(%s) \n",
  	  	  "sw", "$a", arg_cnt , num_regs*WORD_WIDTH, "$fp");
  	  num_regs++;
  	}

	
  	/* 6. Saved registers*/
  	for(; saved_cnt < NUM_SAVED_REGISTER; saved_cnt++) {
  	  fprintf(output, "%10s %10s%d, %d(%s) \n",
  		  "sw", "$s", saved_cnt, num_regs*WORD_WIDTH,"$fp");
  	  num_regs++;
  	}
      /*7. Temporary registers - can be done in function 
	call also but chose here since rest of the stuff 
	also handled here*/
  	for(num_regs = 1, temp_cnt = 0;
  	    temp_cnt < num_tmp_regs_to_be_saved; temp_cnt++) {
  	  fprintf(output, "%10s %10s%d, %d(%s) \n",
  		  "sw", "$t", temp_cnt, 
		  ra_offset+(num_regs*WORD_WIDTH),"$fp");
  	  num_regs++;
  	}
      }
      
      assert( (ra_offset + WORD_WIDTH + space_for_temp_regs) == stack_frame_size - local_variable_offset);
      /* /\* 8. Load the parameters into a local variable*\/ */
      /* fprintf(output,"%10s %10s, %d(%s)\n", */
      /* 	      "sw","$fp", ra_offset, "$sp"); /\*Save return address*\/ */

    }
  }
  /*Pass in over head for local vars 
    needed as the unused register 
    could change offset*/
  push(create_fn_stack_info
       (stack_frame_size,(stack_frame_size -local_variable_offset), 
	num_tmp_regs_to_be_saved,
	instruction->operands[0].data.identifier.name));
}

void mips_print_remainder(FILE* output, struct ir_instruction* instruction)
{
  (void)output; (void) instruction;
  assert(0);/*Unimplemented*/
}

void mips_print_bit_shift(FILE* output, struct ir_instruction* instruction)
{
  char* opcode = NULL;
  switch(instruction->kind){
  case IR_SHIFT_L:{
    opcode = "sll"; break;
  }
  case IR_SHIFT_R:{
    opcode = "srl"; break;
  }
  case IR_SHIFT_RU:{
    opcode = "srlv"; break;
  }
  case IR_SHIFT_LU:{
    opcode = "sllv"; break;
  }
  default:
    assert(0);
    break;
  }

  fprintf(output, "%10s  ",opcode);
  mips_print_operand(output, &instruction->operands[0], false);
  fputs(", ", output);
  mips_print_operand(output, &instruction->operands[1], false);
  fputs(", ", output);
  mips_print_operand(output, &instruction->operands[2], false);
  fputs("\n ", output);

}

void mips_print_branching(FILE* output, struct ir_instruction* instruction)
{

  switch(instruction->kind){
  case IR_GOTO:
    {
      fprintf(output,"%10s      ","b");
      mips_print_operand(output, &instruction->operands[0], false);
      fputs("\n",output);
      break;
    }
  case IR_GOTO_FALSE:
    {
      fprintf(output,"%10s","beq");
      mips_print_operand(output, &instruction->operands[0], false);
      fputs(", $0, ",output);
      mips_print_operand(output, &instruction->operands[1], false);
      fputs("\n",output);
     break;
    }
  case IR_GOTO_TRUE:
    {
      fprintf(output,"%10s","bne");
      mips_print_operand(output, &instruction->operands[0], false);
      fputs(", $0, ",output);
      mips_print_operand(output, &instruction->operands[1], false);
      fputs("\n",output);
      break;
    }
  default:
    assert(0);break;
  }

}

void mips_print_label(FILE* output, struct ir_instruction* instruction)
{
  mips_print_operand(output, &instruction->operands[0],false);
  fputs(":\n", output);
}


/* bool reg_storage_called; */
/* static void set_storage_used(bool flag) */
/* { */
/*   reg_storage_called = flag; */
/* } */
/* static bool get_storage_used() */
/* { */
/*   return reg_storage_called; */
/* } */
/* /\*Use $a registers for storage , they will be used only during  */
/*   calls anyway and will be restored at end of function */
/*   Done that to avoid changing the infra for temp register  */
/*   assignment*\/ */
/* static void mips_print_reg_storage */
/* (FILE* output, struct ir_instruction* instruction) */
/* { */
/*   /\*this takes care of casting from non-word to word type*\/ */
/*   switch(instruction->kind){ */
/*   case IR_CAST_HALF_WORD_TO_UWORD: */
/*     opcode = "shu";break; */
/*   case IR_CAST_HALF_WORD_TO_WORD: */
/*     opcode = "sh"; break; */
/*   case IR_CAST_BYTE_TO_UWORD: */
/*     opcode = "sbu"; break; */
/*   case IR_CAST_BYTE_TO_WORD: */
/*     opcode = "sb"; break; */
/*   default: */
/*     opcode = "sw"; break; */
/*   } */

/*   assert(!get_storage_used() ); */
/*   /\*Move locations addressed by top of $fp to a temp register*\/ */

/*   /\*Use lw to move info from 0($fp) to $a0*\/ */

/*   /\*sw to move from operand register to 0($fp)*\/ */
/*   fprintf("%10s", opcode ); */
/*   mips_print_operand(output, &instruction->operands[0],false); */
/*   fprintf(",%10s \n", "0($fp)"); */
  
/*   /\*move it to an addressed location*\/ */
/*   /\*do the casting and assign the value to a register *\/ */
  
/*   set_storage_used(true); */
/*   return new_operand; */
/* } */

/* static void mips_print_reg_restore(struct ir_section** section, struct ir_operand* operand) */
/* { */
/*   struct ir_operand* new_operand = NULL; */
/*   assert(get_storage_used()); */

/*   assert(0);/\*Unimplemented*\/ */
/*   (void)operand; (void)section; */
/*   /\*restore the value back to top of $fp */
/*     from the passed in operand */
/*    *\/ */
/*   /\*Move locations addressed by top of $fp to a temp register*\/ */

/*   set_storage_used(false); */

/*   return new_operand; */
/* } */

void mips_print_cast(FILE* output, struct ir_instruction* instruction)
{

  /*!!!There are gaps in the cast operation that need handling!!!*/
  const char* mask = NULL;
  /* const char* opcode2 = NULL;/\*In case two instructions needed*\/ */

  switch(instruction->kind) {
  case IR_CAST_WORD_TO_HALF_WORD:
    mask = "0xffff";break;
  case IR_CAST_WORD_TO_BYTE:
    mask = "0x00ff";break;
    
    /*!!!!Sign extension work needed here!!!!*/
  case IR_CAST_HALF_WORD_TO_UWORD:
  case IR_CAST_HALF_WORD_TO_WORD:
  case IR_CAST_BYTE_TO_UWORD:
  case IR_CAST_BYTE_TO_WORD:
    break;
  default:
    assert(0);break;
  }
  fprintf(output, "%s","#Casting operation\n");
  /*Do the first pass casting*/
  if(NULL != mask){
    fprintf(output, "%10s","andi");
    
    /*Print the operands*/
    mips_print_operand(output, &instruction->operands[0],false);
    fputs(",",output);
    mips_print_operand(output, &instruction->operands[1],false);
    fprintf(output,", %10s", mask);
    fputs("\n",output);
  } else {
    /*Just move the rvalue word*/
    fprintf(output, "%10s","or");
    
    /*Print the operands*/
    mips_print_operand(output, &instruction->operands[0],false);
    fputs(",",output);
    mips_print_operand(output, &instruction->operands[1],false);
    fprintf(output,", %10s", "$0");
    fputs("\n",output);
    
  }
  /* /\*Print the opcode*\/ */
  /* fprintf(output, "%10s",opcode1); */

  /* /\*Print the opcode*\/ */
  /* fprintf(output, "%10s",opcode1); */

  /* /\*Print the operands*\/ */
  /* mips_print_operand(output, &instruction->operands[0],false); */
  /* fputs(",",output); */
  /* mips_print_operand(output, &instruction->operands[1],true); */
  /* fputs("\n",output); */
}

void mips_print_transfer_value(FILE* output, struct ir_instruction* instruction)
{
  assert(IR_TRANSFER_VALUE == instruction->kind);
  /* OR with $0 to transfer the value to another register */
  fprintf(output, "%10s","or");
  mips_print_operand(output, &instruction->operands[0],false);
  fputs(",", output);
  mips_print_operand(output, &instruction->operands[1],false);
  fputs(", $0 \n ", output);

}

void mips_print_load_store(FILE* output, struct ir_instruction* instruction)
{
  const char* opcode = NULL;
  switch(instruction->kind){
  case IR_LOAD_WORD:
    opcode = "lw";break;
  case IR_LOAD_BYTE:
    opcode = "lb";break;
  case IR_LOAD_HALF_WORD:
    opcode = "lh";break;
  case IR_STORE_WORD:
    opcode = "sw";break;
  case IR_STORE_BYTE:
    opcode = "sb";break;
  case IR_STORE_HALF_WORD:
    opcode = "sh";break;
  case IR_CONST_INT:
    opcode = "li";break;
  case IR_ADDRESS_OF:
    opcode = "la";  break;
  default:
    assert(0);
    break;
  }

  fprintf(output, "%10s",opcode);
  
  mips_print_operand(output, &instruction->operands[0],false);
  fputs(",", output);
  mips_print_operand(output, &instruction->operands[1],true);
  fputs("\n ", output);

}
static int get_system_call_code(struct ir_operand* operand)
{
  int idx = 0;
  int syscall_code = -1;
  static char *sys_fcn_names[6]= {
    "print_int",
    "print_string",
    "read_int",
    "read_string",
    "sbrk",
    "exit"
  };
  const char* name = NULL;
  assert(OPERAND_STR == operand->kind);
  name = operand->data.str;
  for(; idx < 6; idx++) {
    if ( 0 == strcmp(name, sys_fcn_names[idx])){
      break;
    }
  }
  assert(idx<6);

  switch(idx) {
  case 0: 
    syscall_code = 1;
    break;
  case 1:
    syscall_code = 4;
    break;
  case 2:
    syscall_code = 5;
    break;
  case 3:
    syscall_code = 8;
    break;
  case 4:
    syscall_code = 9;
    break;
  case 5:
    syscall_code = 10;
    break;
  default:assert(0);break;  
  }

  return syscall_code;
}

void mips_print_function_invocation(FILE* output, struct ir_instruction* instruction)
{
  switch(instruction->kind)
    {
    case IR_PARAMETER: {
      /*Parameter */
      int param_idx = instruction->operands[0].data.number;
      int temp_reg = instruction->operands[1].data.temporary;
      struct temp_reg_pair* pair = get_reg(temp_reg);
      fprintf(output,"#%s\n", "Parameter");
      fprintf(output, "%10s %10s%d, %10s%d, %10s\n",
	      "or", "$a", param_idx,(pair->saved_regs)?"$s":"$t",
	      pair->reg_idx, "$0");
      break;
    }
    case IR_RESULT_WORD:    {
      /*Result */
      int temp_reg = instruction->operands[0].data.temporary;
      struct temp_reg_pair* pair = get_reg(temp_reg);
      fprintf(output,"#%s\n", "Result");
      fprintf(output, "%10s %10s%d, %10s, %10s\n",
	      "or", (pair->saved_regs)?"$s":"$t",pair->reg_idx,
	      "$v0","$0");
      break;
    }
    case IR_CALL: {
	/*!!!!TODO- Save any temporary words used!!!!!*/
	fprintf(output,"%10s      ", "jal");
	mips_print_operand(output, &instruction->operands[0],false);
	fputs("\n",output);

	/*!!!!TODO- restore any temporary words after the call!!!*/
	break;
      }
    case IR_SYS_CALL:{
      int code = get_system_call_code(&instruction->operands[0]);
      fprintf(output,"#%10s\n", "syscall");
      fprintf(output,"%10s %10s, %10d\n", "li","$v0",code);
      fprintf(output,"%10s \n", "syscall");
      break;
    }
    case IR_RETURN_WORD:
      {
	struct temp_reg_pair* pair = get_reg
	  (instruction->operands[0].data.temporary);
	fprintf(output,"%10s %10s, %10s%d,  %10s\n", "or", 
		"$v0", (pair->saved_regs)?"$s":"$t", pair->reg_idx,
		"$0");
	break;
      }
    default:
	assert(0);break;
    }
}
void mips_print_misc_instruction(FILE *output, struct ir_instruction *instruction)
{
  switch(instruction->kind){
  case IR_NEGATION: {
    struct temp_reg_pair* pair1 = get_reg
      (instruction->operands[0].data.temporary);
    struct temp_reg_pair* pair2 = get_reg
      (instruction->operands[1].data.temporary);
    struct temp_reg_pair* pair3 = get_reg
      (instruction->operands[2].data.temporary);

    fprintf(output,"%10s %10s%d, %10s%d, %10s%d\n", "neg", 
	    (pair1->saved_regs)?"$s":"$t", pair1->reg_idx,
	    (pair2->saved_regs)?"$s":"$t", pair2->reg_idx,
	    (pair3->saved_regs)?"$s":"$t", pair3->reg_idx);
    break;
  }
  case IR_LOGICAL_NOT: {
    struct temp_reg_pair* pair1 = get_reg
      (instruction->operands[0].data.temporary);
    struct temp_reg_pair* pair2 = get_reg
      (instruction->operands[1].data.temporary);
    struct temp_reg_pair* pair3 = get_reg
      (instruction->operands[2].data.temporary);
    
    fprintf(output,"%10s %10s%d, %10s%d, %10s%d\n", "negu", 
	    (pair1->saved_regs)?"$s":"$t", pair1->reg_idx,
	    (pair2->saved_regs)?"$s":"$t", pair2->reg_idx,
	    (pair3->saved_regs)?"$s":"$t", pair3->reg_idx);
    break;
  }
  default:
    assert(0);break;
  }
}

void mips_print_instruction(FILE *output, struct ir_instruction *instruction) {
  switch (instruction->kind) {
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
  case IR_REMAINDER:
  case IR_REMAINDERU:
    mips_print_arithmetic(output, instruction);
    break;

    
    /*Function definition demarcation*/
  case IR_PROC_BEGIN:
    mips_print_function_begin(output, instruction);
    break;
  case IR_PROC_END:
    mips_print_function_end(output, instruction);
    break;


/*Shift R-type instructions*/
  case IR_SHIFT_L:
  case IR_SHIFT_R:
  case IR_SHIFT_RU:
  case IR_SHIFT_LU:
    mips_print_bit_shift(output, instruction);
    break;

    /*Branch instructions*/  
  case IR_GOTO:
  case IR_GOTO_FALSE:
  case IR_GOTO_TRUE:
    mips_print_branching(output, instruction);
    break;
  
    /*label definition*/
  case IR_LABEL:
    mips_print_label(output, instruction);
    break;
    
    /*Casting*/
  case IR_CAST_WORD_TO_HALF_WORD:
  case IR_CAST_WORD_TO_BYTE:
  case IR_CAST_HALF_WORD_TO_UWORD:
  case IR_CAST_HALF_WORD_TO_WORD:
  case IR_CAST_BYTE_TO_UWORD:
  case IR_CAST_BYTE_TO_WORD:
    mips_print_cast(output, instruction);
    break;

  /*Load Store operations*/
  case IR_LOAD_WORD:
  case IR_LOAD_BYTE:
  case IR_LOAD_HALF_WORD:
  case IR_STORE_WORD:
  case IR_STORE_BYTE:
  case IR_STORE_HALF_WORD:
  case IR_CONST_INT:
  case IR_ADDRESS_OF:
    mips_print_load_store(output, instruction);
    break;

  case IR_TRANSFER_VALUE:
    mips_print_transfer_value(output, instruction);
    break;

  /*Function invocation*/
  case IR_PARAMETER:
  case IR_RESULT_WORD:
  case IR_CALL:
  case IR_SYS_CALL:
  case IR_RETURN_WORD:
    mips_print_function_invocation(output, instruction);
    break;

  /*Miscellaneous*/
  case IR_NEGATION:
  case IR_LOGICAL_NOT:
    mips_print_misc_instruction(output, instruction);
    break;

  case IR_SEQUENCE_POINT:
  reset_register_counter();
  break;
    /* case IR_LOAD_IMMEDIATE: */
    /*   mips_print_load_immediate(output, instruction); */
    /*   break; */
  case IR_NO_OPERATION:
    break;

  default:
    assert(0);
    break;
  }
}
void mips_print_data_section(FILE *output) {
  /*Print strings here*/

  {
    struct _global_string_list* curr_list = 
      global_string_repository.list;

    fprintf(output, "\n%10s\n", ".data");

    while(NULL != curr_list){
      /*Attach all strings*/
      /*!!!!Need to take care of any null characters coming in between!!!!*/
      fprintf(output,"%s: .asciiz \" %s \"\n", 
	      curr_list->label, curr_list->string);
      curr_list = curr_list->next;
    }
  }
  /*Global variables also assigned here*/
  {
    struct _global_decl_list* curr_decl = global_decl_list.list;
      while(NULL != curr_decl){
	/*Attach any identified strings here*/
	fprintf(output,"%s: %s %d \n", 
		curr_decl->name, curr_decl->type_name,
		curr_decl->space);
	curr_decl = curr_decl->next;
      }
  }
}

void mips_print_text_section(FILE *output, struct ir_section *section) {
  struct ir_instruction *instruction;

  fprintf(output, "\n%10s\n", ".text");

  fprintf(output,"\n %10s %10s\n", ".globl", " main");

  for (instruction = section->first; instruction != section->last->next; instruction = instruction->next) {
    mips_print_instruction(output, instruction);
  }
}

void mips_print_program(FILE *output, 
			struct ir_section *section) 
{
  /*Print the global data section*/
  mips_print_data_section(output);
  mips_print_text_section(output, section);

  run_once = true;/*Subsequent prints don't increment variables unecessarily*/
      
}
