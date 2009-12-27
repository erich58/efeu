/*
Sprachumgebung

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/LangType.h>
#include <EFEU/mstring.h>
#include <EFEU/locale.h>
#include <ctype.h>

/*
Die Variable |$1| enthält Informationen über die aktuelle Sprachumgebung.
[|LangType.langdef|]
	enthält den Originalstring für die Sprachdefinition,
[|LangType.language|]
	enthält den ISO 639 Sprachcode,
[|LangType.territory|]
	enthält den ISO 3166 Ländercode.
*/

LangType_t LangType = { NULL, NULL, NULL };

static void lt_clear (LangType_t *lt)
{
	memfree(LangType.langdef);
	memfree(LangType.language);
	memfree(LangType.territory);
}

static char *lt_copy (const char *def, size_t n, char **ptr)
{
	if	(ptr)	*ptr = (char *) (def + n);

	return n ? mstrncpy(def, n) : NULL;
}

static char *lt_language (const char *def, char **ptr)
{
	size_t n;

	if	(ptr)	*ptr = (char *) def;

	if	(def == NULL)	return NULL;

	for (n = 0; def[n] != 0; n++)
		if (!isalpha(def[n])) break;

	return lt_copy(def, n, ptr);
}

static char *lt_territory (const char *def, char **ptr)
{
	size_t n;

	if	(ptr)	*ptr = (char *) def;

	if	(def == NULL || *def != '_')	return NULL;

	def++;

	for (n = 0; def[n] != 0; n++)
		if (!isalpha(def[n])) break;

	return lt_copy(def, n, ptr);
}

static void lt_set (LangType_t *lt, const char *name)
{
	char *p;

	lt->langdef = mstrcpy(name);
	lt->language = lt_language(name, &p);
	lt->territory = lt_territory(p, NULL);
}

/*
Die Funktion |$1| initialisiert die Komponenten der globalen Variablen
|LangType| entsprechend der Sprachdefinition <lang>.
Falls ein Nullpointer übergeben wurde und |LangType| noch nicht
initialisiert wurde, wird die Umgebungsvariable |LANG| zur
Sprachdefinition abgefragt.
Aus der Sprachdefinition werden nur die Komponenten Sprache
und Ländercode extrahiert.
*/

void SetLangType (const char *name)
{
	if	(name == NULL && LangType.langdef == NULL)
		name = getenv("LANG");

	if	(name == NULL)	return;

	lt_clear(&LangType);
	lt_set(&LangType, name);

	if	(LangType.langdef)
		SetLocale(LOC_DATE, LangType.langdef);
}

/*
$SeeAlso
\mref{LangType(7)}.\br
\mref{getenv(3)},
\mref{setlocale(3)} @PRM.
*/
