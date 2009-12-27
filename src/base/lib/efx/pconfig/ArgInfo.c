/*	Informationen über Aufrufparameter abfragen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/pconfig.h>

static void printarg(io_t *io, pardef_t *def)
{
	if	(!def->name)	return;
	if	(!def->desc)	return;

	io_puts(def->name, io);
	io_puts("\t", io);
	io_psub(io, def->desc);
	io_putc('\n', io);
}

void ArgInfo (io_t *io, InfoNode_t *info)
{
	vecbuf_t *vb;
	pardef_t *p;
	size_t n;

	if	((vb = info->par) == NULL) return;

	for (n = vb->used, p = vb->data; n > 0; n--, p++)
		printarg(io, p);
}
