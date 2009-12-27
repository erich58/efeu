/*
Speicherinhalt austauschen

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

#include <EFEU/memalloc.h>

/*
Die Funktion |memswap| vertauscht den Inhalt der Speicherfelder
beginnend bei <ap> und <bp>. Die Variable <n> gibt die Zahl der 
Bytes an, die vertauscht werden sollen.

$Warning
Falls sich die beiden Speicherfelder überlappen, ist das Resultat
unbestimmt.
*/

void memswap(void *ap, void *bp, size_t n)
{
	register uchar_t *a, *b;
	register uchar_t c;

	a = ap;
	b = bp;

	while (n--)
	{
		c = *a;
		*a++ = *b;
		*b++ = c;
	}
}

/*
$SeeAlso
\mref{alloctab(3)}, \mref{memalloc(3)}.\br
\mref{malloc(3S)} @PRM.
*/
