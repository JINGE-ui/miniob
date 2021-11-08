/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
  size_t select_length;
  size_t condition_length;
  size_t from_length;
  size_t value_length;
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
  AggregationOp aggcomp;
	char id[MAX_NUM];
  size_t tuplev_begin;
} ParserContext;

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->condition_length = 0;
  context->from_length = 0;
  context->select_length = 0;
  context->value_length = 0;
  context->tuplev_begin=0;
  context->ssql->sstr.insertion.insert_num = 0;
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 130 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SEMICOLON = 258,
    CREATE = 259,
    DROP = 260,
    TABLE = 261,
    TABLES = 262,
    INDEX = 263,
    SELECT = 264,
    DESC = 265,
    SHOW = 266,
    SYNC = 267,
    INSERT = 268,
    DELETE = 269,
    UPDATE = 270,
    LBRACE = 271,
    RBRACE = 272,
    COMMA = 273,
    TRX_BEGIN = 274,
    TRX_COMMIT = 275,
    TRX_ROLLBACK = 276,
    INT_T = 277,
    STRING_T = 278,
    FLOAT_T = 279,
    DATE_T = 280,
    HELP = 281,
    EXIT = 282,
    DOT = 283,
    INTO = 284,
    VALUES = 285,
<<<<<<< HEAD
    FROM = 286,
    WHERE = 287,
    AND = 288,
    SET = 289,
    ON = 290,
    LOAD = 291,
    DATA = 292,
    INFILE = 293,
    EQ = 294,
    LT = 295,
    GT = 296,
    LE = 297,
    GE = 298,
    NE = 299,
    COUNT = 300,
    MAX = 301,
    MIN = 302,
    AVG = 303,
    INNER = 304,
    JOIN = 305,
    GROUP = 306,
    BY = 307,
    NUMBER = 308,
    FLOAT = 309,
    ID = 310,
    PATH = 311,
    SSS = 312,
    DATE = 313,
    STAR = 314,
    STRING_V = 315
=======
    UNIQUE = 286,
    FROM = 287,
    WHERE = 288,
    AND = 289,
    SET = 290,
    ON = 291,
    LOAD = 292,
    DATA = 293,
    INFILE = 294,
    EQ = 295,
    LT = 296,
    GT = 297,
    LE = 298,
    GE = 299,
    NE = 300,
    COUNT = 301,
    MAX = 302,
    MIN = 303,
    AVG = 304,
    INNER = 305,
    JOIN = 306,
    NUMBER = 307,
    FLOAT = 308,
    ID = 309,
    PATH = 310,
    SSS = 311,
    DATE = 312,
    STAR = 313,
    STRING_V = 314
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
<<<<<<< HEAD
#line 118 "yacc_sql.y"
=======
#line 117 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *datestr;
  char *string;
  int number;
  float floats;
	char *position;

<<<<<<< HEAD
#line 254 "yacc_sql.tab.c"
=======
#line 253 "yacc_sql.tab.c"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

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
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
<<<<<<< HEAD
#define YYLAST   204

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
=======
#define YYLAST   201

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
<<<<<<< HEAD
#define YYNRULES  98
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  203

#define YYUNDEFTOK  2
#define YYMAXUTOK   315
=======
#define YYNRULES  96
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  206

#define YYUNDEFTOK  2
#define YYMAXUTOK   314
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
<<<<<<< HEAD
      55,    56,    57,    58,    59,    60
=======
      55,    56,    57,    58,    59
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
<<<<<<< HEAD
       0,   146,   146,   148,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   172,   177,   182,   188,   194,   200,   206,   212,   218,
     225,   233,   240,   249,   251,   255,   266,   279,   282,   283,
     284,   285,   288,   297,   307,   309,   312,   316,   318,   322,
     325,   328,   332,   338,   348,   358,   375,   377,   382,   388,
     390,   395,   401,   403,   407,   409,   415,   421,   426,   431,
     436,   441,   446,   453,   455,   482,   483,   484,   485,   489,
     491,   495,   497,   501,   503,   508,   529,   549,   569,   591,
     612,   633,   655,   656,   657,   658,   659,   660,   664
=======
       0,   145,   145,   147,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   171,   176,   181,   187,   193,   199,   205,   211,   217,
     224,   229,   237,   244,   253,   255,   259,   270,   283,   286,
     287,   288,   289,   292,   301,   311,   313,   316,   320,   322,
     326,   329,   332,   336,   342,   352,   362,   380,   382,   386,
     388,   394,   399,   404,   409,   414,   421,   428,   436,   438,
     444,   446,   453,   464,   465,   466,   467,   471,   473,   477,
     479,   483,   485,   490,   511,   531,   551,   573,   594,   615,
     637,   638,   639,   640,   641,   642,   646
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "SELECT", "DESC", "SHOW", "SYNC", "INSERT", "DELETE",
  "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN", "TRX_COMMIT",
  "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T", "HELP", "EXIT",
<<<<<<< HEAD
  "DOT", "INTO", "VALUES", "FROM", "WHERE", "AND", "SET", "ON", "LOAD",
  "DATA", "INFILE", "EQ", "LT", "GT", "LE", "GE", "NE", "COUNT", "MAX",
  "MIN", "AVG", "INNER", "JOIN", "GROUP", "BY", "NUMBER", "FLOAT", "ID",
  "PATH", "SSS", "DATE", "STAR", "STRING_V", "$accept", "commands",
  "command", "exit", "help", "sync", "begin", "commit", "rollback",
  "drop_table", "show_tables", "desc_table", "create_index", "drop_index",
