/*	Vektorbuffer vergr��ern
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6
*/

#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>
#include <EFEU/procenv.h>

/*
Die Funktion |$1| sorgt daf�r, da� im Vektorbuffer <buf> mindestens
<dim> Elemente zur Verf�gung stehen. Die Funktion liefert
den Pointer auf das Speicherfeld. Die Komponente <used> wird auf 0
gesetzt.
*/

void *vb_alloc (vecbuf_t *buf, size_t dim)
{
	lfree(_vb_alloc(buf, dim));
	buf->used = 0;
	return buf->data;
}


/*
Die Funktion |$1| sorgt daf�r, da� im Vektorbuffer <buf> mindestens
<dim> Elemente zur Verf�gung stehen, wobei die verwendeten
Datenelemente unver�ndert bleiben.
*/

void *vb_realloc (vecbuf_t *buf, size_t dim)
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
verwendet. Falls der Buffer vergr��ert wurde, liefert sie
die Adresse des alten Datenbuffers, ansonsten einen Nullpointer.
Der alte Datenbuffer kann mit |lfree| freigegeben werden.
Der neue Datenbuffer wird nicht initialisiert!
*/

void *_vb_alloc (vecbuf_t *buf, size_t dim)
{
	if	(dim > buf->size)
	{
		if	(buf->blksize)
		{
			register void *save;

			save = buf->data;
			buf->size = buf->blksize * ((dim + buf->blksize - 1) / buf->blksize);
			buf->data = lmalloc((ulong_t) buf->size * buf->elsize);
			return save;
		}

		message("vecbuf", MSG_EFM, 1, 0);
		exit(1);
	}

	return NULL;
}
