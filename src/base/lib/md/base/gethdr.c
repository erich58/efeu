/*	Header einer multidimensionale Matrix laden
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/preproc.h>

static size_t get_2byte (IO *io)
{
	int a, b;

	a = io_getc(io);
	b = io_getc(io);

	if	(a != EOF && b != EOF)
		return ((a << 8) + b);

	return 0;
}

static size_t get_4byte (IO *io)
{
	unsigned a = get_2byte(io);
	return (a << 16) + get_2byte(io);
}

static size_t get_compact (IO *io)
{
	uint64_t len = 0;

	io_ullread(io, &len, 1);
	return (size_t) len;
}

static mdmat *hdr_v3 (IO *io)
{
	size_t space;
	size_t dim;
	char *p;
	mdmat *md;
	mdaxis *x, **ptr;
	size_t i, n;
	size_t i_head, i_type;

	dim = get_compact(io);
	i_head = get_compact(io);
	i_type = get_compact(io);

	md = new_mdmat();
	md->i_name = get_compact(io);
	md->i_desc = get_compact(io);
	ptr = &md->axis;

	for (n = 0; n < dim; n++)
	{
		*ptr = new_axis(NULL, get_compact(io));
		(*ptr)->i_name = get_compact(io);
		(*ptr)->i_desc = get_compact(io);

		for (i = 0; i < (*ptr)->dim; i++)
		{
			(*ptr)->idx[i].i_name = get_compact(io);
			(*ptr)->idx[i].i_desc = get_compact(io);
		}

		ptr = &(*ptr)->next;
	}

	space = get_compact(io);
	md->sbuf = StrPool_alloc(space);
	StrPool_read(md->sbuf, space, io);

	if	((p = StrPool_get(md->sbuf, i_head)))
	{
		IO *cin = io_cmdpreproc(io_cstr(p));
		CmdEval(cin, NULL);
		io_close(cin);
	}

	md->type = edb_type(StrPool_get(md->sbuf, i_type));
	md->size = md_size(md->axis, md->type->size);

	for (x = md->axis; x; x = x->next)
		x->sbuf = rd_refer(md->sbuf);

	return md;
}

mdmat *md_gethdr(IO *io, int flag)
{
	mdmat *md;
	mdaxis *x;
	mdaxis **ptr;
	size_t j;
	size_t idx;
	size_t magic;
	size_t n;
	size_t dim;
	size_t space;
	size_t recl;

/*	Header laden
*/
	switch ((magic = get_2byte(io)))
	{
	case MD_MAGIC0:
		if	(flag)
			io_error(io, "[mdmat:10]", NULL);

		return NULL;
	case MD_MAGIC1:
		dim = get_2byte(io);
		space = get_4byte(io);
		recl = get_4byte(io);
		(void) get_4byte(io);
		break;
	case MD_MAGIC2:
		dim = get_2byte(io);
		space = get_4byte(io);
		recl = get_4byte(io);
		break;
	case MD_MAGIC3:
		return hdr_v3(io);
	default:
		if	(flag)
			io_error(io, "[mdmat:11]", NULL);

		return NULL;
	}

/*	Datenstruktur initialisieren
*/
	md = new_mdmat();
	md->sbuf = StrPool_alloc(space);
	md->data = NULL;
	ptr = &md->axis;

/*	Indexstruktur initialisieren und Vektordimensionen laden
*/
	for (j = 0; j < dim; j++)
	{
		n = get_4byte(io);
		*ptr = new_axis(md->sbuf, (size_t) n);
		ptr = &(*ptr)->next;
	}

	*ptr = NULL;

/*	Textteil laden
*/
	StrPool_read(md->sbuf, space, io);
	StrPool_start(md->sbuf);
	md->i_name = StrPool_next(md->sbuf);
	md->i_desc = 0;

	if	(magic == MD_MAGIC2)
	{
		IO *cin;
		
		idx = StrPool_next(md->sbuf);
		cin = io_cmdpreproc(io_cstr(StrPool_get(md->sbuf, idx)));
		CmdEval(cin, NULL);
		io_close(cin);
	}

	md->type = mdtype(StrPool_get(md->sbuf, StrPool_next(md->sbuf)));

/*	Kompatiblitätsprüfung
*/
	if	(md->type->recl != recl)
	{
		if	(flag)
			log_error(NULL, "[mdmat:14]", "s", md->type->name);

		rd_deref(md);
		return NULL;
	}

/*	Achsen initialisieren
*/
	md->size = md_size(md->axis, md->type->size);

	for (x = md->axis; x != NULL; x = x->next)
	{
		x->i_name = StrPool_next(md->sbuf);
		x->i_desc = 0;

		for (j = 0; j < x->dim; j++)
		{
			x->idx[j].i_name = StrPool_next(md->sbuf);
			x->idx[j].i_desc = 0;
		}
	}

	return md;
}