=======
  "DOT", "INTO", "VALUES", "UNIQUE", "FROM", "WHERE", "AND", "SET", "ON",
  "LOAD", "DATA", "INFILE", "EQ", "LT", "GT", "LE", "GE", "NE", "COUNT",
  "MAX", "MIN", "AVG", "INNER", "JOIN", "NUMBER", "FLOAT", "ID", "PATH",
  "SSS", "DATE", "STAR", "STRING_V", "$accept", "commands", "command",
  "exit", "help", "sync", "begin", "commit", "rollback", "drop_table",
  "show_tables", "desc_table", "create_index", "drop_index",
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
  "create_table", "attr_def_list", "attr_def", "number", "type", "ID_get",
  "insert", "insert_list", "value_clause", "value_list", "value", "delete",
  "update", "select", "group", "group_list", "inner_join_list",
  "inner_join_on", "select_attr", "aggr_or_attr", "aggr_or_attr_list",
  "AggregationOp", "rel_list", "where", "condition_list", "condition",
  "comOp", "load_data", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
<<<<<<< HEAD
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315
};
# endif

#define YYPACT_NINF (-153)
=======
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314
};
# endif

#define YYPACT_NINF (-168)
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
<<<<<<< HEAD
    -153,     3,  -153,    93,   101,    13,   -49,    50,    16,   -18,
     -11,   -22,    62,    66,    67,    73,    79,    51,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,    39,    45,    53,    55,
    -153,  -153,  -153,  -153,    65,  -153,    80,    94,    97,   111,
     112,  -153,    61,    63,    83,  -153,  -153,  -153,  -153,  -153,
      82,   103,    86,   119,   120,    69,    70,    13,  -153,   -28,
    -153,  -153,    96,    95,    74,    71,    75,    76,  -153,  -153,
    -153,   114,    94,     4,   116,   118,     9,   132,    98,   107,
    -153,   121,    81,   122,    85,    92,  -153,  -153,    87,  -153,
      38,   125,  -153,  -153,    12,  -153,  -153,     2,   113,  -153,
      38,   138,    75,   128,  -153,  -153,  -153,  -153,   131,    99,
     114,   100,    95,   134,   130,   118,   146,   102,  -153,  -153,
    -153,  -153,  -153,  -153,    20,    26,     9,  -153,    95,   104,
     121,   149,   105,   136,  -153,   106,   109,  -153,    38,   139,
     125,  -153,     2,  -153,  -153,   127,  -153,   113,   159,   160,
    -153,  -153,  -153,   147,   162,   114,   115,   163,   130,  -153,
    -153,    32,   117,  -153,  -153,  -153,  -153,  -153,   133,   123,
    -153,  -153,   141,  -153,  -153,     9,    92,     8,   124,   113,
    -153,   126,   129,  -153,  -153,  -153,    10,   152,   135,  -153,
    -153,   152,  -153
=======
    -168,     7,  -168,    17,    39,    16,   -48,    24,    29,    10,
       8,    -8,    46,    58,    68,    69,    70,    47,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,    34,    38,    90,    50,
      56,  -168,  -168,  -168,  -168,   -13,  -168,    77,    99,   102,
     116,   117,  -168,    67,    71,    87,  -168,  -168,  -168,  -168,
    -168,    84,   108,    92,    72,   124,   126,    76,    78,  -168,
      79,    20,  -168,   -16,  -168,  -168,   101,   103,    80,    81,
      85,    86,   105,  -168,  -168,    -4,   120,   125,    99,    13,
     118,   128,    30,   139,   106,   119,  -168,   127,    91,   131,
      95,    96,  -168,  -168,    97,   104,  -168,  -168,    98,  -168,
      55,   135,  -168,  -168,    15,  -168,  -168,    36,   121,  -168,
      55,   150,    85,   140,  -168,  -168,  -168,  -168,   142,   107,
     143,   120,   125,   109,   103,   145,   146,   128,   160,   111,
    -168,  -168,  -168,  -168,  -168,  -168,    37,    43,    30,  -168,
     103,   112,   127,   164,   122,   151,   115,  -168,  -168,   123,
     167,  -168,    55,   154,   135,  -168,    36,  -168,  -168,   144,
    -168,   121,   170,   172,  -168,  -168,  -168,   159,   175,   162,
     125,  -168,   146,  -168,  -168,    49,   129,  -168,  -168,  -168,
    -168,  -168,   177,   148,  -168,   153,  -168,  -168,  -168,    30,
     104,   132,   121,  -168,  -168,  -168
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    20,
      19,    14,    15,    16,    17,     9,    10,    11,    12,    13,
       8,     5,     7,     6,     4,    18,     0,     0,     0,     0,
