/*
:*:count definitions
:de:Zähldefinitionen

$Copyright (C) 2002 Erich Frühstück
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
A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/mdcount.h>
#include <ctype.h>

#define	BSIZE	1000

static char *rt_ident (const void *ptr)
{
	const MdCntDef *def = ptr;
	return msprintf("%s(%s, %d)", def->reftype->label,
		def->obj ? def->obj->type->name : "NULL", def->cltab.used);
}

static void rt_clean (void *ptr)
{
	MdCntDef *def = ptr;
	vb_clean(&def->cltab, NULL);
	UnrefObj(def->obj);
	memfree(def->pfx);
	memfree(def);
}

static RefType reftype = REFTYPE_INIT("MdCntDef", rt_ident, rt_clean);

MdCntDef* MdCntDef_create (EfiType *type, const char *pfx)
{
	MdCntDef *def;
	
	if	(type)
	{
		def = memalloc(sizeof(MdCntDef) + type->size);
		def->obj = LvalObj(&Lval_ptr, type, (void *) (def + 1));
	}
	else
	{
		def = memalloc(sizeof(MdCntDef));
		def->obj = NULL;
	}

	def->pfx = mstrcpy(pfx);
	vb_init(&def->cltab, BSIZE, sizeof(MdEfiClass *));
	rd_init(&reftype, def);
	return def;
}

static int idxfunc (MdClass *opaque_cdef, const void *ptr)
{
	MdEfiClass *cdef = (MdEfiClass *) opaque_cdef;
	int rval = 0;

	if	(cdef->obj)
	{
		PushVarTab(RefVarTab(cdef->obj->type->vtab),
			RefObj(cdef->obj));
		Obj2Data(EvalExpression(cdef->expr), &Type_int, &rval);
		PopVarTab();
	}
	else	Obj2Data(EvalExpression(cdef->expr), &Type_int, &rval);

	return rval;
}

static void label_def (MdEfiClass *cl, IO *io)
{
	StrBuf *sb;
	VecBuf tab;
	Label *label;
	int c;

	sb = sb_acquire();
	cl->dim = 0;
	vb_init(&tab, 16, sizeof(Label));

	while ((c = io_getc(io)) != EOF)
	{
		if	(isspace(c))
			continue;

		sb_setpos(sb, 0);

		do
		{
			sb_putc(c, sb);
			c = io_getc(io);
		}
		while	(c != EOF && !isspace(c));

		label = vb_next(&tab);
		label->name = sb_strcpy(sb);
		sb_setpos(sb, 0);

		while	(c == '\t' || c == ' ')
			c = io_getc(io);

		while	(c != EOF && c != '\n')
		{
			sb_putc(c, sb);
			c = io_getc(io);
		}

		label->desc = sb_strcpy(sb);
	}

	sb_release(sb);
	cl->dim = tab.used;
	cl->label = tab.data;
}

static void label_fmt (MdEfiClass *cl, const char *name, const char *desc)
{
	int n;

	cl->label = memalloc(cl->dim * sizeof(Label));
	PushVarTab(NULL, NULL);
	VarTab_xadd(NULL, "n", NULL, LvalObj(&Lval_ptr, &Type_int, &n));

	for (n = 0; n < cl->dim; n++)
	{
		cl->label[n].name = mpsubvec(name, 0, NULL);
		cl->label[n].desc = mpsubvec(desc, 0, NULL);
	}

	PopVarTab();
}

void MdCntDef_expr (EfiFunc *func, void *rval, void **arg)
{
	MdCntDef *cdef;
	MdEfiClass *cl, **ptr;

	if	(!(cdef = Val_ptr(arg[0])))
		return;

	cl = memalloc(sizeof(MdEfiClass));
	cl->name = mstrpaste(".", cdef->pfx, Val_str(arg[1]));
	cl->desc = mstrcpy(Val_str(arg[2]));
	cl->classify = idxfunc;
	cl->expr = RefObj(Val_obj(arg[3]));
	cl->obj = RefObj(cdef->obj);

	if	(func->dim > 5)
	{
		cl->dim = Val_int(arg[4]);
		label_fmt(cl, Val_str(arg[5]), Val_str(arg[6]));
	}
	else	label_def(cl, Val_io(arg[4]));

	ptr = vb_next(&cdef->cltab);
	*ptr = cl;
}

static void add (MdCountPar *tab, MdEfiClass **ptr, size_t n)
{
	for (; n-- > 0; ptr++)
		MdClass_add(tab, (MdClass *) *ptr, 1);
}

static void xadd (MdCountPar *tab, MdEfiClass **ptr, size_t n,
	const char *ext, EfiObj *obj)
{
	for (; n-- > 0; ptr++)
	{
		MdEfiClass *cl = memalloc(sizeof(MdEfiClass));
		cl->name = mstrpaste(".", (*ptr)->name, ext);
		cl->desc = (*ptr)->desc;
		cl->dim = (*ptr)->dim;
		cl->label = (*ptr)->label;
		cl->classify = (*ptr)->classify;
		cl->expr = RefObj((*ptr)->expr);
		cl->obj = RefObj(obj);
		MdClass_add(tab, (MdClass *) cl, 1);
	}
}

void MdCntDef_cadd (EfiFunc *func, void *rval, void **arg)
{
	MdCntDef *cdef;
	MdCountPar *tab;
	
	cdef = Val_ptr(arg[0]);
	tab = Val_ptr(arg[1]);

	if	(!cdef)	return;

	if	(func->dim > 2)
	{
		EfiObj *obj = arg[3];

		if	(obj && cdef->obj && obj->type != cdef->obj->type)
			dbg_note("md", "[mdmat:304]", NULL);

		xadd(tab, cdef->cltab.data, cdef->cltab.used,
			Val_str(arg[2]), obj);
	}
	else	add(tab, cdef->cltab.data, cdef->cltab.used);
}
