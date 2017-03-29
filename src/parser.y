%verbose
%debug

%{

  #include <stdio.h>
  #include "node.h"
  #include<assert.h>
  #include "type.h"

  int yylex();
  extern int yylineno;
  void yyerror(char const *s);

  #define YYSTYPE struct node *
  #define YYERROR_VERBOSE

  extern struct node *root_node;
  struct node* unimplemented();
%}

%token MINUS_EQUAL  PERCENT_EQUAL PLUS_EQUAL
%token SLASH_EQUAL VBAR_EQUAL VBAR_VBAR

%token BREAK CHAR CONTINUE DO ELSE FOR GOTO IF
%token INT LONG RETURN SHORT SIGNED UNSIGNED VOID WHILE

%token LEFT_PAREN RIGHT_PAREN LEFT_SQUARE RIGHT_SQUARE LEFT_CURLY RIGHT_CURLY

%token AMPERSAND ASTERISK CARET COLON COMMA EQUAL EXCLAMATION GREATER
%token LESS MINUS PERCENT PLUS SEMICOLON SLASH QUESTION TILDE VBAR

%token AMPERSAND_AMPERSAND AMPERSAND_EQUAL ASTERISK_EQUAL CARET_EQUAL
%token  GREATER_EQUAL GREATER_GREATER
%token GREATER_GREATER_EQUAL LESS_EQUAL LESS_LESS LESS_LESS_EQUAL

%token EQUAL_EQUAL EXCLAMATION_EQUAL

 /****DEFINE THE LEFT AND RIGHT ASSOCIATIVITY OF ALL OPERATIONS****/
%left COMMA
%right EQUAL PLUS_EQUAL MINUS_EQUAL PERCENT_EQUAL ASTERISK_EQUAL SLASH_EQUAL VBAR_EQUAL CARET_EQUAL LESS_LESS_EQUAL GREATER_GREATER_EQUAL
/* %right conditional_expr */
%left VBAR_VBAR
%left AMPERSAND_AMPERSAND
%left VBAR
%left CARET
%left AMPERSAND
%left EQUAL_EQUAL EXCLAMATION_EQUAL
%left GREATER LESS GREATER_EQUAL LESS_EQUAL
%left GREATER_GREATER LESS_LESS
%left PLUS MINUS
%left ASTERISK SLASH PERCENT
%right EXCLAMATION
%right TILDE

%token MINUS_MINUS PLUS_PLUS
%token IDENTIFIER NUMBER STRING
%nonassoc IF
%nonassoc ELSE
 /*LET THE RULES DEFINE THE ASSOCIATIVITY FOR THE REST OF THE OPERATIONS LIKE ADDRESS, INDIRECTION,ETC*/
/* %precedence postfix_expr */

%start program

%%

abstract_declarator
     : pointer
     { $$ = node_abstract_declarator($1, NULL);}
     | pointer direct_abstract_declarator
     { $$ = node_abstract_declarator($1, $2);}
     | direct_abstract_declarator
     { $$ = node_abstract_declarator(NULL, $1);}

additive_expr
     : multiplicative_expr
     { $$ = $1;}
     | additive_expr PLUS multiplicative_expr
     { $$ = node_binary_operation(BINOP_ADDITION, $1, $3);}
     | additive_expr MINUS multiplicative_expr
     { $$ = node_binary_operation(BINOP_SUBTRACTION, $1, $3);}

address_expr
   :  AMPERSAND cast_expr
   { $$ = node_unary_operation(UNARY_ADDRESS, $2);}

array_declarator
   :  direct_declarator LEFT_SQUARE constant_expr RIGHT_SQUARE
   { $$ = node_array_declarator($1, $3);}
   |  direct_declarator LEFT_SQUARE RIGHT_SQUARE
   { $$ = node_array_declarator($1,NULL);}

