/*
Datei mapping

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

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/MapFile.h>
#include <errno.h>

EfiType Type_MapFile = REF_TYPE("MapFile", MapFile *);

#define	MAP(x)	((MapFile **) (x))[0]

static EfiObj *get_name (const EfiObj *base, void *data)
{
	MapFile *map = base ? Val_ptr(base->data) : NULL;
	return map ? str2Obj(mstrcpy(map->name)) : NULL;
}

static EfiObj *get_size (const EfiObj *base, void *data)
{
	MapFile *map = base ? Val_ptr(base->data) : NULL;
	uint64_t size = map ? map->size : 0;
	return ConstObj(&Type_uint64, &size);
}

static EfiMember var_map[] = {
	{ "name", &Type_str, get_name, NULL,
		":*:file name\n"
		":de:Dateiname\n" },
	{ "size", &Type_uint64, get_size, NULL,
		":*:file size\n"
		":de:Dateigröße\n" },
};

static void f_null (EfiFunc *func, void *rval, void **arg)
{
	MAP(rval) = NULL;
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	MAP(rval) = MapFile_open(Val_str(arg[0]));

	if	(!MAP(rval))
		dbg_note(NULL, "$!: MapFile($1%#s): $2\n", "ss",
			Val_str(arg[0]), strerror(errno));
}

static void map2io (EfiFunc *func, void *rval, void **arg)
{
	MapFile *map = Val_ptr(arg[0]);
	Val_io(rval) = map ? io_data(map, map->data, map->size) : NULL;
}

static void f_io (EfiFunc *func, void *rval, void **arg)
{
	MapFile *map = Val_ptr(arg[0]);
	Val_io(rval) = NULL;

	if	(map)
	{
		size_t off = *((uint64_t *) arg[1]);
		size_t len = *((uint64_t *) arg[2]);

		if	(off >= map->size)
			return;
		if	(len <= 0)	
			len = map->size - off;
		if	(off + len > map->size)
			return;

		Val_io(rval) = io_data(map, (char *) map->data + off, len);
	}
}

static EfiFuncDef fdef_MapFile[] = {
	{ 0, &Type_MapFile, "_Ptr_ ()", f_null },
	{ 0, &Type_MapFile, "MapFile (str path)", f_load },
	{ FUNC_RESTRICTED, &Type_io, "MapFile ()", map2io },
	{ 0, &Type_io, "IO (MapFile map, uint64_t off, uint64_t len = 0)",
		f_io },
};

void CmdSetup_MapFile(void)
{
	AddType(&Type_MapFile);
	AddFuncDef(fdef_MapFile, tabsize(fdef_MapFile));
	AddEfiMember(Type_MapFile.vtab, var_map, tabsize(var_map));
}
