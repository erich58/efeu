/*	Informationen zu einer Datenmatrix ausgeben
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static void put_mdindex(StrBuf *buf, mdaxis *x, size_t n)
{
	sb_puts(StrPool_get(x->sbuf, x->idx[n].i_name), buf);

	if	(x->idx[n].flags)
	{
		sb_putc('[', buf);

		if	(x->idx[n].flags & MDFLAG_BASE)
			sb_putc('B', buf);
		if	(x->idx[n].flags & MDFLAG_LOCK)
			sb_putc('L', buf);
		if	(x->idx[n].flags & MDFLAG_HIDE)
			sb_putc('H', buf);
		if	(x->idx[n].flags & MDFLAG_TEMP)
			sb_putc('-', buf);

		sb_putc(']', buf);
	}
}

static void put_mdaxis(StrBuf *buf, mdaxis *x, int par)
{
	sb_puts(StrPool_get(x->sbuf, x->i_name), buf);

	if	(x->flags)
	{
		sb_putc('[', buf);

		if	(x->flags & MDXFLAG_MARK)
			sb_putc('m', buf);
		if	(x->flags & MDXFLAG_TIME)
			sb_putc('t', buf);
		if	(x->flags & MDXFLAG_HIDE)
			sb_putc('h', buf);
		if	(x->flags & MDXFLAG_TEMP)
			sb_putc('+', buf);

		sb_putc(']', buf);
	}

	if	(par)
	{
		const char *delim = "=";

		for (size_t n = 0; n < x->dim; n++)
		{
			sb_puts(delim, buf);
			delim = ",";
			put_mdindex(buf, x, n);
		}

		sb_putc('\n', buf);
	}
}

static void align(StrBuf *buf)
{
	sb_nputc(' ', buf, 8 - sb_getpos(buf) % 8);
}

static void mode_type(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	PrintType(out, md->type, 1);

	if	(md->type && md->axis)
	{
		io_xprintf(out, "[%d", md->size / md->type->size);

		if	(par && md->axis->next)
		{
			const char *delim = "=";

			for (mdaxis *x = md->axis; x != NULL; x = x->next)
			{
				io_xprintf(out, "%s%d", delim, x->dim);
				delim = "*";
			}
		}

		io_putc(']', out);
	}
}

static void mode_head(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	mode_type(out, md, buf, 1);

	if	(md->i_name)
	{
		io_xprintf(out, " %#s",
			StrPool_get(md->sbuf, md->i_name));
	}

	if	(md->i_desc)
	{
		io_xprintf(out, " /* %s */",
			StrPool_get(md->sbuf, md->i_desc));
	}

	io_putc('\n', out);
}

static void mode_label(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	const char *delim = "";
	sb_clear(buf);

	for (mdaxis *x = md->axis; x != NULL; x = x->next)
	{
		sb_puts(delim, buf);
		put_mdaxis(buf, x, par);
		delim = par ? "" : " ";
	}

	io_puts(sb_cstr(buf), out);
}

static void mode_xsize(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	mode_type(out, md, buf, 0);

	for (mdaxis *x = md->axis; x != NULL; x = x->next)
	{
		io_putc(' ', out);
		io_puts(StrPool_get(x->sbuf, x->i_name), out);
		io_printf(out, "[%zd]", x->dim);
	}

	io_putc('\n', out);
}

static void put_size(IO *out, int width, size_t size)
{
	io_printf(out, "%*zd ", width, size);
}

static void mode_size(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	int width = 7;

	if	(md->size < 1000)
		width = 3;

	if	(md->type)
	{
		put_size(out, width, md->size);
		put_size(out, width, md->size / md->type->size);
		put_size(out, width, md->type->size);
		mode_head(out, md, buf, 0);
	}

	for (mdaxis *x = md->axis; x != NULL; x = x->next)
	{
		put_size(out, width, x->dim * x->size);
		put_size(out, width, x->dim);
		put_size(out, width, x->size);
		sb_clear(buf);
		put_mdaxis(buf, x, 1);
		io_puts(sb_cstr(buf), out);
	}
}

static void show_data(IO *out, const mdmat *md, StrBuf *buf, mdaxis *x, char *data)
{
	if	(x)
	{
		int k = sb_getpos(buf);

		for (size_t n = 0; n < x->dim; n++)
		{
			sb_setpos(buf, k);
			put_mdindex(buf, x, n);
			align(buf);
			show_data(out, md, buf, x->next, data);
			data += x->size;
		}
	}
	else
	{
		io_puts(sb_cstr(buf), out);
		io_printf(out, "%5zd ",
			(data - (char *) md->data) / md->type->size);
		PrintData(out, md->type, data);
		io_putc('\n', out);
	}
}

static void mode_data(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	sb_clear(buf);

	if	(md->axis)
	{
		for (mdaxis *x = md->axis; x != NULL; x = x->next)
		{
			put_mdaxis(buf, x, 0);
			align(buf);
		}

		io_puts(sb_cstr(buf), out);
		io_printf(out, "%5s ", "#");
		mode_type(out, md, buf, 0);
		io_putc('\n', out);

		sb_clear(buf);
		show_data(out, md, buf, md->axis, md->data);
	}
	else
	{
		PrintData(out, md->type, md->data);
		io_putc('\n', out);
	}
}

static void mode_sep(IO *out, const mdmat *md, StrBuf *buf, int par)
{
	io_putc(par, out);
}

static struct {
	int key; 
	int flag;
	void (*mode)(IO *out, const mdmat *md, StrBuf *buf, int par);
	const char *desc;
} mtab[] = {
	{ 'T', 0, mode_type, "Type with dimension" },
	{ 't', 1, mode_type, "Type with dimension composition" },
	{ 'h', 0, mode_head, "Type and matrix name" },
	{ 'X', 0, mode_label, "Axis names" },
	{ 'x', 1, mode_label, "Axis structure with index list" },
	{ 'S', 0, mode_xsize, "Type and axis dimension" },
	{ 's', 0, mode_size, "Axis dimension and data size" },
	{ 'd', 0, mode_data, "Data with labels" },
	{ ' ', ' ', mode_sep, NULL },
	{ 0, 0, NULL, NULL },
};

static void do_show(IO *out, const mdmat *md, StrBuf *buf, int mode)
{
	for (int i = 0; mtab[i].key; ++i)
	{
		if	(mode == mtab[i].key)
		{
			mtab[i].mode(out, md, buf, mtab[i].flag);
			return;
		}
	}

	for (int i = 0; mtab[i].key; ++i)
	{
		if	(mtab[i].desc)
			io_printf(out, "%c\t%s\n", mtab[i].key, mtab[i].desc);
	}
}

void md_show (IO *out, const mdmat *md, const char *mode)
{
	if	(!md)	return;

	STRBUF(buf, 32);

	if	(!mode)
		mode = "hx";

	for (; *mode; ++mode)
		do_show(out, md, &buf, *mode);

	sb_free(&buf);
}
