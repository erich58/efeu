/*
Datenbankstruktur

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/vecbuf.h>
#include <EFEU/database.h>
#include <EFEU/stdtype.h>

#define	DB_BLKSIZE	128	/* Blockgröße für Zahl der Datenbankeinträge */

static char *db_ident (const void *data)
{
	const EfiDB *db = data;
	return msprintf("%s[%d]", db->type->name, db->buf.used);
}

static void db_clean (void *data)
{
	EfiDB *db = data;
	CleanVecData(db->type, db->buf.used, db->buf.data, 1);
	vb_free(&db->buf);
	memfree(db);
}


static RefType DB_reftype = REFTYPE_INIT("DB", db_ident, db_clean);


/*	Datentype
*/

EfiType Type_DB = REF_TYPE("DataBase", EfiDB *);

#define	Val_DB(x)	((EfiDB **) x)[0]
#define	Ref_DB(x)	rd_refer(Val_DB(x))


/*	Datenbank generieren
*/

EfiDB *DB_create(EfiType *type, size_t blk)
{
	EfiDB *db;

	if	(type == NULL)	return NULL;

	db = memalloc(sizeof(EfiDB));
	db->type = type;
	db->cmp = NULL;
	db->buf.data = NULL;
	db->buf.blksize = blk ? blk : DB_BLKSIZE;
	db->buf.elsize = type->size;
	db->buf.size = 0;
	db->buf.used = 0;
	return rd_init(&DB_reftype, db);
}


#define	DBRVAL	((EfiDB **) rval)[0]

static void f_db_create(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db;
	db = DB_create(Val_type(arg[0]), Val_int(arg[2]));

	if	(db)
		db->cmp = GetFunc(&Type_int, Val_vfunc(arg[1]), 2, db->type, 0, db->type, 0);
	DBRVAL = db;
}

static void f_db_fload(EfiFunc *func, void *rval, void **arg)
{
	IO *io;

	io = io_lnum(io_fileopen(Val_str(arg[1]), "r"));
	DB_load(io, Val_DB(arg[0]), Val_obj(arg[2]));
	io_close(io);
	DBRVAL = Ref_DB(arg[0]);
}

static void f_db_load(EfiFunc *func, void *rval, void **arg)
{
	IO *io;

	io = io_lnum(io_refer(Val_io(arg[1])));
	DB_load(io, Val_DB(arg[0]), Val_obj(arg[2]));
	io_close(io);
	DBRVAL = Ref_DB(arg[0]);
}


static void f_db_insert(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db;
	void *ptr;
	size_t pos, dim;

	db = Ref_DB(arg[0]);
	pos = Val_int(arg[1]);
	dim = Val_int(arg[2]);

	if	(db && (ptr = vb_insert(&db->buf, pos, dim)))
		memset(ptr, 0, dim * db->type->size);

	DBRVAL = db;
}


static void f_db_delete(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db;
	char *ptr;
	size_t pos, dim;

	db = Ref_DB(arg[0]);
	pos = Val_int(arg[1]);
	dim = Val_int(arg[2]);

	if	(db && (ptr = vb_delete(&db->buf, pos, dim)))
	{
		while (dim-- != 0)
		{
			CleanData(db->type, ptr, 0);
			ptr += db->type->size;
		}
	}

	DBRVAL = db;
}

static void f_db_clean (EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db = Ref_DB(arg[0]);

	if	(db && db->buf.used)
	{
		char *ptr = db->buf.data;
		size_t n = db->buf.used;

		while (n-- > 0)
		{
			CleanData(db->type, ptr, 0);
			ptr += db->type->size;
		}

		db->buf.used = 0;
	}

	DBRVAL = db;
}


static EfiObj *db_search(EfiDB *db, EfiObj *obj, int flag)
{
	char *p1, *p2;

	if	(db == NULL)	return NULL;

	obj = EvalObj(RefObj(obj), db->type);

	if	(obj == NULL)	return NULL;

	switch (flag)
	{
	case VB_DELETE:

		p1 = memalloc(db->type->size);
		memcpy(p1, obj->data, db->type->size);
		break;

	case VB_REPLACE:
	case VB_ENTER:

		p1 = memalloc(db->type->size);
		CopyData(db->type, p1, obj->data);
		break;

	default:

		p1 = obj->data;
	}

	p2 = DB_search(db, p1, flag);
	UnrefObj(obj);
	obj = NULL;

	switch (flag)
	{
	case VB_SEARCH:
	case VB_ENTER:

		return p2 ? LvalObj(&Lval_ref, db->type, db, p2)
			: ptr2Obj(NULL);

	case VB_DELETE:
	case VB_REPLACE:

		obj = p2 ? ConstObj(db->type, p2) : ptr2Obj(NULL);
		break;
	}

	if	(p2 != NULL)
	{
		CleanData(db->type, p2, 0);
		memfree(p2);
	}

	if	(p1 != p2)
		memfree(p1);

	return obj;
}


static void f_db_getindex(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db;
	EfiObj *obj;
	char *p;

	db = Val_DB(arg[0]);

	if	(db == NULL)
	{
		Val_int(rval) = 0;
		return;
	}

	obj = EvalObj(RefObj(arg[1]), db->type);
	p = obj ? DB_search(db, obj->data, VB_SEARCH) : NULL;
	UnrefObj(obj);

	Val_int(rval) = p ?  (p - (char *) db->buf.data) /
		db->type->size : db->buf.used;
}

