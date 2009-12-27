/*	Informationsdatenbank ausgeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <ctype.h>

#define	CTRL_NL		0x1
#define	CTRL_KEY	0x2
#define	CTRL_LABEL	0x3
#define	CTRL_TEXT	0x4

typedef struct {
	io_t *io;	/* Ausgabestruktur */
	int (*put) (int c, io_t *io);	/* Ausgabefunktion */
	int last;	/* Letztes ausgegebene Zeichen */
} FILTER;

static int filter_put(int c, FILTER *filter)
{
	if	(c == '$' || (c == INFO_KEY && filter->last == '\n'))
		io_putc(c, filter->io);

	filter->put(c, filter->io);
	filter->last = c;
	return c;
}

static int key_put(int c, io_t *io)
{
	switch (c)
	{
	case '\n':	c = 'n';
	case '\t':	c = 't';
	case '\f':	c = 'f';
	case '\b':	c = 'b';
	case '<':
	case '|':
	case ':':
	case '\\':	io_putc('\\', io); break;
	}

	return io_putc(c, io);
}

static int label_put(int c, io_t *io)
{
	switch (c)
	{
	case '\n':
	case '<':
	case '|':
	case '\\':	io_putc('\\', io); break;
	case '$':	io_putc('$', io); break;
	}

	return io_putc(c, io);
}

static int filter_ctrl(FILTER *filter, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:
		return 0;
	case CTRL_NL:
		if	(filter->last != '\n')
		{
			io_putc('\n', filter->io);
			filter->last = '\n';
		}

		return 0;
	case CTRL_KEY:
		io_putc(INFO_KEY, filter->io);
		filter->put = key_put;
		return 0;
	case CTRL_LABEL:
		io_putc(':', filter->io);
		filter->put = label_put;
		return 0;
	case CTRL_TEXT:
		io_putc('\n', filter->io);
		filter->last = '\n';
		filter->put = io_putc;
		return 0;
	default:
		break;
	}

	return io_vctrl(filter->io, req, list);
}

static void dump_info (io_t *io, InfoNode_t *base, InfoNode_t *info)
{
	if	(info->load)
		info->load(info);

	io_ctrl(io, CTRL_NL);
	io_ctrl(io, CTRL_KEY);
	InfoName(io, base, info);
	io_ctrl(io, CTRL_LABEL);

	if	(info->label)
	{
		io_putc(' ', io);
		io_psub(io, info->label);
	}

	io_ctrl(io, CTRL_TEXT);

	if	(!info->func)
	{
		reg_cpy(1, info->name);
		reg_cpy(2, info->label);
		io_psub(io, info->par);
	}
	else	info->func(io, info);

	io_ctrl(io, CTRL_NL);

	if	(info->list)
	{
		int i = info->list->used;
		InfoNode_t **ip = info->list->data;

		for (; i > 0; i--, ip++)
			dump_info(io, base, *ip);
	}
}


void DumpInfo (io_t *io, InfoNode_t *base, const char *name)
{
	InfoNode_t *info = GetInfo(base, name);

	if	(info)
	{
		FILTER filter;

		filter.io = io;
		filter.put = io_putc;
		filter.last = '\n';
		io = io_alloc();
		io->put = (io_put_t) filter_put;
		io->ctrl = (io_ctrl_t) filter_ctrl;
		io->data = &filter;
		dump_info(io, name ? info : base, info);
		io_close(io);
	}
}
