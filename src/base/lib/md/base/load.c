/*
:*:selectiv load of data cube
:de:Multidimensionale Matrix selektiv laden

$Copyright (C) 1994, 2007 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/mdmat.h>
#include <EFEU/MatchPar.h>

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

static EfiStruct *getentry (EfiType *type, const char *name);

#if	0
static int walk_type (const char *name, EfiType **type, int o1, int o2);
static char **vsel_list = NULL;
static size_t vsel_dim = 0;
static IO *vsel_io = NULL;
#endif

static VSEL_T *vsel_cdef = NULL;
static EfiStruct *vsel_struct = NULL;


/*	Datenmatrix aus Datei laden
*/

static int extname (const char *name)
{
	char *p;

	if	(name == NULL || *name == '|' || *name == '&')
		return 1;

	p = strrchr(name, '.');

	if	(!p)	return 0;

	p++;

	if	(strcmp(p, "gz") == 0)	return 1;
	if	(strcmp(p, "bz2") == 0)	return 1;

	return 0;
}

mdmat *md_fload (const char *name, const char *list, const char *var)
{
	IO *io;
	mdmat *md;
	MapFile *map;

	if	(extname(name))
	{
		io = io_fileopen(name, "rdz");
	}
	else if	((map = MapFile_open(name)))
	{
		if	((md = md_map(map)))
		{
			rd_deref(map);

			if	(list || var)
				return md_reload(md, list, var);

			return md;
		}

		io = io_data(map, map->data, map->size);
	}
	else	io = io_fileopen(name, "rdz");

	md = md_load(io, list, var);
	io_close(io);
	return md;
}

mdmat *md_reload (mdmat *md, const char *list, const char *var)
{
	IO *io;

	io = io_tmpfile();
	md_save(io, md, MDFLAG_LOCK);
	io_rewind(io);
	rd_deref(md);
	md = md_load(io, list, var);
	io_close(io);
	return md;
}


/*	Matrix selektiv laden
*/

mdmat *md_load (IO *io, const char *str, const char *odef)
{
	mdmat *md;
	mdaxis *axis, *x;
	mdaxis **ptr;
	mdlist *def;
	int depth;
	int c;

	c = io_getc(io);
	io_ungetc(c, io);

	if	(c == 'E')
	{
		md = edb2md(edb_open(rd_refer(io)));

		if	(str || odef)
			md = md_reload(md, str, odef);

		return md;
	}

/*	Dateiheader lesen
*/
	if	((md = md_gethdr(io, 1)) == NULL)
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

		md_alloc(md);
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
		*ptr = mkaxis(x, mdlistcmp(StrPool_get(x->sbuf, x->i_name),
			depth++, def));
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
			log_error(NULL, "[mdmat:15]", "ms",
				type2str(md->type), odef);
			return NULL;
		}

		getval = member_get;
		md->type = vsel_struct->type;
		
#if	0
		vsel_io = io_tmpbuf(0);
		vsel_dim = mstrsplit(odef, ",%s", &vsel_list);
		vsel_dim = walk_type(NULL, (EfiType **) &md->type, 0, 0);
		memfree(vsel_list);

		if	(vsel_dim == 0 || md->type->size == 0)
		{
			io_close(vsel_io);
			log_error(NULL, "[mdmat:15]", NULL);
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
	md_alloc(md);
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

static mdaxis *mkaxis (mdaxis *x, mdlist *def)
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
	MatchPar *mp;

	x->priv = NULL;

/*	Bei fehlender Selektion: Achse kopieren
*/
	if	(def == NULL)
	{
		y = new_axis(x->sbuf, x->dim);
		y->i_name = x->i_name;
		y->i_desc = x->i_desc;

		for (i = 0; i < x->dim; i++)
		{
			y->idx[i].i_name = x->idx[i].i_name;
			y->idx[i].i_desc = x->idx[i].i_desc;
		}

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

		mp = MatchPar_create(def->list[i], x->dim);

		if	(mp->flag)	preselect = 0;

		for (j = 0; j < x->dim; j++)
		{
			if	(mp->cmp(mp, StrPool_get(x->sbuf,
					x->idx[j].i_name), j + 1))
			{
				flag = mp->flag;
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

		rd_deref(mp);
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
	x->priv = (void *) sdef;

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
	y = new_axis(x->sbuf, xvars);
	y->i_name = x->i_name;
	y->i_desc = x->i_desc;
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
				mp = MatchPar_list(def->lopt[i], x->dim);
			}
			else	mp = NULL;

			if	(mp != NULL)
			{
				for (j = 0; j < x->dim; j++)
				{
					if	(MatchPar_exec(mp,
							StrPool_get(x->sbuf,
							x->idx[j].i_name), j))
					{
						sdef[j].skip = 0;
						sdef[j].idx[sdef[j].dim++] =
							xvars;
					}
				}
			}

			rd_deref(mp);

			if	(def->list[i][1] != '#')
			{
				y->idx[xvars++].i_name = StrPool_add(y->sbuf,
					def->list[i] + 1);
				continue;
			}

			anz = atoi(def->list[i] + 2);

			for (j = 0; j < anz; j++)
			{
				char *p = msprintf("%s%d%c",
					StrPool_get(x->sbuf, x->i_name),
					xvars + 1, 0);
				y->idx[xvars].i_name = StrPool_add(y->sbuf, p);
				xvars++;
				memfree(p);
			}

			continue;
		}

		for (j = 0; j < x->dim; j++)
		{
			if	(x->idx[j].flags == i + 1)
			{
				sdef[j].skip = sdef[j].tmpload = 0;
				sdef[j].offset = xvars;
				y->idx[xvars].i_name = StrPool_copy(y->sbuf,
					x->sbuf, x->idx[j].i_name);
				y->idx[xvars].i_desc = StrPool_copy(y->sbuf,
					x->sbuf, x->idx[j].i_desc);
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

	return y;
}


/*	Datenwerte lesen
*/

static void getdata (const EfiType *type, char *data, mdaxis *x, mdaxis *y)
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
		sdef = (void *) x->priv;

		for (i = 0; i < x->dim; i++)
		{
			if	(sdef[i].skip)
			{
				skipdata(type, x->next);
				continue;
			}

			if	(sdef[i].tmpload)
			{
				p = (void *) y->priv;
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

static void skipdata (const EfiType *type, mdaxis *x)
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

#if	0
static int walk_type (const char *name, EfiType **type, int o1, int o2)
{
	int n, k;
	int new_offset;
	EfiStruct **st;

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
#endif

static EfiStruct *getentry (EfiType *type, const char *name)
{
	EfiStruct *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(mstrcmp(name, st->name) == 0)
			return st;
	}

	return NULL;
}

/*	Lesefunktion bei Objektselektion
*/

static void std_get (const EfiType *type, void *ptr)
{
	ReadData(type, ptr, load_io);
}


static void member_get (const EfiType *type, void *ptr)
{
	ReadData(data_type, data_buf, load_io);

	memcpy((char *) ptr, data_buf + vsel_struct->offset,
		vsel_struct->type->size);
}
