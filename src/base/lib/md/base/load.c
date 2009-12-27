/*	Multidimensionale Matrix selektiv laden
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdtest.h>

typedef struct {
	short skip;	/* Überlesen */
	short tmpload;	/* Temporär laden */
	size_t offset;	/* Datenoffset */
	size_t dim;	/* Setztiefe */
	size_t *idx;	/* Indexvektor */
} SDEF;

static mdaxis *mkaxis (mdaxis *axis, mdlist *def);
static void getdata (const EfiType *type, char *ptr, mdaxis *x, mdaxis *y);
static void adddata (char *y, char *x, mdaxis *axis);
static void skipdata (const EfiType *type, mdaxis *x);
static void member_get (const EfiType *type, void *ptr);
static void std_get (const EfiType *type, void *ptr);

/*	Variablenselektion
*/

static char *data_buf = NULL;
static EfiType *data_type = NULL;
static IO *load_io = NULL;
static EfiFunc *f_add = NULL;
static int need_add = 0;

static void (*getval) (const EfiType *type, void *p) = NULL;

typedef struct {
	size_t new_pos;
	size_t old_pos;
	size_t size;
} VSEL_T;

static int walk_type (const char *name, EfiType **type, int o1, int o2);
static EfiVar *getentry (EfiType *type, const char *name);

static char **vsel_list = NULL;
static VSEL_T *vsel_cdef = NULL;
static size_t vsel_dim = 0;
static EfiVar *vsel_struct = NULL;
static IO *vsel_io = NULL;


/*	Datenmatrix aus Datei laden
*/

mdmat *md_fload(const char *name, const char *list, const char *var)
{
	IO *io;
	mdmat *md;

	io = io_fileopen(name, "rdz");
	md = md_load(io, list, var);
	io_close(io);
	return md;
}

mdmat *md_reload(mdmat *md, const char *list, const char *var)
{
	IO *io;

	io = io_tmpfile();
	md_save(io, md, MDFLAG_LOCK);
	io_rewind(io);
	md = md_load(io, list, var);
	io_close(io);
	return md;
}


/*	Matrix selektiv laden
*/

mdmat *md_load(IO *io, const char *str, const char *odef)
{
	mdmat *md;
	mdaxis *axis, *x;
	mdaxis **ptr;
	mdlist *def;
	int depth;

/*	Dateiheader lesen
*/
	if	((md = md_gethdr(io)) == NULL)
	{
		return NULL;
	}

	load_io = io;

/*	Schnelles Laden bei fehlenden Selektonsparamatern
*/
	if	(str == NULL && odef == NULL)
	{
		size_t n;
		char *ptr;

		md->data = memalloc(md->size);
		memset(md->data, 0, (size_t) md->size);
		n = md->size / md->type->size;
		ptr = md->data;
		ReadVecData(md->type, n, ptr, load_io);
		md_tsteof(io);
		return md;
	}

/*	Achsen reorganisieren
*/
	def = str2mdlist(str, MDLIST_NEWOPT);
	axis = md->axis;
	ptr = &md->axis;
	depth = 1;

	for (x = axis; x != NULL; x = x->next)
	{
		*ptr = mkaxis(x, mdlistcmp(x->name, depth++, def));
		ptr = &(*ptr)->next;
	}

	del_mdlist(def);
	*ptr = NULL;
	data_buf = memalloc(md->type->size);
	data_type = md->type;

/*	Datentype bestimmen
*/
	if	(odef != NULL)
	{
		vsel_struct = getentry(md->type, odef);

		if	(vsel_struct == NULL)
		{
			dbg_error(NULL, "[mdmat:15]", "ms",
				type2str(md->type), odef);
			return NULL;
		}

		getval = member_get;
		md->type = vsel_struct->type;
		
#if	0
		vsel_io = io_tmpbuf(0);
		vsel_dim = strsplit(odef, ",%s", &vsel_list);
		vsel_dim = walk_type(NULL, (EfiType **) &md->type, 0, 0);
		memfree(vsel_list);

		if	(vsel_dim == 0 || md->type->size == 0)
		{
			io_close(vsel_io);
			dbg_error(NULL, "[mdmat:15]", NULL);
			return NULL;
		}

		vsel_cdef = memalloc(vsel_dim * sizeof(VSEL_T));
		io_rewind(vsel_io);
		io_read(vsel_io, vsel_cdef, sizeof(VSEL_T) * vsel_dim);
		io_close(vsel_io);
		getval = osel_get;
#endif
	}
	else
	{
		vsel_cdef = NULL;
		getval = std_get;
	}

/*	Daten laden
*/
	md->size = md_size(md->axis, md->type->size);
	md->data = memalloc(md->size);
	memset(md->data, 0, (size_t) md->size);

	if	(need_add)
	{
		f_add = mdfunc_add(md->type);

		for (x = md->axis; x != NULL; x = x->next)
			if (x->priv) x->priv = memalloc(x->size);
	}

	getdata(md->type, md->data, axis, md->axis);

	for (x = axis; x != NULL; x = x->next)
		memfree(x->priv);

	for (x = md->axis; x != NULL; x = x->next)
		memfree(x->priv);

	del_axis(axis);
	memfree(data_buf);
	memfree(vsel_cdef);
	md_tsteof(io);
	return md;
}


