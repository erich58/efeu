/*
Hilfsfunktionen für Datenbankdaten

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
#include <EFEU/cmdsetup.h>
#include <EFEU/DBData.h>

EfiType Type_DBFile = REF_TYPE("DBFile", DBFile);

static EfiObj *m_recl (const EfiObj *base, void *data)
{
	DBFile *ptr = base ? Val_ptr(base->data) : NULL;
	int recl = ptr ? ptr->data.recl : 0;
	return int2Obj(recl);
}

static EfiObj *m_dim (const EfiObj *base, void *data)
{
	DBFile *ptr = base ? Val_ptr(base->data) : NULL;
	int dim = ptr ? ptr->data.dim : 0;
	return int2Obj(dim);
}

static EfiMember m_tab[] = {
	{ "recl", &Type_int, m_recl, NULL },
	{ "dim", &Type_int, m_dim, NULL },
};

/*	Datenbankdatei öffnen
*/

static void f_ebcdic (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_refer(Val_ptr(arg[0]));
	int mode = Val_bool(arg[2]) ? DBFILE_CONV : DBFILE_EBCDIC;
	Val_ptr(rval) = DBFile_open(io, mode, Val_int(arg[1]), NULL);
}

static void f_febcdic (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_fopen(Val_ptr(arg[0]), "rdz");
	int mode = Val_bool(arg[2]) ? DBFILE_CONV : DBFILE_EBCDIC;
	Val_ptr(rval) = DBFile_open(io, mode, Val_int(arg[1]), NULL);
}

static void f_text (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_refer(Val_ptr(arg[0]));
	Val_ptr(rval) = DBFile_open(io, DBFILE_TEXT,
		Val_int(arg[2]), Val_str(arg[1]));
}

static void f_ftext (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_fopen(Val_ptr(arg[0]), "rdz");
	Val_ptr(rval) = DBFile_open(io, DBFILE_TEXT,
		Val_int(arg[2]), Val_str(arg[1]));
}

/*	Datensatz laden und darstellen
*/

static void f_next (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = DBFile_next(Val_ptr(arg[0]));
}

static void f_show (EfiFunc *func, void *rval, void **arg)
{
	DBFile_show(Val_ptr(arg[0]), Val_io(arg[1]));
}

static void f_sync (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);

	if	(file)
		DBData_sync(&file->data, Val_int(arg[1]), Val_char(arg[2]));
}

static void f_field (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	int i = Val_int(arg[1]);

	if	(!file)	
		Val_str(rval) = NULL;
	else if	(i == 0)
		Val_str(rval) = rd_ident(file->io);
	else
		Val_str(rval) = mstrcpy(DBData_field(&file->data, i));
}

static void f_list (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	EfiObjList *list, **ptr;

	list = NULL;
	ptr = &list;

	if	(file && file->data.dim)
	{
		int i;

		for (i = 0; i < file->data.dim; i++)
		{
			*ptr = NewObjList(str2Obj(mstrcpy(file->data.tab[i])));
			ptr = &(*ptr)->next;
		}
	}

	Val_list(rval) = list;
}


/*	Wertabfragen
*/

#define	POS	Val_int(arg[1])
#define	LEN	Val_int(arg[2])

static void f_packed (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_int(rval) = (file && file->data.recl) ?
		db_pval(file->data.buf, POS, LEN) : 0;
	Val_int(rval) = file ? DBData_packed(&file->data, POS, LEN) : 0;
}

static void f_bcdval (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_int(rval) = file ? DBData_bcdval(&file->data, POS, LEN) : 0;
}

static void f_binary (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_int(rval) = file ? DBData_binary(&file->data, POS, LEN) : 0;
}

static void f_base37 (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_int(rval) = file ? DBData_base37(&file->data, POS, LEN) : 0;
}

static void f_int (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_int(rval) = file ? DBData_uint(&file->data, POS, LEN) : 0;
}

static void f_uint (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_uint(rval) = file ? DBData_uint(&file->data, POS, LEN) : 0;
}

static void f_str (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_str(rval) = file ? mstrcpy(DBData_str(&file->data,
		POS, LEN)) : NULL;
}

static void f_char (EfiFunc *func, void *rval, void **arg)
{
	DBFile *file = Val_ptr(arg[0]);
	Val_char(rval) = file ? DBData_char(&file->data, POS, LEN) : 0;
}

/*	Zahlen zur Basis 37
*/

static void f_a37l (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = a37l(Val_str(arg[0]));
}

static void f_l37a (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = l37a(Val_int(arg[0]));
}

/*	Funktionstabelle und Initialisierung
*/

static EfiFuncDef fdef[] = {
	{ 0, &Type_int, "a37l (str)", f_a37l },
	{ 0, &Type_str, "l37a (int)", f_l37a },
	{ FUNC_VIRTUAL, &Type_DBFile,
		"DBFile (IO io, str delim = NULL, int sync = 0)", f_text },
	{ FUNC_VIRTUAL, &Type_DBFile,
		"DBFile (str name, str delim = NULL, int sync = 0)", f_ftext },
	{ FUNC_VIRTUAL, &Type_DBFile,
		"DBFile (IO io, int recl, bool flag = false)", f_ebcdic },
	{ FUNC_VIRTUAL, &Type_DBFile,
		"DBFile (str name, int recl, bool flag = false)", f_febcdic },
	{ FUNC_RESTRICTED, &Type_list, "DBFile ()", f_list },
	{ 0, &Type_int, "DBFile::next ()", f_next },
	{ 0, &Type_void, "DBFile::sync (int recl, char c = ' ')", f_sync },
	{ 0, &Type_void, "DBFile::show (IO io = iostd)", f_show },
	{ 0, &Type_uint, "DBFile::packed (int pos, int len)", f_packed },
	{ 0, &Type_uint, "DBFile::bcdval (int pos, int len)", f_bcdval },
	{ 0, &Type_uint, "DBFile::binary (int pos, int len)", f_binary },
	{ 0, &Type_uint, "DBFile::cval (int pos, int len)", f_uint },
	{ 0, &Type_uint, "DBFile::uint (int pos, int len)", f_uint },
	{ 0, &Type_int, "DBFile::base37 (int pos, int len)", f_base37 },
	{ 0, &Type_int, "DBFile::int (int pos, int len)", f_int },
	{ 0, &Type_str, "DBFile::str (int pos, int len)", f_str },
	{ 0, &Type_char, "DBFile::char (int pos, int len)", f_char },
	{ FUNC_VIRTUAL, &Type_str, "operator[] (DBFile dbf, int pos)", f_field },
};

void CmdSetup_dbutil(void)
{
	AddType(&Type_DBFile);
	AddEfiMember(Type_DBFile.vtab, m_tab, tabsize(m_tab));
	AddFuncDef(fdef, tabsize(fdef));
}
