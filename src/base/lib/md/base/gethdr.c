/*	Header einer multidimensionale Matrix laden
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/preproc.h>

static unsigned get_2byte(IO *io)
{
	int a, b;

	a = io_getc(io);
	b = io_getc(io);

	if	(a != EOF && b != EOF)
		return ((a << 8) + b);

	return 0;
}

static unsigned get_4byte(IO *io)
{
	unsigned a = get_2byte(io);
	return (a << 16) + get_2byte(io);
}

mdmat *md_gethdr(IO *io)
{
	mdmat *md;
	mdaxis *x;
	mdaxis **ptr;
	char *strbuf;
	size_t j;
	char *p;
	char *oname;
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
		io_error(io, "[mdmat:10]", NULL);
		exit(EXIT_FAILURE);
		break;
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
	default:
		io_error(io, "[mdmat:11]", NULL);
		return NULL;
	}

/*	Datenstruktur initialisieren
*/
	md = new_mdmat();
	md->data = NULL;
	ptr = &md->axis;

/*	Indexstruktur initialisieren und Vektordimensionen laden
*/
	for (j = 0; j < dim; j++)
	{
		n = get_4byte(io);
		*ptr = new_axis((size_t) n);
		ptr = &(*ptr)->next;
	}

	*ptr = NULL;

/*	Textteil laden
*/
	strbuf = (char *) memalloc(space);
	io_read(io, strbuf, (size_t) space);
	p = strbuf;
	md->title = mstrcpy(nextstr(&p));

	if	(magic == MD_MAGIC2)
	{
		IO *cin = io_cmdpreproc(io_cstr(nextstr(&p)));
		CmdEval(cin, NULL);
		io_close(cin);
	}

	oname = nextstr(&p);
	md->type = mdtype(oname);

/*	Kompatiblitätsprüfung
*/
	if	(md->type->recl != recl)
	{
		dbg_error(NULL, "[mdmat:14]", "s", oname);
		return NULL;
	}

/*	Achsen initialisieren
*/
	md->size = md_size(md->axis, md->type->size);

	for (x = md->axis; x != NULL; x = x->next)
	{
		x->name = mstrcpy(nextstr(&p));

		for (j = 0; j < x->dim; j++)
			x->idx[j].name = mstrcpy(nextstr(&p));
	}

	memfree(strbuf);
	return md;
}
