/*
:*:	language environment
:de:	Sprachumgebung

$Header	<EFEU/$1>

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

#ifndef	EFEU_LangDef_h
#define	EFEU_LangDef_h	1

#include <EFEU/config.h>

/*
:de:
Der Datentype |$1| erlaubt die Abfrage der Komponenten
Sprache und Ländercode einer Sprachdefinitionen der Form
<"<language>[|_|<territory>][<otherparts>]">.
Zusätzliche Komponenten in <otherparts> werden ignoriert. 
Die Originaldefinition <langdef> wird ebenfalls in der Struktur gespeichert.
*/

typedef struct {
	char *langdef;	/* Sprachdefinition */
	char *language;	/* ISO 639 Sprachcode */
	char *territory;/* ISO 3166 Ländercode */
} LangDefType;

void SetLangDef (const char *str);
extern LangDefType LangDef;

/*
$SeeAlso
\mref{LangDef(3)}.\br
\mref{setlocale(3)} @PRM.
*/

#endif	/* EFEU/LangDef.h */
