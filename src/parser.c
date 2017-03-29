/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 4 "parser.y" /* yacc.c:339  */


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

#line 84 "parser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.h".  */
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

/* Copy the second part of user declarations.  */

#line 200 "parser.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  45
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   747

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  65
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  81
/* YYNRULES -- Number of rules.  */
#define YYNRULES  193
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  319

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    68,    68,    70,    72,    76,    78,    80,    84,    88,
      90,    94,    96,    98,   100,   102,   104,   106,   108,   110,
     112,   114,   116,   120,   121,   125,   129,   130,   134,   135,
     139,   142,   143,   147,   149,   151,   155,   157,   161,   163,
     167,   169,   173,   175,   179,   182,   185,   189,   190,   193,
     195,   199,   202,   203,   206,   210,   213,   216,   221,   222,
     223,   224,   227,   231,   232,   235,   240,   243,   244,   248,
     249,   252,   255,   258,   261,   263,   265,   267,   269,   273,
     275,   279,   285,   289,   297,   301,   305,   306,   309,   312,
     314,   318,   319,   320,   323,   324,   325,   328,   332,   336,
     337,   342,   346,   347,   352,   353,   355,   357,   361,   364,
     369,   371,   373,   377,   379,   383,   387,   390,   391,   394,
     398,   403,   404,   405,   406,   407,   410,   414,   418,   422,
     423,   424,   425,   427,   430,   431,   433,   435,   437,   441,
     443,   447,   448,   450,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   468,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   483,   487,   488,   491,
     493,   498,   500,   504,   505,   508,   509,   510,   511,   512,
     513,   514,   515,   516,   519,   523,   527,   529,   531,   533,
     535,   537,   542,   546
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MINUS_EQUAL", "PERCENT_EQUAL",
  "PLUS_EQUAL", "SLASH_EQUAL", "VBAR_EQUAL", "VBAR_VBAR", "BREAK", "CHAR",
  "CONTINUE", "DO", "ELSE", "FOR", "GOTO", "IF", "INT", "LONG", "RETURN",
  "SHORT", "SIGNED", "UNSIGNED", "VOID", "WHILE", "LEFT_PAREN",
  "RIGHT_PAREN", "LEFT_SQUARE", "RIGHT_SQUARE", "LEFT_CURLY",
  "RIGHT_CURLY", "AMPERSAND", "ASTERISK", "CARET", "COLON", "COMMA",
  "EQUAL", "EXCLAMATION", "GREATER", "LESS", "MINUS", "PERCENT", "PLUS",
  "SEMICOLON", "SLASH", "QUESTION", "TILDE", "VBAR", "AMPERSAND_AMPERSAND",
  "AMPERSAND_EQUAL", "ASTERISK_EQUAL", "CARET_EQUAL", "GREATER_EQUAL",
  "GREATER_GREATER", "GREATER_GREATER_EQUAL", "LESS_EQUAL", "LESS_LESS",
  "LESS_LESS_EQUAL", "EQUAL_EQUAL", "EXCLAMATION_EQUAL", "MINUS_MINUS",
  "PLUS_PLUS", "IDENTIFIER", "NUMBER", "STRING", "$accept",
  "abstract_declarator", "additive_expr", "address_expr",
  "array_declarator", "assignment_expr", "bitwise_and_expr",
  "bitwise_negation_expr", "bitwise_or_expr", "bitwise_xor_expr",
  "break_statement", "cast_expr", "character_type_specifier", "comma_expr",
  "compound_statement", "conditional_expr", "conditional_statement",
  "constant_expr", "continue_statement", "decl",
  "declaration_or_statement", "declaration_or_statement_list",
  "declaration_specifiers", "declarator", "direct_abstract_declarator",
  "direct_declarator", "do_statement", "equality_expr", "expr",
  "expression_list", "expression_statement", "for_statement",
  "function_call", "function_declarator", "function_definition",
  "function_def_specifier", "goto_statement", "indirection_expr",
  "initial_clause", "initialized_declarator",
  "initialized_declarator_list", "integer_type_specifier",
  "iterative_statement", "label", "labeled_statement", "logical_and_expr",
  "logical_negation_expr", "logical_or_expr", "multiplicative_expr",
  "named_label", "null_statement", "parameter_decl", "parameter_list",
  "parameter_type_list", "parenthesized_expr", "pointer",
  "pointer_declarator", "postdecrement_expr", "postfix_expr",
  "postincrement_expr", "predecrement_expr", "preincrement_expr",
  "primary_expr", "program", "relational_expr", "return_statement",
  "shift_expr", "signed_type_specifier", "simple_declarator", "statement",
  "subscript_expr", "top_level_decl", "translation_unit", "type_name",
  "type_specifier", "unary_expr", "unary_minus_expr", "unary_plus_expr",
  "unsigned_type_specifier", "void_type_specifier", "while_statement", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319
};
# endif

#define YYPACT_NINF -204

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-204)))

