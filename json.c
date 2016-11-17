/* output in json format */
#include <string.h>
#include <stdio.h>
#include "node.h"
#include "y.tab.h"

FILE *outfp;
void nl(int lev);

void json(Node *n, int lev);
static void object(Node *n, int lev) {
  Node *l = LEFT_CHILD(n);
  Node *r = RIGHT_CHILD(n);
  if (n->attrib == OBJS) {
    if (n->type == nodeNil) return; /* {} */
    object(l, lev);
    if (r) {
      fprintf(outfp, ","); nl(lev);
      object(r, lev);
    }
  } else { /* PAIR */
    fprintf(outfp, "\"%s\": ", l->value.s);
    json(r, lev);
  }
}

static void vector(Node *n, int lev) {
  Node *l = LEFT_CHILD(n);
  Node *r = RIGHT_CHILD(n);
  if (n->type == nodeNil) return; /* {} */
  if (l->attrib == ARRAY) vector(l, lev);
  else json(l, lev);
  fprintf(outfp, ","); nl(lev);
  json(r, lev);
}

void json(Node *n, int lev) {
  switch (n->attrib) {
    case STR: fprintf(outfp, "\"%s\"", n->value.s); break;
    case INT: fprintf(outfp, "%d", n->value.i); break;
    case REAL: fprintf(outfp, "%g", n->value.r); break;
    case True: fprintf(outfp, "true"); break;
    case False: fprintf(outfp, "false"); break;
    case Null: fprintf(outfp, "null"); break;
    case OBJS: fprintf(outfp, "{"); nl(lev+1); object(n, lev+1); fprintf(outfp, "}"); nl(lev); break;
    case PAIR: object(n, lev); break;
    case ARRAY: fprintf(outfp, "["); nl(lev+1); vector(n, lev+1); fprintf(outfp, " ]"); nl(lev); break;
  }
}