assignment_expr
   :  conditional_expr
   {$$ = $1;}
   |  unary_expr EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_ASSIGN, $1, $3);}
   |  unary_expr PLUS_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_ADD_ASSIGN, $1, $3);}
   |  unary_expr MINUS_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_SUBTRACT_ASSIGN, $1, $3);}
   |  unary_expr ASTERISK_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_MULTIPLY_ASSIGN, $1, $3);}
   |  unary_expr SLASH_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_DIVIDE_ASSIGN, $1, $3);}
   |  unary_expr PERCENT_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_REMAINDER_ASSIGN, $1, $3);}
   |  unary_expr LESS_LESS_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_BITSHIFT_LEFT_ASSIGN, $1, $3);}
   |  unary_expr GREATER_GREATER_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_BITSHIFT_RIGHT_ASSIGN, $1, $3);}
   |  unary_expr AMPERSAND_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_AND_ASSIGN, $1, $3);}
   |  unary_expr CARET_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_XOR_ASSIGN, $1, $3);}
   |  unary_expr VBAR_EQUAL assignment_expr
   { $$ = node_binary_operation(BINOP_OR_ASSIGN, $1, $3);}

bitwise_and_expr
   :  equality_expr {$$ = $1;}
   |  bitwise_and_expr AMPERSAND equality_expr
   { $$ = node_binary_operation(BINOP_BITWISE_AND, $1, $3);}

bitwise_negation_expr
   :  TILDE cast_expr
   { $$ = node_unary_operation(UNARY_BITWISE_NEGATION, $2); }

bitwise_or_expr
   :  bitwise_xor_expr {$$ = $1;}
   |  bitwise_or_expr VBAR bitwise_xor_expr
   { $$ = node_binary_operation(BINOP_BITWISE_OR, $1, $3); }

bitwise_xor_expr
   :  bitwise_and_expr {$$ = $1;}
   |  bitwise_xor_expr CARET bitwise_and_expr
   {$$ = node_binary_operation(BINOP_BITWISE_XOR, $1, $3);}

break_statement
   :  BREAK SEMICOLON {$$ = node_break();}

cast_expr
   :  unary_expr {$$ = $1;}
   |  LEFT_PAREN type_name RIGHT_PAREN cast_expr
   { $$ = node_cast($2, $4, NULL, false);}

character_type_specifier
   : CHAR
   { $$ = node_type(TYPE_BASIC, TYPE_WIDTH_CHAR, true);}
   | SIGNED CHAR
   { $$ = node_type(TYPE_BASIC, TYPE_WIDTH_CHAR, true);}
   | UNSIGNED CHAR
   { $$ = node_type(TYPE_BASIC, TYPE_WIDTH_CHAR, false);}

comma_expr
   :  assignment_expr
   {$$ = $1;}
   |  comma_expr COMMA assignment_expr
   {$$ = node_comma_expr($1, $3);}

compound_statement
   :  LEFT_CURLY declaration_or_statement_list RIGHT_CURLY
   {$$ = node_compound_statement($2);}
   |  LEFT_CURLY RIGHT_CURLY
   {$$ = node_compound_statement(NULL);}

conditional_expr
   :  logical_or_expr
   { $$ = $1;}
   |  logical_or_expr QUESTION expr COLON conditional_expr
   { $$ = node_if_else_trenary($1, $3, $5);}

conditional_statement
   :  IF LEFT_PAREN expr RIGHT_PAREN statement
   { $$ = node_if($3, $5);}
   |  IF LEFT_PAREN expr RIGHT_PAREN statement ELSE statement
   { $$ = node_if_else($3,$5,$7);}

constant_expr
   : conditional_expr {$$ = $1;}

continue_statement
   :  CONTINUE SEMICOLON {$$ = node_continue();}

decl
   :  declaration_specifiers initialized_declarator_list SEMICOLON
   { $$ = node_decl($1,$2);}

declaration_or_statement
   : decl {$$ = $1;}
   | statement {$$ = $1;}

declaration_or_statement_list
   :  declaration_or_statement
   { $$ = node_decl_or_statement_list(NULL, $1); }
   |  declaration_or_statement_list declaration_or_statement
   { $$ = node_decl_or_statement_list($1, $2); }

declaration_specifiers
   :  type_specifier {$$=$1;}

declarator
   :  pointer_declarator {$$ = $1;}
   |  direct_declarator {$$ = $1;}

direct_abstract_declarator
   :  LEFT_PAREN abstract_declarator RIGHT_PAREN
   { $$ = node_direct_abstract_declarator($2, NULL,
					  DIRECT_ABS_DECL_PARENTHESIZED);
   }
   | direct_abstract_declarator LEFT_SQUARE constant_expr RIGHT_SQUARE
   { $$ = node_direct_abstract_declarator($1, $3,
					  DIRECT_ABS_DECL_SQUARE);}
   |  LEFT_SQUARE constant_expr RIGHT_SQUARE
   { $$ = node_direct_abstract_declarator(NULL, $2,
					  DIRECT_ABS_DECL_SQUARE);}
   |  direct_abstract_declarator LEFT_SQUARE RIGHT_SQUARE
   { $$ = node_direct_abstract_declarator($1, NULL,
					  DIRECT_ABS_DECL_SQUARE);}

