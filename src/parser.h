/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    MINUS_EQUAL = 258,
    PERCENT_EQUAL = 259,
    PLUS_EQUAL = 260,
    SLASH_EQUAL = 261,
    VBAR_EQUAL = 262,
    VBAR_VBAR = 263,
    BREAK = 264,
    CHAR = 265,
    CONTINUE = 266,
    DO = 267,
    ELSE = 268,
    FOR = 269,
    GOTO = 270,
    IF = 271,
    INT = 272,
    LONG = 273,
    RETURN = 274,
    SHORT = 275,
    SIGNED = 276,
    UNSIGNED = 277,
    VOID = 278,
    WHILE = 279,
    LEFT_PAREN = 280,
    RIGHT_PAREN = 281,
    LEFT_SQUARE = 282,
    RIGHT_SQUARE = 283,
    LEFT_CURLY = 284,
    RIGHT_CURLY = 285,
    AMPERSAND = 286,
    ASTERISK = 287,
    CARET = 288,
    COLON = 289,
    COMMA = 290,
    EQUAL = 291,
    EXCLAMATION = 292,
    GREATER = 293,
    LESS = 294,
    MINUS = 295,
    PERCENT = 296,
    PLUS = 297,
    SEMICOLON = 298,
    SLASH = 299,
    QUESTION = 300,
    TILDE = 301,
    VBAR = 302,
    AMPERSAND_AMPERSAND = 303,
    AMPERSAND_EQUAL = 304,
    ASTERISK_EQUAL = 305,
    CARET_EQUAL = 306,
    GREATER_EQUAL = 307,
    GREATER_GREATER = 308,
    GREATER_GREATER_EQUAL = 309,
    LESS_EQUAL = 310,
    LESS_LESS = 311,
    LESS_LESS_EQUAL = 312,
    EQUAL_EQUAL = 313,
    EXCLAMATION_EQUAL = 314,
    MINUS_MINUS = 315,
    PLUS_PLUS = 316,
    IDENTIFIER = 317,
    NUMBER = 318,
    STRING = 319
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
