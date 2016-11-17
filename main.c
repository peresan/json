#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "node.h"
#include "y.tab.h"
#ifdef YYDEBUG
  extern int yydebug;
#endif

char *dupstr(const char *);
FILE *outfp;

void nl(int lev) {
  if (lev) fprintf(outfp, "\n%*s", 2*lev, " ");
  else fprintf(outfp, "\n");
}

void xml(Node *n);
void json(Node *n, int lev);
int flat(Node *n, char *tag);
void print(Node *tree, char **res, int rescnt);
char **resources(char *file, int *rescnt);

int errors, yyparse(void);
char *infile = "stdin";

int yyerror(char *s)
{
  extern int yylineno;
  extern char *getyytext();
  fprintf(stderr, "%s: %s at or before '%s' in line %d\n", infile, s, getyytext(), yylineno);
  errors++;
  return 1;
}

char *dupstr(const char *s)
{
  char *d;
  if (s == 0) return 0;
  d = (char*)malloc(strlen(s)+1);
  if (d == 0) return 0;
  strcpy(d, s);
  return d;
}

int main(int argc, char *argv[]) {
  extern char **yynames;
  extern Node *tree;
  char **res;
  int f = 0, j = 0, p = 0, x = 0, rescnt = 0;
#ifdef YYDEBUG
  yydebug = getenv("YYDEBUG") ? 1 : 0;
#endif

  if (argc > 1 && strcmp(argv[1], "-f") == 0) f = 1;
  if (argc > 1 && strcmp(argv[1], "-j") == 0) j = 1;
  if (argc > 1 && strcmp(argv[1], "-p") == 0) p = 1;
  if (argc > 1 && strcmp(argv[1], "-x") == 0) x = 1;
  if (argc < 1 || argc > 4) {
    fprintf(stderr, "USAGE: %s -f|-j|-p|-x|resource [infile] [outfile]\n", argv[0]);
    return 1;
  }

  if ((f+j+p+x) == 0 && (res = resources(argv[1], &rescnt)) == 0) return 2;

  if (argc > 2 && strcmp(argv[2], "-") != 0) {
    extern FILE *yyin;
    if ((yyin = fopen(infile = argv[2], "r")) == NULL) {
      perror(argv[2]);
      return 1;
    }
  }

  outfp = stdout;
  if (argc > 3 && strcmp(argv[3], "-") != 0)
    if ((outfp = fopen(argv[3], "w")) == NULL) {
      perror(argv[3]);
      return 1;
    }

  if (yyparse() != 0 || errors > 0) {
    fprintf(stderr, "%d errors in %s\n", errors, infile);
    return 1;
  }
  if (f) flat(tree, infile);
  else if (j) json(tree, 0);
  else if (x) xml(tree);
  else if (p) printNode(tree, outfp, yynames);
  else print(tree, res, rescnt);
  if (outfp) fclose(outfp);
  return 0;
}