#define YYTABLE_NINF -109

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     169,  -204,  -204,    18,    36,    12,   186,  -204,  -204,  -204,
      23,  -204,    47,  -204,    61,  -204,  -204,   169,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,    49,    65,  -204,  -204,    71,
      79,    23,    83,  -204,  -204,    89,    66,  -204,  -204,    -2,
     -12,  -204,  -204,   250,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,   100,  -204,   169,   138,    23,  -204,    66,    90,    97,
     101,   362,   127,   109,   152,    37,   157,   430,  -204,   649,
     649,   649,   649,   649,  -204,   649,   683,   683,   151,  -204,
    -204,    99,  -204,  -204,   162,  -204,   150,   177,  -204,  -204,
     193,  -204,  -204,  -204,  -204,  -204,  -204,   306,    23,  -204,
     106,   172,  -204,  -204,  -204,  -204,  -204,  -204,   187,  -204,
     181,  -204,    11,    19,  -204,  -204,  -204,  -204,   156,  -204,
    -204,  -204,  -204,    93,  -204,    34,  -204,  -204,    21,  -204,
    -204,  -204,  -204,    22,  -204,   195,   205,  -204,  -204,  -204,
     207,  -204,  -204,  -204,  -204,  -204,  -204,   209,   396,  -204,
     196,   649,  -204,   199,   649,   180,   210,   219,  -204,  -204,
    -204,  -204,  -204,  -204,   649,  -204,  -204,   649,   649,   649,
     649,   649,   649,  -204,  -204,   649,   649,  -204,   362,   649,
     649,   649,   649,   649,   649,   443,   649,  -204,  -204,   649,
     649,   649,   649,   649,   649,   649,   649,   649,   649,   649,
     649,   649,   649,   649,   649,   649,    22,   649,  -204,  -204,
     221,   -17,   169,  -204,  -204,   227,   477,  -204,  -204,   203,
    -204,   228,  -204,   229,   180,  -204,   128,  -204,   649,    19,
      19,   106,   177,   162,  -204,    93,    93,  -204,   150,   181,
     223,  -204,  -204,  -204,  -204,  -204,    32,   225,    34,    34,
      34,    34,    99,    99,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,   232,   235,   490,   221,  -204,
     649,   530,   233,   543,   362,   362,  -204,   649,  -204,   649,
    -204,  -204,  -204,  -204,   249,   252,   362,   257,   583,   596,
     241,   272,  -204,  -204,  -204,  -204,   243,  -204,   362,   362,
     262,   362,   263,   636,   362,  -204,  -204,  -204,   362,  -204,
     362,   362,   265,  -204,  -204,  -204,  -204,   362,  -204
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    33,   148,   151,   144,   150,   189,   192,    93,   167,
       0,   168,     0,   173,     0,    91,   169,   133,    51,    92,
     174,   152,   145,    34,   149,   153,   146,    35,   188,   191,
     187,     0,   117,   155,    61,    88,    53,    60,    89,     0,
       0,    52,    58,     0,    82,     1,   170,   154,   147,   190,
     186,     0,   118,     0,     0,     0,    46,   119,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    39,     0,
       0,     0,     0,     0,   109,     0,     0,     0,   129,   130,
     131,   141,   180,    36,    28,   179,    99,    26,   161,   104,
      66,   158,    11,   159,   162,    47,    49,     0,     0,    95,
      23,     0,   156,    96,   123,   164,   181,   160,     0,   157,
     102,   178,    40,     5,    97,   165,   132,   125,   175,   124,
     183,   182,   121,    63,   163,   134,    48,   122,    31,   176,
     177,    94,    59,   112,   113,   115,     0,    10,   129,    44,
       0,    31,    88,    90,    70,    30,    45,     0,     0,   108,
       0,     0,   140,     0,     0,   172,     0,     0,     8,    85,
     101,   184,   185,    25,     0,   127,   128,     0,     0,     0,
       0,     0,     0,    38,    50,     0,     0,    69,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   120,   126,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   110,
       4,     2,     0,    81,     9,     0,     0,    87,    86,     0,
      84,     0,   139,     0,     0,   171,     2,   116,     0,     7,
       6,    24,    27,    29,    37,    64,    65,    98,   100,   103,
       0,   105,   107,   106,    80,    67,     0,     0,   137,   135,
     138,   136,   142,   143,    14,    17,    13,    16,    22,    12,
      20,    15,    21,    19,    18,     0,     0,     0,     3,   114,
       0,     0,     0,     0,     0,     0,    32,     0,    79,     0,
     166,    54,    56,    57,     0,     0,     0,     0,     0,     0,
       0,    42,   193,    41,    68,    55,     0,    78,     0,     0,
       0,     0,     0,     0,     0,    62,    75,    77,     0,    76,
       0,     0,     0,    43,    72,    73,    74,     0,    71
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -204,  -121,   -26,  -204,  -204,   -91,   123,  -204,   116,   129,
    -204,   -55,  -204,  -204,   285,   -45,  -204,  -203,  -204,   -32,
     201,  -204,     6,    -3,  -180,   -39,  -204,   131,   -62,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,   246,
    -204,  -204,  -204,  -204,  -204,   122,  -204,  -204,    52,   240,
    -204,    92,  -204,  -204,  -204,   -30,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,    48,  -204,   -69,  -204,  -204,   -61,
    -204,   288,  -204,  -204,  -204,   -33,  -204,  -204,  -204,  -204,
    -204
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   265,    81,    82,    34,    83,    84,    85,    86,    87,
      88,    89,     8,    90,    91,    92,    93,   140,    94,     9,
      96,    97,    98,    51,   210,    36,    99,   100,   101,   246,
     102,   103,   104,    37,    11,    12,   105,   106,   219,    38,
      39,    13,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   134,   135,   136,   116,    40,    41,   117,   118,   119,
     120,   121,   122,    14,   123,   124,   125,    15,    42,   126,
     127,    16,    17,   157,    18,   128,   129,   130,    19,    20,
     131
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     147,    57,    52,   153,   266,   156,    10,    35,   206,   139,
     207,    95,   208,    31,   158,   159,   160,   161,   162,   180,
     163,   141,    23,    10,   195,   196,   197,   198,   199,    24,
      25,   268,    26,    55,   225,    21,   141,   141,   141,   141,
     141,    56,   141,   165,   166,    33,   268,   206,    31,   207,
      33,   182,   142,    22,    32,    32,   181,   200,   278,   133,
     183,    45,    67,   184,   284,    95,    47,   279,    69,    70,
     201,   202,   203,   155,    71,   204,    43,    72,   205,    73,
     152,   234,    48,    75,    33,    33,   218,   193,    49,   221,
     194,    53,   223,    54,   245,   142,    50,    76,    77,   138,
      79,    80,   156,   211,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,    32,   217,   237,   -83,   240,
     248,   249,   250,   251,   247,   226,   132,   241,   242,   243,
     209,   189,   190,   144,   141,   141,   141,   141,   141,   167,
     145,   168,   141,   141,   146,   191,   141,   141,   192,   141,
     141,   141,   148,   224,   272,   207,   141,   141,   141,   141,
     141,   141,   139,    67,   175,   176,   137,   252,   253,    69,
      70,   149,    57,   276,   141,    71,   211,   151,    72,     1,
      73,   185,   154,   186,    75,  -108,     2,     3,   294,     4,
       5,     6,     7,   169,   226,   141,    27,   170,    76,    77,
     138,    79,    80,    28,    29,   224,    30,   207,   285,   287,
     171,   290,    32,   291,   292,   177,   187,   188,   133,   229,
     230,   178,   139,   235,   236,   297,   300,   302,   172,   179,
     212,   213,   293,   215,   141,   214,   227,   306,   307,   220,
     309,   312,   222,   313,   141,   228,   273,   314,   267,   315,
     316,    58,   270,   280,   274,   275,   318,   277,   281,    59,
       1,    60,    61,   282,    62,    63,    64,     2,     3,    65,
       4,     5,     6,     7,    66,    67,   288,   295,   296,    43,
      68,    69,    70,   298,   303,   304,   305,    71,   308,   310,
      72,   317,    73,    74,   233,   238,    75,    44,   174,   232,
     231,   143,   239,   150,   269,    46,     0,    58,     0,     0,
      76,    77,    78,    79,    80,    59,     1,    60,    61,     0,
      62,    63,    64,     2,     3,    65,     4,     5,     6,     7,
      66,    67,     0,     0,     0,    43,   173,    69,    70,     0,
       0,     0,     0,    71,     0,     0,    72,     0,    73,    74,
       0,     0,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    58,     0,     0,    76,    77,    78,    79,
      80,    59,     0,    60,    61,     0,    62,    63,    64,     0,
       0,    65,     0,     0,     0,     0,    66,    67,     0,     0,
       0,    43,     0,    69,    70,     0,     0,     0,     0,    71,
       0,     0,    72,     0,    73,    74,     1,     0,    75,     0,
       0,     0,     0,     2,     3,     0,     4,     5,     6,     7,
       0,    67,    76,    77,    78,    79,    80,    69,    70,     0,
       0,     0,     0,    71,     0,     0,    72,     0,    73,   216,
       1,     0,    75,     0,     0,     0,     0,     2,     3,     0,
       4,     5,     6,     7,     0,    67,    76,    77,   138,    79,
      80,    69,    70,     0,     0,     0,     0,    71,    67,   244,
      72,     0,    73,     0,    69,    70,    75,     0,     0,     0,
      71,     0,     0,    72,     0,    73,     0,     0,     0,    75,
      76,    77,   138,    79,    80,     0,     0,     0,     0,     0,
       0,     0,    67,    76,    77,   138,    79,    80,    69,    70,
       0,     0,     0,     0,    71,    67,     0,    72,   283,    73,
     271,    69,    70,    75,     0,     0,     0,    71,     0,     0,
      72,     0,    73,     0,     0,     0,    75,    76,    77,   138,
      79,    80,     0,     0,     0,     0,     0,     0,     0,     0,
      76,    77,   138,    79,    80,    67,   286,     0,     0,     0,
       0,    69,    70,     0,     0,     0,     0,    71,    67,     0,
      72,     0,    73,     0,    69,    70,    75,     0,     0,     0,
      71,     0,     0,    72,     0,    73,   289,     0,     0,    75,
      76,    77,   138,    79,    80,     0,     0,     0,     0,     0,
       0,     0,     0,    76,    77,   138,    79,    80,    67,   299,
       0,     0,     0,     0,    69,    70,     0,     0,     0,     0,
      71,    67,   301,    72,     0,    73,     0,    69,    70,    75,
       0,     0,     0,    71,     0,     0,    72,     0,    73,     0,
       0,     0,    75,    76,    77,   138,    79,    80,     0,     0,
       0,     0,     0,     0,     0,     0,    76,    77,   138,    79,
      80,    67,   311,     0,     0,     0,     0,    69,    70,     0,
       0,     0,     0,    71,    67,     0,    72,     0,    73,     0,
      69,    70,    75,     0,     0,     0,    71,     0,     0,    72,
       0,    73,     0,     0,     0,    75,    76,    77,   138,    79,
      80,     0,     0,     0,     0,     0,     0,     0,   164,    76,
      77,   138,    79,    80,    69,    70,     0,     0,     0,     0,
      71,     0,     0,    72,     0,    73,     0,     0,     0,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    76,    77,   138,    79,    80
};

