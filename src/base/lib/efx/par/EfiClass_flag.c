/*
Testklassifikation

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

#include <EFEU/EfiClass.h>

typedef struct {
	REFVAR;
	EfiObj *expr;
} TPar;

static void TPar_free (void *data)
{
	TPar *par = data;
	UnrefObj(par->expr);
	memfree(par);
}

static RefType TPar_reftype = REFTYPE_INIT("TestPar", NULL, TPar_free);

static int test_update (const EfiObj *obj, void *par)
{
	if	(par)
	{
		TPar *bf = par;
		int rval;

		PushVarTab(RefVarTab(obj->type->vtab), RefObj(obj));
		rval = Obj2bool(RefObj(bf->expr));
		PopVarTab();
		return rval;
	}
	else	return 1;
}

static void create_flag (EfiClassArg *def, const EfiType *type,
	const char *opt, const char *arg, void *par)
{
	EfiObj *expr;
	char *name;
	char *p;

 	def->type = NewEnumType(NULL, 1);
	name = mstrcut(arg, &p, " \t\n:", 1);
 	AddEnumKey(def->type, name ? name : mstrcpy("TRUE"), 
 		mstrcpy(p ? p : (opt ? opt : "true")), 1);
 	def->type = AddEnumType(def->type);
	def->update = test_update;

	if	(opt)
	{
		IO *io = io_cstr(opt);
		expr = Parse_cmd(io);
		io_close(io);
	}
	else	expr = NULL;

	if	(expr)
	{
		TPar *par = memalloc(sizeof *par);
		par->expr = expr;
		def->par = rd_init(&TPar_reftype, par);
	}
	else	def->par = NULL;
}

EfiClass EfiClass_flag = EFI_CLASS(NULL,
	"flag", "[expr]=label desc", create_flag, NULL,
	"flag expression"
);
