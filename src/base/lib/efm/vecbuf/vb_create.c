/*
Vektorbuffer anfordern/freigeben

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
#include <EFEU/memalloc.h>
#include <EFEU/procenv.h>

#define	BLKSIZE	64

/*
Die Funktion |$1| liefert einen neuen Vektorbuffer
mit Buffergröße <blk> und Elementgröße <size>.
*/

VecBuf *vb_create (size_t blk, size_t size)
{
	VecBuf *vb = memalloc(sizeof(VecBuf));
	vb_init(vb, blk ? blk : BLKSIZE, size);
	return vb;
}

/*
Die Funktion |$1| gibt einen mit |vb_create| angeforderten Vektorbuffer
wieder frei.
*/

void vb_destroy (VecBuf *vb)
{
	if	(vb)
	{
		vb_free(vb);
		memfree(vb);
	}
}

/*
$SeeAlso
\mref{vecbuf(3)}.
*/