<<<<<<< HEAD
      75,    76,    77,    78,    71,    70,     0,    73,     0,     0,
       0,    23,     0,     0,     0,    24,    25,    26,    22,    21,
       0,     0,     0,     0,     0,     0,     0,     0,    66,     0,
      29,    28,     0,    81,     0,     0,     0,     0,    27,    31,
      72,    79,    73,     0,     0,     0,     0,     0,     0,     0,
      42,    33,     0,     0,     0,    62,    74,    68,     0,    67,
       0,    44,    49,    50,     0,    51,    52,     0,    83,    53,
       0,     0,     0,     0,    38,    39,    40,    41,    36,     0,
      79,     0,    81,     0,    47,     0,     0,     0,    92,    93,
      94,    95,    96,    97,     0,     0,     0,    82,    81,     0,
      33,     0,     0,     0,    80,     0,    56,    69,     0,     0,
      44,    43,     0,    87,    85,    88,    86,    83,     0,     0,
      34,    32,    37,     0,     0,    79,     0,     0,    47,    46,
      45,     0,     0,    84,    54,    98,    35,    30,    64,     0,
      55,    48,     0,    89,    90,     0,    62,    59,     0,    83,
      63,     0,     0,    57,    91,    65,    59,    59,     0,    60,
      58,    59,    61
=======
       0,    73,    74,    75,    76,    70,    61,     0,    68,     0,
       0,     0,    23,     0,     0,     0,    24,    25,    26,    22,
      21,     0,     0,     0,     0,     0,     0,     0,     0,    62,
       0,     0,    64,     0,    29,    28,     0,    79,     0,     0,
       0,     0,     0,    27,    32,    70,    70,    77,    68,     0,
       0,     0,     0,     0,     0,     0,    43,    34,     0,     0,
       0,     0,    71,    63,     0,    57,    69,    66,     0,    65,
       0,    45,    50,    51,     0,    52,    53,     0,    81,    54,
       0,     0,     0,     0,    39,    40,    41,    42,    37,     0,
       0,    70,    77,     0,    79,     0,    48,     0,     0,     0,
      90,    91,    92,    93,    94,    95,     0,     0,     0,    80,
      79,     0,    34,     0,     0,     0,     0,    72,    78,     0,
       0,    67,     0,     0,    45,    44,     0,    85,    83,    86,
      84,    81,     0,     0,    35,    33,    38,     0,     0,     0,
      77,    56,    48,    47,    46,     0,     0,    82,    55,    96,
      36,    30,     0,    59,    49,     0,    87,    88,    31,     0,
      57,     0,    81,    58,    89,    60
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
<<<<<<< HEAD
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,    31,    64,  -153,  -153,  -153,
    -153,    23,    49,     7,  -100,  -153,  -153,  -153,  -153,   -99,
      -9,  -153,  -153,   137,   110,  -153,  -118,  -113,  -152,  -135,
    -103,  -153
=======
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,    33,    60,  -168,  -168,  -168,
    -168,    23,    51,     9,  -110,  -168,  -168,  -168,   -11,  -168,
    -168,   130,   110,   -77,  -168,  -129,  -121,  -167,  -146,  -116,
    -168
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
<<<<<<< HEAD
      26,    27,    28,    29,    30,   113,    91,   163,   118,    92,
      31,   126,   101,   149,   107,    32,    33,    34,   167,   193,
     122,   186,    46,    47,    68,    48,    95,    87,   137,   108,
     134,    35
=======
      26,    27,    28,    29,    30,   123,    97,   177,   128,    98,
      31,   138,   111,   163,   117,    32,    33,    34,   134,   200,
      47,    48,    72,    69,    49,   105,    93,   149,   118,   146,
      35
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
<<<<<<< HEAD
     124,   157,   144,     2,   135,   173,    49,     3,     4,   146,
     138,    52,     5,     6,     7,     8,     9,    10,    11,    51,
      53,    97,    12,    13,    14,   158,   191,    83,   191,    15,
      16,    84,    98,    54,   154,   156,   192,   195,   198,    17,
     127,   128,   129,   130,   131,   132,   133,   178,   168,   171,
     189,   128,   129,   130,   131,   132,   133,    50,    40,    41,
      42,    43,   102,   103,   104,    55,   105,   106,    44,    56,
      57,   183,    45,   102,   103,   153,    58,   105,   106,   102,
     103,   155,    59,   105,   106,   102,   103,   182,    60,   105,
     106,   102,   103,    65,    61,   105,   106,   199,   200,    36,
      62,    37,   202,   114,   115,   116,   117,    38,    63,    39,
      64,    66,    67,    69,    70,    71,    72,    74,    73,    76,
      75,    77,    78,    79,    80,    81,    85,    86,    89,    88,
      90,    93,    94,    99,   100,   109,   111,   110,   119,   112,
     120,   121,   123,   125,   139,   141,   136,   142,   148,   151,
     145,   147,   161,   164,   143,   172,   169,   152,   162,   159,
     166,   165,   174,   175,   176,   177,   180,   179,   185,   188,
     191,   160,   184,   170,   150,   181,   140,   190,   187,   194,
       0,   196,     0,     0,   197,     0,     0,     0,     0,     0,
     201,     0,    96,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    82
