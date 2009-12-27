/*	Informationsdaten ausgeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	NODE_KEY	'*'

/*	Ausgabefilter für Einträge
*/

typedef struct {
	io_t *io;	/* Ausgabestruktur */
	int last;	/* letztes ausgegebene Zeichen */
} FILTER;

static int filter_put(int c, FILTER *filter)
{
	if	(c == NODE_KEY && filter->last == '\n')
		io_putc('\\', filter->io);

	io_putc(c, filter->io);
	filter->last = c;
	return c;
}

static int filter_ctrl(FILTER *filter, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:	return 0;
	default:	break;
	}

	return io_vctrl(filter->io, req, list);
}

static void save_entry(io_t *io, InfoEntry_t *entry, size_t dim)
{
	FILTER filter;

	filter.io = io;
	filter.last = '\n';
	io = io_alloc();
	io->put = (io_put_t) filter_put;
	io->ctrl = (io_ctrl_t) filter_ctrl;
	io->data = &filter;

	for (; dim > 0; dim--, entry++)
		if (entry->func) entry->func(io, entry->data);

	io_close(io);
}


void SaveInfo (io_t *io, InfoNode_t *root, InfoNode_t *node)
{
	int n;
	InfoNode_t **lp;

	if	(node == NULL)	return;

	if	(node->prev && node != root)
	{
		io_putc(NODE_KEY, io);
		InfoName(io, root, node, ":<\n");

		if	(node->head)
		{
			io_putc(':', io);
			InfoHead(io, node, "<\n");
		}

		io_putc('\n', io);
	}

	if	(node->entry.used)
		save_entry(io, node->entry.data, node->entry.used);

	for (n = node->list.used, lp = node->list.data; n > 0; n--, lp++)
		SaveInfo(io, root, *lp);
}
