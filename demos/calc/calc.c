
/*  A Bison parser, made from calc.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	EOS	257
#define	BAD	258
#define	HELP	259
#define	HEX	260
#define	DECIMAL	261
#define	QUIT	262
#define	ABS	263
#define	BIN	264
#define	FIB	265
#define	GCD	266
#define	KRON	267
#define	LCM	268
#define	LUCNUM	269
#define	NEXTPRIME	270
#define	POWM	271
#define	ROOT	272
#define	SQRT	273
#define	NUMBER	274
#define	VARIABLE	275
#define	LOR	276
#define	LAND	277
#define	EQ	278
#define	NE	279
#define	LE	280
#define	GE	281
#define	LSHIFT	282
#define	RSHIFT	283
#define	UMINUS	284

#line 1 "calc.y"

/* A simple integer desk calculator using yacc and gmp.

Copyright 2000, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA. */


/* This is a simple program, meant only to show one way to use GMP for this
   sort of thing.  There's few features, and error checking is minimal.
   Standard input is read, calc_help() below shows the inputs accepted.

   Expressions are evaluated as they're read.  If user defined functions
   were wanted it'd be necessary to build a parse tree like pexpr.c does, or
   a list of operations for a stack based evaluator.  That would also make
   it possible to detect and optimize evaluations "mod m" like pexpr.c does.

   A stack is used for intermediate values in the expression evaluation,
   separate from the yacc parser stack.  This is simple, makes error
   recovery easy, minimizes the junk around mpz calls in the rules, and
   saves initializing or clearing "mpz_t"s during a calculation.  A
   disadvantage though is that variables must be copied to the stack to be
   worked on.  A more sophisticated calculator or language system might be
   able to avoid that when executing a compiled or semi-compiled form.

   Avoiding repeated initializing and clearing of "mpz_t"s is important.  In
   this program the time spent parsing is obviously much greater than any
   possible saving from this, but a proper calculator or language should
   take some trouble over it.  Don't be surprised if an init/clear takes 3
   or more times as long as a 10 limb addition, depending on the system (see
   the mpz_init_realloc_clear example in tune/README).  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gmp.h"
#define NO_CALC_H /* because it conflicts with normal calc.c stuff */
#include "calc-common.h"


#define numberof(x)  (sizeof (x) / sizeof ((x)[0]))


void
calc_help (void)
{
  printf ("Examples:\n");
  printf ("    2+3*4        expressions are evaluated\n");
  printf ("    x=5^6        variables a to z can be set and used\n");
  printf ("Operators:\n");
  printf ("    + - *        arithmetic\n");
  printf ("    / %%          division and remainder (rounding towards negative infinity)\n");
  printf ("    ^            exponentiation\n");
  printf ("    !            factorial\n");
  printf ("    << >>        left and right shifts\n");
  printf ("    <= >= >      \\ comparisons, giving 1 if true, 0 if false\n");
  printf ("    == != <      /\n");
  printf ("    && ||        logical and/or, giving 1 if true, 0 if false\n");
  printf ("Functions:\n");
  printf ("    abs(n)       absolute value\n");
  printf ("    bin(n,m)     binomial coefficient\n");
  printf ("    fib(n)       fibonacci number\n");
  printf ("    gcd(a,b,..)  greatest common divisor\n");
  printf ("    kron(a,b)    kronecker symbol\n");
  printf ("    lcm(a,b,..)  least common multiple\n");
  printf ("    lucnum(n)    lucas number\n");
  printf ("    nextprime(n) next prime after n\n");
  printf ("    powm(b,e,m)  modulo powering, b^e%%m\n");
  printf ("    root(n,r)    r-th root\n");
  printf ("    sqrt(n)      square root\n");
  printf ("Other:\n");
  printf ("    hex          \\ set hex or decimal for input and output\n");
  printf ("    decimal      /   (\"0x\" can be used for hex too)\n");
  printf ("    quit         exit program (EOF works too)\n");
  printf ("    ;            statements are separated with a ; or newline\n");
  printf ("    \\            continue expressions with \\ before newline\n");
  printf ("    # xxx        comments are # though to newline\n");
  printf ("Hex numbers must be entered in upper case, to distinguish them from the\n");
  printf ("variables a to f (like in bc).\n");
}