=======
     136,   147,   171,   158,   187,    67,    50,     2,   102,   103,
     150,     3,     4,   160,    67,    68,     5,     6,     7,     8,
       9,    10,    11,    36,   101,    37,    12,    13,    14,   172,
     107,    51,    52,    15,    16,   205,   168,   170,    89,    53,
      54,   108,    90,   139,    17,    39,    55,    40,    38,    56,
     185,   193,   182,   202,   157,   140,   141,   142,   143,   144,
     145,    57,    41,    42,    43,    44,    41,    42,    43,    44,
      45,    58,    59,    60,    46,   196,   140,   141,   142,   143,
     144,   145,   112,   113,   114,    61,   115,   116,    62,   112,
     113,   167,    63,   115,   116,   112,   113,   169,    64,   115,
     116,   112,   113,   195,    65,   115,   116,   112,   113,    70,
      66,   115,   116,   124,   125,   126,   127,    71,    73,    74,
      75,    76,    78,    79,    80,    77,    82,    83,    81,    84,
      85,    91,    86,    87,    94,   109,    92,    95,    67,    96,
      99,   100,   119,   104,   110,   122,   120,   129,   121,   130,
     131,   132,   135,   137,   133,   148,   151,   153,   154,   156,
     159,   155,   161,   165,   162,   166,   173,   175,   178,   179,
     181,   183,   186,   188,   176,   189,   190,   180,   191,   192,
     198,   201,   152,   197,   199,   174,   204,   184,   164,   203,
       0,   194,     0,     0,     0,     0,     0,     0,   106,     0,
       0,    88
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

