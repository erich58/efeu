/*
Vektorbuffer vergrößern

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

#define	E_NEXP "[efm:1]$!: can not expand constant data buffer.\n"

/*
Die Funktion |$1| sorgt dafür, daß im Vektorbuffer <buf> mindestens
<dim> Elemente zur Verfügung stehen. Die Funktion liefert
den Pointer auf das Speicherfeld. Die Komponente <used> wird auf 0
gesetzt.
*/

void *vb_alloc (VecBuf *buf, size_t dim)
{
	lfree(_vb_alloc(buf, dim));
	buf->used = 0;
	return buf->data;
}


/*
Die Funktion |$1| sorgt dafür, daß im Vektorbuffer <buf> mindestens
<dim> Elemente zur Verfügung stehen, wobei die verwendeten
Datenelemente unverändert bleiben.
*/

void *vb_realloc (VecBuf *buf, size_t dim)
{
	void *save;

	if	((save = _vb_alloc(buf, dim)) != NULL)
	{
		size_t n = buf->used * buf->elsize;

		memcpy(buf->data, save, n);
		memset((char *) buf->data + n, 0, buf->size * buf->elsize - n);
		lfree(save);
	}

	return buf->data;
}

/*
Die Funktion |$1| wird intern zur Buffererweiterung
verwendet. Falls der Buffer vergrößert wurde, liefert sie
die Adresse des alten Datenbuffers, ansonsten einen Nullpointer.
Der alte Datenbuffer kann mit |lfree| freigegeben werden.
Der neue Datenbuffer wird nicht initialisiert!
*/

void *_vb_alloc (VecBuf *buf, size_t dim)
{
	if	(dim > buf->size)
	{
		if	(buf->blksize)
		{
			register void *save;

			save = buf->data;
			buf->size = buf->blksize * ((dim + buf->blksize - 1) / buf->blksize);
			buf->data = lmalloc(buf->size * buf->elsize);
			return save;
		}

		dbg_error(NULL, E_NEXP, NULL);
	}

	return NULL;
}
