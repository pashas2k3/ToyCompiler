%option yylineno
%option nounput

%{
/*
 * scanner.l
 *
 * This file contains the specification for the (f)lex generated scanner
 * for the CSCI-E295 source language.
 *
 */

  #include <stdlib.h>
  #include <errno.h>
  #include <string.h>

  #define YY_NO_INPUT

  #include "testToken.h"
  
  #define MAX_STR_CONST 509 /*From C89 standards*/

  int echoFcn(int);
%}

newline         \n[\r]?
ws              [ \t\v\f]

digit           [[:digit:]]
letter          [[:alpha:]]

id              {letter}({letter}|{digit})*
number          {digit}+

%x              comment
%x              string
%s              escape
%%
     char string_buf[MAX_STR_CONST];
     char *string_buf_ptr = NULL;
{newline}   /* do nothing; lex is counting lines */
{ws}        /* do nothing */

  /* operators begin */
\*          return echoFcn(ASTERISK);
\+          return echoFcn(PLUS);
-           return echoFcn(MINUS);
\/          return echoFcn(SLASH);
\=          return echoFcn(EQUAL);

\(          return echoFcn(LEFT_PAREN);
\)          return echoFcn(RIGHT_PAREN);
\{           return echoFcn(LEFT_CURLY);
\}           return echoFcn(RIGHT_CURLY);
;           return echoFcn(SEMICOLON);
  /* operators end */

  /* Separator*/
,           return echoFcn(COMMA);
:           return echoFcn(COLON);
!          return echoFcn(EXCLAMATION);



  /* Handle '0' by itself begin*/

  /* Handle '0' by itself end*/

  /* constants begin */
  /* Do we need the checks for values at scanner level itself?*/
{number}          { /*yylval =  node_number*/
                  return echoFcn(NUMBER);}
{number}.{number} {
                  printf("ERROR: floating point operation not allowed");
                  return -1;}
  /* constants end */
  
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
{id}        {/*yylval = node_identifier(yytext, yyleng)*/return echoFcn(IDENTIFIER); }

  /* Handle escape sequences*/
"\\ "           {return echoFcn('\\');}
"\\n"           {return echoFcn('\n');}
"\\t"           {return echoFcn('\t'); }                 
"\\r"           {return echoFcn('\r'); }                 
"\\b"           {return echoFcn('\b'); }                 
"\\f"           {return echoFcn('\f'); }                 
'\\'[0-7]{1,3}  {
		   unsigned int result;
		   (void) sscanf( yytext + 1, "%o", &result );
		   if ( result > 0xff )
                     {
                      printf("ERROR: Constant is out of bounds");
                     }
		   *string_buf_ptr++ = result;
		   }
'\\'[0-9]+      { printf("ERROR: Constant is out of bounds");} 

  /* Handle different integer sizes */


  /* Handle Strings begin */

\"             {string_buf_ptr = string_buf;BEGIN(string);}

<string>{
   \n          {printf("ERROR:String broken by newline at %d\n", yylineno);return -1;}
   <<EOF>>     {printf("ERROR:Untermintaed string ended by EOF");return -1;}
   \"          { *string_buf_ptr = '\0'; 
                  /*!!!!!!!!Passing string!!!!!!!!!!!!!!!*/
                  /*yylval = node_number(yytext, yyleng);*/
	 BEGIN(INITIAL); return echoFcn(STRING);}
   [^\\\n\"]+   {
                char * yptr = yytext; 
                 while(*yptr)
                   *string_buf_ptr = *yptr++; 
                string_buf_ptr++;}

   /* escape sequences handling also !!!*/
   /* To make this combined with escape sequence handling for regular escape sequence characters*/
   \\n          {*string_buf_ptr++ = '\n'; }
   \\t          {*string_buf_ptr++ = '\t'; }                 
   \\r          {*string_buf_ptr++ = '\r'; }                 
   \\b          {*string_buf_ptr++ = '\b'; }                 
   \\f          {*string_buf_ptr++ = '\f'; }                 
   \\[0-7]{1,3} {/* octal escape sequence */
		   int result;
		   (void) sscanf( yytext + 1, "%o", &result );
		   if ( result > 0xff )
                      printf("ERROR: Constant is out of bounds");
		   *string_buf_ptr++ = result;
		   }
   \\[0-9]+      {printf("ERROR:Bad escape sequence passed");
                   }
}

  /* Handle Strings end */

  /* Handle Comments begin */
"*/"          {printf("Unmatched comment at %d \n",yylineno); return -1;}
"/*"          {BEGIN(comment);}
<comment>{
      "*/"    {BEGIN(INITIAL);}
      [^*]+   {/*Do nothing */}
      "*"     /*Do nothing again*/
      <<EOF>> {printf("Comment not terminated\n");return -1;}
}
  /* Handle Comments end */


  /* Specific Error conditions begin */

  /* 1. Carriage return alone - error*/

  /* 2. Carriage return alone - error*/
  /* Error conditions end*/
<<EOF>>     {printf("Successful completion");}
.           {printf("ERROR: Unmatched rule \n");return -1;}
%%
int main(int argc, char *argv[])
{
  yylex();  
}

int echoFcn(int tokenId)
{
  printf("Token ID:%d,  %s \n", tokenId, yytname[tokenId - IDENTIFIER + 3]);

  return tokenId;
}