static const yytype_int16 yycheck[] =
{
<<<<<<< HEAD
     100,   136,   120,     0,   107,   157,    55,     4,     5,   122,
     110,    29,     9,    10,    11,    12,    13,    14,    15,     3,
      31,    17,    19,    20,    21,   138,    18,    55,    18,    26,
      27,    59,    28,    55,   134,   135,    28,   189,    28,    36,
      28,    39,    40,    41,    42,    43,    44,   165,   148,   152,
     185,    39,    40,    41,    42,    43,    44,     7,    45,    46,
      47,    48,    53,    54,    55,     3,    57,    58,    55,     3,
       3,   171,    59,    53,    54,    55,     3,    57,    58,    53,
      54,    55,     3,    57,    58,    53,    54,    55,    37,    57,
      58,    53,    54,    28,    55,    57,    58,   196,   197,     6,
      55,     8,   201,    22,    23,    24,    25,     6,    55,     8,
      55,    31,    18,    16,     3,     3,    55,    34,    55,    16,
      38,    35,     3,     3,    55,    55,    30,    32,    57,    55,
      55,    55,    18,    17,    16,     3,    29,    39,    16,    18,
      55,    49,    55,    18,     6,    17,    33,    16,    18,     3,
      50,    17,     3,    17,    55,    28,    17,    55,    53,    55,
      51,    55,     3,     3,    17,     3,     3,    52,    35,    28,
      18,   140,    55,   150,   125,   168,   112,   186,    55,    55,
      -1,    55,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67
=======
     110,   117,   148,   132,   171,    18,    54,     0,    85,    86,
     120,     4,     5,   134,    18,    28,     9,    10,    11,    12,
      13,    14,    15,     6,    28,     8,    19,    20,    21,   150,
      17,     7,     3,    26,    27,   202,   146,   147,    54,    29,
      32,    28,    58,    28,    37,     6,    54,     8,    31,     3,
     166,   180,   162,   199,   131,    40,    41,    42,    43,    44,
      45,     3,    46,    47,    48,    49,    46,    47,    48,    49,
      54,     3,     3,     3,    58,   185,    40,    41,    42,    43,
      44,    45,    52,    53,    54,    38,    56,    57,    54,    52,
      53,    54,    54,    56,    57,    52,    53,    54,     8,    56,
      57,    52,    53,    54,    54,    56,    57,    52,    53,    32,
      54,    56,    57,    22,    23,    24,    25,    18,    16,     3,
       3,    54,    35,    39,    16,    54,    54,     3,    36,     3,
      54,    30,    54,    54,    54,    17,    33,    56,    18,    54,
      54,    36,     3,    18,    16,    18,    40,    16,    29,    54,
      54,    54,    54,    18,    50,    34,     6,    17,    16,    16,
      51,    54,    17,     3,    18,    54,    54,     3,    17,    54,
       3,    17,    28,     3,    52,     3,    17,    54,     3,    17,
       3,    28,   122,    54,    36,   152,    54,   164,   137,   200,
      -1,   182,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,    71
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
<<<<<<< HEAD
       0,    62,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    19,    20,    21,    26,    27,    36,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    81,    86,    87,    88,   102,     6,     8,     6,     8,
      45,    46,    47,    48,    55,    59,    93,    94,    96,    55,
       7,     3,    29,    31,    55,     3,     3,     3,     3,     3,
      37,    55,    55,    55,    55,    28,    31,    18,    95,    16,
       3,     3,    55,    55,    34,    38,    16,    35,     3,     3,
      55,    55,    94,    55,    59,    30,    32,    98,    55,    57,
      55,    77,    80,    55,    18,    97,    95,    17,    28,    17,
      16,    83,    53,    54,    55,    57,    58,    85,   100,     3,
      39,    29,    18,    76,    22,    23,    24,    25,    79,    16,
      55,    49,    91,    55,    85,    18,    82,    28,    39,    40,
      41,    42,    43,    44,   101,   101,    33,    99,    85,     6,
      77,    17,    16,    55,    97,    50,    98,    17,    18,    84,
      83,     3,    55,    55,    85,    55,    85,   100,    98,    55,
      76,     3,    53,    78,    17,    55,    51,    89,    85,    17,
      82,   101,    28,    99,     3,     3,    17,     3,    97,    52,
       3,    84,    55,    85,    55,    35,    92,    55,    28,   100,
      91,    18,    28,    90,    55,    99,    55,    55,    28,    90,
      90,    55,    90
=======
       0,    61,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    19,    20,    21,    26,    27,    37,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    80,    85,    86,    87,   100,     6,     8,    31,     6,
       8,    46,    47,    48,    49,    54,    58,    90,    91,    94,
      54,     7,     3,    29,    32,    54,     3,     3,     3,     3,
       3,    38,    54,    54,     8,    54,    54,    18,    28,    93,
      32,    18,    92,    16,     3,     3,    54,    54,    35,    39,
      16,    36,    54,     3,     3,    54,    54,    54,    91,    54,
      58,    30,    33,    96,    54,    56,    54,    76,    79,    54,
      36,    28,    93,    93,    18,    95,    92,    17,    28,    17,
      16,    82,    52,    53,    54,    56,    57,    84,    98,     3,
      40,    29,    18,    75,    22,    23,    24,    25,    78,    16,
      54,    54,    54,    50,    88,    54,    84,    18,    81,    28,
      40,    41,    42,    43,    44,    45,    99,    99,    34,    97,
      84,     6,    76,    17,    16,    54,    16,    93,    95,    51,
      96,    17,    18,    83,    82,     3,    54,    54,    84,    54,
      84,    98,    96,    54,    75,     3,    52,    77,    17,    54,
      54,     3,    84,    17,    81,    99,    28,    97,     3,     3,
      17,     3,    17,    95,    83,    54,    84,    54,     3,    36,
      89,    28,    98,    88,    54,    97
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
<<<<<<< HEAD
       0,    61,    62,    62,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    76,    77,    77,    78,    79,    79,
      79,    79,    80,    81,    82,    82,    83,    84,    84,    85,
      85,    85,    85,    86,    87,    88,    89,    89,    89,    90,
      90,    90,    91,    91,    92,    92,    93,    94,    94,    94,
      94,    94,    94,    95,    95,    96,    96,    96,    96,    97,
      97,    98,    98,    99,    99,   100,   100,   100,   100,   100,
     100,   100,   101,   101,   101,   101,   101,   101,   102
=======
       0,    60,    61,    61,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    72,    73,    74,    75,    75,    76,    76,    77,    78,
      78,    78,    78,    79,    80,    81,    81,    82,    83,    83,
      84,    84,    84,    84,    85,    86,    87,    88,    88,    89,
      89,    90,    90,    90,    90,    91,    91,    91,    92,    92,
      93,    93,    93,    94,    94,    94,    94,    95,    95,    96,
      96,    97,    97,    98,    98,    98,    98,    98,    98,    98,
      99,    99,    99,    99,    99,    99,   100
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     4,     3,     3,
<<<<<<< HEAD
       9,     4,     8,     0,     3,     5,     2,     1,     1,     1,
       1,     1,     1,     7,     0,     3,     4,     0,     3,     1,
       1,     1,     1,     5,     8,     9,     0,     4,     6,     0,
       3,     5,     0,     6,     0,     3,     2,     4,     4,     6,
       1,     1,     3,     0,     3,     1,     1,     1,     1,     0,
       3,     0,     3,     0,     3,     3,     3,     3,     3,     5,
       5,     7,     1,     1,     1,     1,     1,     1,     8
=======
       9,    10,     4,     8,     0,     3,     5,     2,     1,     1,
       1,     1,     1,     1,     7,     0,     3,     4,     0,     3,
       1,     1,     1,     1,     5,     8,     8,     0,     6,     0,
       3,     1,     2,     4,     2,     4,     4,     6,     0,     3,
       0,     3,     5,     1,     1,     1,     1,     0,     3,     0,
       3,     0,     3,     3,     3,     3,     3,     5,     5,     7,
       1,     1,     1,     1,     1,     1,     8
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (scanner, YY_("syntax error: cannot back up")); \
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
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
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
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
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
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

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
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
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
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
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
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
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
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

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
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
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
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
      yychar = yylex (&yylval, scanner);
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 21:
<<<<<<< HEAD
#line 172 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1584 "yacc_sql.tab.c"
    break;

  case 22:
#line 177 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1592 "yacc_sql.tab.c"
    break;

  case 23:
#line 182 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1600 "yacc_sql.tab.c"
    break;

  case 24:
#line 188 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1608 "yacc_sql.tab.c"
    break;

  case 25:
#line 194 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1616 "yacc_sql.tab.c"
    break;

  case 26:
#line 200 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1624 "yacc_sql.tab.c"
    break;

  case 27:
#line 206 "yacc_sql.y"
=======
#line 171 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1581 "yacc_sql.tab.c"
    break;

  case 22:
#line 176 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1589 "yacc_sql.tab.c"
    break;

  case 23:
#line 181 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1597 "yacc_sql.tab.c"
    break;

  case 24:
#line 187 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1605 "yacc_sql.tab.c"
    break;

  case 25:
#line 193 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1613 "yacc_sql.tab.c"
    break;

  case 26:
#line 199 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1621 "yacc_sql.tab.c"
    break;

  case 27:
#line 205 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
<<<<<<< HEAD
#line 1633 "yacc_sql.tab.c"
    break;

  case 28:
#line 212 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1641 "yacc_sql.tab.c"
    break;

  case 29:
#line 218 "yacc_sql.y"
=======
#line 1630 "yacc_sql.tab.c"
    break;

  case 28:
#line 211 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1638 "yacc_sql.tab.c"
    break;

  case 29:
#line 217 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
<<<<<<< HEAD
#line 1650 "yacc_sql.tab.c"
    break;

  case 30:
#line 226 "yacc_sql.y"
=======
#line 1647 "yacc_sql.tab.c"
    break;

  case 30:
#line 225 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string), 0);
		}
