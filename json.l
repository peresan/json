%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "y.tab.h"
static int strsz;
static char *str;
int yywrap(), yyerror(char*);
%}
%option yylineno
%x STRNG
H [A-Fa-f0-9]
I [0-9]+
E ([Ee][+-]?[0-9]+)
R ([-]?{I}[.]{I}{E}?)|([-]?{I}{E})
%%
[ \t\n\r]+	;
"true"		return True;
"false"		return False;
"null"		return Null;
[{}\[\],:]	return *yytext;
[-]?{I}		yylval.i = strtol(yytext, 0, 0); return INT;
{R}		yylval.r = strtod(yytext, 0); return REAL;

\"([^"\\]|\\.)*\"	{ strsz = yyleng; REJECT; }
\"		if (strsz > 0) { BEGIN STRNG; str = malloc(strsz); *str = 0; }
<STRNG>\"		BEGIN 0; yylval.s = str; return STR;
<STRNG>\\\"             strcat(str, "\"");
<STRNG>\\\\             strcat(str, "\\");
<STRNG>\\b              strcat(str, "\b");
<STRNG>\\f              strcat(str, "\f");
<STRNG>\\n              strcat(str, "\n");
<STRNG>\\r              strcat(str, "\r");
<STRNG>\\t              strcat(str, "\t");
<STRNG>\\{H}{H}{H}{H}	{ unsigned char s[3]; s[2] = 0;
			  s[0] = strtol(yytext+1,0,16) >> 16;
			  s[1] = strtol(yytext+3,0,16);
			  strcat(str, (char*)s); }
<STRNG>.|\n             strcat(str, yytext);
.			{ char s[30] = "unrecognised character: "; yyerror(strcat(s,yytext)); }
%%
int yywrap() { return 1; }

char *getyytext() { return yytext; }
