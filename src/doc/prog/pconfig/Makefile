#	Makefile zum Kompilieren der Beispiele

LIST=	vhello1 vhello2 vhello3

all::	$(LIST)

clean::
	rm -f $(LIST)

vhello1: vhello1.c; cc -o $@ vhello1.c
vhello2: vhello2.c; efeucc -g -o $@ vhello2.c -lefm
vhello3: vhello3.esh; efeuscript vhello3.esh $@