int  ibase = 0;
int  obase = 10;


/* The stack is a fixed size, which means there's a limit on the nesting
   allowed in expressions.  A more sophisticated program could let it grow
   dynamically.  */

mpz_t    stack[100];
mpz_ptr  sp = stack[0];

#define CHECK_OVERFLOW()                                                  \
  if (sp >= stack[numberof(stack)])                                       \
    {                                                                     \
      fprintf (stderr,                                                    \
               "Value stack overflow, too much nesting in expression\n"); \
      YYERROR;                                                            \
    }

#define CHECK_EMPTY()                                                   \
  if (sp != stack[0])                                                   \
    {                                                                   \
      fprintf (stderr, "Oops, expected the value stack to be empty\n"); \
      sp = stack[0];                                                    \
    }


mpz_t  variable[26];

#define CHECK_VARIABLE(var)                                             \
  if ((var) < 0 || (var) >= numberof (variable))                        \
    {                                                                   \
      fprintf (stderr, "Oops, bad variable somehow: %d\n", var);        \
      YYERROR;                                                          \
    }


#define CHECK_UI(name,z)                                                   \
  if (! mpz_fits_ulong_p (z))                                              \
    {                                                                      \
      fprintf (stderr,                                                     \
               "Operand must fit in an \"unsigned long\" for %s\n", name); \
      YYERROR;                                                             \
    }


#line 144 "calc.y"
typedef union {
  char  *str;
  int   var;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		118
#define	YYFLAG		-32768
#define	YYNTBASE	44

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 50)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    39,     2,     2,     2,    36,     2,     2,    41,
    42,    34,    32,    43,    33,     2,    35,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    24,
    40,    25,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    38,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    26,    27,    28,
    29,    30,    31,    37
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     8,    12,    15,    16,    18,    22,    24,
    26,    28,    30,    34,    38,    42,    46,    50,    54,    58,
    62,    66,    69,    72,    76,    80,    84,    88,    92,    96,
   100,   104,   109,   116,   121,   126,   133,   138,   143,   148,
   157,   164,   169,   171,   173,   175,   179,   181
};

static const short yyrhs[] = {    46,
     0,    45,    46,     0,    46,     3,     0,    45,    46,     3,
     0,     1,     3,     0,     0,    47,     0,    21,    40,    47,
     0,     5,     0,     6,     0,     7,     0,     8,     0,    41,
    47,    42,     0,    47,    32,    47,     0,    47,    33,    47,
     0,    47,    34,    47,     0,    47,    35,    47,     0,    47,
    36,    47,     0,    47,    38,    47,     0,    47,    30,    47,
     0,    47,    31,    47,     0,    47,    39,     0,    33,    47,
     0,    47,    24,    47,     0,    47,    28,    47,     0,    47,
    26,    47,     0,    47,    27,    47,     0,    47,    29,    47,
     0,    47,    25,    47,     0,    47,    23,    47,     0,    47,
    22,    47,     0,     9,    41,    47,    42,     0,    10,    41,
    47,    43,    47,    42,     0,    11,    41,    47,    42,     0,
    12,    41,    48,    42,     0,    13,    41,    47,    43,    47,
    42,     0,    14,    41,    49,    42,     0,    15,    41,    47,
    42,     0,    16,    41,    47,    42,     0,    17,    41,    47,
    43,    47,    43,    47,    42,     0,    18,    41,    47,    43,
    47,    42,     0,    19,    41,    47,    42,     0,    21,     0,
    20,     0,    47,     0,    48,    43,    47,     0,    47,     0,
    49,    43,    47,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   168,   170,   172,   174,   175,   177,   179,   184,   190,   191,
   192,   193,   198,   200,   201,   202,   203,   204,   205,   207,
   209,   211,   213,   215,   216,   217,   218,   219,   220,   222,
   223,   225,   226,   228,   230,   231,   233,   234,   236,   237,
   238,   240,   242,   248,   258,   260,   262,   264
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EOS","BAD",
"HELP","HEX","DECIMAL","QUIT","ABS","BIN","FIB","GCD","KRON","LCM","LUCNUM",
"NEXTPRIME","POWM","ROOT","SQRT","NUMBER","VARIABLE","LOR","LAND","'<'","'>'",
"EQ","NE","LE","GE","LSHIFT","RSHIFT","'+'","'-'","'*'","'/'","'%'","UMINUS",
"'^'","'!'","'='","'('","')'","','","top","statements","statement","e","gcdlist",
"lcmlist", NULL
};
#endif

