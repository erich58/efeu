/*	Fehlerhändling
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#ifndef	EFEU_MSGTAB_H
#define	EFEU_MSGTAB_H	1

#include <EFEU/mstring.h>

#define	MSG_EFEU	"efeu"

typedef struct {
	int num;	/* Formatnummer */
	char *fmt;	/* Formatstring */
} msgdef_t;

char *getmsg (const char *name, int num, const char *def);

void errmsg (const char *name, int num);
void liberror (const char *name, int num);

#endif	/* EFEU_MSGTAB_H */
