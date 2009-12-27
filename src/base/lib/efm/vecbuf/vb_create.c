/*	Vektorbuffer anfordern/freigeben
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>
#include <EFEU/procenv.h>

/*
Die Funktion |$1| liefert einen neuen Vektorbuffer
mit Buffergröße <blk> und Elementgröße <size>.
*/

vecbuf_t *vb_create (size_t blk, size_t size)
{
	vecbuf_t *vb = memalloc(sizeof(vecbuf_t));
	vb_init(vb, blk, size);
	return vb;
}

/*
Die Funktion |$1| gibt einen mit |vb_create| angeforderten Vektorbuffer
wieder frei.
*/

void vb_destroy (vecbuf_t *vb)
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
