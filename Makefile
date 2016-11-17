LANG=json
EXT=json
.SUFFIXES: .$(EXT) .asm .obj .exe
LEX=flex -l
YACC=byacc -dv
CC=gcc -w -g
OBJS=find.c flat.c json.c xml.c main.c

$(LANG): $(LANG).y $(LANG).l $(OBJS) node.o
	$(YACC) -dv $(LANG).y
	$(LEX) -l $(LANG).l
	$(CC) -o $(LANG) -Wall -DYYDEBUG lex.yy.c y.tab.c $(OBJS) node.o

clean::
	rm -f *.o $(LANG) lex.yy.c y.tab.c y.tab.h y.output
