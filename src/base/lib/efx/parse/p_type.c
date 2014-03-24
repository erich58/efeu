/*
Datentype bestimmen

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <EFEU/parsedef.h>

#define	INC_PFX	"types"

EfiType *XGetType (const char *p)
{
	EfiType *type;
	char *fname;

	if	((type = GetType(p)))
		return type;

	if	((fname = fsearch(IncPath, INC_PFX, p, "so")))
	{
		char *setup = mstrpaste("_", p, "setup");
		loadlib(fname, setup);
		memfree(setup);
		memfree(fname);
	}
	else if	((fname = fsearch(IncPath, INC_PFX, p, "hdr")))
	{
		Efi *interp = Efi_ptr(NULL);
		IO *io = io_cmdpreproc(io_fileopen(fname, "rd"));

		PushVarTab(RefVarTab(GlobalVar), NULL);
		EfiSrc_hdr(interp, fname);
		CmdEval(io, NULL);
		EfiSrc_pop(interp);
		PopVarTab();
		io_close(io);
		memfree(fname);
	}

	return GetType(p);
}

static EfiType *get_type (IO *io)
{
	EfiParseDef *parse;
	EfiType *type;
	void *p;

	type = NULL;

	if	(!io_scan(io, SCAN_NAME, &p))
		return NULL;

	if	((parse = GetParseDef(p)) != NULL)
	{
		EfiObj *obj = (*parse->func)(io, parse->data);

		if	(obj && obj->type == &Type_type)
			type = Val_type(obj->data);

		UnrefObj(obj);
		return type;
	}

	if	((type = XGetType(p)))
		return type;

	io_error(io, "[efmain:128]", "s", p);
	return NULL;
}

EfiType *Parse_type(IO *io, EfiType *type)
{
	EfiObj *obj;

	if	(type == NULL)
	{
		if	(!(type = get_type(io)))
		{
			io_error(io, "[efmain:123]", NULL);
			return NULL;
		}
	}

	EfiType_version(type, io);
	
	while (io_eat(io, " \t") == '[')
	{
		io_getc(io);

		if	(io_eat(io, " \t") == ']')
		{
			io_getc(io);
			type = NewVecType(type, 0);
		}
		else
		{
			obj = EvalObj(Parse_index(io), &Type_int);

			if	(obj == NULL)	return NULL;

			type = NewVecType(type, Val_int(obj->data));
			UnrefObj(obj);
		}
	}

	return type;
}

EfiType *str2Type (const char *def)
{
	IO *io = io_cstr(def);
	EfiType *type = Parse_type(io, NULL);
	io_close(io);
	return type;
}