<<<<<<< HEAD
#line 1659 "yacc_sql.tab.c"
    break;

  case 31:
#line 234 "yacc_sql.y"
=======
#line 1656 "yacc_sql.tab.c"
    break;

  case 31:
#line 230 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string), 1);
		}
#line 1665 "yacc_sql.tab.c"
    break;

  case 32:
#line 238 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
<<<<<<< HEAD
#line 1668 "yacc_sql.tab.c"
    break;

  case 32:
#line 241 "yacc_sql.y"
=======
#line 1674 "yacc_sql.tab.c"
    break;

  case 33:
#line 245 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
<<<<<<< HEAD
#line 1680 "yacc_sql.tab.c"
    break;

  case 34:
#line 251 "yacc_sql.y"
                                   {    }
#line 1686 "yacc_sql.tab.c"
    break;

  case 35:
#line 256 "yacc_sql.y"
=======
#line 1686 "yacc_sql.tab.c"
    break;

  case 35:
#line 255 "yacc_sql.y"
                                   {    }
#line 1692 "yacc_sql.tab.c"
    break;

  case 36:
#line 260 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
<<<<<<< HEAD
#line 1701 "yacc_sql.tab.c"
    break;

  case 36:
#line 267 "yacc_sql.y"
=======
#line 1707 "yacc_sql.tab.c"
    break;

  case 37:
#line 271 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), 4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
<<<<<<< HEAD
#line 1716 "yacc_sql.tab.c"
    break;

  case 37:
#line 279 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1722 "yacc_sql.tab.c"
    break;

  case 38:
#line 282 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1728 "yacc_sql.tab.c"
    break;

  case 39:
#line 283 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1734 "yacc_sql.tab.c"
    break;

  case 40:
#line 284 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1740 "yacc_sql.tab.c"
    break;

  case 41:
#line 285 "yacc_sql.y"
                    {(yyval.number)=DATES;}
#line 1746 "yacc_sql.tab.c"
    break;

  case 42:
#line 289 "yacc_sql.y"
=======
#line 1722 "yacc_sql.tab.c"
    break;

  case 38:
#line 283 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1728 "yacc_sql.tab.c"
    break;

  case 39:
#line 286 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1734 "yacc_sql.tab.c"
    break;

  case 40:
#line 287 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1740 "yacc_sql.tab.c"
    break;

  case 41:
#line 288 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1746 "yacc_sql.tab.c"
    break;

  case 42:
#line 289 "yacc_sql.y"
                    {(yyval.number)=DATES;}
#line 1752 "yacc_sql.tab.c"
    break;

  case 43:
#line 293 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
<<<<<<< HEAD
#line 1755 "yacc_sql.tab.c"
    break;

  case 43:
#line 298 "yacc_sql.y"
=======
#line 1761 "yacc_sql.tab.c"
    break;

  case 44:
#line 302 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			//insert_tuplevalue_init(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->tuplev_begin, CONTEXT->value_length);
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string));

      //临时变量清零
	  CONTEXT->tuplev_begin=0;
      CONTEXT->value_length=0;
    }
<<<<<<< HEAD
#line 1769 "yacc_sql.tab.c"
    break;

  case 45:
#line 309 "yacc_sql.y"
                                        {		
	}
#line 1776 "yacc_sql.tab.c"
    break;

  case 46:
#line 312 "yacc_sql.y"
=======
#line 1775 "yacc_sql.tab.c"
    break;

  case 46:
#line 313 "yacc_sql.y"
                                        {		
	}
#line 1782 "yacc_sql.tab.c"
    break;

  case 47:
#line 316 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                                      {
		insert_tuplevalue_init(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->tuplev_begin, CONTEXT->value_length);
		CONTEXT->tuplev_begin = CONTEXT->value_length;
	}
<<<<<<< HEAD
#line 1785 "yacc_sql.tab.c"
    break;

  case 48:
#line 318 "yacc_sql.y"
                              { 
	  }
#line 1792 "yacc_sql.tab.c"
    break;

  case 49:
#line 322 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1800 "yacc_sql.tab.c"
    break;

  case 50:
#line 325 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1808 "yacc_sql.tab.c"
    break;

  case 51:
#line 328 "yacc_sql.y"
=======
#line 1791 "yacc_sql.tab.c"
    break;

  case 49:
#line 322 "yacc_sql.y"
                              { 
	  }
#line 1798 "yacc_sql.tab.c"
    break;

  case 50:
#line 326 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1806 "yacc_sql.tab.c"
    break;

  case 51:
#line 329 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1814 "yacc_sql.tab.c"
    break;

  case 52:
#line 332 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
<<<<<<< HEAD
#line 1817 "yacc_sql.tab.c"
    break;

  case 52:
#line 332 "yacc_sql.y"
=======
#line 1823 "yacc_sql.tab.c"
    break;

  case 53:
