/*
Verwalten, Suchen und Sortieren von Einträgen in einem Vektorbuffer

$Copyright (C) 1996 Erich Frühstück
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

#include <EFEU/vecbuf.h>

static void move (char *a, char *b, size_t n)
{
	while (n-- > 0)
	{
		*(a++) = *b;
		*(b++) = 0;
	}
}

static void swap (char *a, char *b, size_t n)
{
	while (n-- > 0)
	{
		char c = *a;
		*(a++) = *b;
		*(b++) = c;
	}
}

/*
Die Funktion |$1| dient zum Verwalten von Einträgen
in einem sortierten Vektorbuffer. Der Suchschlüssel <data>
dient zur Suche von Einträgen und als Zwischenbuffer für
Daten. Bei neuen Einträgen werden die Daten vom Suchschlüssel
in den Vektorbuffer übertragen, bei gelöschten Einträgen
werden die Daten in den Suchschlüssel geschrieben.

Die Vergleichsfunktion <comp> wird mit zwei Pointern
auf Datenelemente aufgerufen und muß einen Wert
kleiner, gleich oder größer 0 liefern, je nachdem ob
der erste Datenwert kleiner, gleich oder größer als der zweite
Datenwert ist.

Als Modus <mode> ist eines der folgenden Werte zulässig:

[|VB_SEARCH|]
	Falls der Eintrag gefunden wurde, liefert die Funktion
	den Pointer auf den Eintrag, ansonsten einen Nullpointer.
	Der Suchschlüssel wird nicht verändert.
[|VB_ENTER|]
	Falls der Eintrag gefunden wurde, liefert die Funktion den
	Pointer auf den Eintrag. Der Suchschlüssel wird nicht
	verändert. Falls der Eintrag noch nicht vorhanden ist, wird
	ein neuer Eintrag generiert und die Daten vom Suchschlüssel
	übernommen. Der Suchschlüssel wird anschließend gelöscht.
	Die Funktion liefert den Pointer auf den neuen Eintrag.
[|VB_REPLACE|]
	Falls der Eintrag bereits vorhanden ist,
	werden die Daten mit dem Suchschlüssel ausgetauscht,
	und die Funktion liefert den Pointer auf den Suchschlüssel.
	Ansonsten wird ein neuer Eintrag generiert
	und die Daten vom Suchschlüssel übernommen.
	Der Suchschlüssel wird anschließend gelöscht
	und die Funktion liefert einen Nullpointer.
[|VB_DELETE|]
	Falls der Eintrag in der Tabelle gefunden wird,
	wird er gelöscht. Die Datenwerte werden in den Suchschlüssel
	übertragen und die Funktion liefert
	den Pointer auf den Suchschlüssel.
	Ansonsten wird ein Nullpointer zurückgegeben.
*/

void *vb_search (VecBuf *buf, void *data,
	int (*comp) (const void *a, const void *b), int mode)
{
	char *p1;
	int i, i1, i2, v;

	if	(!buf)	return NULL;

/*	Suche nach Eintrag
*/
	i2 = buf->used;
	i1 = comp ? 0 : i2;

	while (i1 != i2)
	{
		i = (i1 + i2) / 2;
		p1 = (char *) buf->data + i * buf->elsize;
		v = comp(data, p1);

		if	(v > 0)	i1 = i + 1;
		else if	(v < 0)	i2 = i;
		else	
		{
			switch (mode)
			{
			case VB_ENTER:
			case VB_SEARCH:

				return p1;

			case VB_DELETE:

				memcpy(data, p1, buf->elsize);
				vb_delete(buf, i, 1);
				return data;


			case VB_REPLACE:

				swap(p1, data, buf->elsize);
				return data;

			default:

				break;
			}

			i2 = i + 1;
			break;
		}
	}

/*	Datenwert übernehmen
*/
	if	(mode == VB_ENTER || mode == VB_REPLACE)
	{
		p1 = vb_insert(buf, i2, 1);
		move(p1, data, buf->elsize);

		if	(mode == VB_ENTER)	return p1;
	}

	return NULL;
}

/*
Die Funktion |$1| sortiert die Daten eines Vektorbuffers
mit der Bibliotheksfunktion |qsort|.
*/

void vb_qsort (VecBuf *buf, int (*comp) (const void *a, const void *b))
{
	if	(buf && buf->used > 1)
		qsort(buf->data, buf->used, buf->elsize, comp);
}

/*
Die Funktion |$1| reduziert die Daten bezüglich gleicher Einträge.
*/

void vb_uniq (VecBuf *buf, int (*comp) (const void *a, const void *b))
{
	buf->used = vuniq(buf->data, buf->used, buf->elsize, comp);
}

/*
Die Funktion |$1| verwendet die Bibliotheksfunktion
|bsearch| zur Suche eines Eintrages und ist äquivalent zu
|vb_search| mit Flag |VB_SEARCH|.
*/

void *vb_bsearch (VecBuf *buf, const void *key,
	int (*comp) (const void *a, const void *b))
{
	if	(buf && buf->used > 0)
		return bsearch(key, buf->data, buf->used, buf->elsize, comp);
	else	return NULL;
}

/*
$SeeAlso
\mref{vecbuf(7)}, \mref{types(7)}.\br
\mref{bsearch(3C)}, \mref{qsort(3C)} @PRM
*/
