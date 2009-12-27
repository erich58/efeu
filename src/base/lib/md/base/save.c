/*	Header einer multidimensionale Matrix ausgeben
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/data.h>

#define	STRLEN(x)	(x ? strlen(x) + 1 : 1)

static size_t putstr (const char *str, io_t *io);

static void put_2byte(unsigned x, io_t *io)
{
	io_putc((x >> 8) & 0xFF, io);
	io_putc(x & 0xFF, io);
}

static void put_4byte(unsigned x, io_t *io)
{
	io_putc((x >> 24) & 0xFF, io);
	io_putc((x >> 16) & 0xFF, io);
	io_putc((x >> 8) & 0xFF, io);
	io_putc(x & 0xFF, io);
}

void md_puthdr(io_t *io, mdmat_t *md, unsigned mask)
{
	mdaxis_t *x;
	ulong_t n;
	size_t j;
	int k;
	char *oname;
	ulong_t dim;
	ulong_t space;
	ulong_t recl;
	ulong_t nel;

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

static size_t putstr(const char *str, io_t *io)
{
	size_t n;

	n = io_puts(str, io);
	io_putc(0, io);
	return n + 1;
}

void md_puteof(io_t *io)
{
	io_write(io, MD_EOFMARK, 4);
}


/*	Daten ausgeben
*/

void md_putdata(io_t *io, const Type_t *type, mdaxis_t *x,
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
	else	IOData(type, (iofunc_t) io_dbwrite, io, data);
}


void md_save(io_t *io, mdmat_t *md, unsigned mask)
{
	if	(md)
	{
		if	(md->type->iodata)
		{
			md_puthdr(io, md, mask);
			md_putdata(io, md->type, md->axis, mask, md->data);
			md_puteof(io);
		}
		else
		{
			reg_set(1, type2str(md->type));
			errmsg(MSG_MDMAT, 24);
		}
	}
}


void md_fsave(const char *name, mdmat_t *md, unsigned mask)
{
	io_t *io;

	io = io_fileopen(name, "wz");
	md_save(io, md, mask);
	io_close(io);
}