#line 336 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
             {
		    (yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
		value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
<<<<<<< HEAD
#line 1826 "yacc_sql.tab.c"
    break;

  case 53:
#line 339 "yacc_sql.y"
=======
#line 1832 "yacc_sql.tab.c"
    break;

  case 54:
#line 343 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
<<<<<<< HEAD
#line 1838 "yacc_sql.tab.c"
    break;

  case 54:
#line 349 "yacc_sql.y"
=======
#line 1844 "yacc_sql.tab.c"
    break;

  case 55:
#line 353 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
<<<<<<< HEAD
#line 1850 "yacc_sql.tab.c"
    break;

  case 55:
#line 359 "yacc_sql.y"
=======
#line 1856 "yacc_sql.tab.c"
    break;

  case 56:
#line 363 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
<<<<<<< HEAD
#line 1870 "yacc_sql.tab.c"
    break;

  case 57:
#line 377 "yacc_sql.y"
                                {
		RelAttr attr;
		relation_attr_init(&attr, NULL, (yyvsp[-1].string));
		selects_append_groupby(&CONTEXT->ssql->sstr.selection,&attr);
	}
#line 1880 "yacc_sql.tab.c"
    break;

  case 58:
#line 382 "yacc_sql.y"
                                       {
		RelAttr attr;
		relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_groupby(&CONTEXT->ssql->sstr.selection,&attr);
	}
#line 1890 "yacc_sql.tab.c"
    break;

  case 60:
#line 390 "yacc_sql.y"
                             {
		RelAttr attr;
		relation_attr_init(&attr, NULL, (yyvsp[-1].string));
		selects_append_groupby(&CONTEXT->ssql->sstr.selection,&attr);
	}
#line 1900 "yacc_sql.tab.c"
    break;

  case 61:
#line 395 "yacc_sql.y"
                                    {
		RelAttr attr;
		relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_groupby(&CONTEXT->ssql->sstr.selection,&attr);
	}
#line 1910 "yacc_sql.tab.c"
    break;

  case 63:
#line 403 "yacc_sql.y"
                                                              {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
	}
#line 1918 "yacc_sql.tab.c"
    break;

  case 65:
#line 409 "yacc_sql.y"
                                     {

	}
#line 1926 "yacc_sql.tab.c"
    break;

  case 66:
#line 415 "yacc_sql.y"
                                  {

	}
#line 1934 "yacc_sql.tab.c"
    break;

  case 67:
#line 421 "yacc_sql.y"
=======
#line 1876 "yacc_sql.tab.c"
    break;

  case 58:
#line 382 "yacc_sql.y"
                                                              {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
	}
#line 1884 "yacc_sql.tab.c"
    break;

  case 60:
#line 388 "yacc_sql.y"
                                     {

	}
#line 1892 "yacc_sql.tab.c"
    break;

  case 61:
#line 394 "yacc_sql.y"
         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1902 "yacc_sql.tab.c"
    break;

  case 62:
#line 399 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1912 "yacc_sql.tab.c"
    break;

  case 63:
#line 404 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1922 "yacc_sql.tab.c"
    break;

  case 64:
#line 409 "yacc_sql.y"
                                  {

	}
#line 1930 "yacc_sql.tab.c"
    break;

  case 65:
#line 414 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                                         {
		RelAttr attr;
		aggr_relation_attr_init(&attr, NULL, "*", CONTEXT->aggcomp);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection,&attr);
	}
<<<<<<< HEAD
#line 1944 "yacc_sql.tab.c"
    break;

  case 68:
#line 426 "yacc_sql.y"
=======
#line 1942 "yacc_sql.tab.c"
    break;

  case 66:
#line 421 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                                         {   
		RelAttr attr;
		aggr_relation_attr_init(&attr, NULL, (yyvsp[-1].string), CONTEXT->aggcomp);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection,&attr);
	}
#line 1954 "yacc_sql.tab.c"
    break;

<<<<<<< HEAD
  case 69:
#line 431 "yacc_sql.y"
=======
  case 67:
#line 428 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                                                {
		RelAttr attr;
		aggr_relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string), CONTEXT->aggcomp);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection,&attr);
	}
<<<<<<< HEAD
#line 1964 "yacc_sql.tab.c"
    break;

  case 70:
