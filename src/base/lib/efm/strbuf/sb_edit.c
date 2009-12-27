/*
:*:editing dynamic string buffer
:de:Dynamisches Zeichenfeld editieren

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
:*:
The function |$1| deletes <n> character at the actual position.
:de:
Die Funktion |$1| löscht <n> Zeichen an der aktuellen Position.
Der Rückgabewert ist ein Pointer auf die ans Ende des Buffers verschobenen
Zeichen.
*/

void *sb_delete (StrBuf *sb, size_t n)
{
	if	(sb && sb->pos + sb->nfree + n <= sb->size)
	{
		sb_move(sb, sb->pos, sb->pos + n,
			sb->size - sb->nfree - sb->pos - n);
		sb->nfree += n;
		return sb->data + sb->size - sb->nfree;
	}
	else	return NULL;
}

/*
:*:
The function |$1| inserts the character <c> <n> times at the actual position
and returns the address of the insertion point.
:de:
Die Funktion |$1| fügt das Zeichen <c> <n>-mal an der aktuellen Position ein
und liefert die Adresse der ersten Einfügeposition
*/

void *sb_insert (StrBuf *sb, int c, size_t n)
{
	unsigned char *p;

	if	(!sb)	return NULL;

	if	(sb->pos + sb->nfree == sb->size)
	{
		p = sb_expand(sb, n);
	}
	else
	{
		int pos = sb->pos;
		sb->pos = sb->size - sb->nfree;
		sb_expand(sb, n);
		p = sb->data + pos;
		memmove(p + n, p, sb->pos - pos);
		sb->pos = pos;
	}

	memset(p, c, n);
	sb->nfree -= n;
	sb->pos += n;
	return p;
}

/*
$SeeAlso
\mref{strbuf(3)},
\mref{strbuf(7)}.
*/
