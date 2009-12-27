#	Makefile zum Kompilieren der Beispiele

LIST=	hello1 hello2

all::	$(LIST)

clean::
	rm -f $(LIST)

hello1: hello1.c; cc -o $@ hello1.c
hello2: hello2.esh; efeuscript hello2.esh $@