#line 436 "yacc_sql.y"
               {  
		RelAttr attr;
		relation_attr_init(&attr, NULL, "*");
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 1974 "yacc_sql.tab.c"
    break;
=======
#line 1966 "yacc_sql.tab.c"
    break;

  case 69:
#line 438 "yacc_sql.y"
                                        {
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286

  case 71:
#line 441 "yacc_sql.y"
         {
		RelAttr attr;
		relation_attr_init(&attr, NULL, (yyvsp[0].string));
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
<<<<<<< HEAD
#line 1984 "yacc_sql.tab.c"
    break;

  case 72:
#line 446 "yacc_sql.y"
                    {
		RelAttr attr;
		relation_attr_init(&attr, (yyvsp[-2].string), (yyvsp[0].string));
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 1994 "yacc_sql.tab.c"
    break;

  case 74:
#line 455 "yacc_sql.y"
                                              {

	}
#line 2002 "yacc_sql.tab.c"
    break;

  case 75:
#line 482 "yacc_sql.y"
                { CONTEXT->aggcomp = COUNT_AGG;}
#line 2008 "yacc_sql.tab.c"
    break;

  case 76:
#line 483 "yacc_sql.y"
              { CONTEXT->aggcomp = MAX_AGG;}
#line 2014 "yacc_sql.tab.c"
    break;

  case 77:
#line 484 "yacc_sql.y"
              { CONTEXT->aggcomp = MIN_AGG;}
#line 2020 "yacc_sql.tab.c"
    break;

  case 78:
#line 485 "yacc_sql.y"
              { CONTEXT->aggcomp = AVG_AGG;}
#line 2026 "yacc_sql.tab.c"
    break;

  case 80:
#line 491 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2034 "yacc_sql.tab.c"
    break;

  case 82:
#line 497 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2042 "yacc_sql.tab.c"
    break;

  case 84:
#line 503 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2050 "yacc_sql.tab.c"
    break;

  case 85:
#line 509 "yacc_sql.y"
=======
#line 1974 "yacc_sql.tab.c"
    break;

  case 71:
#line 446 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 1986 "yacc_sql.tab.c"
    break;

  case 72:
#line 453 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 1998 "yacc_sql.tab.c"
    break;

  case 73:
#line 464 "yacc_sql.y"
                { CONTEXT->aggcomp = COUNT_AGG;}
#line 2004 "yacc_sql.tab.c"
    break;

  case 74:
#line 465 "yacc_sql.y"
              { CONTEXT->aggcomp = MAX_AGG;}
#line 2010 "yacc_sql.tab.c"
    break;

  case 75:
#line 466 "yacc_sql.y"
              { CONTEXT->aggcomp = MIN_AGG;}
#line 2016 "yacc_sql.tab.c"
    break;

  case 76:
#line 467 "yacc_sql.y"
              { CONTEXT->aggcomp = AVG_AGG;}
#line 2022 "yacc_sql.tab.c"
    break;

  case 78:
#line 473 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2030 "yacc_sql.tab.c"
    break;

  case 80:
#line 479 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2038 "yacc_sql.tab.c"
    break;

  case 82:
#line 485 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2046 "yacc_sql.tab.c"
    break;

  case 83:
#line 491 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
<<<<<<< HEAD
#line 2075 "yacc_sql.tab.c"
    break;

  case 86:
#line 530 "yacc_sql.y"
=======
#line 2071 "yacc_sql.tab.c"
    break;

  case 84:
#line 512 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
<<<<<<< HEAD
#line 2099 "yacc_sql.tab.c"
    break;

  case 87:
#line 550 "yacc_sql.y"
=======
#line 2095 "yacc_sql.tab.c"
    break;

  case 85:
#line 532 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
<<<<<<< HEAD
#line 2123 "yacc_sql.tab.c"
    break;

  case 88:
#line 570 "yacc_sql.y"
=======
#line 2119 "yacc_sql.tab.c"
    break;

  case 86:
#line 552 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
<<<<<<< HEAD
#line 2149 "yacc_sql.tab.c"
    break;

  case 89:
#line 592 "yacc_sql.y"
=======
#line 2145 "yacc_sql.tab.c"
    break;

  case 87:
#line 574 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    }
<<<<<<< HEAD
#line 2174 "yacc_sql.tab.c"
    break;

  case 90:
#line 613 "yacc_sql.y"
=======
#line 2170 "yacc_sql.tab.c"
    break;

  case 88:
#line 595 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    }
<<<<<<< HEAD
#line 2199 "yacc_sql.tab.c"
    break;

  case 91:
#line 634 "yacc_sql.y"
=======
#line 2195 "yacc_sql.tab.c"
    break;

  case 89:
#line 616 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    }
<<<<<<< HEAD
#line 2222 "yacc_sql.tab.c"
    break;

  case 92:
#line 655 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2228 "yacc_sql.tab.c"
    break;

  case 93:
#line 656 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2234 "yacc_sql.tab.c"
    break;

  case 94:
#line 657 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2240 "yacc_sql.tab.c"
    break;

  case 95:
#line 658 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2246 "yacc_sql.tab.c"
    break;

  case 96:
#line 659 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2252 "yacc_sql.tab.c"
    break;

  case 97:
#line 660 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2258 "yacc_sql.tab.c"
    break;

  case 98:
#line 665 "yacc_sql.y"
=======
#line 2218 "yacc_sql.tab.c"
    break;

  case 90:
#line 637 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2224 "yacc_sql.tab.c"
    break;

  case 91:
#line 638 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2230 "yacc_sql.tab.c"
    break;

  case 92:
#line 639 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2236 "yacc_sql.tab.c"
    break;

  case 93:
#line 640 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2242 "yacc_sql.tab.c"
    break;

  case 94:
#line 641 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2248 "yacc_sql.tab.c"
    break;

  case 95:
#line 642 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2254 "yacc_sql.tab.c"
    break;

  case 96:
#line 647 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
<<<<<<< HEAD
#line 2267 "yacc_sql.tab.c"
    break;


#line 2271 "yacc_sql.tab.c"
=======
#line 2263 "yacc_sql.tab.c"
    break;


#line 2267 "yacc_sql.tab.c"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286

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
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

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
      yyerror (scanner, YY_("syntax error"));
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
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
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
        yyerror (scanner, yymsgp);
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
                      yytoken, &yylval, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

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
                  yystos[yystate], yyvsp, scanner);
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
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, scanner);
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
<<<<<<< HEAD
#line 670 "yacc_sql.y"
=======
#line 652 "yacc_sql.y"
>>>>>>> e64db892a051e793e506693cc71d76b68eb5f286

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