direct_declarator
   :  simple_declarator {$$ = $1;}
   |  LEFT_PAREN declarator RIGHT_PAREN {$$ = $2;}
   |  function_declarator {$$ = $1;}
   |  array_declarator {$$ = $1;}

do_statement
  :  DO statement WHILE LEFT_PAREN expr RIGHT_PAREN SEMICOLON
  { $$ =  node_do_while($2, $5); }

equality_expr
   :  relational_expr {$$ = $1;}
   |  equality_expr EQUAL_EQUAL relational_expr
   { $$ = node_binary_operation(BINOP_EQUALITY,
				$1, $3);}
   |  equality_expr EXCLAMATION_EQUAL relational_expr
   { $$ = node_binary_operation(BINOP_INEQUALITY,
				$1, $3);}

expr
   :  comma_expr {$$ = $1;}

expression_list
   :  assignment_expr {$$ = $1;}
   |  expression_list COMMA assignment_expr
   {$$ = node_expression_list($1, $3);}

expression_statement
   :  expr SEMICOLON {$$ = $1;}
   |  error SEMICOLON {yyerrok;}

for_statement
   :  FOR LEFT_PAREN initial_clause SEMICOLON expr
   SEMICOLON expr RIGHT_PAREN statement
   {$$ = node_for_expr_loop($3, $5, $7, $9);}
   |  FOR LEFT_PAREN SEMICOLON expr SEMICOLON expr
   RIGHT_PAREN statement
   {$$ = node_for_expr_loop(NULL, $4, $6, $8);}
   |  FOR LEFT_PAREN initial_clause SEMICOLON
   SEMICOLON expr RIGHT_PAREN statement
   {$$ = node_for_expr_loop($3, NULL, $6, $8);}
   |  FOR LEFT_PAREN initial_clause SEMICOLON expr SEMICOLON RIGHT_PAREN statement
   {$$ = node_for_expr_loop($3, $5, NULL, $8);}
   |  FOR LEFT_PAREN SEMICOLON SEMICOLON expr RIGHT_PAREN statement
   {$$ = node_for_expr_loop(NULL, NULL, $5, $7);}
   |  FOR LEFT_PAREN initial_clause SEMICOLON SEMICOLON RIGHT_PAREN statement
   {$$ = node_for_expr_loop($3,NULL, NULL, $7);}
   |  FOR LEFT_PAREN SEMICOLON expr SEMICOLON RIGHT_PAREN statement
   {$$ = node_for_expr_loop(NULL, $4, NULL, $7);}
   |  FOR LEFT_PAREN SEMICOLON SEMICOLON RIGHT_PAREN statement
   {$$ = node_for_expr_loop(NULL,NULL, NULL, $6);}

function_call
   :  postfix_expr LEFT_PAREN expression_list RIGHT_PAREN
   { $$ = node_function_call($1, $3); }
   | postfix_expr LEFT_PAREN  RIGHT_PAREN
   { $$ = node_function_call($1, node_type(TYPE_VOID, 0, true ));}

function_declarator
   :  direct_declarator LEFT_PAREN parameter_type_list RIGHT_PAREN
    {$$ = node_function_declarator($1,$3);}
   /* |  direct_declarator LEFT_PAREN  RIGHT_PAREN */
   /* { $$ = node_function_declarator($1, node_type(TYPE_VOID, 0, true ));} */

function_definition
   :  function_def_specifier compound_statement
   {$$ = node_function_definition($1, $2);}

function_def_specifier
   :  declaration_specifiers declarator
   {  $$= node_function_def_specifier($1,$2); }
   /* |  declarator */
   /* {  $$ = node_function_def_specifier(node_type(TYPE_BASIC, */
   /* 						 TYPE_WIDTH_INT, */
   /* 						 true), $1); } */

goto_statement
   :  GOTO named_label SEMICOLON
   {$$ = node_goto($2);}

