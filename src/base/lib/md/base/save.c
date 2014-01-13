/*	Header einer multidimensionale Matrix ausgeben
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

#define	STRLEN(x)	(x ? strlen(x) + 1 : 1)

static void put_2byte (unsigned x, IO *io)
{
	io_putc((x >> 8) & 0xFF, io);
	io_putc(x & 0xFF, io);
}

static void put_compact (uint64_t x, IO *io)
{
	io_ullwrite(io, &x, 1);
}

#define	put_index(tg, src, idx, io) \
	put_compact(StrPool_copy(tg, src, idx), io)

void md_puthdr (IO *io, mdmat *md, unsigned mask)
{
	mdaxis *x;
	mdindex *idx;
	size_t n;
	StrPool *sbuf;
	char *p;
	size_t dim;

	sbuf = NewStrPool();

	put_2byte(MD_MAGIC3, io);
	put_compact(md_dim(md->axis), io);

	p = TypeHead(md->type);
	put_compact(StrPool_add(sbuf, p), io);
	memfree(p);

	p = type2str(md->type);
	put_compact(StrPool_add(sbuf, p), io);
	memfree(p);

	put_index(sbuf, md->sbuf, md->i_name, io);
	put_index(sbuf, md->sbuf, md->i_desc, io);

	for (x = md->axis; x != NULL; x = x->next)
	{
		dim = 0;

		for (idx = x->idx, n = x->dim; n-- > 0; idx++)
		{
			if	(idx->flags & mask)
				continue;

			dim++;
		}

		put_compact(dim, io);
		put_index(sbuf, x->sbuf, x->i_name, io);
		put_index(sbuf, x->sbuf, x->i_desc, io);

		for (idx = x->idx, n = x->dim; n-- > 0; idx++)
		{
			if	(idx->flags & mask)
				continue;

			put_index(sbuf, x->sbuf, idx->i_name, io);
			put_index(sbuf, x->sbuf, idx->i_desc, io);
		}
	}

/*	Textteil ausgeben
*/
	put_compact(sbuf->used, io);
	io_write(io, sbuf->mdata, sbuf->used);
	rd_deref(sbuf);
}


/*	String ausgeben
*/

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
		else	log_note(NULL, "[mdmat:24]", "m", type2str(md->type));
	}
}


void md_fsave(const char *name, mdmat *md, unsigned mask)
{
	IO *io;

	io = io_fileopen(name, "wdz");
	md_save(io, md, mask);
	io_close(io);
}