static const short yyr1[] = {     0,
    44,    44,    45,    45,    45,    46,    46,    46,    46,    46,
    46,    46,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    47,    47,    48,    48,    49,    49
};

static const short yyr2[] = {     0,
     1,     2,     2,     3,     2,     0,     1,     3,     1,     1,
     1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     2,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     4,     6,     4,     4,     6,     4,     4,     4,     8,
     6,     4,     1,     1,     1,     3,     1,     3
};

static const short yydefact[] = {     0,
     0,     9,    10,    11,    12,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    44,    43,     0,     0,
     6,     1,     7,     5,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    43,    23,     0,     2,
     3,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
     0,    45,     0,     0,    47,     0,     0,     0,     0,     0,
     0,     8,    13,     4,    31,    30,    24,    29,    26,    27,
    25,    28,    20,    21,    14,    15,    16,    17,    18,    19,
    32,     0,    34,    35,     0,     0,    37,     0,    38,    39,
     0,     0,    42,     0,    46,     0,    48,     0,     0,    33,
    36,     0,    41,     0,    40,     0,     0,     0
};

static const short yydefgoto[] = {   116,
    21,    22,    23,    63,    66
};

static const short yypact[] = {    39,
    17,-32768,-32768,-32768,-32768,   -20,     0,     2,    25,    28,
    30,    33,    34,    37,    40,    46,-32768,   -18,   122,   122,
    89,    67,   462,-32768,   122,   122,   122,   122,   122,   122,
   122,   122,   122,   122,   122,   122,-32768,   -36,   252,    87,
-32768,   122,   122,   122,   122,   122,   122,   122,   122,   122,
   122,   122,   122,   122,   122,   122,   122,-32768,   273,   142,
   294,   462,   -38,   164,   462,   -24,   315,   336,   186,   208,
   357,   462,-32768,-32768,   479,   495,   511,   511,   511,   511,
   511,   511,    29,    29,    50,    50,   -36,   -36,   -36,   -36,
-32768,   122,-32768,-32768,   122,   122,-32768,   122,-32768,-32768,
   122,   122,-32768,   378,   462,   399,   462,   230,   420,-32768,
-32768,   122,-32768,   441,-32768,    91,    92,-32768
};

static const short yypgoto[] = {-32768,
-32768,    90,   -19,-32768,-32768
};


#define	YYLAST		550


static const short yytable[] = {    38,
    39,    57,    58,    94,    95,    59,    60,    61,    62,    64,
    65,    67,    68,    69,    70,    71,    72,    97,    98,    24,
    25,    36,    75,    76,    77,    78,    79,    80,    81,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    -6,     1,
    26,    -6,    27,     2,     3,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    52,    53,    54,    55,    56,    28,    57,    58,    29,    41,
    30,    19,   104,    31,    32,   105,   106,    33,   107,    20,
    34,   108,   109,    54,    55,    56,    35,    57,    58,    74,
   117,   118,   114,     2,     3,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    19,     0,     0,     0,     0,     0,     0,     0,    20,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    37,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    19,     0,     0,     0,     0,     0,
     0,     0,    20,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,     0,    57,
    58,     0,     0,     0,    92,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
     0,    57,    58,     0,     0,     0,    96,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,     0,    57,    58,     0,     0,     0,   101,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,     0,    57,    58,     0,     0,     0,
   102,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,     0,    57,    58,     0,
     0,     0,   112,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,     0,    57,
    58,     0,     0,    73,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,     0,
    57,    58,     0,     0,    91,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
     0,    57,    58,     0,     0,    93,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,     0,    57,    58,     0,     0,    99,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,     0,    57,    58,     0,     0,   100,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,     0,    57,    58,     0,     0,   103,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,     0,    57,    58,     0,     0,   110,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,     0,    57,    58,     0,     0,
   111,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,     0,    57,    58,     0,
     0,   113,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    56,     0,    57,    58,
     0,     0,   115,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,     0,    57,
    58,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,     0,    57,    58,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,     0,    57,    58,-32768,-32768,-32768,-32768,-32768,-32768,
    50,    51,    52,    53,    54,    55,    56,     0,    57,    58
};

