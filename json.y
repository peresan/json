%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "node.h"
int yyerror(char *s);
Node *tree;
%}

%union {
	int i;                  /* integer value */
	double r;               /* real value */
	char *s;                /* symbol name or string literal */
	Node *n;                /* node pointer */
};
%token <i> INT
%token <r> REAL
%token <s> STR

%token True False Null OBJS PAIR ARRAY

%type <n> json object pair pairs array value values
%%
json	: value			{ tree = $1; }
	;

object	: '{' pairs '}'		{ $$ = uniNode(OBJS, $2); }
	| '{' '}'		{ $$ = nilNode(OBJS); }
	;

pairs	: pair
	| pairs ',' pair	{ $$ = binNode(OBJS, $1, $3); }
	;

pair	: STR ':' value		{ $$ = binNode(PAIR, strNode(STR, $1), $3); }
	;

array	: '[' values ']'	{ $$ = $2; }
	| '[' ']'		{ $$ = nilNode(ARRAY); }
	;

values	: value
	| values ',' value	{ $$ = binNode(ARRAY, $1, $3); }

value	: STR			{ $$ = strNode(STR, $1); }
	| INT			{ $$ = intNode(INT, $1); }
	| REAL			{ $$ = realNode(REAL, $1); }
	| object
	| array
	| True			{ $$ = nilNode(True); }
	| False			{ $$ = nilNode(False); }
	| Null			{ $$ = nilNode(Null); }
	;
%%
char **yynames =
#if YYDEBUG > 0
	(char**)yyname;
#else
	0;
#endif
