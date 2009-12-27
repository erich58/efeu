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

static void test_create (EfiClassArg *def, const char *opt, const char *arg)
{
	EfiObj *expr;

 	def->type = NewEnumType(NULL, 1);
 	AddEnumKey(def->type, mstrcpy(arg ? arg : "TRUE"), 
 		mstrcpy(opt ? opt : "true"), 1);
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

EfiClass EfiClass_test = EFI_CLASS(NULL,
	"test", "[expr]=label", test_create,
	"test expression"
);
