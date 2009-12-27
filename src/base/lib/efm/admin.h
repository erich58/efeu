/*	Makro zur Einrichtung einer Administrationsfunktion
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

$Header	<EFEU/$1>

$SeeAlso
\mref{types(5)}.\br

$Description
Eine Administrationsfunktion erlaubt das Erzeugen, Freigeben, Löschen und
Kopieren eines Datentypes.

---- verbatim
create	x = admin(NULL, NULL)	ALLOC
copy	x = admin(NULL, y)	ALLOC + COPY

delete	NULL = admin(x, NULL)	CLEAN + FREE
clean	x = admin(x, x)		CLEAN
assign	x = admin(x, y)		CLEAN + COPY
----
*/

#ifndef	EFEU_admin_h
#define	EFEU_admin_h	1

#include <EFEU/types.h>

#define	ADMIN(type, name, alloc, copy, clean, free)	\
type *name (type *tg, const type *src)	\
{					\
	if	(tg)			\
	{				\
		clean			\
					\
		if	(src == NULL)	\
		{			\
			free		\
			return NULL;	\
		}			\
					\
		if	(src == tg)	\
			return tg;	\
	}				\
	else				\
	{				\
		alloc			\
					\
		if	(src == NULL)	\
			return tg;	\
	}				\
					\
	copy				\
	return tg;			\
}

#if	0	/* Beispiele */
ADMIN(TYPE, NAME,
	/* ALLOC */
	tg = memalloc(sizeof(int));
,
	/* COPY */
	memcpy(tg, src, sizeof(TYPE)),
,
	/* CLEAN */
	memset(tg, 0, sizeof(TYPE)),
,
	/* FREE */
	memfree(tg);
)

ADMIN(char, str_admin,
	/* ALLOC */
,
	/* COPY */
	tg = mstrcpy(src);
,
	/* CLEAN */
	memfree(tg);
	tg = NULL;
,
	/* FREE */
)

ADMIN(int, int_admin,
	/* ALLOC */
	tg = memalloc(sizeof(int));
,
	/* COPY */
	*tg = *src;
,
	/* CLEAN */
	*tg = 0;
,
	/* FREE */
	memfree(tg);
)

#endif

#endif /* EFEU/admin.h	*/