indirection_expr
   :  ASTERISK cast_expr
   {$$ = node_unary_operation(UNARY_INDIRECTION, $2);}

initial_clause
   :  expr {$$ = $1;}
   |  decl {$$ = $1;}

initialized_declarator
   :  declarator {$$ = $1;}

initialized_declarator_list
   :  initialized_declarator
   {$$ =  $1;}
   |  initialized_declarator_list COMMA initialized_declarator
   {$$ = node_initialized_declarator($1, $3);}

integer_type_specifier
   :  signed_type_specifier {$$ = $1;}
   |  unsigned_type_specifier {$$ = $1;}
   |  character_type_specifier {$$ = $1;}

iterative_statement
   :  while_statement {$$ = $1;}
   |  do_statement {$$ = $1;}
   |  for_statement {$$ = $1;}

label
   :  named_label
   {$$ = node_label($1);}

labeled_statement
   :  label COLON statement
   {$$ = node_labeled_statement($1, $3);}

logical_and_expr
   :  bitwise_or_expr {$$ = $1;}
   |  logical_and_expr AMPERSAND_AMPERSAND bitwise_or_expr
   { $$ = node_binary_operation(BINOP_AND,
				$1, $3);}

logical_negation_expr
   :  EXCLAMATION cast_expr
   {$$ = node_unary_operation(UNARY_NOT, $2);}

logical_or_expr
   :  logical_and_expr {$$ = $1;}
   |  logical_or_expr VBAR_VBAR logical_and_expr
   {$$ = node_binary_operation(BINOP_OR,
				$1, $3);}

multiplicative_expr
   :  cast_expr {$$ = $1;}
   |  multiplicative_expr ASTERISK cast_expr
   {$$ = node_binary_operation(BINOP_MULTIPLICATION, $1, $3);}
   |  multiplicative_expr SLASH cast_expr
   {$$ = node_binary_operation(BINOP_DIVISION, $1, $3);}
   |  multiplicative_expr PERCENT cast_expr
   {$$ = node_binary_operation(BINOP_REMAINDER, $1, $3);}

named_label
   :  IDENTIFIER {$$ = $1;}

null_statement
   :  SEMICOLON {/*No operation*/}

/* octal* see grammar*/

parameter_decl
   :  declaration_specifiers declarator
   { $$ = node_parameter_decl($1, $2);}
   |  declaration_specifiers abstract_declarator
   { $$ = node_parameter_decl($1, $2);}
   |  declaration_specifiers
   { $$ = node_parameter_decl($1, NULL);}

parameter_list
   :  parameter_decl
   { $$ = $1;}
   |  parameter_list COMMA parameter_decl
   { $$ = node_prm_list($1, $3);}

parameter_type_list
   :  parameter_list
   {$$ = $1;}

parenthesized_expr
   :  LEFT_PAREN expr RIGHT_PAREN {$$=$2;}

pointer
   :  ASTERISK {$$ =  node_pointer(NULL);}
   |  ASTERISK pointer {$$ =  node_pointer($2);}

pointer_declarator
   :  pointer direct_declarator
   {$$ = node_pointer_declarator($1,$2);}

postdecrement_expr
   :  postfix_expr MINUS_MINUS
   { $$ = node_unary_operation(UNARY_POST_DEC,
			       $1);}

postfix_expr
   : primary_expr {$$ = $1;}
   | subscript_expr {$$ = $1;}
   | function_call {$$ = $1;}
   | postincrement_expr {$$ = $1;}
   | postdecrement_expr {$$ = $1;}

postincrement_expr
   :  postfix_expr PLUS_PLUS
   {$$ = node_unary_operation(UNARY_POST_INC, $1);}

predecrement_expr
   :  MINUS_MINUS unary_expr
   {$$ = node_unary_operation(UNARY_PRE_DEC, $2);}

preincrement_expr
   :  PLUS_PLUS unary_expr
   {$$ = node_unary_operation(UNARY_PRE_INC, $2);}

primary_expr
   : IDENTIFIER {$$ = $1;}
   | NUMBER {$$ = $1;}
   | STRING {$$ = $1;}
   | parenthesized_expr {$$ = $1;}

program: translation_unit {root_node = $1;}

