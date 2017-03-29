%option yylineno
%option nounput

%{
/*
 * scanner.lex
 *
 * This file contains the specification for the (f)lex generated scanner
 * for the CSCI-E295 source language.
 *
 */

  #include <stdlib.h>
  #include <errno.h>
  #include <string.h>

 #define DEBUG 0 /*If you want debug messages printed - for standalone builds */

 #define YYSTYPE struct node *

  #define YY_NO_INPUT

  #include "node.h"
  #include "parser.h"

  #define MAX_STR_CONST 509 /*From C89 standards*/


  void errorFcn(char const* message);
  int echoFcn(int tokenId);
  int nodeFromChar(int charVal);
  extern void yyerror(char const* message);
%}

newline         \n[\r]?
ws              [ \t\v\f]

digit           [[:digit:]]
letter          [[:alpha:]]

graphicalChar   [\$\@\`\*\+\-\/]

id              ({letter}|_)({letter}|{digit}|_)*
number          {digit}+

%x              comment
%x              string
%%
     char string_buf[MAX_STR_CONST];
     char *string_buf_ptr = NULL;
{newline}   /* do nothing; lex is counting lines */
{ws}        /* do nothing */

  /* operators begin */
\*          return echoFcn(ASTERISK);
\+          return echoFcn(PLUS);
\-           return echoFcn(MINUS);
\/          return echoFcn(SLASH);
\=          return echoFcn(EQUAL);
\%           return echoFcn(PERCENT);
\&           return echoFcn(AMPERSAND);
\<           return echoFcn(LESS);
\>           return echoFcn(GREATER);
\?           return echoFcn(QUESTION);
\|           return echoFcn(VBAR);

\&&          return echoFcn(AMPERSAND_AMPERSAND);
\&=          return echoFcn(AMPERSAND_EQUAL);
\*=          return echoFcn(ASTERISK_EQUAL);
\^=          return echoFcn(CARET_EQUAL);
\==          return echoFcn(EQUAL_EQUAL);
\!=          return echoFcn(EXCLAMATION_EQUAL);
\>=          return echoFcn(GREATER_EQUAL);
\>>          return echoFcn(GREATER_GREATER);
\>>=         return echoFcn(GREATER_GREATER_EQUAL);
\<=          return echoFcn(LESS_EQUAL);
\<<          return echoFcn(LESS_LESS);
\<<=         return echoFcn(LESS_LESS_EQUAL);
\-=          return echoFcn(MINUS_EQUAL);
\%=          return echoFcn(PERCENT_EQUAL);
\+=          return echoFcn(PLUS_EQUAL);
\/=          return echoFcn(SLASH_EQUAL);
\|=          return echoFcn(VBAR_EQUAL);
\|\|          return echoFcn(VBAR_VBAR);
\+\+          return echoFcn(PLUS_PLUS);
\-\-         return echoFcn(MINUS_MINUS);

  /* operators end */

  /* Separator*/
\(          return echoFcn(LEFT_PAREN);
\)          return echoFcn(RIGHT_PAREN);
\{           return echoFcn(LEFT_CURLY);
\}           return echoFcn(RIGHT_CURLY);
\[           return echoFcn(LEFT_SQUARE);
\]           return echoFcn(RIGHT_SQUARE);

,           return echoFcn(COMMA);
:           return echoFcn(COLON);
!          return echoFcn(EXCLAMATION);
;            return echoFcn(SEMICOLON);


  /* Handle '0' by itself begin*/

  /* Handle '0' by itself end*/

  /* numeric constants begin */
{number}          {
                   yylval =  node_number(yytext);
                   return echoFcn(NUMBER);}
  /* numeric constants end */

  /* reserved words begin*/
do          return echoFcn(DO);
for         return echoFcn(FOR);
return      return echoFcn(RETURN);
break       return echoFcn(BREAK);
short       return echoFcn(SHORT);
else        return echoFcn(ELSE);
goto        return echoFcn(GOTO);
signed      return echoFcn(SIGNED);
unsigned    return echoFcn(UNSIGNED);
char        return echoFcn(CHAR);
if          return echoFcn(IF);
void        return echoFcn(VOID);
int         return echoFcn(INT);
continue    return echoFcn(CONTINUE);
long        return echoFcn(LONG);
while       return echoFcn(WHILE);
  /* reserved words end*/

  /* identifiers */
{id}        {
             yylval = node_identifier(yytext, yyleng);
             return echoFcn(IDENTIFIER); }

  /* Handle escape sequences*/
  /*"\\n"           {return ('\n');}
    "\\t"           {return ('\t'); }
    "\\r"           {return ('\r'); }
    "\\b"           {return ('\b'); }
    "\\f"           {return ('\f'); }
    "\\v"           {return ('\v');}
    "\\a"           {return ('\a');}
    "\\\'"          {return ('\'');}
    "\\\""          {return ('\"');}
    "\\?"           {return ('\?');}

    \\[0-7]{1,3}  {
    unsigned int result;
    (void) sscanf( yytext + 1, "%o", &result );
    if ( result > 0xff )
    {
    errorFcn("Constant is out of bounds at line");
    }
    *string_buf_ptr++ = result;
    }
    \\{number}      { errorFcn("Constant is out of bounds");} */

 /* Handle character begin*/
\'\'                {errorFcn("empty char invalid"); return -1;}
\'(\\\\)\'           {return nodeFromChar('\\');}
\'(\\n)\'           {return nodeFromChar('\n');}
\'(\\t)\'           {return nodeFromChar('\t');}
\'(\\r)\'           {return nodeFromChar('\r'); }
\'(\\b)\'           {return nodeFromChar('\b'); }
\'(\\f)\'           {return nodeFromChar('\f'); }
\'(\\v)\'           {return nodeFromChar('\v');}
\'(\\a)\'           {return nodeFromChar('\a');}
\'(\\\')\'          {return nodeFromChar('\'');}
\'(\\\")\'          {return nodeFromChar('\"');}
\'"\\?"\'           {return nodeFromChar('\?');}
\'(\\[0-7]{1,3})\'  { unsigned int result ;
                      char* yptr = yytext+2;/*ignoring the quote and back slash in start*/
               	      char* yend = yytext+yyleng-2;/*ignoring single quotes at two ends */
                      result = strtoul(yptr, &(yend),8);
		      if ( result > 0xff )
			{
                        errorFcn("Constant is out of bounds 1");return -1;
			}
		      return nodeFromChar(result);
                     }
\'(\\{number})\'      {errorFcn("Constant is out of bounds 2");}
\'([^\']+)\'          {/*Rest of single characters*/
                       char* yptr = yytext;
                        if (yyleng > 3)
                         {
                            errorFcn("Invalid multi character entry"); return -1;
                         }
			return nodeFromChar(*(yptr+1)); }

  /* Handle character end*/

  /* Handle Strings begin */

\"             {string_buf_ptr = string_buf;BEGIN(string);}

<string>{
  {newline}   {errorFcn("String broken by newline");return -1;}
  <<EOF>>     {errorFcn("Unterminated string ended by EOF");BEGIN(INITIAL);return -1;}
   \"          { *string_buf_ptr++ = '\0';
              #if DEBUG
                 printf("STRING value:%s \n", string_buf);
              #endif

                 yylval = node_string(string_buf, (string_buf_ptr-string_buf)/sizeof(char));
	         BEGIN(INITIAL); return echoFcn(STRING);}
   [^\\\n\"]+   {char * yptr = yytext;
                 while(*yptr)
                   *string_buf_ptr++ = *yptr++;}
   \\\n         /* Do nothing with continuation*/
   \\n          {*string_buf_ptr++ = '\n';}
   \\\\         {*string_buf_ptr++ = '\\';}
   \\t          {*string_buf_ptr++ = '\t';}
   \\r          {*string_buf_ptr++ = '\r';}
   \\b          {*string_buf_ptr++ = '\b';}
   \\f          {*string_buf_ptr++ = '\f';}
   \\v          {*string_buf_ptr++ = '\v';}
   \\a          {*string_buf_ptr++ = '\a';}
   \\\'         {*string_buf_ptr++ = '\'';}
   \\\"         {*string_buf_ptr++ = '\"';}
   \\?          {*string_buf_ptr++ = '\?';}
   \\[0-7]{1,3} {/* octal escape sequence */
		   unsigned int result;
		   (void) sscanf( yytext + 1, "%o", &result );
		   if ( result > 0xff )
		     errorFcn("Constant is out of bounds");
		   *string_buf_ptr++ = result;}
   \\{number}      {errorFcn("Bad escape sequence passed");}
}
  /* Handle Strings end */

  /* Handle Comments begin */
"*/"          {errorFcn("Unmatched comment"); return -1;}
"/*"          {BEGIN(comment);}
<comment>{
      "*/"    {BEGIN(INITIAL);}
      .       {/*Do nothing */}
      <<EOF>> {errorFcn("Comment not terminated");BEGIN(INITIAL);return -1;}
}
  /* Handle Comments end */


  /* Specific Error conditions begin */

  /* 1. Carriage return alone - error*/

  /* 2. Carriage return alone - error*/
  /* Error conditions end*/
<<EOF>>     {
             #if DEBUG
              printf("Successful completion\n");
             #endif
             return 0;}
.           {errorFcn(" Unmatched rule");return -1;}
%%
int echoFcn(int tokenId)
{
#if DEBUG
  static const char *const debugIDName[] =
  { "IDENTIFIER", "NUMBER", "STRING",
  "BREAK", "CHAR", "CONTINUE", "DO", "ELSE", "FOR", "GOTO", "IF", "INT",
  "LONG", "RETURN", "SHORT", "SIGNED", "UNSIGNED", "VOID", "WHILE",
  "LEFT_PAREN", "RIGHT_PAREN", "LEFT_SQUARE", "RIGHT_SQUARE", "LEFT_CURLY",
  "RIGHT_CURLY", "AMPERSAND", "ASTERISK", "CARET", "COLON", "COMMA",
  "EQUAL", "EXCLAMATION", "GREATER", "LESS", "MINUS", "PERCENT", "PLUS",
  "SEMICOLON", "SLASH", "QUESTION", "TILDE", "VBAR", "AMPERSAND_AMPERSAND",
  "AMPERSAND_EQUAL", "ASTERISK_EQUAL", "CARET_EQUAL", "EQUAL_EQUAL",
  "EXCLAMATION_EQUAL", "GREATER_EQUAL", "GREATER_GREATER",
  "GREATER_GREATER_EQUAL", "LESS_EQUAL", "LESS_LESS", "LESS_LESS_EQUAL",
  "MINUS_EQUAL", "MINUS_MINUS", "PERCENT_EQUAL", "PLUS_EQUAL", "PLUS_PLUS",
  "SLASH_EQUAL", "VBAR_EQUAL", "VBAR_VBAR"};

  printf(" %s \n", debugIDName[tokenId - IDENTIFIER]);
#endif

  return tokenId;
}

int nodeFromChar(int charVal)
{
    char str[15];
    sprintf(str, "%d", charVal);
    yylval = node_number(str);

#if DEBUG
    printf("Char: %c \n", charVal);
#endif
    return echoFcn(NUMBER);
}

void errorFcn(char const* message)
{

#if DEBUG
  yyerror(message);
  /*printf("\n ERROR: %s at %d for %s\n", message,yylineno, yytext);*/
#endif
}
