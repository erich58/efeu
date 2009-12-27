/*	Vektorbuffer aus Datei laden
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/vecbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

void *vb_load (vecbuf_t *buf, FILE *file, size_t dim)
{
	vb_realloc(buf, dim);
	buf->used = fread_compat(buf->data, buf->elsize, dim, file);

	if	(buf->used != dim)
		fileerror(file, MSG_EFM, 2, 0);

	return buf->data;
}