static const yytype_int16 yycheck[] =
{
      61,    40,    32,    65,   207,    67,     0,    10,    25,    54,
      27,    43,   133,    25,    69,    70,    71,    72,    73,     8,
      75,    54,    10,    17,     3,     4,     5,     6,     7,    17,
      18,   211,    20,    35,   155,    17,    69,    70,    71,    72,
      73,    43,    75,    76,    77,    62,   226,    25,    25,    27,
      62,    32,    55,    17,    32,    32,    45,    36,    26,    53,
      41,     0,    25,    44,   267,    97,    17,    35,    31,    32,
      49,    50,    51,    67,    37,    54,    29,    40,    57,    42,
      43,   172,    17,    46,    62,    62,   148,    53,    17,   151,
      56,    25,   154,    27,   185,    98,    17,    60,    61,    62,
      63,    64,   164,   133,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,    32,   148,   178,    29,   181,
     189,   190,   191,   192,   186,   155,    26,   182,   183,   184,
     133,    38,    39,    43,   167,   168,   169,   170,   171,    40,
      43,    42,   175,   176,    43,    52,   179,   180,    55,   182,
     183,   184,    25,    25,   216,    27,   189,   190,   191,   192,
     193,   194,   207,    25,    58,    59,    28,   193,   194,    31,
      32,    62,   211,   228,   207,    37,   206,    25,    40,    10,
      42,    25,    25,    27,    46,    34,    17,    18,   279,    20,
      21,    22,    23,    31,   224,   228,    10,    47,    60,    61,
      62,    63,    64,    17,    18,    25,    20,    27,   270,   271,
      33,   273,    32,   274,   275,    43,    60,    61,   212,   167,
     168,    34,   267,   175,   176,   286,   288,   289,    35,    48,
      35,    26,   277,    24,   267,    28,    26,   298,   299,    43,
     301,   303,    43,   304,   277,    26,    43,   308,    27,   310,
     311,     1,    25,    28,    26,    26,   317,    34,    26,     9,
      10,    11,    12,    28,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    43,    28,    26,    29,
      30,    31,    32,    26,    43,    13,    43,    37,    26,    26,
      40,    26,    42,    43,   171,   179,    46,    12,    97,   170,
     169,    55,   180,    63,   212,    17,    -1,     1,    -1,    -1,
      60,    61,    62,    63,    64,     9,    10,    11,    12,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    -1,    -1,    -1,    29,    30,    31,    32,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    42,    43,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    60,    61,    62,    63,
      64,     9,    -1,    11,    12,    -1,    14,    15,    16,    -1,
      -1,    19,    -1,    -1,    -1,    -1,    24,    25,    -1,    -1,
      -1,    29,    -1,    31,    32,    -1,    -1,    -1,    -1,    37,
      -1,    -1,    40,    -1,    42,    43,    10,    -1,    46,    -1,
      -1,    -1,    -1,    17,    18,    -1,    20,    21,    22,    23,
      -1,    25,    60,    61,    62,    63,    64,    31,    32,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    42,    43,
      10,    -1,    46,    -1,    -1,    -1,    -1,    17,    18,    -1,
      20,    21,    22,    23,    -1,    25,    60,    61,    62,    63,
      64,    31,    32,    -1,    -1,    -1,    -1,    37,    25,    26,
      40,    -1,    42,    -1,    31,    32,    46,    -1,    -1,    -1,
      37,    -1,    -1,    40,    -1,    42,    -1,    -1,    -1,    46,
      60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    60,    61,    62,    63,    64,    31,    32,
      -1,    -1,    -1,    -1,    37,    25,    -1,    40,    28,    42,
      43,    31,    32,    46,    -1,    -1,    -1,    37,    -1,    -1,
      40,    -1,    42,    -1,    -1,    -1,    46,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    61,    62,    63,    64,    25,    26,    -1,    -1,    -1,
      -1,    31,    32,    -1,    -1,    -1,    -1,    37,    25,    -1,
      40,    -1,    42,    -1,    31,    32,    46,    -1,    -1,    -1,
      37,    -1,    -1,    40,    -1,    42,    43,    -1,    -1,    46,
      60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    62,    63,    64,    25,    26,
      -1,    -1,    -1,    -1,    31,    32,    -1,    -1,    -1,    -1,
      37,    25,    26,    40,    -1,    42,    -1,    31,    32,    46,
      -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    42,    -1,
      -1,    -1,    46,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    63,
      64,    25,    26,    -1,    -1,    -1,    -1,    31,    32,    -1,
      -1,    -1,    -1,    37,    25,    -1,    40,    -1,    42,    -1,
      31,    32,    46,    -1,    -1,    -1,    37,    -1,    -1,    40,
      -1,    42,    -1,    -1,    -1,    46,    60,    61,    62,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    60,
      61,    62,    63,    64,    31,    32,    -1,    -1,    -1,    -1,
      37,    -1,    -1,    40,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    62,    63,    64
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    10,    17,    18,    20,    21,    22,    23,    77,    84,
      87,    99,   100,   106,   128,   132,   136,   137,   139,   143,
     144,    17,    17,    10,    17,    18,    20,    10,    17,    18,
      20,    25,    32,    62,    69,    88,    90,    98,   104,   105,
     120,   121,   133,    29,    79,     0,   136,    17,    17,    17,
      17,    88,   120,    25,    27,    35,    43,    90,     1,     9,
      11,    12,    14,    15,    16,    19,    24,    25,    30,    31,
      32,    37,    40,    42,    43,    46,    60,    61,    62,    63,
      64,    67,    68,    70,    71,    72,    73,    74,    75,    76,
      78,    79,    80,    81,    83,    84,    85,    86,    87,    91,
      92,    93,    95,    96,    97,   101,   102,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   119,   122,   123,   124,
     125,   126,   127,   129,   130,   131,   134,   135,   140,   141,
     142,   145,    26,    87,   116,   117,   118,    28,    62,    80,
      82,   140,    88,   104,    43,    43,    43,   134,    25,    62,
     114,    25,    43,    93,    25,    87,    93,   138,    76,    76,
      76,    76,    76,    76,    25,   140,   140,    40,    42,    31,
      47,    33,    35,    30,    85,    58,    59,    43,    34,    48,
       8,    45,    32,    41,    44,    25,    27,    60,    61,    38,
      39,    52,    55,    53,    56,     3,     4,     5,     6,     7,
      36,    49,    50,    51,    54,    57,    25,    27,    66,    88,
      89,   120,    35,    26,    28,    24,    43,    84,    93,   103,
      43,    93,    43,    93,    25,    66,   120,    26,    26,   113,
     113,    92,    74,    71,    70,   129,   129,   134,    73,   110,
      93,    76,    76,    76,    26,    70,    94,    93,   131,   131,
     131,   131,    67,    67,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    66,    82,    27,    89,   116,
      25,    43,    93,    43,    26,    26,    76,    34,    26,    35,
      28,    26,    28,    28,    82,    93,    26,    93,    43,    43,
      93,   134,   134,    80,    70,    28,    26,   134,    26,    26,
      93,    26,    93,    43,    13,    43,   134,   134,    26,   134,
      26,    26,    93,   134,   134,   134,   134,    26,   134
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    65,    66,    66,    66,    67,    67,    67,    68,    69,
      69,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    71,    71,    72,    73,    73,    74,    74,
      75,    76,    76,    77,    77,    77,    78,    78,    79,    79,
      80,    80,    81,    81,    82,    83,    84,    85,    85,    86,
      86,    87,    88,    88,    89,    89,    89,    89,    90,    90,
      90,    90,    91,    92,    92,    92,    93,    94,    94,    95,
      95,    96,    96,    96,    96,    96,    96,    96,    96,    97,
      97,    98,    99,   100,   101,   102,   103,   103,   104,   105,
     105,   106,   106,   106,   107,   107,   107,   108,   109,   110,
     110,   111,   112,   112,   113,   113,   113,   113,   114,   115,
     116,   116,   116,   117,   117,   118,   119,   120,   120,   121,
     122,   123,   123,   123,   123,   123,   124,   125,   126,   127,
     127,   127,   127,   128,   129,   129,   129,   129,   129,   130,
     130,   131,   131,   131,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   133,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   135,   136,   136,   137,
     137,   138,   138,   139,   139,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   141,   142,   143,   143,   143,   143,
     143,   143,   144,   145
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     3,     3,     2,     4,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     2,     1,     3,     1,     3,
       2,     1,     4,     1,     2,     2,     1,     3,     3,     2,
       1,     5,     5,     7,     1,     2,     3,     1,     1,     1,
       2,     1,     1,     1,     3,     4,     3,     3,     1,     3,
       1,     1,     7,     1,     3,     3,     1,     1,     3,     2,
       2,     9,     8,     8,     8,     7,     7,     7,     6,     4,
       3,     4,     2,     2,     3,     2,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       3,     2,     1,     3,     1,     3,     3,     3,     1,     1,
       2,     2,     1,     1,     3,     1,     3,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     2,     2,     2,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       2,     1,     3,     3,     1,     2,     2,     3,     1,     2,
       1,     1,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     1,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     2,     2,     1,
       3,     2,     1,     5
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 69 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_abstract_declarator((yyvsp[0]), NULL);}
#line 1626 "parser.c" /* yacc.c:1646  */
    break;

  case 3:
