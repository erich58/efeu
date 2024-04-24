/*	Informationen zu einer Datenmatrix ausgeben
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static void put_mdaxis(StrBuf *buf, mdaxis *x)
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
}

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

static void align(StrBuf *buf)
{
	sb_nputc(' ', buf, 8 - sb_getpos(buf) % 8);
}

static void show_data(IO *out, mdmat *md, StrBuf *buf, mdaxis *x, char *data)
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
		io_puts("\n", out);
	}
}

#define	F_HEAD	1
#define	F_AXIS	2
#define	F_FLAGS	4
#define	F_SIZE	8
#define	F_DATA	16

void md_show (IO *out, mdmat *md, const char *mode)
{
	mdaxis *x;
	const char *delim;
	int flags = 0;

	if	(!md)	return;

	STRBUF(buf, 32);

	if	(mode)
	{
		for (; *mode; ++mode)
		{
			switch(*mode)
			{
			case 'h':	flags |= F_HEAD; break;
			case 'f':	flags |= F_FLAGS; break;
			case 'x':	flags |= F_AXIS; break;
			case 's':	flags |= F_SIZE; break;
			case 'd':	flags |= F_DATA; break;
			default:	break;
			}
		}
	}
	else
	{
		flags = F_HEAD | F_FLAGS;
	}

	if	(flags & F_HEAD)
	{
		PrintType(out, md->type, 1);
		io_xprintf(out, "[%d", md->size / md->type->size);
		delim = "=";

		for (x = md->axis; x != NULL; x = x->next)
		{
			io_xprintf(out, "%s%d", delim, x->dim);
			delim = "*";
		}

		io_putc(']', out);

		if	(md->i_name)
		{
			io_xprintf(out, " %#s",
				StrPool_get(md->sbuf, md->i_name));
		}

		io_putc('\n', out);
	}

	if	(flags & (F_AXIS | F_FLAGS))
	{
		delim = "";
		sb_clear(&buf);

		for (x = md->axis; x != NULL; x = x->next)
		{
			sb_puts(delim, &buf);
			put_mdaxis(&buf, x);

			if	(flags & F_FLAGS)
			{
				delim = "=";

				for (size_t n = 0; n < x->dim; n++)
				{
					sb_puts(delim, &buf);
					delim = ",";
					put_mdindex(&buf, x, n);
				}

				sb_putc('\n', &buf);
				delim = "";
			}
			else
			{
				delim = " ";
			}
		}

		io_puts(sb_cstr(&buf), out);
	}

	if	(flags & F_SIZE)
	{
		io_printf(out, "%-7s %7zd %7zd %7zd\n",
			md->type->name, md->size,
			md->size / md->type->size, md->type->size);

		for (mdaxis *x = md->axis; x != NULL; x = x->next)
		{
			io_printf(out, "%-7s %7zd %7zd %7zd\n",
				StrPool_get(x->sbuf, x->i_name),
				x->dim * x->size, x->dim, x->size);
		}
	}

	if	(flags & F_DATA)
	{
		sb_clear(&buf);

		for (x = md->axis; x != NULL; x = x->next)
		{
			put_mdaxis(&buf, x);
			align(&buf);
		}

		io_puts(sb_cstr(&buf), out);
		io_printf(out, "%5s %s[%zd]\n", "#",
			md->type->name, md->size / md->type->size);

		sb_clear(&buf);
		show_data(out, md, &buf, md->axis, md->data);
	}

	sb_free(&buf);
}
