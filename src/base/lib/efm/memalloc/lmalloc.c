/*	Speicherplatzanforderung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

Bei Problemen mit dynamisch angeforderten Speichersegmenten
kann der Makro MEMCHECK auf 1 gesetzt werden. Jedes Segment
wird zusätzlich mit einer Anfangs- und Endekennung versehen.
Bei der Freigabe werden diese Kennungen überprüft.

Falls die Speicherplatzanforderung fehlschlägt, kann nicht
auf die Standardfehlerroutinen zurückgegriffen werden, da
diese eventuell selbst ein Speichersegment anfordern müssen.
*/

#include <EFEU/memalloc.h>

#define	MEMCHECK	0

#define	ERR	"sorry: malloc(%lu) failed\n"


#if	MEMCHECK

#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"

#define	MSG1	"lcheck(%p): Anfangsmarkierung wurde überschrieben.\n"
#define	MSG2	"lcheck(%p): Endemarkierung wurde überschrieben.\n"

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
Größe <size>. Durch |lfree| kann der Speicherplatz
wieder freigegeben werden.

$Warnings
Falls über das Ende eines reservierten Speicherplatzes
hinausgeschrieben wird, kommt
es zu einem unkontrollierten Fehlverhalten.
Meistens führt ein späterer Aufruf von |$1| zu einem
Absturz des Programms.

$Diagnostics
Wird ein Speicherfeld der Größe 0 angefordert, liefert
die Funktion |$1| einen Nullpointer.
Kann der gewünschte Speicherplatz nicht reserviert werden,
wird die Verarbeitung des rufenden Kommandos mit einer Fehlermeldung
abgebrochen. Der Rückgabewert von |$1| muß daher nicht überprüft
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
Speicherbereich wieder frei. Ein Nullpointer ist als Argument zulässig.
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
Ende überschrieben wurde. Vorraussetzung ist dabei, daß vorm Kompilieren
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
Für kleine Speichersegmente sollten
die Funktionen |memalloc| und |memfree| eingesetzt
werden.

$Warnings
Bei Übergabe eines Pointers an die Funktion
|lfree|, der nicht durch einen vorangegangenen Aufruf von
|lmalloc| stammt, ist das Verhalten unbestimmt und kann
ebenfalls zu einem Programmabsturz führen.
Die Funktionen |memalloc| und |memfree| sind diesbezüglich robust.
$SeeAlso
\mref{alloctab(3)}, \mref{memalloc(3)}.\br
\mref{malloc(3S)} im @PRM.
*/