#line 71 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_abstract_declarator((yyvsp[-1]), (yyvsp[0]));}
#line 1632 "parser.c" /* yacc.c:1646  */
    break;

  case 4:
#line 73 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_abstract_declarator(NULL, (yyvsp[0]));}
#line 1638 "parser.c" /* yacc.c:1646  */
    break;

  case 5:
#line 77 "parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]);}
#line 1644 "parser.c" /* yacc.c:1646  */
    break;

  case 6:
#line 79 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_ADDITION, (yyvsp[-2]), (yyvsp[0]));}
#line 1650 "parser.c" /* yacc.c:1646  */
    break;

  case 7:
#line 81 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_SUBTRACTION, (yyvsp[-2]), (yyvsp[0]));}
#line 1656 "parser.c" /* yacc.c:1646  */
    break;

  case 8:
#line 85 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_unary_operation(UNARY_ADDRESS, (yyvsp[0]));}
#line 1662 "parser.c" /* yacc.c:1646  */
    break;

  case 9:
#line 89 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_array_declarator((yyvsp[-3]), (yyvsp[-1]));}
#line 1668 "parser.c" /* yacc.c:1646  */
    break;

  case 10:
#line 91 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_array_declarator((yyvsp[-2]),NULL);}
#line 1674 "parser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 95 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1680 "parser.c" /* yacc.c:1646  */
    break;

  case 12:
