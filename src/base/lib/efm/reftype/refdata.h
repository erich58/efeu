/*	Datenstrukturen mit Referenzzähler
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_REFDATA_H
#define	EFEU_REFDATA_H	1

#include <EFEU/types.h>

typedef struct {
	char *label;		/* Bezeichnung */
	ident_t ident;		/* Identifikation */
	admin_t admin;		/* Speicheradministration */
	int debug;		/* Debugflag */
} reftype_t;

#define	REFTYPE(name, label, ident, admin)	\
	reftype_t name = { label, (ident_t) ident, (admin_t) admin, 0 }

#define	REFVAR		const reftype_t *reftype; size_t refcount
#define	REFDATA(type)	type, 1

typedef struct {
	REFVAR;
} refdata_t;

void *rd_create (const reftype_t *type);
void *rd_refer (const void *data);
void rd_deref (void *data);
void *rd_admin (void *tg, const void *src);

char *rd_ident (const void *data);
void rd_debug (const void *data, const char *fmt, ...);

#endif	/* EFEU/refdata.h */
