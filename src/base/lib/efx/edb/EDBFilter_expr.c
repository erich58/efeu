/*
:*:apply expression to record
:de:Einfacher Transformationsfilter

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
#include <EFEU/cmdeval.h>

typedef struct EXPR EXPR;

struct EXPR {
	REFVAR;
	EDB *base;
	EfiObj *expr;
};

static void expr_clean (void *data)
{
	EXPR *par = data;
	rd_deref(par->base);
	UnrefObj(par->expr);
	memfree(par);
}

static RefType expr_reftype = REFTYPE_INIT("EDB_EXPR", NULL, expr_clean);

static int expr_read (EfiType *type, void *data, void *opaque_par)
{
	EXPR *par = opaque_par;
	int rval = 0;

	PushVarTab(RefVarTab(par->base->obj->type->vtab),
		RefObj(par->base->obj));

	while (edb_read(par->base))
	{
		CmdEval_stat = 0;
		UnrefEval(RefObj(par->expr));

		switch (CmdEval_stat)
		{
		case CmdEval_Normal:	rval = 1; break;
		case CmdEval_Cont:	continue;
		default:		break;
		}

		break;
	}

	CmdEval_stat = 0;
	PopVarTab();
	return rval;
}

static EDB *expr_create (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EXPR *par;
	EDB *edb;
	IO *io;

	par = memalloc(sizeof *par);
	par->base = rd_refer(base);

	io = io_cstr(opt);
	PushVarTab(RefVarTab(base->obj->type->vtab), RefObj(base->obj));
	par->expr = Parse_block(io, EOF);
	PopVarTab();
	io_close(io);

	edb = edb_share(base);
	edb->read = expr_read;
	edb->ipar = rd_init(&expr_reftype, par);
	return edb;
}


EDBFilter EDBFilter_expr = EDB_FILTER(NULL,
	"expr", "[commands]", expr_create, NULL,
	":*:apply commands to record"
	":de:Befehle auf Datensatz anwenden"
);