#line 97 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1686 "parser.c" /* yacc.c:1646  */
    break;

  case 13:
#line 99 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_ADD_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1692 "parser.c" /* yacc.c:1646  */
    break;

  case 14:
#line 101 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_SUBTRACT_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1698 "parser.c" /* yacc.c:1646  */
    break;

  case 15:
#line 103 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_MULTIPLY_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1704 "parser.c" /* yacc.c:1646  */
    break;

  case 16:
#line 105 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_DIVIDE_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1710 "parser.c" /* yacc.c:1646  */
    break;

  case 17:
#line 107 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_REMAINDER_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1716 "parser.c" /* yacc.c:1646  */
    break;

  case 18:
#line 109 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_BITSHIFT_LEFT_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1722 "parser.c" /* yacc.c:1646  */
    break;

  case 19:
#line 111 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_BITSHIFT_RIGHT_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1728 "parser.c" /* yacc.c:1646  */
    break;

  case 20:
#line 113 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_AND_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1734 "parser.c" /* yacc.c:1646  */
    break;

  case 21:
#line 115 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_XOR_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1740 "parser.c" /* yacc.c:1646  */
    break;

  case 22:
#line 117 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_OR_ASSIGN, (yyvsp[-2]), (yyvsp[0]));}
#line 1746 "parser.c" /* yacc.c:1646  */
    break;

  case 23:
#line 120 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1752 "parser.c" /* yacc.c:1646  */
    break;

  case 24:
#line 122 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_BITWISE_AND, (yyvsp[-2]), (yyvsp[0]));}
#line 1758 "parser.c" /* yacc.c:1646  */
    break;

  case 25:
#line 126 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_unary_operation(UNARY_BITWISE_NEGATION, (yyvsp[0])); }
#line 1764 "parser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 129 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1770 "parser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 131 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_BITWISE_OR, (yyvsp[-2]), (yyvsp[0])); }
#line 1776 "parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 134 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1782 "parser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 136 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_BITWISE_XOR, (yyvsp[-2]), (yyvsp[0]));}
#line 1788 "parser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 139 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_break();}
#line 1794 "parser.c" /* yacc.c:1646  */
    break;

  case 31:
#line 142 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1800 "parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 144 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_cast((yyvsp[-2]), (yyvsp[0]), NULL, false);}
#line 1806 "parser.c" /* yacc.c:1646  */
    break;

  case 33:
#line 148 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_type(TYPE_BASIC, TYPE_WIDTH_CHAR, true);}
#line 1812 "parser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 150 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_type(TYPE_BASIC, TYPE_WIDTH_CHAR, true);}
#line 1818 "parser.c" /* yacc.c:1646  */
    break;

  case 35:
#line 152 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_type(TYPE_BASIC, TYPE_WIDTH_CHAR, false);}
#line 1824 "parser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 156 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1830 "parser.c" /* yacc.c:1646  */
    break;

  case 37:
#line 158 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_comma_expr((yyvsp[-2]), (yyvsp[0]));}
#line 1836 "parser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 162 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_compound_statement((yyvsp[-1]));}
#line 1842 "parser.c" /* yacc.c:1646  */
    break;

  case 39:
#line 164 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_compound_statement(NULL);}
#line 1848 "parser.c" /* yacc.c:1646  */
    break;

  case 40:
#line 168 "parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]);}
#line 1854 "parser.c" /* yacc.c:1646  */
    break;

  case 41:
#line 170 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_if_else_trenary((yyvsp[-4]), (yyvsp[-2]), (yyvsp[0]));}
#line 1860 "parser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 174 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_if((yyvsp[-2]), (yyvsp[0]));}
#line 1866 "parser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 176 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_if_else((yyvsp[-4]),(yyvsp[-2]),(yyvsp[0]));}
#line 1872 "parser.c" /* yacc.c:1646  */
    break;

  case 44:
#line 179 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1878 "parser.c" /* yacc.c:1646  */
    break;

  case 45:
#line 182 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_continue();}
#line 1884 "parser.c" /* yacc.c:1646  */
    break;

  case 46:
#line 186 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_decl((yyvsp[-2]),(yyvsp[-1]));}
#line 1890 "parser.c" /* yacc.c:1646  */
    break;

  case 47:
#line 189 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1896 "parser.c" /* yacc.c:1646  */
    break;

  case 48:
#line 190 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1902 "parser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 194 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_decl_or_statement_list(NULL, (yyvsp[0])); }
#line 1908 "parser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 196 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_decl_or_statement_list((yyvsp[-1]), (yyvsp[0])); }
#line 1914 "parser.c" /* yacc.c:1646  */
    break;

  case 51:
