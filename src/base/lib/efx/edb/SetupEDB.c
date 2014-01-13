/*	Initialisierung von EDB
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/


#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
#include <EFEU/stdtype.h>
#include <EFEU/CmpDef.h>
#include <EFEU/EDBFilter.h>
#include <EFEU/EDBJoin.h>

#define	EDB_DUMMY	1	/* Expermimentell */

#define	INFO_NAME	\
	"EDB"
#define	INFO_HEAD	\
	":*:EDB interface" \
	":de:EDB-Datenschnittstelle"

EfiType Type_EDB = REF_TYPE("EDB", EDB *);

static EfiObj *get_desc (const EfiObj *base, void *data)
{
	EDB *edb = base ? Val_ptr(base->data) : NULL;
	return edb ? LvalObj(&Lval_ref, &Type_str, edb, &edb->desc) : NULL;
}

static EfiObj *get_head (const EfiObj *base, void *data)
{
	EDB *edb = base ? Val_ptr(base->data) : NULL;
	return edb ? LvalObj(&Lval_ref, &Type_str, edb, &edb->head) : NULL;
}

static EfiObj *get_type (const EfiObj *base, void *data)
{
	EDB *edb = base ? Val_ptr(base->data) : NULL;
	return edb && edb->obj ? ConstObj(&Type_type, &edb->obj->type) : NULL;
}

static EfiObj *get_data (const EfiObj *base, void *data)
{
	EDB *edb = base ? Val_ptr(base->data) : NULL;
	return edb ? RefObj(edb->obj) : NULL;
}

static EfiMember edb_var[] = {
	{ "head", &Type_str, get_head, NULL,
		":*:header of data file\n"
		":de:Header für Datenverarbeitung\n" },
	{ "desc", &Type_str, get_desc, NULL,
		":*:description of data file\n"
		":de:Beschreibung der Daten\n" },
	{ "type", &Type_type, get_type, NULL },
	{ "data", NULL, get_data, NULL },
};

#if	EDB_DUMMY
static void f_dummy (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_alloc(NULL, NULL);
}
#endif

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_alloc(RefObj(arg[0]),
		mstrcpy(Val_str(arg[1])));
}

static void f_tcreate (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_alloc(LvalObj(NULL, Val_type(arg[0])),
		mstrcpy(Val_str(arg[1])));
}

static void f_xcreate (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_alloc(LvalObj(NULL, Val_type(arg[0])), NULL);
}

static void f_open (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_open(io_refer(Val_io(arg[0])));
}

static void f_filter (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_filter(rd_refer(Val_ptr(arg[0])), Val_str(arg[1]));
}

static void f_trans (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_trans(rd_refer(Val_ptr(arg[0])), Val_str(arg[1]));
}

static size_t mktab (EfiObjList *list, EDB ***ptr)
{
	int n = ObjListLen(list);

	*ptr = memalloc(n * sizeof **ptr);

	for (n = 0; list; list = list->next)
		(*ptr)[n++] = rd_refer(Val_ptr(list->obj->data));

	return n;
}

static void f_edbcat (EfiFunc *func, void *rval, void **arg)
{
	EDB **tab;
	size_t n;

	n = mktab(Val_list(arg[0]), &tab);
	Val_ptr(rval) = edb_cat(tab, n);
	memfree(tab);
}

static void f_edbmerge (EfiFunc *func, void *rval, void **arg)
{
	EDB **tab;
	CmpDef *comp;
	char *cdef;
	size_t n;

	if	((n = mktab(Val_list(arg[2]), &tab)))
	{
		cdef = Val_str(arg[0]);
		comp = cdef ? cmp_create(tab[0]->obj->type, cdef, NULL) : NULL;

		if	(Val_bool(arg[1]))
		{
			int i;

			for (i = 0; i < n; i++)
				tab[i] = edb_sort(tab[i], rd_refer(comp));
		}

		Val_ptr(rval) = edb_merge(comp, tab, n);
		memfree(tab);
	}	
	else	Val_ptr(rval) = NULL;
}

static void f_edbopen (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_filter(
		edb_fopen(NULL, Val_str(arg[0])), Val_str(arg[1]));
}

static void f_edbjoin (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = edb_join(rd_refer(Val_ptr(arg[0])),
		rd_refer(Val_ptr(arg[1])),
		Val_str(arg[2]));
}

static void f_read (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = edb_read(Val_ptr(arg[0])) != NULL;
}

static void f_unread (EfiFunc *func, void *rval, void **arg)
{
	edb_unread(Val_ptr(arg[0]));
}

