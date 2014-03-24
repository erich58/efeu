/*
Teilklassifikation einer Aufzählung

$Copyright (C) 2009 Erich Frühstück
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

#include <EFEU/EfiClass.h>
#include <EFEU/MatchPar.h>
#include <EFEU/parsearg.h>

typedef struct {
	const char *name;
	char *desc;
	int key;
	int val;
} IDX;

typedef struct {
	REFVAR;
	size_t dim;
	IDX *tab;
	int *map;
} SPar;

static RefType SPar_reftype = REFTYPE_INIT("SubClassPar", NULL, memfree);

static int cmp_idx (const void *ap, const void *bp)
{
	const IDX *a = ap;
	const IDX *b = bp;

	if	(a->key < b->key)	return -1;
	if	(a->key > b->key)	return 1;

	return 0;
}

#if	0
static int map_update (const EfiObj *obj, void *opaque_par)
{
	SPar *par = opaque_par;
	int idx = Val_int(obj->data);
	return (idx >= 0 && idx <= par->dim) ? par->map[idx] : 0;
}
#endif

static int sub_update (const EfiObj *obj, void *opaque_par)
{
	if	(opaque_par)
	{
		SPar *par = opaque_par;
		IDX idx, *ptr;

		idx.key = Val_int(obj->data);
		ptr = bsearch(&idx, par->tab, par->dim,
			sizeof par->tab[0], cmp_idx);
		return ptr ? ptr->val : 0;
	}
	else	return 1;
}

static VECBUF(idx_buf, 100, sizeof(IDX));

static IDX *make_idx (const EfiType *type)
{
	idx_buf.used = 0;

	while (type)
	{
		IDX *idx;
		VarTabEntry *p;
		size_t k;

		if	(!type->vtab)	continue;

		p = type->vtab->tab.data;
		k = type->vtab->tab.used;

		for (; k-- > 0; p++)
		{
			if	(!p->obj || p->obj->type != type)
				continue;

			idx = vb_next(&idx_buf);
			idx->name = p->name;
			idx->desc = p->desc;
			idx->key = Val_int(p->obj->data);
			idx->val = 0;
		}

		type = type->base;
	}

	vb_qsort(&idx_buf, cmp_idx);
	return idx_buf.data;
}

static SPar *make_par (const EfiType *type)
{
	SPar *par = memalloc(sizeof *par + idx_buf.used * idx_buf.elsize);
	par->dim = idx_buf.used;
	par->tab = vb_fetch(&idx_buf, par + 1);
	return rd_init(&SPar_reftype, par);
}

static void create_sub (EfiClassArg *def, const EfiType *type,
	const char *opt, const char *arg, void *xpar)
{
	size_t n, k;
	IDX *idx;
	MatchPar *match;

 	def->type = NewEnumType(NULL, 1);
	def->update = sub_update;
	idx = make_idx (type);
	match = MatchPar_list(arg, idx_buf.used);

	for (n = k = 0; n < idx_buf.used; n++)
	{
		if	(MatchPar_exec(match, idx[n].name, n + 1))
		{
			idx[n].val = ++k;
			AddEnumKey(def->type, mstrcpy(idx[n].name),
				mstrcpy(idx[n].desc), k);
		}
	}

	rd_deref(match);
 	def->type = AddEnumType(def->type);
	def->par = make_par (type);
}

static EfiClass EfiClass_sub = EFI_CLASS(NULL,
	"select", "matchlist", create_sub, NULL,
	"label selection"
);

static void create_grp (EfiClassArg *def, const EfiType *type,
	const char *opt, const char *arg, void *xpar)
{
	size_t n, k;
	IDX *idx;
	MatchPar *match;
	AssignArg *x;
	StrBuf *sb;
	char *p;

 	def->type = NewEnumType(NULL, 1);
	def->update = sub_update;
	idx = make_idx (type);

	p = (char *) arg;
	k = 0;
	sb = NULL;

	while ((x = assignarg(p, &p, " \t;")))
	{
		int inc;
		io_xprintf(ioerr, "%#s %#s %#s\n", x->name, x->opt, x->arg);

		if	(x->name && x->name[0] && x->name[0] != '*')
		{
			sb = x->opt ? NULL : sb_acquire();
			k++;
			inc = 0;
		}
		else	inc = 1;

		match = MatchPar_list(x->arg, idx_buf.used);

		for (n = 0; n < idx_buf.used; n++)
		{
			if	(idx[n].val)
				continue;

			if	(!MatchPar_exec(match, idx[n].name, n + 1))
				continue;

			if	(inc)
			{
				k++;
				AddEnumKey(def->type, mstrcpy(idx[n].name),
					mstrcpy(idx[n].desc), k);
			}
			else if	(sb)
			{
				if	(sb->pos)
					sb_puts(", ", sb);

				sb_puts(idx[n].name, sb);
			}

			idx[n].val = k;
		}

		rd_deref(match);

		if	(!inc)
		{
			AddEnumKey(def->type, mstrcpy(x->name),
				sb ? sb_cpyrelease(sb) : mstrcpy(x->opt), k);
		}

		memfree(x);
	}

	/*
	for (n = 0; n < idx_buf.used; n++)
	{

		fprintf(stderr, "%d %d %s\n",
			idx[n].key, idx[n].val, idx[n].name);
	}
	*/

 	def->type = AddEnumType(def->type);
	def->par = make_par (type);
}

static EfiClass EfiClass_grp = EFI_CLASS(NULL,
	"group", "name[desc]=matchlist ...", create_grp, NULL,
	"label grouping"
);

void EfiClassEnum (void)
{
	AddEfiPar(&Type_enum, &EfiClass_sub);
	AddEfiPar(&Type_enum, &EfiClass_grp);
}