#line 199 "parser.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]);}
#line 1920 "parser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 202 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1926 "parser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 203 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1932 "parser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 207 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_direct_abstract_declarator((yyvsp[-1]), NULL,
					  DIRECT_ABS_DECL_PARENTHESIZED);
   }
#line 1940 "parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 211 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_direct_abstract_declarator((yyvsp[-3]), (yyvsp[-1]),
					  DIRECT_ABS_DECL_SQUARE);}
#line 1947 "parser.c" /* yacc.c:1646  */
    break;

  case 56:
#line 214 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_direct_abstract_declarator(NULL, (yyvsp[-1]),
					  DIRECT_ABS_DECL_SQUARE);}
#line 1954 "parser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 217 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_direct_abstract_declarator((yyvsp[-2]), NULL,
					  DIRECT_ABS_DECL_SQUARE);}
#line 1961 "parser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 221 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1967 "parser.c" /* yacc.c:1646  */
    break;

  case 59:
#line 222 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[-1]);}
#line 1973 "parser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 223 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1979 "parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 224 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1985 "parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 228 "parser.y" /* yacc.c:1646  */
    { (yyval) =  node_do_while((yyvsp[-5]), (yyvsp[-2])); }
#line 1991 "parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 231 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 1997 "parser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 233 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_EQUALITY,
				(yyvsp[-2]), (yyvsp[0]));}
#line 2004 "parser.c" /* yacc.c:1646  */
    break;

  case 65:
#line 236 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_INEQUALITY,
				(yyvsp[-2]), (yyvsp[0]));}
#line 2011 "parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 240 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2017 "parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 243 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2023 "parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 245 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_expression_list((yyvsp[-2]), (yyvsp[0]));}
#line 2029 "parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 248 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[-1]);}
#line 2035 "parser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 249 "parser.y" /* yacc.c:1646  */
    {yyerrok;}
#line 2041 "parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 254 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop((yyvsp[-6]), (yyvsp[-4]), (yyvsp[-2]), (yyvsp[0]));}
#line 2047 "parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 257 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop(NULL, (yyvsp[-4]), (yyvsp[-2]), (yyvsp[0]));}
#line 2053 "parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 260 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop((yyvsp[-5]), NULL, (yyvsp[-2]), (yyvsp[0]));}
#line 2059 "parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 262 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop((yyvsp[-5]), (yyvsp[-3]), NULL, (yyvsp[0]));}
#line 2065 "parser.c" /* yacc.c:1646  */
    break;

  case 75:
#line 264 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop(NULL, NULL, (yyvsp[-2]), (yyvsp[0]));}
#line 2071 "parser.c" /* yacc.c:1646  */
    break;

  case 76:
#line 266 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop((yyvsp[-4]),NULL, NULL, (yyvsp[0]));}
#line 2077 "parser.c" /* yacc.c:1646  */
    break;

  case 77:
#line 268 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop(NULL, (yyvsp[-3]), NULL, (yyvsp[0]));}
#line 2083 "parser.c" /* yacc.c:1646  */
    break;

  case 78:
#line 270 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_for_expr_loop(NULL,NULL, NULL, (yyvsp[0]));}
#line 2089 "parser.c" /* yacc.c:1646  */
    break;

  case 79:
#line 274 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_function_call((yyvsp[-3]), (yyvsp[-1])); }
#line 2095 "parser.c" /* yacc.c:1646  */
    break;

  case 80:
#line 276 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_function_call((yyvsp[-2]), node_type(TYPE_VOID, 0, true ));}
#line 2101 "parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 280 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_function_declarator((yyvsp[-3]),(yyvsp[-1]));}
#line 2107 "parser.c" /* yacc.c:1646  */
    break;

  case 82:
#line 286 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_function_definition((yyvsp[-1]), (yyvsp[0]));}
#line 2113 "parser.c" /* yacc.c:1646  */
    break;

  case 83:
#line 290 "parser.y" /* yacc.c:1646  */
    {  (yyval)= node_function_def_specifier((yyvsp[-1]),(yyvsp[0])); }
#line 2119 "parser.c" /* yacc.c:1646  */
    break;

  case 84:
#line 298 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_goto((yyvsp[-1]));}
#line 2125 "parser.c" /* yacc.c:1646  */
    break;

  case 85:
#line 302 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_INDIRECTION, (yyvsp[0]));}
#line 2131 "parser.c" /* yacc.c:1646  */
    break;

  case 86:
#line 305 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2137 "parser.c" /* yacc.c:1646  */
    break;

  case 87:
#line 306 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2143 "parser.c" /* yacc.c:1646  */
    break;

  case 88:
#line 309 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2149 "parser.c" /* yacc.c:1646  */
    break;

  case 89:
#line 313 "parser.y" /* yacc.c:1646  */
    {(yyval) =  (yyvsp[0]);}
#line 2155 "parser.c" /* yacc.c:1646  */
    break;

  case 90:
#line 315 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_initialized_declarator((yyvsp[-2]), (yyvsp[0]));}
#line 2161 "parser.c" /* yacc.c:1646  */
    break;

  case 91:
#line 318 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2167 "parser.c" /* yacc.c:1646  */
    break;

  case 92:
#line 319 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2173 "parser.c" /* yacc.c:1646  */
    break;

  case 93:
#line 320 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2179 "parser.c" /* yacc.c:1646  */
    break;

  case 94:
#line 323 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2185 "parser.c" /* yacc.c:1646  */
    break;

  case 95:
#line 324 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2191 "parser.c" /* yacc.c:1646  */
    break;

  case 96:
#line 325 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2197 "parser.c" /* yacc.c:1646  */
    break;

  case 97:
#line 329 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_label((yyvsp[0]));}
#line 2203 "parser.c" /* yacc.c:1646  */
    break;

  case 98:
#line 333 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_labeled_statement((yyvsp[-2]), (yyvsp[0]));}
#line 2209 "parser.c" /* yacc.c:1646  */
    break;

  case 99:
#line 336 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2215 "parser.c" /* yacc.c:1646  */
    break;

  case 100:
#line 338 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_binary_operation(BINOP_AND,
				(yyvsp[-2]), (yyvsp[0]));}
#line 2222 "parser.c" /* yacc.c:1646  */
    break;

  case 101:
