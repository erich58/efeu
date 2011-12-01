/*
Suchen eines Bereichs in einem Vektor

$Copyright (C) 2011 Erich Frühstück
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

/*
Die Funktion |$1| dient zum suchen von Einträgen
in einem sortierten Vektor. Die Funktion liefert die
Startposition des Vektors und die Position des nächsten Elements.

Die Vergleichsfunktion <comp> wird mit zwei Pointern
auf Datenelemente aufgerufen und muß einen Wert
kleiner, gleich oder größer 0 liefern, je nachdem ob
der erste Datenwert kleiner, gleich oder größer als der zweite
Datenwert ist.
*/

struct {
	void *key;
	int (*comp) (const void *a, const void *b);
} MDATA;


static VecMatch mval (size_t start, size_t end)
{
	VecMatch match = { start, end };
	return match;
}

VecMatch vmatch (void *key, void *base, size_t dim, size_t size,
	int (*comp) (const void *a, const void *b))
{
	int i, i0, i1, i2, i3;
	int k;

	if	(!comp || !dim)
		return mval(0, dim);
	
	i0 = 0;
	i3 = dim - 1;

	k = comp(key, base);

	if	(k < 0)
		return mval(0, 0);

	i1 = (k > 0) ? i3 : i0;

	k = comp(key, (char *) base + i3 * size);

	if	(k > 0)
		return mval(dim, dim);

	i2 = (k < 0) ? i0 : i3;

	while (i1 - i0 > 1)
	{
		i = (i0 + i1) / 2;
		k = comp(key, (char *) base + i * size);

		if	(k > 0)	i0 = i;
		else		i1 = i;
	}

	while (i3 - i2 > 1)
	{
		i = (i2 + i3) / 2;
		k = comp(key, (char *) base + i * size);

		if	(k < 0)	i3 = i;
		else		i2 = i;
	}

	return mval(i1, i2 + 1);
}
