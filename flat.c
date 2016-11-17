#include <string.h>
#include <stdio.h>
#include <alloca.h>
#include "node.h"
#include "y.tab.h"

FILE *outfp;
static void flatprint(char *tag, Node *n) {
  switch (n->attrib) {
    case STR: fprintf(outfp, "%s %s\n", tag, n->value.s); break;
    case INT: fprintf(outfp, "%s %d\n", tag, n->value.i); break;
    case REAL: fprintf(outfp, "%s %g\n", tag, n->value.r); break;
    case True: fprintf(outfp, "%s true\n", tag); break;
    case False: fprintf(outfp, "%s false\n", tag); break;
    case Null: fprintf(outfp, "%s null\n", tag); break;
  }
}

int flat(Node *n, char *tag) {
  int level = -1;
  if (n) {
    Node *l = LEFT_CHILD(n);
    Node *r = RIGHT_CHILD(n);
    switch (n->attrib) {
      case OBJS:
	flat(l, tag);
        if (r) flat(r, tag);
	break;
      case PAIR: {
        char *s = l->value.s;
	char *t = alloca(strlen(tag)+strlen(s)+2);
	sprintf(t, "%s.%s", tag, s);
	if (r->type == nodeOpr) flat(r, t);
	else flatprint(t, r);
	break; }
      case ARRAY: {
	char *t = alloca(strlen(tag)+10);
	if (r == 0) break; /* nilNode */
        if (l->attrib == ARRAY) {
	  level = flat(l, tag) + 1;
	} else {
	  sprintf(t, "%s.1", tag);
	  if (l->type == nodeOpr) flat(l, t);
	  else flatprint(t, l);
	  level = 2;
	}
	sprintf(t, "%s.%d", tag, level);
	if (r->type == nodeOpr) flat(r, t);
	else flatprint(t, r);
	break; }
      default: fprintf(stderr, "invalid node: %d\n", n->attrib); break;
    }
  }
  return level;
}
