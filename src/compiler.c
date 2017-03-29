#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "node.h"
#include "symbol.h"
#include "type.h"
#include "ir.h"
#include "mips.h"


#define YYSTYPE struct node *
#include "scanner.h"
#include "parser.h"

int yyparse();
extern int yynerrs;

extern int errno;

struct node *root_node;

static void print_errors_from_pass(FILE *output, char *pass, int num_errors) {
  fprintf(output, "%s encountered %d %s.\n",
          pass, num_errors, (num_errors == 1 ? "error" : "errors"));
}

int scan_only(FILE *output) {
  /* Begin scanning. */
  int num_errors = 0;
  int token = yylex();

  while (0 != token) {
    char *token_type; const char *token_name;

    /*
     * Print the line number. Use printf formatting and tabs to keep columns
     * lined up.
     */
    fprintf(output, "line = %-5d", yylineno);

    /*
     * Print the scanned text. Try to use formatting but give up instead of
     * truncating if the text is too long.
     */
    if (yyleng <= 20) {
      fprintf(output, "   text = %-20s", yytext);
    } else {
      fprintf(output, "   text = %s", yytext);
    }

    if (token > 0) {
      /* Found a token! */

      /* To avoid issues of mapping enums to string array, *
	 using dumb switch case here for getting token name*/

      /* Look up and print the token's name. */
      /* Print information about the token. */
      switch (token) {
      case CHAR:    token_type = "rsvwd";token_name = "CHAR";break;
      case SHORT:   token_type = "rsvwd";token_name = "SHORT";break;
      case INT:     token_type = "rsvwd";token_name = "INT";break;
      case LONG:    token_type = "rsvwd";token_name = "LONG";break;
      case SIGNED:  token_type = "rsvwd";token_name = "SIGNED";break;
      case UNSIGNED:token_type = "rsvwd";token_name = "UNSIGNED";break;
      case VOID:    token_type = "rsvwd";token_name = "VOID";break;

      case DO:      token_type = "rsvwd";token_name = "DO";break;
      case FOR:     token_type = "rsvwd";token_name = "FOR";break;
      case RETURN:  token_type = "rsvwd";token_name = "RETURN";break;
      case BREAK:   token_type = "rsvwd";token_name = "BREAK";break;
      case CONTINUE:token_type = "rsvwd";token_name = "CONTINUE";break;
      case WHILE:   token_type = "rsvwd";token_name = "WHILE";break;

      case IF:      token_type = "rsvwd";token_name = "IF";break;
      case ELSE:    token_type = "rsvwd";token_name = "ELSE";break;
      case GOTO:    token_type = "rsvwd";token_name = "GOTO";break;

      case ASTERISK:token_type = "op"; token_name = "ASTERISK";break;
      case MINUS:   token_type = "op"; token_name = "MINUS";break;
      case PLUS:    token_type = "op"; token_name = "PLUS";break;
      case EQUAL:   token_type = "op"; token_name = "EQUAL";break;
      case SLASH:   token_type = "op"; token_name = "SLASH";break;
      case LEFT_PAREN:token_type = "op"; token_name = "LEFT_PAREN";break;
      case RIGHT_PAREN:token_type = "op"; token_name = "RIGHT_PAREN";break;
      case LEFT_SQUARE:token_type = "op"; token_name = "LEFT_SQUARE";break;
      case RIGHT_SQUARE:token_type = "op"; token_name = "RIGHT_SQUARE";break;
      case LEFT_CURLY:token_type = "op"; token_name = "LEFT_CURLY";break;
      case RIGHT_CURLY:token_type = "op"; token_name = "RIGHT_CURLY";break;
      case AMPERSAND:token_type = "op"; token_name = "AMPERSAND";break;
      case SEMICOLON:token_type = "op"; token_name = "SEMICOLON";break;
      case CARET:    token_type = "op"; token_name = "CARET";break;
      case COLON:    token_type = "op"; token_name = "COLON";break;
      case COMMA:    token_type = "op"; token_name = "COMMA";break;
      case EXCLAMATION:token_type = "op"; token_name = "EXCLAMATION";break;
      case GREATER:  token_type = "op"; token_name = "GREATER";break;
      case LESS:     token_type = "op"; token_name = "LESS";break;
      case PERCENT:  token_type = "op"; token_name = "PERCENT";break;
      case QUESTION: token_type = "op"; token_name = "QUESTION";break;
      case TILDE:    token_type = "op"; token_name = "TILDE";break;
      case VBAR:     token_type = "op"; token_name = "VBAR";break;
      case AMPERSAND_AMPERSAND:token_type = "op"; token_name = "AMPERSAND_AMPERSAND";break;
      case AMPERSAND_EQUAL:token_type = "op"; token_name = "AMPERSAND_EQUAL";break;
      case ASTERISK_EQUAL:token_type = "op"; token_name = "ASTERISK_EQUAL";break;
      case CARET_EQUAL:   token_type = "op"; token_name = "CARET_EQUAL";break;
      case EQUAL_EQUAL:   token_type = "op"; token_name = "EQUAL_EQUAL";break;
      case EXCLAMATION_EQUAL: token_type = "op"; token_name = "EXCLAMATION_EQUAL";break;
      case GREATER_EQUAL: token_type = "op"; token_name = "GREATER_EQUAL";break;
      case GREATER_GREATER:token_type = "op"; token_name = "GREATER_GREATER";break;
      case GREATER_GREATER_EQUAL:token_type = "op"; token_name = "GREATER_GREATER_EQUAL";break;
      case LESS_EQUAL:     token_type = "op"; token_name = "LESS_EQUAL";break;
      case LESS_LESS:      token_type = "op"; token_name = "LESS_LESS";break;
      case LESS_LESS_EQUAL:token_type = "op"; token_name = "LESS_LESS_EQUAL";break;
      case MINUS_EQUAL:    token_type = "op"; token_name = "MINUS_EQUAL";break;
      case MINUS_MINUS:    token_type = "op"; token_name = "MINUS_MINUS";break;
      case PERCENT_EQUAL:  token_type = "op"; token_name = "PERCENT_EQUAL";break;
      case PLUS_EQUAL:     token_type = "op"; token_name = "PLUS_EQUAL";break;
      case PLUS_PLUS:      token_type = "op"; token_name = "PLUS_PLUS";break;
      case SLASH_EQUAL:    token_type = "op"; token_name = "SLASH_EQUAL";break;
      case VBAR_EQUAL:     token_type = "op"; token_name = "VBAR_EQUAL";break;
      case VBAR_VBAR:      token_type = "op"; token_name = "VBAR_VBAR";break;

      case NUMBER:  token_type = "number"; token_name = "NUMBER";break;

      case IDENTIFIER:	token_type = "id"; token_name = "IDENTIFIER";break;

      case STRING:  token_type = "string"; token_name = "STRING";break;

      default:
	assert(0);
	break;
      }
    } else {
      token_type = "error"; token_name = "ERROR";
      num_errors++;
    }

    fprintf(output, "   %5s = %-20s", token_type, token_name);

    if (0 == strcmp("number", token_type)) {
      /* Print the type and value. */
      const char* typeStr = NULL;
      if (type_is_unsigned(yylval->data.number.result.type))
	typeStr = "UNSIGNED";
      else
	typeStr = "SIGNED";
      fprintf(output, "   type = %8s %-12s   value = %-10lu\n",
              typeStr, "LONG", yylval->data.number.value);
      /* Overflow*/
      if(yylval->data.number.overflow)
	fprintf(output, "\n ERROR:Overflow detected \n");

    } else if (0 == strcmp("id", token_type)) {
        fprintf(output, "    name = %s\n", yylval->data.identifier.name);
    } else if (0 == strcmp("string", token_type)){
      int cnt = 0;
      fprintf(output, "    length = %d, string = [", yylval->data.string.strLen);
      for (cnt = 0; cnt < yylval->data.string.strLen; cnt++)
	fprintf( output, "%d ", yylval->data.string.str[cnt]);
      fprintf(output, "]\n");
    } else {
      fputs("\n", output);
    }

    token = yylex();
  }

  return num_errors;
}

