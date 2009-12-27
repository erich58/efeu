/*	Informationen zu einer Datenmatrix ausgeben
	$Copyright (C) 2006 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>


void md_show (IO *io, mdmat *md)
{
	mdaxis *x;
	char *delim;
	int i, n;
	int lock;

	if	(!md)	return;

	PrintType(io, md->type, 1);
	io_printf(io, "[%d", md->size / md->type->size);
	delim = "=";

	for (x = md->axis; x != NULL; x = x->next)
	{
		io_printf(io, "%s%d", delim, x->dim);
		delim = "*";
	}

	io_putc(']', io);

	if	(md->title)
		io_printf(io, " %#s\n", md->title);
	else	io_putc('\n', io);

	for (x = md->axis; x != NULL; x = x->next)
	{
		n = io_puts(x->name, io);

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

			if	(x->idx[i].flags & MDFLAG_BASE)
			{
				n += io_puts("[", io);
				n += io_puts(x->idx[i].name, io);
				n += io_puts("]", io);
			}
			else	n += io_puts(x->idx[i].name, io);
		}

		if	(lock)
			io_putc(')', io);

		io_putc('\n', io);
	}
}
