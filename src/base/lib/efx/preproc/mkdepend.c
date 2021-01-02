/*
Eingabe - Preprozessor

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <EFEU/MakeDepend.h>
#include <ctype.h>

void Func_target (EfiFunc *func, void *rval, void **arg)
{
	AddTarget(Val_str(arg[0]));
}

void Func_depend (EfiFunc *func, void *rval, void **arg)
{
	AddDepend(Val_str(arg[0]));
}

void Func_makedepend (EfiFunc *func, void *rval, void **arg)
{
	MakeDependList(Val_io(arg[1]), Val_str(arg[0]));
}

static EfiObjList *make_list (VecBuf *buf)
{
	EfiObjList *list, **ptr;
	int n;
	char **dep;

	list = NULL;
	ptr = &list;

	for (n = buf->used, dep = buf->data; n-- > 0; dep++)
	{
		*ptr = NewObjList(str2Obj(mstrcpy(*dep)));
		ptr = &(*ptr)->next;
	}

	return list;
}

void Func_dependlist (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = make_list(&DependList);
}

void Func_targetlist (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = make_list(&TargetList);
}