relational_expr
   :  shift_expr {$$ = $1;}
   |  relational_expr LESS shift_expr
   {$$ = node_binary_operation(BINOP_LESS, $1, $3);}
   |  relational_expr LESS_EQUAL shift_expr
   {$$ = node_binary_operation(BINOP_LESS_EQUAL, $1, $3);}
   |  relational_expr GREATER shift_expr
   {$$ = node_binary_operation(BINOP_GREATER, $1, $3);}
   |  relational_expr GREATER_EQUAL shift_expr
   {$$ = node_binary_operation(BINOP_GREATER, $1, $3);}

return_statement
   :  RETURN expr SEMICOLON
   { $$ = node_return($2);}
   |  RETURN SEMICOLON
   { $$ = node_return(NULL); }

shift_expr
   :  additive_expr {$$ = $1;}
   |  shift_expr GREATER_GREATER additive_expr
   {  $$ = node_binary_operation(BINOP_BITSHIFT_RIGHT, $1, $3);}
   |  shift_expr LESS_LESS additive_expr
   {  $$ = node_binary_operation(BINOP_BITSHIFT_LEFT, $1, $3);}

  /*Using int size for everyone for now */
signed_type_specifier
   :  SHORT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
   |  SHORT INT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
   |  SIGNED SHORT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
   |  SIGNED SHORT INT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
   |  INT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_INT,true);}
   |  SIGNED INT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_INT,true);}
   |  SIGNED {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_INT,true);}
   |  LONG {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
   |  LONG INT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
   |  SIGNED LONG {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
   |  SIGNED LONG INT {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}

simple_declarator
   :  IDENTIFIER {$$ = $1;} /*Does this need a separate node??*/

statement
   :   expression_statement {$$ = $1;}
   |   labeled_statement {$$ = $1;}
   |   compound_statement {$$ = $1;}
   |   conditional_statement {$$ = $1;}
   |   iterative_statement {$$ = $1;}
   |   break_statement {$$ = $1;}
   |   continue_statement {$$ = $1;}
   |   return_statement {$$ = $1;}
   |   goto_statement {$$ = $1;}
   |   null_statement {$$ = $1;}

subscript_expr
   :  postfix_expr LEFT_SQUARE expr RIGHT_SQUARE
   { $$ = node_subscript_expr($1, $3);}

top_level_decl
   :  decl {$$ = $1;}
   |  function_definition {$$ = $1;}

translation_unit
   :  top_level_decl
   {$$ = node_translation_unit(NULL, $1); }
   |  translation_unit top_level_decl
   {$$ = node_translation_unit($1, $2);}

/*typedef*/
type_name
   :  declaration_specifiers abstract_declarator
   {$$ = node_type_name($1,$2);}
   |  declaration_specifiers
   {$$ = node_type_name($1,NULL);}

type_specifier
   :  integer_type_specifier {$$ = $1;}
   |  void_type_specifier {$$ = $1;}

unary_expr
   :  postfix_expr {$$ = $1;}
   |  unary_minus_expr {$$ = $1;}
   |  unary_plus_expr {$$ = $1;}
   |  logical_negation_expr {$$ = $1;}
   |  bitwise_negation_expr {$$ = $1;}
   |  address_expr {$$ = $1;}
   |  indirection_expr {$$ = $1;}
   |  preincrement_expr {$$ = $1;}
   |  predecrement_expr {$$ = $1;}

unary_minus_expr
   :  MINUS cast_expr
   {$$ = node_unary_operation(UNARY_MINUS, $2);}

unary_plus_expr
   : PLUS cast_expr
   {$$ = node_unary_operation(UNARY_PLUS, $2);}

unsigned_type_specifier
   : UNSIGNED SHORT INT
   {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT, false);}
   | UNSIGNED SHORT
   {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT, false);}
   | UNSIGNED INT
   {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_INT, false);}
   | UNSIGNED
   {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_INT, false);}
   | UNSIGNED LONG INT
   {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_LONG, false);}
   | UNSIGNED LONG
   {$$ = node_type(TYPE_BASIC,TYPE_WIDTH_LONG, false);}

void_type_specifier
   :
   VOID
   {$$ = node_type(TYPE_VOID, 0, true );}

while_statement
   :  WHILE LEFT_PAREN expr RIGHT_PAREN statement
   { $$ = node_while_statement($3, $5);}

%%

void yyerror(char const *s) {
  fprintf(stderr, "ERROR at line %d: %s\n", yylineno, s);
}
struct node* unimplemented()
{
  assert(0);
  return NULL;
}
