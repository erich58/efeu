/*	Speicherplatzanforderung für TexMerge
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0

Fall0 die Speicherplatzanforderung fehlschlägt, kann nicht
auf die Standardfehlerroutinen zurückgegriffen werden, da
diese eventuell selbst ein Speichersegment anfordern müssen.
*/

#include "texmerge.h"


/*	Anfordern eines Speichersegmentes
*/

int alloc_debug = 0;
int alloc_count = 0;

static void alloc_msg(const char *name, int flag, size_t size, void *p)
{
	if	(!p || !alloc_debug)	return;

	alloc_count++;
	fprintf(stderr, "%p\t%05d", p, alloc_count);

	if	(name)	fprintf(stderr, "\t%s", name);

	if	(flag)	fprintf(stderr, "\t[%d]", (int) size);
	else		fprintf(stderr, "\tdel");

	putc('\n', stderr);
}

void *AllocData(const char *name, size_t size)
{
	void *p;

#if	UNIX_VERSION
	p = memalloc(size);
#else
	if	(size == 0)
	{
		p = NULL;
	}
	else if	((p = malloc(size)) == NULL)
	{
		Error(3, size);
		exit(EXIT_FAILURE);
	}
#endif
	alloc_msg(name, 1, size, p);
	return p;
}


/*	Freigabe eines Speichersegmentes
*/

void FreeData(const char *name, void *p)
{
	alloc_msg(name, 0, 0, p);

#if	UNIX_VERSION
	memfree(p);
#else
	if	(p != NULL)
	{
		*((char *) p) = 0;
		free(p);
	}
#endif
}
