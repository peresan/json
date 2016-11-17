#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "y.tab.h"
#ifdef YYDEBUG
  extern int yydebug;
#endif

char *dupstr(const char *);
FILE *outfp;

static Node *find(Node *n, char **res, int rescnt, char *str);

static Node *pair(Node *n, char **res, int rescnt, char *str, int len) {
  Node *p = RIGHT_CHILD(n);
  char *s;

  if (p == 0) return 0;
  if (n->attrib == OBJS)
    return find(n, res, rescnt, str);

  s = LEFT_CHILD(n)->value.s;
  if (strncmp(s, str, len) == 0) {
    if (str[len] == 0) return p;
    return find(p, res, rescnt, str+len+1);
  }
  return 0;
}

static Node *array(Node *n, char **res, int rescnt, char *str, int i) {
  if (n == 0 || i == 0) return 0;
  if (i == 1) {
    n = n->attrib == ARRAY ? LEFT_CHILD(n) : n;
    return *str ? find(n, res, rescnt, str) : n;
  }
  if (n->attrib == ARRAY)
    if (i == 2) {
      n = RIGHT_CHILD(n);
      return *str ? find(n, res, rescnt, str) : n;
    }
    else
      return array(LEFT_CHILD(n), res, rescnt, str, i-1);
  return 0;
}

static Node *find(Node *n, char **res, int rescnt, char *str) {
  char *next = strchr(str, '.');
  int len = next ? next-str : strlen(str);

#ifdef YYDEBUG
  if (yydebug) printf("%d#%s\n", len, str);
#endif
  if (str[0] == '*' && n->attrib == ARRAY)
    return n;
  if (isdigit(str[0]))
    return array(n, res, rescnt, str+len+1, atoi(str));
  else {
    while (n != 0 && n->attrib == OBJS) {
      Node *p = RIGHT_CHILD(n);
      if (p != 0 && (p = pair(p, res, rescnt, str, len)) != 0)
        return p;
      n = LEFT_CHILD(n);
    }
    if (n->attrib == PAIR)
      return pair(n, res, rescnt, str, len);
  }
  return 0;
}

void print(Node *tree, char **res, int rescnt) {
  extern char **yynames;
  int i;

  for (i = 0; i < rescnt; i++) {
    Node *n;
#ifdef YYDEBUG
    if (yydebug) printf("%d: %s\n", i, res[i]);
#endif
    if ((n = find(tree, res, rescnt, res[i])) != 0)
      switch (n->type) {
        case nodeStr: fprintf(outfp, "%s: %s\n", res[i], n->value.s); break;
        case nodeInt: fprintf(outfp, "%s: %d\n", res[i], n->value.i); break;
        case nodeReal: fprintf(outfp, "%s: %g\n", res[i], n->value.r); break;
	default: printNode(n, 0, yynames);
      }
    else
      fprintf(stderr, "%s: not found\n", res[i]);
  }
}

char **resources(char *file, int *rescnt) {
  FILE *fp = fopen(file, "r");
  int line = 0, len = 100;
  char buf[BUFSIZ], **res;

  if (fp == 0) { perror(file); return 0; }
  res = (char**)malloc(len*sizeof(char*));
  while (fgets(buf, BUFSIZ-1, fp)) {
    line++;
    if (buf[0] == '#') continue;
    if (*rescnt == len)
      res = (char**)realloc(res, (len += 100) * sizeof(char*));
    buf[strlen(buf)-1] = 0;
    res[(*rescnt)++] = dupstr(buf);
  }
  fclose(fp);
  return res;
}
