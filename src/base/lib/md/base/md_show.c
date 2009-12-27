/*	Informationen zu einer Datenmatrix ausgeben
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>


void md_show (IO *io, mdmat *md)
{
	mdaxis *x;
	char *delim;
	int i, n;
	int lock;
	char *p;

	if	(!md)	return;

	PrintType(io, md->type, 1);
	io_xprintf(io, "[%d", md->size / md->type->size);
	delim = "=";

	for (x = md->axis; x != NULL; x = x->next)
	{
		io_xprintf(io, "%s%d", delim, x->dim);
		delim = "*";
	}

	io_putc(']', io);

	if	(md->i_name)
		io_xprintf(io, " %#s\n", StrPool_get(md->sbuf, md->i_name));
	else	io_putc('\n', io);

	for (x = md->axis; x != NULL; x = x->next)
	{
		n = io_puts(StrPool_get(x->sbuf, x->i_name), io);

		if	(x->flags)
		{
			n += io_puts("[", io);

			if	(x->flags & MDXFLAG_MARK)
				n += io_puts("m", io);
			if	(x->flags & MDXFLAG_TIME)
				n += io_puts("t", io);
			if	(x->flags & MDXFLAG_HIDE)
				n += io_puts("h", io);
			if	(x->flags & MDXFLAG_TEMP)
				n += io_puts("x", io);

			n += io_puts("]", io);
		}

		n += io_puts(":", io);
		lock = 0;

		for (i = 0; i < x->dim; i++)
		{
			if	(lock && !(x->idx[i].flags & MDFLAG_LOCK))
			{
				n += io_puts(")", io);
				lock = 0;
			}

			if	(n > 70)
			{
				io_puts("\n\t", io);
				n = 8;
			}
			else	n += io_nputc(' ', io, 1);

			if	(!lock && (x->idx[i].flags & MDFLAG_LOCK))
			{
				n += io_puts("(", io);
				lock = 1;
			}

			p = StrPool_get(x->sbuf, x->idx[i].i_name);

			if	(x->idx[i].flags & MDFLAG_BASE)
			{
				n += io_puts("[", io);
				n += io_puts(p, io);
				n += io_puts("]", io);
			}
			else	n += io_puts(p, io);
		}

		if	(lock)
			io_putc(')', io);

		io_putc('\n', io);
	}
}
