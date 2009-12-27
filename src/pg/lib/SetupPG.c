/*
:*:	seting up esh-interface to PostgreSQL
:de:	Esh-Interface zu PostgreSQL initialisieren

$Copyright (C) 2001 Erich Frühstück
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

#include <DB/PG.h>
#include <EFEU/stdtype.h>

Type_t Type_PG = REF_TYPE("PG", PG_t *);

static Obj_t *pg_status (const Var_t *var, const Obj_t *obj)
{
	return str2Obj(mstrcpy(PG_status(obj ? Val_ptr(obj->data) : NULL)));
}

static Obj_t *pg_nfields (const Var_t *var, const Obj_t *obj)
{
	return int2Obj(PG_nfields(obj ? Val_ptr(obj->data) : NULL));
}

static Obj_t *pg_ntuples (const Var_t *var, const Obj_t *obj)
{
	return int2Obj(PG_ntuples(obj ? Val_ptr(obj->data) : NULL));
}

static Obj_t *pg_label (const Var_t *var, const Obj_t *obj)
{
	PG_t *pg;
	ObjList_t *list, **ptr;
	int j, k;
	
	if	(obj == NULL)	return NULL;

	pg = Val_ptr(obj->data);

	if	(pg == NULL || pg->res == NULL ||
		PQresultStatus(pg->res) != PGRES_TUPLES_OK)
		return NULL;

	k = PQnfields(pg->res);
	list = NULL;
	ptr = &list;

	for (j = 0; j < k; j++)
	{
		*ptr = NewObjList(str2Obj(mstrcpy(PQfname(pg->res, j))));
		ptr = &(*ptr)->next;
	}

	return Obj_list(list);
}

static VarDef_t pg_var[] = {
	{ "expandlim", &Type_int, &PG_expandlim, NULL },
};

static MemberDef_t pg_memb[] = {
	{ "status", &Type_str, pg_status, NULL },
	{ "nfields", &Type_int, pg_nfields, NULL },
	{ "ntuples", &Type_int, pg_ntuples, NULL },
	{ "label", &Type_list, pg_label, NULL },
};

/*	Funktionen
*/

static void f_print (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PG_print(Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void f_conn (Func_t *func, void *rval, void **arg)
{
	char *def = Val_str(arg[0]);

	if	(strchr(def, '=') == NULL)
	{
		char *p = mstrpaste("=", "dbname", def);
		Val_ptr(rval) = PG(p);
		memfree(p);
	}
	else	Val_ptr(rval) = PG(def);
}

static void f_vconn (Func_t *func, void *rval, void **arg)
{
	strbuf_t *sb;
	ObjList_t *l;
	char *s;

	s = Val_str(arg[0]);

	if	(s == NULL)
	{
		Val_ptr(rval) = NULL;
		return;
	}

	sb = new_strbuf(0);
	sb_printf(sb, "dbname=%s", s);

	for (l = Val_list(arg[1]); l != NULL; l = l->next)
	{
		s = Obj2str(RefObj(l->obj));
		sb_putc(' ', sb);
		sb_puts(s, sb);
		memfree(s);
	}

	sb_putc(0, sb);
	Val_ptr(rval) = PG(sb->data);
	del_strbuf(sb);
}

static void do_exec (void *rval, void **arg, ExecStatusType stat)
{
	ObjList_t *l;
	char *cmd;
	int i;
	
	for (i = 1, l = Val_list(arg[2]); l != NULL; i++, l = l->next)
		reg_set(i, Obj2str(RefObj(l->obj)));

	cmd = parsub(Val_str(arg[1]));
	Val_bool(rval) = PG_exec(Val_ptr(arg[0]), cmd, stat);
	memfree(cmd);
}

static void f_exec (Func_t *func, void *rval, void **arg)
{
	do_exec(rval, arg, PGRES_EMPTY_QUERY);
}

static void f_query (Func_t *func, void *rval, void **arg)
{
	do_exec(rval, arg, PGRES_TUPLES_OK);
}

static void f_command (Func_t *func, void *rval, void **arg)
{
	do_exec(rval, arg, PGRES_COMMAND_OK);
}

static void f_fname (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(PG_fname(Val_ptr(arg[0]), Val_int(arg[1])));
}

static void f_value (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(PG_value(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2])));
}

