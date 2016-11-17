/* output in xml format */
#include <string.h>
#include <stdio.h>
#include "node.h"
#include "y.tab.h"

char *dupstr(const char *);
FILE *outfp;
void nl(int lev);

static void xmldata(Node *n, int lev, char *env);
static void xmlobj(Node *n, int lev, char *env) {
  if (n->attrib == OBJS) {
    Node *l = LEFT_CHILD(n);
    Node *r = RIGHT_CHILD(n);
    if (n->type == nodeNil) return; /* {} */
    xmlobj(l, lev, env);
    if (r) {
      nl(lev);
      xmlobj(r, lev, env);
    }
  } else xmldata(n, lev, env);
}

static void xmlarray(Node *n, int lev, char *env) {
  Node *l = LEFT_CHILD(n);
  Node *r = RIGHT_CHILD(n);
  if (n->type == nodeNil) return; /* {} */
  if (l->attrib == ARRAY) xmlarray(l, lev, env);
  else {
    fprintf(outfp, "<%s>", env);
    xmldata(l, lev, env);
    fprintf(outfp, "</%s>", env);
  }
  nl(lev);
  fprintf(outfp, "<%s>", env);
  xmldata(r, lev, env);
  fprintf(outfp, "</%s>", env);
}

static void xmldata(Node *n, int lev, char *env) {
  switch (n->attrib) {
    case STR: fprintf(outfp, "%s", n->value.s); break;
    case INT: fprintf(outfp, "%d", n->value.i); break;
    case REAL: fprintf(outfp, "%g", n->value.r); break;
    case True: fprintf(outfp, "true"); break;
    case False: fprintf(outfp, "false"); break;
    case Null: fprintf(outfp, "null"); break;
    case OBJS: nl(lev+1); xmlobj(n, lev+1, env); nl(lev); break;
    case ARRAY: nl(lev+1); xmlarray(n, lev+1, env); nl(lev); break;
    case PAIR: {
      char *tag = LEFT_CHILD(n)->value.s;
      char *env = dupstr(tag); /* turn into singular /by removing the last 's' */
      if (strlen(env)) env[strlen(env)-1] = 0;
      fprintf(outfp, "<%s>", tag);
      xmldata(RIGHT_CHILD(n), lev, env);
      fprintf(outfp, "</%s>", tag);
      break;
    }
  }
}

void xml(Node *n) {
  fprintf(outfp, "<?xml version=\"1.0\"?>\n<issue>");
  xmldata(n, 0, "issues");
  fprintf(outfp, "</issue>\n");
}
