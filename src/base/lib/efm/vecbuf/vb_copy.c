/*
Daten in Vektorbuffer kopieren

$Copyright (C) 2001 Erich Fr�hst�ck
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
<data> in den Vektorbuffer <buf>. Der Datentype von <data> mu� zum
Datenbuffer kompatibel sein.
*/

void *vb_copy (VecBuf *buf, void *data, size_t dim)
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
<data> ans Ende des Vektorbuffers <buf>. Der Datentype von <data> mu� zum
Datenbuffer kompatibel sein.
*/

void *vb_append (VecBuf *buf, void *data, size_t dim)
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
Die Funktion |$1| kopiert die in <buf> vorhandenen Datenelemente
auf die Speicheradresse <data>. Falls anstelle von <data> ein
Nullpointer �bergeben wurde, wird der ben�tigte Speicherbereich
mit |memalloc| generiert. Ansonsten mu� <data> auf einen 
Speicherbereich zeigen, der gen�gend gro� ist um alle in <buf>
gespeicherten Werte zu �bernehmen.

Falls anstelle von <buf> ein Nullpointer �bergeben wurde, liefert
liefert die Funktion einen Nullpointer.
Falls keine Daten zu kopieren sind, liefert die Funktion <data> ohne
darauf zuzugreifen.
Sind Daten zu kopieren, liefert die Funktion <data> oder den bei
bedarf eingericheten Speicherbereich.
*/

void *vb_fetch (VecBuf *buf, void *data)
{
	if	(!buf)		return NULL;
	if	(!buf->used)	return data;

	if	(!data)	data = memalloc(buf->used * buf->elsize);

	memcpy(data, buf->data, buf->used * buf->elsize);
	return data;
}

/*
$SeeAlso
\mref{vecbuf(3)},
\mref{vb_alloc(3)},
\mref{vecbuf(7)}.
*/