int main(int argc, char **argv) {
  FILE *output = NULL;
  FILE* input = NULL;
  int result = -1;
  struct symbol_table symbol_table;
  char *stage = NULL;
  int opt;

  output = NULL;
  optarg = NULL;
  stage = "mips";
  while (-1 != (opt = getopt(argc, argv, "o:s:i:"))) {
    switch (opt) {
    case 'o':
      output = fopen(optarg, "w");
      if (NULL == output) {
	fprintf(stdout, "Could not open output file %s: %s", optarg, strerror(errno));
	return -1;
      }
      break;
    case 's':
      stage = optarg;
      break;
    case 'i':
      input = fopen(optarg, "r");
      if(input == NULL)
	printf ("Unable to open file %s \n", optarg);
      break;
    }
  }
  /* Figure out whether we're using stdin/stdout or file in/file out. */
  if (input == NULL) {
    yyin = stdin;
  } else {
    yyin = input;
  }

  if (0 == strcmp("scanner", stage)) {
    FILE* loggedOutput = stdout;
    int num_errors = 0;
    int retVal = 0;

    if(output != NULL)
      loggedOutput = output;

    num_errors = scan_only(loggedOutput);

    if (num_errors > 0) {
      print_errors_from_pass(loggedOutput, "Scanner", num_errors);/*As yyerror is not being used here -yynerrs*/
      retVal = 2;
    }
    if(output != NULL)
      fclose(output);
    if(input != NULL)
      fclose(input);

    return retVal;
  }

  if (NULL == output) {
    output = fopen("output.s", "w");
  }

  result = yyparse();
  if (yynerrs > 0) {
    result = 1;
  }
  switch (result) {
    case 0:
      /* Successful parse. */
      break;

    case 1:
      print_errors_from_pass(stdout, "Parser", yynerrs);
      return 1;

    case 2:
      fprintf(stdout, "Parser ran out of memory.\n");
      return 2;
  }
  if (0 == strcmp("parser", stage)) {
    fprintf(stdout, "=============== PARSE TREE ===============\n");
    node_print_translation_unit(stdout, root_node);
    return 0;
  }

  symbol_initialize_table(&symbol_table);
  symbol_add_from_translation_unit(&symbol_table, root_node);
  if (symbol_table_num_errors > 0) {
    print_errors_from_pass(stdout, "Symbol table", symbol_table_num_errors);
    return 3;
  }
  fprintf(stdout, "================= SYMBOLS ================\n");
  symbol_print_table(stdout, &symbol_table);
  if (0 == strcmp("symbol", stage)) {
    fprintf(stdout, "=============== PARSE TREE ===============\n");
    node_print_translation_unit(stdout, root_node);
    return 0;
  }

  type_assign_in_translation_unit(root_node);
  if (type_checking_num_errors > 0) {
    print_errors_from_pass(stdout, "Type checking", type_checking_num_errors);
    return 4;
  }
  fprintf(stdout, "=============== PARSE TREE ===============\n");
  node_print_translation_unit(stdout, root_node);
  if (0 == strcmp("type_check", stage)) {
    return 0;
  }

  ir_generate_translation_unit(root_node, &symbol_table);
  if (ir_generation_num_errors > 0) {
    print_errors_from_pass(stdout, "IR generation", 
			   ir_generation_num_errors);
    return 5;
  }

  fprintf(stdout, "=================== IR ===================\n");
  ir_print_section(stdout, root_node->ir);
  if(!OPTIMIZATION)
    {
      if (0 == strcmp("ir", stage)) {
	return 0;
      }
    }

  if (ir_generation_num_errors > 0) {
    print_errors_from_pass(stdout, "IR", 
			   ir_generation_num_errors);
    
    return 5;
  }
  if(OPTIMIZATION){
    ir_optimize(root_node->ir);
    fprintf(stdout, "=================== IR(OPTIMIZED) ===================\n");
    ir_print_section(stdout, root_node->ir);
    if (0 == strcmp("ir", stage)) {
      return 0;
    }
  }

  fprintf(stdout, "================== MIPS ==================\n");
  mips_print_program(stdout, root_node->ir);
  fputs("\n\n", stdout);

  mips_print_program(output, root_node->ir);
  fputs("\n\n", output);

  return 0;
}
