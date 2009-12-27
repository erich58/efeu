/*	Vektorbuffer
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>
#include <EFEU/procenv.h>

/*
Die Funktion |vb_init| initialisiert den Vektorbuffer <buf>
mit Blockgröße <blk> und Datengröße <size>.
*/

void vb_init (vecbuf_t *buf, size_t blk, size_t size)
{
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

void vb_clean (vecbuf_t *buf, clean_t clean)
{
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

void vb_free (vecbuf_t *buf)
{
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
im Vektorbuffer. Die Komponente |used| wird um 1 erhöht.
Bei Bedarf wird der Buffer vergrößert.
*/

void *vb_next (vecbuf_t *buf)
{
	if	(buf->used >= buf->size)
		vb_realloc(buf, buf->used + 1);

	return ((char *) buf->data + buf->elsize * buf->used++);
}

/*
Die Funktion |$1| liefert das Element an Position <pos> des Vektorbuffers
<dim> oder einen Nullpointer, falls weniger Elemente enthalten sind.
*/

void *vb_data (vecbuf_t *buf, size_t pos)
{
	if	(pos >= buf->used)	return NULL;

	return ((char *) buf->data + buf->elsize * pos);
}

/*
Die Funktion |$1| fügt <dim> Element an der Position <pos>
ein. Die Funktion liefert die Startadresse der eingefügten Elemente.
Falls <pos> größer als die Komponente |used| ist oder <dim> den
Wert 0 hat, wird kein Element eingefügt und die Funktion
liefert einen Nullpointer.
*/

void *vb_insert (vecbuf_t *buf, size_t pos, size_t dim)
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

void *vb_delete (vecbuf_t *buf, size_t pos, size_t dim)
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
