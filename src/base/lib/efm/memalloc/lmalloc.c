/*	Speicherplatzanforderung
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6

Bei Problemen mit dynamisch angeforderten Speichersegmenten
kann der Makro MEMCHECK auf 1 gesetzt werden. Jedes Segment
wird zus�tzlich mit einer Anfangs- und Endekennung versehen.
Bei der Freigabe werden diese Kennungen �berpr�ft.

Falls die Speicherplatzanforderung fehlschl�gt, kann nicht
auf die Standardfehlerroutinen zur�ckgegriffen werden, da
diese eventuell selbst ein Speichersegment anfordern m�ssen.
*/

#include <EFEU/memalloc.h>

#define	MEMCHECK	0

#define	ERR	"sorry: malloc(%lu) failed\n"


#if	MEMCHECK

#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"

#define	MSG1	"lcheck(%p): Anfangsmarkierung wurde �berschrieben.\n"
#define	MSG2	"lcheck(%p): Endemarkierung wurde �berschrieben.\n"

typedef struct {
#if	REFBYTEORDER
	size_t size;
	char check[CHECK_SIZE];
#else
	char check[CHECK_SIZE];
	size_t size;
#endif
} MEMHEAD;

static void *checkmalloc(size_t size)
{
	MEMHEAD *head;
	char *p;

	head = malloc(size + sizeof(MEMHEAD) + CHECK_SIZE);

	if	(head)
	{
		p = (char *) (head + 1);
		head->size = size;
		memcpy(head->check, CHECK_MASK, CHECK_SIZE);
		memcpy(p + size, CHECK_MASK, CHECK_SIZE);
		return p;
	}
	else	return NULL;
}

#endif

/*
Die Funktion |$1| reserviert einen Speicherplatz der
Gr��e <size>. Durch |lfree| kann der Speicherplatz
wieder freigegeben werden.

$Warnings
Falls �ber das Ende eines reservierten Speicherplatzes
hinausgeschrieben wird, kommt
es zu einem unkontrollierten Fehlverhalten.
Meistens f�hrt ein sp�terer Aufruf von |$1| zu einem
Absturz des Programms.

$Diagnostics
Wird ein Speicherfeld der Gr��e 0 angefordert, liefert
die Funktion |$1| einen Nullpointer.
Kann der gew�nschte Speicherplatz nicht reserviert werden,
wird die Verarbeitung des rufenden Kommandos mit einer Fehlermeldung
abgebrochen. Der R�ckgabewert von |$1| mu� daher nicht �berpr�ft
werden.
*/

void *lmalloc(size_t size)
{
	void *p;

	if	(size == 0)	return NULL;

#if	MEMCHECK
	if	((p = checkmalloc(size)) == NULL)
#else
	if	((p = malloc(size)) == NULL)
#endif
	{
		fprintf(stderr, ERR, (ulong_t) size);
		exit(EXIT_FAILURE);
	}

	return p;
}

/*
Die Funktion |$1| gibt einen von |lmalloc| angeforderten
Speicherbereich wieder frei. Ein Nullpointer ist als Argument zul�ssig.
*/

void lfree(void *p)
{
#if	MEMCHECK
	lcheck(p);
#else
	if	(p != NULL)
		free(p);
#endif
}

/*
Die Funktion |$1| testet ein Speichersegment, ob sein Anfang oder sein
Ende �berschrieben wurde. Vorraussetzung ist dabei, da� vorm Kompilieren
der Makro |MEMCHECK| auf 1 gesetzt wurde.
*/

void lcheck(void *p)
{
#if	MEMCHECK
	MEMHEAD *head;

	if	(p == NULL)	return;

	head = ((MEMHEAD *) p) - 1;

	if	(memcmp(head->check, CHECK_MASK, CHECK_SIZE) != 0)
		fprintf(stderr, MSG1, p);

	if	(memcmp(((char *) p) + head->size, CHECK_MASK, CHECK_SIZE) != 0)
		fprintf(stderr, MSG2, p);
#else
	;
#endif
}

/*
$Notes
F�r kleine Speichersegmente sollten
die Funktionen |memalloc| und |memfree| eingesetzt
werden.

$Warnings
Bei �bergabe eines Pointers an die Funktion
|lfree|, der nicht durch einen vorangegangenen Aufruf von
|lmalloc| stammt, ist das Verhalten unbestimmt und kann
ebenfalls zu einem Programmabsturz f�hren.
Die Funktionen |memalloc| und |memfree| sind diesbez�glich robust.
$SeeAlso
\mref{alloctab(3)}, \mref{memalloc(3)}.\br
\mref{malloc(3S)} im @PRM.
*/
