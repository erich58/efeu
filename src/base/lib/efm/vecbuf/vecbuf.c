/*
Vektorbuffer

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

/*
Die Funktion |vb_init| initialisiert den Vektorbuffer <buf>
mit Blockgröße <blk> und Datengröße <size>.
*/

void vb_init (VecBuf *buf, size_t blk, size_t size)
{
	if	(!buf)	return;

	buf->data = NULL;
	buf->blksize = blk;
	buf->elsize = size;
	buf->size = 0;
	buf->used = 0;
}

/*
Die Funktion |$1| wendet die Funktion <clean> auf alle
verwendeten Datenelemente im Vektorbuffer <buf> an und setzt
die Komponente |used| auf 0.
*/

void vb_clean (VecBuf *buf, void (*clean) (void *ptr))
{
	if	(!buf)	return;

	if	(clean)
	{
		while (buf->used-- > 0)
			clean((char *) buf->data + buf->elsize * buf->used);
	}

	buf->used = 0;
}

/*
Die Funktion |$1| gibt das im
Vektorbuffer verwendete Speicherfeld frei. Die Komponenten
|data|, |size| und |used| werden auf 0 gesetzt.
*/

void vb_free (VecBuf *buf)
{
	if	(!buf)	return;

	if	(buf->blksize)
	{
		lfree(buf->data);
		buf->data = NULL;
		buf->size = 0;
	}

	buf->used = 0;
}

/*
Die Funktion |$1| liefert den Pointer auf das nächste Element
im Vektorbuffer. Das Speicherfeld des Elements wird mit 0
initialisiert. Die Komponente |used| wird um 1 erhöht.
Bei Bedarf wird der Buffer vergrößert.
*/

void *vb_next (VecBuf *buf)
{
	if	(!buf)	return NULL;

	if	(buf->used >= buf->size)
		vb_realloc(buf, buf->used + 1);

	return memset((char *) buf->data + buf->elsize * buf->used++,
		0, buf->elsize);
}

/*
Die Funktion |$1| liefert das Element an Position <pos> des Vektorbuffers
<dim> oder einen Nullpointer, falls weniger Elemente enthalten sind.
*/

void *vb_data (VecBuf *buf, size_t pos)
{
	if	(!buf || pos >= buf->used)	return NULL;

	return ((char *) buf->data + buf->elsize * pos);
}

/*
Die Funktion |$1| fügt <dim> Element an der Position <pos>
ein. Die Funktion liefert die Startadresse der eingefügten Elemente.
Falls <pos> größer als die Komponente |used| ist oder <dim> den
Wert 0 hat, wird kein Element eingefügt und die Funktion
liefert einen Nullpointer.
*/

void *vb_insert (VecBuf *buf, size_t pos, size_t dim)
{
	char *ptr, *save;
	size_t i, n, offset;

	if	(buf == NULL || pos > buf->used || dim == 0)
		return NULL;

	n = pos * buf->elsize;
	offset = dim * buf->elsize;

/*	Genügend Speicherplatz für neue Elemente vorhanden
*/
	if	(buf->used + dim <= buf->size)
	{
		ptr = (char *) buf->data + buf->used * buf->elsize;

		for (i = buf->used; i > pos; i--)
		{
			ptr -= buf->elsize;
			memcpy(ptr + offset, ptr, buf->elsize);
		}
	}

/*	Tabelle umkopieren
*/
	else if	(buf->blksize != 0)
	{
		save = _vb_alloc(buf, buf->used + dim);
		n = pos * buf->elsize;
		ptr = (char *) buf->data + n;
		memcpy(buf->data, save, n);
		memset(ptr, 0, offset);
		memcpy(ptr + offset, save + n, (buf->used - pos) * buf->elsize);
		lfree(save);
	}
	else	return NULL;

	buf->used += dim;
	return ptr;
}

/*
Die Funktion |$1| löscht <dim> Elemente ab Position
<pos>. Die Datenwerte werden zum Ende des Speicherfeldes
verschoben. Die Funktion liefert den Pointer auf die gelöschten
Elemente.
Falls <pos> + <dim> größer als die Komponente |used| ist
wird kein Element gelöscht und die Funktion
liefert einen Nullpointer.
*/

void *vb_delete (VecBuf *buf, size_t pos, size_t dim)
{
	char *ptr;
	size_t i, j, size;
	int c;

	if	(buf == NULL || pos + dim > buf->used || dim == 0)
		return NULL;

	ptr = (char *) buf->data + pos * buf->elsize;
	size = (buf->used - pos) * buf->elsize;
	buf->used -= dim;
	dim *= buf->elsize;

	if	(dim == size)	return ptr;

	for (i = 0; i < dim; i++)
	{
		c = ptr[i];

		for (j = dim + i; j < size; j += dim)
			ptr[j - dim] = ptr[j];

		ptr[j - dim] = c;
	}

	return ptr + size - dim;
}