/*	Neue Achse generieren
*/

static mdaxis *mkaxis(mdaxis *x, mdlist *def)
{
	size_t xvars;
	int preselect;
	int flag;
	int anz;
	size_t i, j;
	size_t depth;
	mdaxis *y;
	SDEF *sdef;
	size_t *ptr;
	mdtest *test;
	StrBuf *sb;
	char *p;

	x->priv = NULL;

/*	Bei fehlender Selektion: Achse kopieren
*/
	if	(def == NULL)
	{
		y = new_axis(x->dim);
		y->name = mstrcpy(x->name);

		for (i = 0; i < x->dim; i++)
			y->idx[i].name = mstrcpy(x->idx[i].name);

		return y;
	}

/*	Selektionsflags setzen
*/
	xvars = 0;
	preselect = 1;
	depth = 0;

	for (i = 0; i < def->dim; i++)
	{
		if	(def->list[i] == NULL)
			continue;

		if	(def->list[i][0] == ':')
		{
			if	(def->list[i][1] == '#')
			{
				xvars += atoi(def->list[i] + 2);
			}
			else	xvars++;

			if	(def->lopt[i])	depth++;

			continue;
		}

		test = mdtest_create(def->list[i], x->dim);

		if	(test->flag)	preselect = 0;

		for (j = 0; j < x->dim; j++)
		{
			if	(test->cmp(test, x->idx[j].name, j + 1))
			{
				flag = test->flag;
			}
			else if	(preselect)
			{
				flag = 1;
			}
			else	continue;

			if	(flag && !x->idx[j].flags)
			{
				x->idx[j].flags = i + 1;
			}
			else if	(!flag)
			{
				x->idx[j].flags = 0;
			}
		}

		mdtest_clean(test);
		preselect = 0;
	}

/*	Achsendimension bestimmen
*/
	for (j = 0; j < x->dim; j++)
		if (x->idx[j].flags) xvars++;

/*	Zuweisungsstruktur generieren
*/
	sdef = memalloc(x->dim * sizeof(SDEF) + x->dim * depth * sizeof(size_t));
	ptr = (size_t *) (sdef + x->dim);
	x->priv = sdef;

	for (j = 0; j < x->dim; j++)
	{
		sdef[j].skip = 1;
		sdef[j].tmpload = 1;
		sdef[j].offset = 0;
		sdef[j].dim = 0;
		sdef[j].idx = ptr;
		ptr += depth;
	}

/*	Neue Achsenbezeichner zusammenstellen
*/
	y = new_axis(xvars);
	sb = new_strbuf(32);
	sb_putstr(x->name, sb);
	y->priv = NULL;
	xvars = 0;

	if	(depth)	need_add = 1;

	for (i = 0; i < def->dim; i++)
	{
		if	(def->list[i] == NULL)
			continue;

		if	(def->list[i][0] == ':')
		{
			if	(def->lopt[i])
			{
				test = mdmktestlist(def->lopt[i], x->dim);
			}
			else	test = NULL;

			if	(test != NULL)
			{
				for (j = 0; j < x->dim; j++)
				{
					if	(mdtest_eval(test, x->idx[j].name, j))
					{
						sdef[j].skip = 0;
						sdef[j].idx[sdef[j].dim++] = xvars;
					}
				}
			}

			mdtest_clean(test);

			if	(def->list[i][1] != '#')
			{
				sb_putstr(def->list[i] + 1, sb);
				xvars++;
				continue;
			}

			anz = atoi(def->list[i] + 2);

			for (j = 0; j < anz; j++)
			{
				sb_printf(sb, "%s%d%c", x->name, xvars + 1, 0);
				xvars++;
			}

			continue;
		}

		for (j = 0; j < x->dim; j++)
		{
			if	(x->idx[j].flags == i + 1)
			{
				sdef[j].skip = sdef[j].tmpload = 0;
				sdef[j].offset = xvars;
				sb_putstr(x->idx[j].name, sb);
				xvars++;
			}
		}
	}

	for (j = 0; j < x->dim; j++)
	{
		if	(sdef[j].tmpload)
		{
			y->priv = (void *) 1;
			break;
		}
	}

	p = (char *) sb->data;
	y->name = mstrcpy(nextstr(&p));

	for (j = 0; j < y->dim; j++)
		y->idx[j].name = mstrcpy(nextstr(&p));

	del_strbuf(sb);
	return y;
}


