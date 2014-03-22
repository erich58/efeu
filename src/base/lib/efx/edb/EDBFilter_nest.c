/*	Verschachteln von Datensätzen

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/EDBFilter.h>
#include <EFEU/konvobj.h>
#include <EFEU/parsearg.h>
#include <EFEU/MatchPar.h>
#include <EFEU/printobj.h>

typedef struct {
	int grp;
	EfiStruct *st;
	EfiObj *obj;
} NEST_VAR;


static int var_cmp (const void *ap, const void *bp)
{
	const NEST_VAR *a = ap;
	const NEST_VAR *b = bp;

	if	(a->grp < b->grp)	return -1;
	if	(a->grp > b->grp)	return 1;

	return 0;
}

typedef struct {
	AssignArg *def;
	int grp;
	int uniq;
	MatchPar *match;
	EfiStruct *list;
	EfiStruct **ptr;
	EfiStruct *vec;
	EfiType *type;
	NEST_VAR *var;
	size_t dim;
} NEST_DEF;

static void list_var (IO *out, EfiStruct *st);

static void nestdef_clean (void *data)
{
	NEST_DEF *def = data;
	memfree(def->def);
	rd_deref(def->match);
	DelEfiStruct(def->list);
}

static NEST_DEF *nestdef_get (VecBuf *buf, const char *name, int idx)
{
	NEST_DEF *def;
	size_t n;

	def = buf->data;

	for (n = 1; n < buf->used; n++)
		if (MatchPar_exec(def[n].match, name, idx))
			return def + n;

	return def;
}


typedef struct {
	REFVAR;
	EDB *base;
	VecBuf def_tab;
	StrBuf buf;
	IO *io;
	NEST_VAR *tab;
	EfiObj *key;
	size_t dim;
	int uniq;
} NEST;

static void nest_clean (void *data)
{
	NEST *par = data;
	rd_deref(par->base);
	vb_clean(&par->def_tab, nestdef_clean);
	UnrefObj(par->key);
	io_close(par->io);
	sb_free(&par->buf);
	memfree(par);
}

static RefType nest_reftype = REFTYPE_INIT("EDB_NEST", NULL, nest_clean);

static void add_def (NEST *nest, AssignArg *x)
{
	NEST_DEF *def = vb_next(&nest->def_tab);
	def->grp = nest->def_tab.used - 1;
	def->def = x;
	def->uniq = (x && mstrcmp(x->opt, "uniq") == 0);
	def->match = x ? MatchPar_list(x->arg ? x->arg : x->name,
		nest->dim) : NULL;
	def->list = NULL;
	def->ptr = &def->list;
	def->vec = NULL;
	def->var = NULL;
	def->dim = 0;
	nest->uniq |= def->uniq;
}

static NEST_VAR *new_var (NEST_VAR *ptr, const EfiObj *obj, EfiStruct *st)
{
	if	(!obj->lval || st->member || st->dim)
	{
		EfiName name;
		EfiObj *cobj;

		name.obj = RefObj(obj);
		name.name = mstrcpy(st->name);

		obj = NewObj(&Type_mname, &name);
		cobj = EvalObj(RefObj(obj), NULL);

		if	(!cobj)
			return NULL;

		ptr->st = NewEfiStruct(cobj->type, st->name, 0);
		ptr->obj = RefObj(obj);
		UnrefObj(cobj);
		return ptr;
	}

	ptr->st = NewEfiStruct(st->type, st->name, st->dim);
	ptr->obj = LvalObj(&Lval_obj, st->type, RefObj(obj),
		(char *) obj->data + st->offset);
	return ptr;
}

static void list_var (IO *out, EfiStruct *st)
{
	for (; st; st = st->next)
		io_xprintf(out, "%s\t%s\n", st->name, st->desc);
}

static void set_val (NEST_VAR *var, char *ptr)
{
	Obj2Data(RefObj(var->obj), var->st->type, ptr + var->st->offset);
}

static void set_def (NEST_DEF *def, char *ptr)
{
	int i;

	for (i = 0; i < def->dim; i++)
		set_val(def->var + i, ptr);
}

static int vec_cmp (unsigned char *data, size_t n1, size_t n2)
{
	if	(n1 != n2)	return 0;

	return memcmp(data, data + n1, n2) == 0;
}

static void vec_uniq (NEST *nest, EfiVec *vec)
{
	char *p;
	size_t p0, p1, p2;
	size_t i, n;

	if	(vec->buf.used < 2)	return;

	io_rewind(nest->io);
	p = vec->buf.data;
	p0 = 0;
	WriteData(vec->type, p, nest->io);
	p1 = nest->buf.pos;

	for (i = n = 1; i < vec->buf.used; i++)
	{
		p += vec->buf.elsize;
		WriteData(vec->type, p, nest->io);
		p2 = nest->buf.pos;

		if	(vec_cmp(nest->buf.data + p0, p1 - p0, p2 - p1))
		{
			nest->buf.pos = p1;
			sb_sync(&nest->buf);
		}
		else
		{
			p0 = p1;
			p1 = p2;
			n++;
		}
	}

	io_rewind(nest->io);
	EfiVec_resize(vec, n);
	ReadVecData(vec->type, n, vec->buf.data, nest->io);
}

static int nest_read (EfiType *type, void *data, void *par)
{
	NEST *nest = par;
	NEST_DEF *def;
	int pos;
	int dim, n;

	if	(!edb_read(nest->base))
		return 0;

	def = nest->def_tab.data;
	dim = nest->def_tab.used;
	set_def(def, data);
	io_rewind(nest->io);
	WriteData(nest->key->type, data, nest->io);
	pos = nest->buf.pos;

	for (n = 1; n < dim; n++)
	{
		EfiVec *vec = Val_ptr((char *) data + def[n].vec->offset);
		EfiVec_resize(vec, 0);
		set_def(def + n, vb_next(&vec->buf));
	}

	while ((edb_read(nest->base)))
	{
		set_def(def, nest->key->data);
		nest->buf.pos = pos;
		sb_sync(&nest->buf);
		WriteData(nest->key->type, nest->key->data, nest->io);

		if	(2 * pos != nest->buf.pos)
			break;

		if	(memcmp(nest->buf.data, nest->buf.data + pos, pos))
			break;

		for (n = 1; n < dim; n++)
		{
			EfiVec *vec = Val_ptr((char *) data +
				def[n].vec->offset);
			set_def(def + n, vb_next(&vec->buf));
		}
	}

	edb_unread(nest->base);

	if	(!nest->uniq)	return 1;

	for (n = 1; n < dim; n++)
	{
		if	(def[n].uniq)
		{
			EfiVec *vec = Val_ptr((char *) data +
				def[n].vec->offset);
			vec_uniq(nest, vec);
		}
	}

	return 1;
}

static EDB *fdef_nest (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	AssignArg *x;
	EfiStruct *st;
	EfiStruct *list, **ptr;
	NEST_DEF *def;
	NEST *nest;
	char *p;
	size_t dim, n;
	EDB *edb;

	if	(!base || !base->obj)
		return base;

	if	(!opt || !*opt || *opt == '?')
	{
		list_var(ioerr, base->obj->type->list);
		exit(EXIT_SUCCESS);
	}

	for (dim = 0, st = base->obj->type->list; st; st = st->next, dim++)
		;

	nest = memalloc(sizeof *nest + dim * sizeof nest->tab[0]);
	nest->tab = (void *) (nest + 1);
	nest->dim = dim;
	nest->base = base;
	sb_init(&nest->buf, 0);
	nest->io = io_strbuf(&nest->buf);
	nest->uniq = 0;
	rd_init(&nest_reftype, nest);

	vb_init(&nest->def_tab, 8, sizeof *def);
	add_def(nest, NULL);

	while ((x = assignarg(opt, &p, " \t\n;")) != NULL)
	{
		add_def(nest, x);
		opt = p;
	}

	for (st = base->obj->type->list, n = 0; st; st = st->next, n++)
	{
		NEST_VAR *var = new_var(nest->tab + n, base->obj, st);

		if	(!var)	continue;

		def = nestdef_get(&nest->def_tab, st->name, n + 1);
		var->grp = def->grp;
  		*def->ptr = var->st;
		def->ptr = &var->st->next;
	}

	qsort(nest->tab, nest->dim, sizeof nest->tab[0], var_cmp);
	def = nest->def_tab.data;

	for (n = 0; n < nest->dim; n++)
	{
		NEST_DEF *ptr = def + nest->tab[n].grp;

		if	(!ptr->dim)
			ptr->var = nest->tab + n;

		ptr->dim++;
	}

	for (n = 0; n < nest->def_tab.used; n++)
	{
		if	(def[n].list)
		{
			if	(def[n].list->next)
			{
				def[n].type = MakeStruct(NULL, NULL,
					RefEfiStruct(def[n].list));
			}
			else	def[n].type = def[n].list->type;
		}
		else	def[n].type = &Type_void;
	}

	nest->key = NewObj(def->type, NULL);
	list = NULL;
	ptr = &list;

	for (st = def->list; st; st = st->next)
	{
		*ptr = NewEfiStruct(st->type, st->name, st->dim);
		ptr = &(*ptr)->next;
	}

	for (n = 1; n < nest->def_tab.used; n++)
	{
		def[n].vec = NewEfiStruct(NewVecType(def[n].type, 0),
			def[n].def->name, 0);
		*ptr = def[n].vec;
		ptr = &(*ptr)->next;
	}

	edb = edb_create(MakeStruct(NULL, NULL, list));
	edb->read = nest_read;
	edb->ipar = nest;
	return edb;
}

EDBFilter EDBFilter_nest = EDB_FILTER(NULL,
	"nest", "[name=list]", fdef_nest, NULL,
	":*:nest data records"
	":de:Datensätze verschachteln"
);

