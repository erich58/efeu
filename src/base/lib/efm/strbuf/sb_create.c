/*
:*:	Dynamic string buffer
:de:	Arbeiten mit dynamischen Zeichenfeldern

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

#define	SB_SIZE	1000

static ALLOCTAB(sb_tab, "StrBuf", 32, sizeof(StrBuf));


/*
:*:
The function |$1| creates a new string buffer with blocksize <bsize>.
:de:
Die Funktion |$1| liefert ein neues Zeichenfeld mit Buffergröße <blksize>.
*/

StrBuf *sb_create (size_t blksize)
{
	StrBuf *buf = new_data(&sb_tab);
	sb_init(buf, blksize);
	return buf;
}

void sb_destroy (StrBuf *sb)
{
	if	(sb)
	{
		sb_free(sb);
		del_data(&sb_tab, sb);
	}
}

/*
$SeeAlso
\mref{strbuf(7)},
\mref{memalloc(7)}.
*/
