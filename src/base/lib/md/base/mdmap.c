/*
Map-Dateien generieren/laden
*/

#include <EFEU/mdmat.h>
#include <EFEU/MapFile.h>
#include <EFEU/preproc.h>

typedef struct {
	unsigned char name[8];
	size_t key;
} MAGIC;

static MAGIC magic = {
#ifdef	PTRSIZE_8
	"EDCM8-1", 0x3837363534333231ull,
#else
	"EDCM4-1", 0x34333231ul,
#endif
};

typedef struct {
	size_t flags;
	size_t dim;
	size_t nidx;
	size_t space;
} XDIM;


int md_fputmap (mdmat *md, const char *path)
{
	if	(md && md->type->recl)
	{
		IO *out = io_fopen(path, "w");
		int stat = md_putmap(md, out);
		io_close(out);
		return stat;
	}

	return 0;
}

int md_putmap (mdmat *md, IO *out)
{
	XDIM xdim;
	StrPool *sbuf;
	mdaxis *x;
	char *p;
	size_t n;
	size_t *val;
	
	if	(!md)	return 0;

	if	(md->type->recl == 0)
		return 0;

	if	(!out)	return 0;

	/* calculate sizes */

	sbuf = NewStrPool();

	xdim.flags = 0;
	xdim.dim = 0;
	xdim.nidx = 0;

	for (x = md->axis; x; x = x->next)
	{
		xdim.dim++;
		xdim.nidx += x->dim;
	}

	/* compose strings */

	n = 4 + 3 * xdim.dim + 2 * xdim.nidx;
	val = lmalloc(n * sizeof *val);
	n = 0;

	p = TypeHead(md->type);
	val[n++] = StrPool_add(sbuf, p);
	memfree(p);

	p = type2str(md->type);
	val[n++] = StrPool_add(sbuf, p);
	memfree(p);

	val[n++] = StrPool_copy(sbuf, md->sbuf, md->i_name);
	val[n++] = StrPool_copy(sbuf, md->sbuf, md->i_desc);

	for (x = md->axis; x; x = x->next)
	{
		mdindex *idx;
		size_t k;

		val[n++] = x->dim;
		val[n++] = StrPool_copy(sbuf, x->sbuf, x->i_name);
		val[n++] = StrPool_copy(sbuf, x->sbuf, x->i_desc);

		for (k = x->dim, idx = x->idx; k-- > 0; idx++)
		{
			val[n++] = StrPool_copy(sbuf, x->sbuf, idx->i_name);
			val[n++] = StrPool_copy(sbuf, x->sbuf, idx->i_desc);
		}
	}

	StrPool_align(sbuf, sizeof(size_t));
	xdim.space = StrPool_offset(sbuf);

	/* write map file */

	io_write(out, &magic, sizeof magic);
	io_write(out, &xdim, sizeof xdim);
	io_write(out, val, n * sizeof *val);
	lfree(val);
	StrPool_write(sbuf, out);
	rd_deref(sbuf);
	io_write(out, md->data, md->size);
	return 1;
}

static mdmat *clean_up (MapFile *map, const char *msg)
{
	io_puts(msg, ioerr);
	return NULL;
}

mdmat *md_map (MapFile *map)
{
	mdmat *md;
	mdaxis **ptr;
	unsigned char *data;
	size_t size;
	XDIM *xdim;
	StrPool *sbuf;
	size_t i, n;
	size_t *val;
	char *p;

	if	(!map)	return NULL;

	data = map->data;
	size = map->size;

	/* magic key */

	if	(size < sizeof magic || memcmp(data, &magic, sizeof magic) != 0)
		return NULL;

	data += sizeof magic;
	size -= sizeof magic;

	/* axis and labels */

	if	(size < sizeof *xdim)
		return clean_up(map, "axis: EOF\n");

	xdim = (void *) data;
	data += sizeof *xdim;
	size -= sizeof *xdim;
	n = (4 + 3 * xdim->dim + 2 * xdim->nidx) * sizeof *val;

	if	(size < n)
		return clean_up(map, "axis: EOF\n");

	val = (void *) data;
	data += n;
	size -= n;

	sbuf = NewStrPool();
	sbuf->csize = xdim->space;

	if	(size < sbuf->csize)
	{
		rd_deref(sbuf);
		return clean_up(map, "label: EOF\n");
	}

	sbuf->cpos = sbuf->csize;
	sbuf->cdata = (void *) data;
	data += sbuf->csize;
	size -= sbuf->csize;

	/* create data matrix */

	if	((p = StrPool_get(sbuf, *(val++))))
	{
		IO *cin = io_cmdpreproc(io_cstr(p));
		CmdEval(cin, NULL);
		io_close(cin);
	}

	md = new_mdmat();
	md->map = rd_refer(map);
	md->sbuf = sbuf;
	md->type = edb_type(StrPool_get(sbuf, *(val++)));
	md->i_name = *(val++);
	md->i_desc = *(val++);
	ptr = &md->axis;

	for (n = 0; n < xdim->dim; n++)
	{
		*ptr = new_axis(sbuf, *(val++));
		(*ptr)->i_name = *(val++);
		(*ptr)->i_desc = *(val++);

		for (i = 0; i < (*ptr)->dim; i++)
		{
			(*ptr)->idx[i].i_name = *(val++);
			(*ptr)->idx[i].i_desc = *(val++);
		}

		ptr = &(*ptr)->next;
	}
	
	md->size = md_size(md->axis, md->type->size);

	/* data */

	if	(size < md->size)
	{
		rd_deref(md);
		return clean_up(map, "data: EOF\n");
	}

	md->data = data;
	return md;
}

mdmat *md_getmap (const char *path)
{
	MapFile *map = MapFile_open(path);

	if	(map)
	{
		mdmat *md = md_map(map);
		rd_deref(map);
		return md;
	}

	return NULL;
}