static ObjList_t *data_list (PG_t *pg, int tuple, int nfields)
{
	ObjList_t *list, **ptr;
	char *p;
	int j;

	list = NULL;
	ptr = &list;

	for (j = 0; j < nfields; j++)
	{
		p = PQgetvalue(pg->res, tuple, j);
		*ptr = NewObjList(str2Obj(mstrcpy(p)));
		ptr = &(*ptr)->next;
	}

	return list;
}


static void f_data (Func_t *func, void *rval, void **arg)
{
	PG_t *pg = Val_ptr(arg[0]);
	int i, n, k;

	if	(pg && pg->res)
	{
		n = PQntuples(pg->res);
		k = PQnfields(pg->res);
	}
	else	n = k = 0;

	i = Val_int(arg[1]);
	Val_ptr(rval) = (i < n) ? data_list(pg, i, k) : NULL;
}

static void f_pg2list (Func_t *func, void *rval, void **arg)
{
	PG_t *pg = Val_ptr(arg[0]);
	ObjList_t **ptr;
	int i, n, k;

	ptr = rval;
	*ptr = NULL;

	if	(pg && pg->res)
	{
		n = PQntuples(pg->res);
		k = PQnfields(pg->res);
	}
	else	n = k = 0;

	for (i = 0; i < n; i++)
	{
		*ptr = NewObjList(Obj_list(data_list(pg, i, k)));
		ptr = &(*ptr)->next;
	}
}

static void f_open (Func_t *func, void *rval, void **arg)
{
	Val_io(rval) = PG_open(rd_refer(Val_ptr(arg[0])),
		Val_str(arg[1]), Val_str(arg[2]));
}

static void f_mdmat (Func_t *func, void *rval, void **arg)
{
	Val_mdmat(rval) = PG_mdmat(Val_ptr(arg[0]),
		Val_type(arg[1]), Val_str(arg[2]), Val_str(arg[3]));
}

static void f_close (Func_t *func, void *rval, void **arg)
{
	rd_deref(Val_ptr(arg[0]));
	Val_ptr(arg[0]) = NULL;
}

static FuncDef_t pg_func[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, PG)", f_print },
	{ 0, &Type_PG, "PG (str db)", f_conn },
	{ 0, &Type_PG, "PG (str db, ...)", f_vconn },
	{ 0, &Type_bool, "PG::exec (str cmd, ...)", f_exec },
	{ 0, &Type_bool, "PG::query (str cmd, ...)", f_query },
	{ 0, &Type_bool, "PG::command (str cmd, ...)", f_command },
	{ 0, &Type_str, "PG::fname (int field)", f_fname },
	{ 0, &Type_str, "PG::value (int tuple, int field)", f_value },
	{ 0, &Type_io, "PG::open (str name, str mode)", f_open },
	{ 0, &Type_mdmat, "PG::mdmat (Type_t type, str val, str axis)",
		f_mdmat },
	{ FUNC_VIRTUAL, &Type_list, "operator[] (PG, int)", f_data },
	{ 0, &Type_list, "PG ()", f_pg2list },
	{ FUNC_VIRTUAL, &Type_void, "close (PG & pg)", f_close },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter mit Schnittstellenfunktionen
zum PostgreSQL Datenbankserver. Diese Funktion wird implizit beim
Laden der gemeinsam genutzen Programmbibliothek aufgerufen.
:*:
The function |$1| expands the esh-Interpreter with the frontend
to PostgreSQL. This function is implicitly called on loading the
shared library.
*/

void SetupPG (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	SetupStd();
	SetupMdMat();
	AddType(&Type_PG);
	AddVarDef(Type_PG.vtab, pg_var, tabsize(pg_var));
	AddMember(Type_PG.vtab, pg_memb, tabsize(pg_memb));
	AddFuncDef(pg_func, tabsize(pg_func));
}

void _init (void)
{
	SetupPG();
}

/*
$SeeAlso
\mref{refdata(3)},
\mref{PG(3)},
\mref{PG_exec(3)},
\mref{PG_open(3)},
\mref{PG_query(3)},
\mref{PG(7)}.\br
PostgreSQL Programmer's Guide.
*/