/*	Datenwerte lesen
*/

static void getdata(const EfiType *type, char *data, mdaxis *x, mdaxis *y)
{
	size_t i, j;
	SDEF *sdef;
	char *p;

	if	(x == NULL)
	{
		getval(type, data);
	}
	else if	(x->priv)
	{
		sdef = x->priv;

		for (i = 0; i < x->dim; i++)
		{
			if	(sdef[i].skip)
			{
				skipdata(type, x->next);
				continue;
			}

			if	(sdef[i].tmpload)
			{
				p = y->priv;
				memset(p, 0, y->size);
			}
			else	p = data + sdef[i].offset * y->size;

			getdata(type, p, x->next, y->next);

			for (j = 0; j < sdef[i].dim; j++)
				adddata(data + sdef[i].idx[j] * y->size, p, y->next);
		}
	}
	else
	{
		for (i = 0; i < x->dim; i++)
		{
			getdata(type, data, x->next, y->next);
			data += y->size;
		}
	}
}

static void adddata (char *y, char *x, mdaxis *axis)
{
	if	(axis != NULL)
	{
		size_t i;

		for (i = 0; i < axis->dim; i++)
		{
			adddata(y, x, axis->next);
			y += axis->size;
			x += axis->size;
		}
	}
	else	CallVoidFunc(f_add, y, x);
}

static void skipdata(const EfiType *type, mdaxis *x)
{
	if	(x != NULL)
	{
		size_t i;

		for (i = 0; i < x->dim; i++)
			skipdata(type, x->next);
	}
	else	ReadData(data_type, data_buf, load_io);
}


/*	Objektselektion generieren
*/

static int walk_type(const char *name, EfiType **type, int o1, int o2)
{
	int n, k;
	int new_offset;
	EfiVar **st;

/*	Test auf Selektion der kompletten Struktur
*/
	if	(patselect(name, vsel_list, vsel_dim))
	{
		VSEL_T x;

		x.new_pos = o1;
		x.old_pos = o2;
		x.size = (*type)->size;
		io_write(vsel_io, &x, sizeof(VSEL_T));
		return 1;
	}

/*	Test auf Selektion von Komponenten
*/
	st = &(*type)->list;
	new_offset = 0;
	n = 0;

	while (*st != NULL)
	{
		char *p;

		p = mstrpaste(".", name, (*st)->name);
		k = walk_type(p, &(*st)->type, o1 + new_offset,
			o2 + (*st)->offset);

		if	(k != 0)
		{
			(*st)->offset = new_offset;
			new_offset += (*st)->type->size;
			st = &(*st)->next;
			n += k;
		}
		else	*st = (*st)->next;

		memfree(p);
	}

	if	(n)
	{
		(*type)->size = new_offset;
	}

/*	Test auf Reduzierbarkeit
*/
	if	(n && !(*type)->list->dim && !(*type)->list->next)
	{
		*type = (*type)->list->type;
	}

	return n;
}

static EfiVar *getentry (EfiType *type, const char *name)
{
	EfiVar *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(mstrcmp(name, st->name) == 0)
			return st;
	}

	return NULL;
}

/*	Lesefunktion bei Objektselektion
*/

static void std_get(const EfiType *type, void *ptr)
{
	ReadData(type, ptr, load_io);
}


static void member_get(const EfiType *type, void *ptr)
{
	ReadData(data_type, data_buf, load_io);

	memcpy((char *) ptr, data_buf + vsel_struct->offset,
		vsel_struct->type->size);
}
