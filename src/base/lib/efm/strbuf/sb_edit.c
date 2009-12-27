/*
Dynamisches Zeichenfeld editieren

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

#include <EFEU/strbuf.h>

/*
Die Funktion |$1| löscht das Zeichen an der aktuellen Position.
*/

int sb_delete (strbuf_t *sb)
{
	int i, c, n;

	if	(sb->nfree >= sb->size)
		return EOF;

	sb->nfree++;
	n = sb->size - sb->nfree;
	c = sb->data[sb->pos];

	for (i = sb->pos; i < n; i++)
		sb->data[i] = sb->data[i + 1];

	if	(sb->pos > n)	sb->pos = n;

	return c;
}

/*
Die Funktion |$1| fügt das Zeichen <c> an der aktuellen Position ein.
*/

void sb_insert (int c, strbuf_t *sb)
{
	int i;

	if	(sb->pos >= sb->size - sb->nfree)
	{
		sb_put(c, sb);
		return;
	}

	if	(--sb->nfree < 0)
		sb_expand(sb);

	for (i = sb->size - sb->nfree; i > sb->pos; i--)
		sb->data[i] = sb->data[i-1];

	sb->data[sb->pos++] = c;
}

/*
$SeeAlso
\mref{strbuf(3)}, \mref{sb_getc(3)}, \mref{sb_putc(3)}.\br
*/