#line 343 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_NOT, (yyvsp[0]));}
#line 2228 "parser.c" /* yacc.c:1646  */
    break;

  case 102:
#line 346 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2234 "parser.c" /* yacc.c:1646  */
    break;

  case 103:
#line 348 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_OR,
				(yyvsp[-2]), (yyvsp[0]));}
#line 2241 "parser.c" /* yacc.c:1646  */
    break;

  case 104:
#line 352 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2247 "parser.c" /* yacc.c:1646  */
    break;

  case 105:
#line 354 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_MULTIPLICATION, (yyvsp[-2]), (yyvsp[0]));}
#line 2253 "parser.c" /* yacc.c:1646  */
    break;

  case 106:
#line 356 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_DIVISION, (yyvsp[-2]), (yyvsp[0]));}
#line 2259 "parser.c" /* yacc.c:1646  */
    break;

  case 107:
#line 358 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_REMAINDER, (yyvsp[-2]), (yyvsp[0]));}
#line 2265 "parser.c" /* yacc.c:1646  */
    break;

  case 108:
#line 361 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2271 "parser.c" /* yacc.c:1646  */
    break;

  case 109:
#line 364 "parser.y" /* yacc.c:1646  */
    {/*No operation*/}
#line 2277 "parser.c" /* yacc.c:1646  */
    break;

  case 110:
#line 370 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_parameter_decl((yyvsp[-1]), (yyvsp[0]));}
#line 2283 "parser.c" /* yacc.c:1646  */
    break;

  case 111:
#line 372 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_parameter_decl((yyvsp[-1]), (yyvsp[0]));}
#line 2289 "parser.c" /* yacc.c:1646  */
    break;

  case 112:
#line 374 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_parameter_decl((yyvsp[0]), NULL);}
#line 2295 "parser.c" /* yacc.c:1646  */
    break;

  case 113:
#line 378 "parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]);}
#line 2301 "parser.c" /* yacc.c:1646  */
    break;

  case 114:
#line 380 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_prm_list((yyvsp[-2]), (yyvsp[0]));}
#line 2307 "parser.c" /* yacc.c:1646  */
    break;

  case 115:
#line 384 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2313 "parser.c" /* yacc.c:1646  */
    break;

  case 116:
#line 387 "parser.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[-1]);}
#line 2319 "parser.c" /* yacc.c:1646  */
    break;

  case 117:
#line 390 "parser.y" /* yacc.c:1646  */
    {(yyval) =  node_pointer(NULL);}
#line 2325 "parser.c" /* yacc.c:1646  */
    break;

  case 118:
#line 391 "parser.y" /* yacc.c:1646  */
    {(yyval) =  node_pointer((yyvsp[0]));}
#line 2331 "parser.c" /* yacc.c:1646  */
    break;

  case 119:
#line 395 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_pointer_declarator((yyvsp[-1]),(yyvsp[0]));}
#line 2337 "parser.c" /* yacc.c:1646  */
    break;

  case 120:
#line 399 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_unary_operation(UNARY_POST_DEC,
			       (yyvsp[-1]));}
#line 2344 "parser.c" /* yacc.c:1646  */
    break;

  case 121:
#line 403 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2350 "parser.c" /* yacc.c:1646  */
    break;

  case 122:
#line 404 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2356 "parser.c" /* yacc.c:1646  */
    break;

  case 123:
#line 405 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2362 "parser.c" /* yacc.c:1646  */
    break;

  case 124:
#line 406 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2368 "parser.c" /* yacc.c:1646  */
    break;

  case 125:
#line 407 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2374 "parser.c" /* yacc.c:1646  */
    break;

  case 126:
#line 411 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_POST_INC, (yyvsp[-1]));}
#line 2380 "parser.c" /* yacc.c:1646  */
    break;

  case 127:
#line 415 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_PRE_DEC, (yyvsp[0]));}
#line 2386 "parser.c" /* yacc.c:1646  */
    break;

  case 128:
#line 419 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_PRE_INC, (yyvsp[0]));}
#line 2392 "parser.c" /* yacc.c:1646  */
    break;

  case 129:
#line 422 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2398 "parser.c" /* yacc.c:1646  */
    break;

  case 130:
#line 423 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2404 "parser.c" /* yacc.c:1646  */
    break;

  case 131:
#line 424 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2410 "parser.c" /* yacc.c:1646  */
    break;

  case 132:
#line 425 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2416 "parser.c" /* yacc.c:1646  */
    break;

  case 133:
#line 427 "parser.y" /* yacc.c:1646  */
    {root_node = (yyvsp[0]);}
#line 2422 "parser.c" /* yacc.c:1646  */
    break;

  case 134:
#line 430 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2428 "parser.c" /* yacc.c:1646  */
    break;

  case 135:
#line 432 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_LESS, (yyvsp[-2]), (yyvsp[0]));}
#line 2434 "parser.c" /* yacc.c:1646  */
    break;

  case 136:
#line 434 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_LESS_EQUAL, (yyvsp[-2]), (yyvsp[0]));}
#line 2440 "parser.c" /* yacc.c:1646  */
    break;

  case 137:
#line 436 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_GREATER, (yyvsp[-2]), (yyvsp[0]));}
#line 2446 "parser.c" /* yacc.c:1646  */
    break;

  case 138:
#line 438 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_binary_operation(BINOP_GREATER, (yyvsp[-2]), (yyvsp[0]));}
#line 2452 "parser.c" /* yacc.c:1646  */
    break;

  case 139:
#line 442 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_return((yyvsp[-1]));}
#line 2458 "parser.c" /* yacc.c:1646  */
    break;

  case 140:
#line 444 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_return(NULL); }
#line 2464 "parser.c" /* yacc.c:1646  */
    break;

  case 141:
#line 447 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2470 "parser.c" /* yacc.c:1646  */
    break;

  case 142:
#line 449 "parser.y" /* yacc.c:1646  */
    {  (yyval) = node_binary_operation(BINOP_BITSHIFT_RIGHT, (yyvsp[-2]), (yyvsp[0]));}
#line 2476 "parser.c" /* yacc.c:1646  */
    break;

  case 143:
#line 451 "parser.y" /* yacc.c:1646  */
    {  (yyval) = node_binary_operation(BINOP_BITSHIFT_LEFT, (yyvsp[-2]), (yyvsp[0]));}
