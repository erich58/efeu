/*
:*:	data typees with reference counter
:de:	Datenstrukturen mit Referenzzähler

$Header	<EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	EFEU_refdata_h
#define	EFEU_refdata_h	1

#include <EFEU/types.h>

typedef struct {
	char *label;		/* Bezeichnung */
	ident_t ident;		/* Identifikation */
	clean_t clean;		/* Löschfunktion */
	admin_t admin;		/* Speicheradministration */
	int sync;		/* Debug - Synchronisation */
	void *log;		/* Log - File */
} reftype_t;

/*
:de:
Der Makro |$1| initialisiert einen neuen Referenztype mit
Identifikationsfunktion <ident> und Speicheradministrationsfunktion
<admin>
*/

#define	ADMINREFTYPE(name, label, ident, admin)	\
	reftype_t name = { label, (ident_t) ident, \
		NULL, (admin_t) admin, 0, NULL }

/*
:de:
Der Makro |$1| initialisiert einen neuen Referenztype mit
Identifikationsfunktion <ident> und Aufräumfunktion <clean>.
*/

#define	REFTYPE(name, label, ident, clean)	\
	reftype_t name = { label, (ident_t) ident, \
		(clean_t) clean, NULL, 0, NULL }

/*
:de:
Der Makro |$1| liefert die Referenzvariablen für eine Referenzstruktur.
Er muß zu Beginn der Datentypedefinition stehen.
*/

#define	REFVAR		const reftype_t *reftype; size_t refcount

/*
:de:
Der Makro |$1| liefert die Initialisierungswerte für eine
Referenzstruktur.
*/

#define	REFDATA(type)	type, 1

typedef struct {
	REFVAR;
} refdata_t;

extern void *rd_init (const reftype_t *type, void *data);
extern void *rd_refer (const void *data);
extern void rd_deref (void *data);

extern char *rd_ident (const void *data);
extern void rd_debug (const void *data, const char *fmt, ...);

extern void *rd_create (const reftype_t *type);

#endif	/* EFEU/refdata.h */
