/*
:*:	data types with reference counter
:de:	Datenstrukturen mit Referenzzähler

$Header	<EFEU/$1>

$Copyright (C) 1994, 2002 Erich Frühstück
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

#include <EFEU/config.h>

/*
:*:
The data type |$1| defines the reference type.
:de:
Die Datentruktur |$1| definiert den Referenzdatentyps.
*/

typedef struct {
	char *label;		/* Bezeichnung */
	char *(*ident) (const void *data); /* Identifikationsfunktion */
	void (*clean) (void *data); /* Löschfunktion */
	int sync;		/* Debug - Synchronisation */
	FILE *log;		/* Log - File */
} RefType;

/*
:*:
The macro |$1| expands to the initialization values for a
reference type.
:de:
Der Makro |$1| expandiert zu den Initialisierungswerten eines Referenztyps.
*/

#define	REFTYPE_INIT(label, ident, clean)	\
{ label, ident, clean, 0, NULL }

/*
:*:
The Macro allowes to declare a RefData compatible structure.
He must be placed at the beginning of the structure.
:de:
Der Makro |$1| liefert die Referenzvariablen für eine Referenzstruktur.
Er muß zu Beginn der Datentypedefinition stehen.
*/

#define	REFVAR		const RefType *reftype; size_t refcount

/*
:*:
The macro |$1| generates the initial values for the reference values
of a reference type. He is used by the initialization of an object
with reference type.
:de:
Der Makro |$1| liefert die Initialisierungswerte für die
Referenzvariablen. Er wird bei der Initialisierung eines
Referenzobjektes verwendet.
*/

#define	REFDATA(type)	type, 1

/*
:*:
The datatype |$1| is the basic type for all reference types.
He includes only reference values.
:de:
Der Datentype |$1| ist eine Oberklasse für alle Referenzdatentypen.
Er enthält nur die Referenzvariablen.
*/

typedef struct {
	REFVAR;
} RefData;

extern void *rd_init (const RefType *type, void *data);
extern void *rd_refer (const void *data);
extern void rd_deref (void *data);

extern char *rd_ident (const void *data);
extern void rd_debug (const void *data, const char *fmt, ...);

/*
$Example
:*:
The following example shows the declaration of the reference type |FOO|.
The function |foo_create| returns a new pointer of type |FOO|.
The extern variabale |foo_default| provides a default value.
:de:
Das folgende Beispiel zeigt die Deklaration des Referenztyps |FOO|.
Ein neuer Pointer kann mit |foo_create| eingerichtet werden. Weiters
wird ein vorbelegter Defaultwert bereitgestellt.

foo.h

---- verbatim
typedef struct {
	REFVAR;
	int foo_value;
} FOO;

extern FOO *foo_create (int value);
extern FOO *foo_default;
----

foo.c

---- verbatim
#include "foo.h"

static char *foo_ident (const void *data)
{
	const FOO *foo = data;
	return msprintf("FOO: %d", foo->foo_value);
}

static void foo_clean (void *data)
{
	memfree(data);
}

static RefType foo_reftype = REFTYPE_INIT("foo", foo_ident, foo_clean);

FOO *foo_create (int value)
{
	FOO *foo = memalloc(sizeof(FOO));
	foo->foo_value = value;
	return rd_init(&foo_reftype, foo);
}

static FOO foo_buf = { REFDATA(&foo_reftype), 0 };
FOO *foo_default = &foo_buf;
----

$Note
:*:
You should always use |REFTYPE_INIT| to initialize the reference type,
because the internal structure of RefType may change. The 
macro provides a constant interface.
:de:
Ein Refernztyp sollte immer mit |REFTYPE_INIT| initialisiert
werden, da sich die Zusammensetzung der Struktur ändern kann. Der
Makro liefert eine konstante Schnittstelle.
*/

#endif	/* EFEU/refdata.h */