static const short yycheck[] = {    19,
    20,    38,    39,    42,    43,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    42,    43,     3,
    41,    40,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    56,    57,     0,     1,
    41,     3,    41,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    32,    33,    34,    35,    36,    41,    38,    39,    41,     3,
    41,    33,    92,    41,    41,    95,    96,    41,    98,    41,
    41,   101,   102,    34,    35,    36,    41,    38,    39,     3,
     0,     0,   112,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    33,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    41,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
    39,    -1,    -1,    -1,    43,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    -1,    38,    39,    -1,    -1,    -1,    43,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    -1,    38,    39,    -1,    -1,    -1,    43,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,
    43,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    -1,    38,    39,    -1,
    -1,    -1,    43,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
    39,    -1,    -1,    42,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    -1,
    38,    39,    -1,    -1,    42,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    -1,    38,    39,    -1,    -1,    42,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    -1,    38,    39,    -1,    -1,    42,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    -1,    38,    39,    -1,    -1,    42,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    -1,    38,    39,    -1,    -1,    42,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    -1,    38,    39,    -1,    -1,    42,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    -1,    38,    39,    -1,    -1,
    42,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    -1,    38,    39,    -1,
    -1,    42,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
    -1,    -1,    42,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
    39,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    -1,    38,    39,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    -1,    38,    39,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    -1,    38,    39
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 5:
#line 175 "calc.y"
{ sp = stack[0]; yyerrok; ;
    break;}
case 7:
#line 179 "calc.y"
{
      mpz_out_str (stdout, obase, sp); putchar ('\n');
      sp--;
      CHECK_EMPTY ();
    ;
    break;}
case 8:
#line 184 "calc.y"
{
      CHECK_VARIABLE (yyvsp[-2].var);
      mpz_swap (variable[yyvsp[-2].var], sp);
      sp--;
      CHECK_EMPTY ();
    ;
    break;}
case 9:
#line 190 "calc.y"
{ calc_help (); ;
    break;}
case 10:
#line 191 "calc.y"
{ ibase = 16; obase = -16; ;
    break;}
case 11:
#line 192 "calc.y"
{ ibase = 0;  obase = 10; ;
    break;}
case 12:
#line 193 "calc.y"
{ exit (0); ;
    break;}
case 14:
#line 200 "calc.y"
{ sp--; mpz_add    (sp, sp, sp+1); ;
    break;}
case 15:
#line 201 "calc.y"
{ sp--; mpz_sub    (sp, sp, sp+1); ;
    break;}
case 16:
#line 202 "calc.y"
{ sp--; mpz_mul    (sp, sp, sp+1); ;
    break;}
case 17:
#line 203 "calc.y"
{ sp--; mpz_fdiv_q (sp, sp, sp+1); ;
    break;}
case 18:
#line 204 "calc.y"
{ sp--; mpz_fdiv_r (sp, sp, sp+1); ;
    break;}
case 19:
#line 205 "calc.y"
{ CHECK_UI ("exponentiation", sp);
                     sp--; mpz_pow_ui (sp, sp, mpz_get_ui (sp+1)); ;
    break;}
case 20:
#line 207 "calc.y"
{ CHECK_UI ("lshift", sp);
                     sp--; mpz_mul_2exp (sp, sp, mpz_get_ui (sp+1)); ;
    break;}
case 21:
#line 209 "calc.y"
{ CHECK_UI ("rshift", sp);
                     sp--; mpz_fdiv_q_2exp (sp, sp, mpz_get_ui (sp+1)); ;
    break;}
case 22:
#line 211 "calc.y"
{ CHECK_UI ("factorial", sp);
                     mpz_fac_ui (sp, mpz_get_ui (sp)); ;
    break;}
case 23:
#line 213 "calc.y"
{ mpz_neg (sp, sp); ;
    break;}
case 24:
#line 215 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) <  0); ;
    break;}
