/*
Daten in Vektorbuffer kopieren

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

#include <EFEU/vecbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

/*
Die Funktion |$1| kopiert <dim> Datenelemente von der Speicheradresse
<data> in den Vektorbuffer <buf>. Der Datentype von <data> muß zum
Datenbuffer kompatibel sein.
*/

void *vb_copy (vecbuf_t *buf, void *data, size_t dim)
{
	if	(buf)
	{
		vb_alloc(buf, dim);
		buf->used = dim;
		memcpy(buf->data, data, dim * buf->elsize);
		return buf->data;
	}
	else	return NULL;
}

/*
Die Funktion |$1| kopiert <dim> Datenelemente von der Speicheradresse
<data> ans Ende des Vektorbuffers <buf>. Der Datentype von <data> muß zum
Datenbuffer kompatibel sein.
*/

void *vb_append (vecbuf_t *buf, void *data, size_t dim)
{
	if	(buf)
	{
		vb_realloc(buf, buf->used + dim);
		memcpy((char *) buf->data + buf->elsize * buf->used,
			data, dim * buf->elsize);
		buf->used += dim;
		return buf->data;
	}
	else	return NULL;
}

/*
$SeeAlso
\mref{vecbuf(3)},
\mref{vb_alloc(3)},
\mref{vecbuf(7)}.
*/