#line 2482 "parser.c" /* yacc.c:1646  */
    break;

  case 144:
#line 455 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
#line 2488 "parser.c" /* yacc.c:1646  */
    break;

  case 145:
#line 456 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
#line 2494 "parser.c" /* yacc.c:1646  */
    break;

  case 146:
#line 457 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
#line 2500 "parser.c" /* yacc.c:1646  */
    break;

  case 147:
#line 458 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT,true);}
#line 2506 "parser.c" /* yacc.c:1646  */
    break;

  case 148:
#line 459 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_INT,true);}
#line 2512 "parser.c" /* yacc.c:1646  */
    break;

  case 149:
#line 460 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_INT,true);}
#line 2518 "parser.c" /* yacc.c:1646  */
    break;

  case 150:
#line 461 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_INT,true);}
#line 2524 "parser.c" /* yacc.c:1646  */
    break;

  case 151:
#line 462 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
#line 2530 "parser.c" /* yacc.c:1646  */
    break;

  case 152:
#line 463 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
#line 2536 "parser.c" /* yacc.c:1646  */
    break;

  case 153:
#line 464 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
#line 2542 "parser.c" /* yacc.c:1646  */
    break;

  case 154:
#line 465 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_LONG,true);}
#line 2548 "parser.c" /* yacc.c:1646  */
    break;

  case 155:
#line 468 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2554 "parser.c" /* yacc.c:1646  */
    break;

  case 156:
#line 471 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2560 "parser.c" /* yacc.c:1646  */
    break;

  case 157:
#line 472 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2566 "parser.c" /* yacc.c:1646  */
    break;

  case 158:
#line 473 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2572 "parser.c" /* yacc.c:1646  */
    break;

  case 159:
#line 474 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2578 "parser.c" /* yacc.c:1646  */
    break;

  case 160:
#line 475 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2584 "parser.c" /* yacc.c:1646  */
    break;

  case 161:
#line 476 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2590 "parser.c" /* yacc.c:1646  */
    break;

  case 162:
#line 477 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2596 "parser.c" /* yacc.c:1646  */
    break;

  case 163:
#line 478 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2602 "parser.c" /* yacc.c:1646  */
    break;

  case 164:
#line 479 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2608 "parser.c" /* yacc.c:1646  */
    break;

  case 165:
#line 480 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2614 "parser.c" /* yacc.c:1646  */
    break;

  case 166:
#line 484 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_subscript_expr((yyvsp[-3]), (yyvsp[-1]));}
#line 2620 "parser.c" /* yacc.c:1646  */
    break;

  case 167:
#line 487 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2626 "parser.c" /* yacc.c:1646  */
    break;

  case 168:
#line 488 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2632 "parser.c" /* yacc.c:1646  */
    break;

  case 169:
#line 492 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_translation_unit(NULL, (yyvsp[0])); }
#line 2638 "parser.c" /* yacc.c:1646  */
    break;

  case 170:
#line 494 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_translation_unit((yyvsp[-1]), (yyvsp[0]));}
#line 2644 "parser.c" /* yacc.c:1646  */
    break;

  case 171:
#line 499 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type_name((yyvsp[-1]),(yyvsp[0]));}
#line 2650 "parser.c" /* yacc.c:1646  */
    break;

  case 172:
#line 501 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type_name((yyvsp[0]),NULL);}
#line 2656 "parser.c" /* yacc.c:1646  */
    break;

  case 173:
#line 504 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2662 "parser.c" /* yacc.c:1646  */
    break;

  case 174:
#line 505 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2668 "parser.c" /* yacc.c:1646  */
    break;

  case 175:
#line 508 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2674 "parser.c" /* yacc.c:1646  */
    break;

  case 176:
#line 509 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2680 "parser.c" /* yacc.c:1646  */
    break;

  case 177:
#line 510 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2686 "parser.c" /* yacc.c:1646  */
    break;

  case 178:
#line 511 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2692 "parser.c" /* yacc.c:1646  */
    break;

  case 179:
#line 512 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2698 "parser.c" /* yacc.c:1646  */
    break;

  case 180:
#line 513 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2704 "parser.c" /* yacc.c:1646  */
    break;

  case 181:
#line 514 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2710 "parser.c" /* yacc.c:1646  */
    break;

  case 182:
#line 515 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2716 "parser.c" /* yacc.c:1646  */
    break;

  case 183:
#line 516 "parser.y" /* yacc.c:1646  */
    {(yyval) = (yyvsp[0]);}
#line 2722 "parser.c" /* yacc.c:1646  */
    break;

  case 184:
#line 520 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_MINUS, (yyvsp[0]));}
#line 2728 "parser.c" /* yacc.c:1646  */
    break;

  case 185:
#line 524 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_unary_operation(UNARY_PLUS, (yyvsp[0]));}
#line 2734 "parser.c" /* yacc.c:1646  */
    break;

  case 186:
#line 528 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT, false);}
#line 2740 "parser.c" /* yacc.c:1646  */
    break;

  case 187:
#line 530 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_SHORT, false);}
#line 2746 "parser.c" /* yacc.c:1646  */
    break;

  case 188:
#line 532 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_INT, false);}
#line 2752 "parser.c" /* yacc.c:1646  */
    break;

  case 189:
#line 534 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_INT, false);}
#line 2758 "parser.c" /* yacc.c:1646  */
    break;

  case 190:
#line 536 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_LONG, false);}
#line 2764 "parser.c" /* yacc.c:1646  */
    break;

  case 191:
#line 538 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_BASIC,TYPE_WIDTH_LONG, false);}
#line 2770 "parser.c" /* yacc.c:1646  */
    break;

  case 192:
#line 543 "parser.y" /* yacc.c:1646  */
    {(yyval) = node_type(TYPE_VOID, 0, true );}
#line 2776 "parser.c" /* yacc.c:1646  */
    break;

  case 193:
#line 547 "parser.y" /* yacc.c:1646  */
    { (yyval) = node_while_statement((yyvsp[-2]), (yyvsp[0]));}
#line 2782 "parser.c" /* yacc.c:1646  */
    break;


#line 2786 "parser.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 549 "parser.y" /* yacc.c:1906  */


void yyerror(char const *s) {
  fprintf(stderr, "ERROR at line %d: %s\n", yylineno, s);
}
struct node* unimplemented()
{
  assert(0);
  return NULL;
}
