/*	Header einer multidimensionale Matrix ausgeben
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

#define	STRLEN(x)	(x ? strlen(x) + 1 : 1)

static size_t putstr (const char *str, IO *io);

static void put_2byte(unsigned x, IO *io)
{
	io_putc((x >> 8) & 0xFF, io);
	io_putc(x & 0xFF, io);
}

static void put_4byte(unsigned x, IO *io)
{
	io_putc((x >> 24) & 0xFF, io);
	io_putc((x >> 16) & 0xFF, io);
	io_putc((x >> 8) & 0xFF, io);
	io_putc(x & 0xFF, io);
}

void md_puthdr(IO *io, mdmat *md, unsigned mask)
{
	mdaxis *x;
	unsigned n;
	size_t j;
	int k;
	char *oname;
	unsigned dim;
	unsigned space;
	unsigned recl;
	unsigned nel;

/*	Stringlänge und Zahl der Elemente bestimmen
*/
	oname = type2str(md->type);
	dim = md_dim(md->axis);
	space = STRLEN(md->title);
	space += STRLEN(oname);
	recl = md->type->recl;
	nel = 1;

	for (x = md->axis; x != NULL; x = x->next)
	{
		for (j = k = 0; j < x->dim; j++)
		{
			if	(!(x->idx[j].flags & mask))
			{
				space += STRLEN(x->idx[j].name);
				k++;
			}
		}

		space += STRLEN(x->name);
		nel *= k;
	}

	space = 4 * ((space + 3) / 4);

/*	Header ausgeben
*/
	put_2byte(MD_MAGIC, io);
	put_2byte(dim, io);
	put_4byte(space, io);
	put_4byte(recl, io);
	put_4byte(nel, io);

/*	Gruppendimensionen ausgeben
*/
	x = md->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		for (j = n = 0; j < x->dim; j++)
			if (!(x->idx[j].flags & mask)) n++;

		put_4byte(n, io);
	}

/*	Textteil ausgeben
*/
	n = putstr(md->title, io);
	n += putstr(oname, io);
	x = md->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		n += putstr(x->name, io);

		for (j = 0; j < x->dim; j++)
		{
			if	(!(x->idx[j].flags & mask))
			{
				n += putstr(x->idx[j].name, io);
			}
		}
	}

	io_nputc(0, io, space - n);
	memfree(oname);
}


/*	String ausgeben
*/

static size_t putstr(const char *str, IO *io)
{
	size_t n;

	n = io_puts(str, io);
	io_putc(0, io);
	return n + 1;
}

void md_puteof(IO *io)
{
	io_write(io, MD_EOFMARK, 4);
}


/*	Daten ausgeben
*/

void md_putdata(IO *io, const EfiType *type, mdaxis *x,
	unsigned mask, void *data)
{
	if	(x != NULL)
	{
		size_t i;

		for (i = 0; i < x->dim; i++)
		{
			if	(!(x->idx[i].flags & mask))
				md_putdata(io, type, x->next, mask, data);

			data = ((char *) data) + x->size;
		}
	}
	else	WriteData(type, data, io);
}


void md_save(IO *io, mdmat *md, unsigned mask)
{
	if	(md)
	{
		if	(md->type->write || md->type->recl)
		{
			md_puthdr(io, md, mask);
			md_putdata(io, md->type, md->axis, mask, md->data);
			md_puteof(io);
		}
		else	dbg_note(NULL, "[mdmat:24]", "m", type2str(md->type));
	}
}


void md_fsave(const char *name, mdmat *md, unsigned mask)
{
	IO *io;

	io = io_fileopen(name, "wdz");
	md_save(io, md, mask);
	io_close(io);
}
