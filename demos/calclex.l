/* Lexical analyzer for calc.y. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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
Place - Suite 330, Boston, MA 02111-1307, USA.
*/


%{
#include "calc.h"

#define numberof(x)  (sizeof (x) / sizeof ((x)[0]))
%}

%%

[ \t\f] { /* white space is skipped */ }

[;\n]   { /* semicolon or newline separates statements */
          return EOS; }
\\\n    { /* escaped newlines are skipped */ }


#(([^\\\n]*)\\)+\n {
            /* comment through to escaped newline is skipped */ }
#[^\n]*\n { /* comment through to newline is a separator */
            return EOS; }
#[^\n]* {   /* comment through to EOF skipped */ }


[-+*/%()<>^!=,] { return yytext[0]; }
"<="    { return LE; }
">="    { return GE; }
"=="    { return EQ; }
"!="    { return NE; }
"<<"    { return LSHIFT; }
">>"    { return RSHIFT; }
"&&"    { return LAND; }
"||"    { return LOR; }

(0[xX])?[0-9A-F]+ {
        yylval.str = yytext;
        return NUMBER; }

[a-zA-Z][a-zA-Z0-9]* {
        static struct {
          char  *name;
          int   value;
        } table[] = {
          { "abs",       ABS },
          { "bin",       BIN },
          { "decimal",   DECIMAL },
          { "fib",       FIB },
          { "hex",       HEX },
          { "gcd",       GCD },
          { "lcm",       LCM },
          { "nextprime", NEXTPRIME },
          { "powm",      POWM },
          { "quit",      QUIT },
          { "root",      ROOT },
          { "sqrt",      SQRT },
        };
        int  i;

        for (i = 0; i < numberof (table); i++)
          if (strcmp (yytext, table[i].name) == 0)
            return table[i].value;

        if (yytext[0] >= 'a' && yytext[0] <= 'z' && yytext[1] == '\0')
          {
            yylval.var = yytext[0] - 'a';
            return VARIABLE;
          }

        return BAD;
}

. { return BAD; }

%%

int
yywrap ()
{
  return 1;
}
