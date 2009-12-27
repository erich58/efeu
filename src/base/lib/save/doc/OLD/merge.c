/*	Mischstruktur erzeugen/verwalten
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

static Merge_t *merge_admin (Merge_t *tg, const Merge_t *src)
{
	if	(tg)
	{
		io_close(tg->input);
		rd_deref(tg->out);
		del_strbuf(tg->buf);
		memfree(tg);
		return NULL;
	}
	else
	{
		tg = memalloc(sizeof(Merge_t));
		tg->buf = new_strbuf(0);
		return tg;
	}
}

static char *merge_ident (Merge_t *merge)
{
	reg_set(1, io_ident(merge->input));
	reg_set(2, rd_ident(merge->out));
	return parsub("$1 -> $2");
}

REFTYPE(Merge_reftype, "Merge", merge_ident, merge_admin);

Merge_t *Merge_open (DocOut_t *out)
{
	Merge_t *merge = rd_create(&Merge_reftype);
	merge->out = out;
	return merge;
}

/*	IO - Interface für Mischstruktur
*/

static int merge_ctrl (Merge_t *merge, int req, va_list list)
{
	size_t stat;

	switch (req)
	{
	case IO_CLOSE:

		rd_deref(merge);
		return 0;

	case IO_IDENT:

		*va_arg(list, char **) = rd_ident(merge);
		return 0;

	case IO_FLUSH:

		return io_vctrl(merge->out->io, req, list);

	default:

		return io_vctrl(merge->input, req, list);
	}

	return stat;
}

io_t *Merge_io (Merge_t *merge, int (*get) (Merge_t *merge),
	int (*put) (int c, Merge_t *merge))
{
	io_t *io = io_alloc();
	io->get = (io_get_t) get;
	io->put = (io_put_t) put;
	io->ctrl = (io_ctrl_t) merge_ctrl;
	io->data = rd_refer(merge);
	return io;
}