static void f_write (EfiFunc *func, void *rval, void **arg)
{
	edb_write(Val_ptr(arg[0]));
}

static void f_datain (EfiFunc *func, void *rval, void **arg)
{
	EDB *edb = Val_ptr(arg[0]);
	edb_closein(edb);
	edb_data(edb, io_refer(Val_io(arg[1])), Val_str(arg[2]));
}

static void f_out (EfiFunc *func, void *rval, void **arg)
{
	edb_out(Val_ptr(arg[0]), io_refer(Val_io(arg[1])), Val_str(arg[2]));
}

static void f_fout (EfiFunc *func, void *rval, void **arg)
{
	edb_fout(Val_ptr(arg[0]), Val_str(arg[1]), Val_str(arg[2]));
}

static void f_copy (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = edb_copy(Val_ptr(arg[0]), Val_int(arg[1]));
}

static void f_closein (EfiFunc *func, void *rval, void **arg)
{
	edb_closein(Val_ptr(arg[0]));
}

static void f_closeout (EfiFunc *func, void *rval, void **arg)
{
	edb_closeout(Val_ptr(arg[0]));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	EfiVec_load(Val_ptr(arg[0]), Val_str(arg[1]),
		NULL, Val_str(arg[2]));
}

static void f_xload (EfiFunc *func, void *rval, void **arg)
{
	EfiVec_load(Val_ptr(arg[0]), Val_str(arg[1]),
		Val_str(arg[2]), Val_str(arg[3]));
}

static void f_save (EfiFunc *func, void *rval, void **arg)
{
	EfiVec_save(Val_ptr(arg[0]), Val_str(arg[1]),
		Val_str(arg[2]), Val_str(arg[3]));
}

static EfiFuncDef edb_func[] = {
#if	EDB_DUMMY
	{ 0, &Type_EDB, "EDB (void)", f_dummy },
#endif
	{ 0, &Type_EDB, "EDB (. & obj, str desc = NULL)", f_create },
	{ 0, &Type_EDB, "EDB (restricted Type_t type, str desc = NULL)",
		f_tcreate },
	{ 0, &Type_EDB, "Type_t ()", f_xcreate },
	{ 0, &Type_EDB, "EDB (restricted IO io)", f_open },
	{ 0, &Type_EDB, "EDB::filter (str list)", f_filter },
	{ 0, &Type_EDB, "EDB::trans (str def)", f_trans },
	{ 0, &Type_EDB, "edbcat (...)", f_edbcat },
	{ 0, &Type_EDB, "edbopen (str fname, str filter = NULL)", f_edbopen },
	{ 0, &Type_EDB, "edbjoin (EDB a, EDB b, str par)", f_edbjoin },
	{ 0, &Type_EDB, "edbmerge (str cmp, bool sort, ...)", f_edbmerge },

	{ 0, &Type_bool, "EDB::read ()", f_read },
	{ 0, &Type_void, "EDB::unread ()", f_unread },
	{ 0, &Type_void, "EDB::write ()", f_write },
	{ 0, &Type_int, "EDB::copy (int lim)", f_copy },

	{ 0, &Type_void, "EDB::datain (restricted IO io, str mode = NULL)",
		f_datain },

	{ 0, &Type_void, "EDB::out (restricted IO io, str mode = NULL)",
		f_out },
	{ 0, &Type_void, "EDB::fout (str fname, str mode = NULL)", f_fout },
	
	{ 0, &Type_void, "EDB::closein ()", f_closein },
	{ 0, &Type_void, "EDB::closeout ()", f_closeout },

	{ 0, &Type_void, "EfiVec::load(str name, "
		"str filter = NULL)", f_load },
	{ 0, &Type_void, "EfiVec::xload(str name, "
		"str mode, str filter = NULL)", f_xload },
	{ 0, &Type_void, "EfiVec::save(str name, "
		"str mode = NULL, str desc = NULL)", f_save },
};


void SetupEDB (void)
{
	static int init_done = 0;
	InfoNode *info;

	if	(init_done)	return;

	init_done = 1;

	AddType(&Type_EDB);
	AddEfiMember(Type_EDB.vtab, edb_var, tabsize(edb_var));
	AddFuncDef(edb_func, tabsize(edb_func));

	info = AddInfo(NULL, INFO_NAME, INFO_HEAD, NULL, NULL);
	EDBFilterInfo(info);
	EDBPrintInfo(info);
	EDBMetaInfo(info);
	EDBJoinInfo(info);
}