case 25:
#line 216 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) <= 0); ;
    break;}
case 26:
#line 217 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) == 0); ;
    break;}
case 27:
#line 218 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) != 0); ;
    break;}
case 28:
#line 219 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) >= 0); ;
    break;}
case 29:
#line 220 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) >  0); ;
    break;}
case 30:
#line 222 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_sgn (sp) && mpz_sgn (sp+1)); ;
    break;}
case 31:
#line 223 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_sgn (sp) || mpz_sgn (sp+1)); ;
    break;}
case 32:
#line 225 "calc.y"
{ mpz_abs (sp, sp); ;
    break;}
case 33:
#line 226 "calc.y"
{ sp--; CHECK_UI ("binomial", sp+1);
                                    mpz_bin_ui (sp, sp, mpz_get_ui (sp+1)); ;
    break;}
case 34:
#line 228 "calc.y"
{ CHECK_UI ("fibonacci", sp);
                                    mpz_fib_ui (sp, mpz_get_ui (sp)); ;
    break;}
case 36:
#line 231 "calc.y"
{ sp--; mpz_set_si (sp,
                                          mpz_kronecker (sp, sp+1)); ;
    break;}
case 38:
#line 234 "calc.y"
{ CHECK_UI ("lucnum", sp);
                                    mpz_lucnum_ui (sp, mpz_get_ui (sp)); ;
    break;}
case 39:
#line 236 "calc.y"
{ mpz_nextprime (sp, sp); ;
    break;}
case 40:
#line 237 "calc.y"
{ sp -= 2; mpz_powm (sp, sp, sp+1, sp+2); ;
    break;}
case 41:
#line 238 "calc.y"
{ sp--; CHECK_UI ("nth-root", sp+1);
                                    mpz_root (sp, sp, mpz_get_ui (sp+1)); ;
    break;}
case 42:
#line 240 "calc.y"
{ mpz_sqrt (sp, sp); ;
    break;}
case 43:
#line 242 "calc.y"
{
        sp++;
        CHECK_OVERFLOW ();
        CHECK_VARIABLE (yyvsp[0].var);
        mpz_set (sp, variable[yyvsp[0].var]);
      ;
    break;}
case 44:
#line 248 "calc.y"
{
        sp++;
        CHECK_OVERFLOW ();
        if (mpz_set_str (sp, yyvsp[0].str, ibase) != 0)
          {
            fprintf (stderr, "Invalid number: %s\n", yyvsp[0].str);
            YYERROR;
          }
      ;
    break;}
case 46:
#line 260 "calc.y"
{ sp--; mpz_gcd (sp, sp, sp+1); ;
    break;}
case 48:
#line 264 "calc.y"
{ sp--; mpz_lcm (sp, sp, sp+1); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 266 "calc.y"


yyerror (char *s)
{
  fprintf (stderr, "%s\n", s);
}

int calc_option_readline = -1;

int
main (int argc, char *argv[])
{
  int  i;

  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "--readline") == 0)
        calc_option_readline = 1;
      else if (strcmp (argv[i], "--noreadline") == 0)
        calc_option_readline = 0;
      else if (strcmp (argv[i], "--help") == 0)
        {
          printf ("Usage: calc [--option]...\n");
          printf ("  --readline    use readline\n");
          printf ("  --noreadline  don't use readline\n");
          printf ("  --help        this message\n");
          printf ("Readline is only available when compiled in,\n");
          printf ("and in that case it's the default on a tty.\n");
          exit (0);
        }
      else
        {
          fprintf (stderr, "Unrecognised option: %s\n", argv[i]);
          exit (1);
        }
    }

#if WITH_READLINE
  calc_init_readline ();
#else
  if (calc_option_readline == 1)
    {
      fprintf (stderr, "Readline support not available\n");
      exit (1);
    }
#endif

  for (i = 0; i < numberof (variable); i++)
    mpz_init (variable[i]);

  for (i = 0; i < numberof (stack); i++)
    mpz_init (stack[i]);

  return yyparse ();
}