static void f_db_find(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = db_search(Val_DB(arg[0]), arg[1], VB_SEARCH);
}

static void f_db_get(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = db_search(Val_DB(arg[0]), arg[1], VB_ENTER);
}

static void f_db_replace(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = db_search(Val_DB(arg[0]), arg[1], VB_REPLACE);
}

static void f_db_remove(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = db_search(Val_DB(arg[0]), arg[1], VB_DELETE);
}


static void f_db_fsave(EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	EfiDB *db;

	io = io_fileopen(Val_str(arg[1]), "w");
	db = Ref_DB(arg[0]);
	DB_save(io, db, Val_bool(arg[2]),
		Val_vfunc(arg[3]), Val_str(arg[4]));
	io_close(io);
	DBRVAL = db;
}

static void f_db_save(EfiFunc *func, void *rval, void **arg)
{
	register EfiDB *db = Ref_DB(arg[0]);
	DB_save(Val_io(arg[1]), db, Val_bool(arg[2]),
		Val_vfunc(arg[3]), Val_str(arg[4]));
	DBRVAL = db;
}

static void f_db_index(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db;
	int n;

	db = Val_DB(arg[0]);
	n = Val_int(arg[1]);

	if	(db == NULL || n < 0 || n >= db->buf.used)
	{
		log_note(NULL, "[efmain:35]", NULL);
		return;
	}

	Val_obj(rval) = LvalObj(&Lval_ref, db->type, db,
		(char *) db->buf.data + n * db->type->size);
}

/*
static void f_db_print(EfiFunc *func, void *rval, void **arg)
{
	char *p;

	p = db_ident(Val_DB(arg[1]));
	Val_int(rval) = io_puts(p, Val_io(arg[0]));
	memfree(p);
}
*/


static void f_db_sort(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *db;

	if	((db = Ref_DB(arg[0])) == NULL)
		return;

	db->cmp = GetFunc(&Type_int, Val_vfunc(arg[1]), 2, db->type, 0, db->type, 0);
	DB_sort(db);
	DBRVAL = db;
}


static void f_db_dim(EfiFunc *func, void *rval, void **arg)
{
	EfiDB *x = Val_DB(arg[0]);
	Val_int(rval) = x ? x->buf.used : 0;
}


/*	Konvertierung in Liste
*/

static void DB2List (EfiFunc *func, void *rval, void **arg)
{
	int i;
	EfiObjList *list, **ptr;
	char *data;
	EfiDB *db;

	list = NULL;
	ptr = &list;
	db = Val_DB(arg[0]);

	if	(db != NULL)
	{
		data = db->buf.data;

		for (i = 0; i < db->buf.used; i++)
		{
			*ptr = NewObjList(LvalObj(&Lval_ref, db->type, db, data));
			data += db->type->size;
			ptr = &(*ptr)->next;
		}
	}

	Val_list(rval) = list;
}

		
/*	Initialisieren
*/

static EfiFuncDef db_func[] = {
	{ FUNC_RESTRICTED, &Type_list, "DataBase ()", DB2List },
	{ 0, &Type_DB, "DataBase (Type_t, VirFunc cmp = NULL, int bs = 0)",
		f_db_create },
	{ 0, &Type_DB, "DataBase::load (IO io, Expr_t expr = NULL)",
		f_db_load },
	{ 0, &Type_DB, "DataBase::fload (str name, Expr_t expr = NULL)",
		f_db_fload },
	{ 0, &Type_DB, "DataBase::save (IO io, bool mode = false, "
		"VirFunc test = NULL, str list = NULL)", f_db_save },
	{ 0, &Type_DB, "DataBase::fsave (str name, bool = false, "
		"VirFunc test = NULL, str list = NULL)", f_db_fsave },
	{ 0, &Type_obj, "DataBase::operator+= (.)", f_db_replace },
	{ 0, &Type_obj, "DataBase::operator-= (.)", f_db_remove },
	{ 0, &Type_obj, "DataBase::find (.)", f_db_find },
	{ 0, &Type_obj, "DataBase::get (.)", f_db_get },
	{ 0, &Type_obj, "DataBase::replace (.)", f_db_replace },
	{ 0, &Type_obj, "DataBase::remove (.)", f_db_remove },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (restricted DataBase, "
		"restricted int n)", f_db_index },
	{ FUNC_VIRTUAL, &Type_obj,
		"operator[] (restricted DataBase, .)", f_db_find },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (restricted DataBase, .)",
		f_db_get },
	{ 0, &Type_int, "DataBase::index (.)", f_db_getindex },
	{ 0, &Type_DB, "DataBase::insert (int, int = 1)", f_db_insert },
	{ 0, &Type_DB, "DataBase::delete (int, int = 1)", f_db_delete },
	{ 0, &Type_DB, "DataBase::clean ()", f_db_clean },
	{ 0, &Type_DB, "DataBase::sort (VirFunc func)", f_db_sort },
	{ FUNC_VIRTUAL, &Type_int, "dim(DataBase)", f_db_dim },
};


void SetupDataBase(void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;

	AddType(&Type_DB);
	AddFuncDef(db_func, tabsize(db_func));
}
