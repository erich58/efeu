/*
EDB-Datenfiles auszählen

$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mstring.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/parsearg.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/CmdPar.h>
#include <EFEU/printobj.h>
#include <EFEU/EDB.h>
#include <EFEU/EfiCount.h>
#include <EFEU/Random.h>
#include <EFEU/Debug.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>
#include <Math/func.h>
#include <EFEU/StatData.h>
#include <ctype.h>

#define	VERSION		"1.0"
#define	KEY_MAGIC	"EDB"
#define	KEY_DATA	"@data"

double SampleWeight = 1.;

static void add_weight (MdCount *cnt, void *data, void *buf)
{
	Val_double(data) = SampleWeight;
}

static unsigned event = 0;

static int set_event (MdCount *cnt, void *buf, const void *data)
{
	Val_uint(buf) = ++event;
	return 1;
}

static void add_event (MdCount *cnt, void *data, void *buf)
{
	Val_uint(data) = Val_uint(buf);
}

static MdCount vtab[] = {
	{ "event", "unsigned", "event number", set_event, NULL, add_event },
	{ "weight", "double", "count weight", NULL, NULL, add_weight },
};


static int total_idx (MdClass *c, const void *data)
{
	return 0;
}

static Label total_tab[] = {
	{ "TOTAL", "Totality" },
};

static MdClass cltab[] = {
	{ "total", "Totality", tabsize(total_tab), total_tab, total_idx },
};

static void cnt_exit (EDB *edb, char *err)
{
	if	(err)
	{
		dbg_message(NULL, DBG_ERR, err, rd_ident(edb->ipar), "m",
			Type2str(edb->obj->type));
	}

	rd_deref(edb);
	exit(EXIT_SUCCESS);
}

static int cnt_counter = 0;
static EfiObj *rec_obj = NULL;
static EDB *rec_edb = NULL;
static EfiStruct *rec_var = NULL;

static void std_hook (MdCountList *clist, mdmat *md)
{
	cnt_counter++;
	md_count_add(clist, md->axis, md->data);
}

static char *rec_name (const char *name)
{
	StrBuf *sb;
	
	sb = sb_create(32);

	if	(isdigit((unsigned char) *name))
		sb_putc('_', sb);

	for (; *name; ++name)
		sb_putc((*name == '_' || isalnum((unsigned char) *name)) ?
			*name : '_', sb);

	return sb2str(sb);
}

static EfiType *rec_type (MdClass *cdef)
{
	EfiType *type;
	int i;

	if	(cdef->dim == 0)	return &Type_uint;

	type = NewEnumType(NULL, EnumTypeRecl(cdef->dim));

	for (i = 0; i < cdef->dim; i++)
		AddEnumKey(type, mstrcpy(cdef->label[i].name),
			mstrcpy(cdef->label[i].desc), i + 1);

	return AddEnumType(type);
}

static EDB *make_edb (mdmat *md)
{
	mdaxis *x;
	EfiStruct *cvar, **ptr;
	EfiObj *obj;
	EfiType *type;
	MdCntGrp *s;
	MdCountList *clist;

	cvar = NULL;
	ptr = &cvar;

	for (clist = md->x_priv; clist != NULL; clist = clist->next)
	{
		*ptr = NewEfiStruct(clist->type, NULL, 0);
		(*ptr)->name = rec_name(clist->cnt->name);
		(*ptr)->desc = mstrcpy(clist->cnt->desc);
		ptr = &(*ptr)->next;
	}

	rec_var = NULL;
	ptr = &rec_var;

	for (x = md->axis; x != NULL; x = x->next)
	{
		EfiStruct *xvar, **xp;

		xvar = NULL;
		xp = &xvar;
		type = NULL;

		for (s = x->priv; s != NULL; s = s->next)
		{
			if	(s->cdef->name == NULL)
				continue;

			type = rec_type(s->cdef);
			*xp = NewEfiStruct(type, NULL, 0);
			(*xp)->name = rec_name(s->cdef->name);
			(*xp)->desc = mstrcpy(s->cdef->desc);
			(*xp)->par = s;
			xp = &(*xp)->next;
		}

		if	(xvar->next)
			type = MakeStruct(NULL, NULL, RefEfiStruct(xvar));

		*ptr = NewEfiStruct(type, StrPool_get(x->sbuf, x->i_name), 0);
		(*ptr)->par = xvar;
		ptr = &(*ptr)->next;
	}

	*ptr = cvar;

	obj = LvalObj(NULL, MakeStruct(NULL, NULL, rec_var));
	rec_obj = LvalObj(&Lval_ptr, md->type,
		(char *) obj->data + cvar->offset);
	return edb_alloc(obj, mstrcpy(StrPool_get(md->sbuf, md->i_name)));
}

static void std_init (MdCountList *clist, mdmat *md)
{
	CleanData(rec_obj->type, rec_obj->data, 0);
}

static char *ext_init_ptr (mdaxis *x, char *ptr)
{
	if	(x && x->priv)
	{
		MdCntGrp *s;

		for (s = x->priv; s != NULL; s = s->next)
		{
			if	(s->flag)
				return ext_init_ptr(x->next,
					ptr + s->idx * x->size);
	
			ptr += s->cdef->dim * x->size;
		}

		return NULL;
	}
	else	return ptr;
}

static void ext_init (MdCountList *clist, mdmat *md)
{
	void *p = ext_init_ptr(md->axis, md->data);

	CleanData(rec_obj->type, rec_obj->data, 0);

	if	(p)
		CopyData(rec_obj->type, rec_obj->data, p);
}

static void (*rec_init) (MdCountList *clist, mdmat *md) = std_init;

static void rec_print (MdCountList *clist, mdmat *md)
{
	MdCountList *p;
	EfiStruct *base, *var;

	cnt_counter++;
	rec_init(clist, md);

	for (p = clist; p != NULL; p = p->next)
		p->cnt->add(p->cnt, (char *) rec_obj->data + p->offset,
			p->data);

	for (base = rec_var; base && base->par; base = base->next)
	{
		char *p = (char *) rec_edb->obj->data + base->offset;

		for (var = base->par; var != NULL; var = var->next)
		{
			MdCntGrp *grp = var->par;
			char *p2 = p + var->offset;

			if	(grp->cdef->dim)
				Val_int(p2) = grp->flag ? 1 + grp->idx : 0;
			else	memcpy(p2, &grp->idx, var->type->size);
		}
	}

	edb_write(rec_edb);
}


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	EDB *edb;
	MdCountPar *CountPar;
	MdCountList *clist;
	mdmat *md;
	mdaxis **ptr;
	char *rec_mode;
	int need_init;
	int cnt_limit;
	int cnt_rec;
	unsigned recnum;
	char *oname;
	IO *io;
	char *p;

	SetProgName(arg[0]);
	SetVersion("$Id: edbcnt.c,v 1.15 2007-11-12 16:32:57 ef Exp $");
	SetupStd();
	SetupUtil();
	SetupPreproc();
	SetupEDB();
	SetupTimeSeries();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupMathFunc();
	SetupStatData();
	SetupDebug();
	ParseCommand(&narg, arg);

	SampleWeight = atof(GetResource("Weight", "1."));
	cnt_limit = GetIntResource("CountLimit", 0);
	rec_mode = GetResource("RecordMode", NULL);

/*	Datenbank öffnen
*/
	edb = edb_fopen(NULL, GetResource("Input", NULL));
	edb = edb_filter(edb, GetResource("Filter", NULL));

/*	Auswertungsdefinition bestimmen
*/
	CountPar = MdCountPar_create();
	CountPar->weight = SampleWeight;
	MdCountPar_range(CountPar, GetResource("Range", NULL));
	MdCountPar_init(CountPar, edb->obj, GetResource("Method", NULL));

	MdClass_add(CountPar, cltab, tabsize(cltab));
	recnum = 0;
	MdCntObjClass(CountPar, RefObj(edb->obj),
		"data", mstrcpy(edb->desc));

	if	(rec_mode)
	{
		MdCount_add(CountPar, vtab, tabsize(vtab));
		MdCntObj(CountPar, RefObj(edb->obj),
			"data",
			mstrcpy(edb->desc));
		MdCntObj(CountPar, LvalObj(&Lval_ptr, &Type_int, &cnt_counter),
			"count.index",
			mlangcpy(":*:count index :de:Zählindex", NULL));
		MdCntObj(CountPar, LvalObj(&Lval_ptr, &Type_uint, &recnum),
			"recnum",
			mlangcpy(":*:record number :de:Satzindex", NULL));
	}

	if	(GetFlagResource("StartInfo"))
	{
		InfoNode *info;
		char  *mode;
	
		info = AddInfo(NULL, "cdef", "Zähldefinitionen", NULL, NULL);
		MdCountInfo(info, CountPar);
		MdClassInfo(info, CountPar);
		mode = GetResource("InfoMode", NULL);
		BrowseInfo(mstrpaste(":", mode, "/cdef"));
		exit(EXIT_SUCCESS);
	}

/*	Klassen auflisten
*/
	if	(GetFlagResource("ShowClass"))
	{
		MdShowClass(iostd, CountPar, GetResource("ClassList", NULL));
		cnt_exit(edb, NULL);
	}

/*	Zähler Bestimmen
*/
	p = GetResource("Counter", NULL);

	if	(p == NULL || *p == '?')
	{
		md_showcnt(iostd, CountPar);
		cnt_exit(edb, NULL);
	}

	if	((clist = MdCountList_get(CountPar, p)) == NULL)
		cnt_exit(edb, "[2]");

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->sbuf = NewStrPool();
	p = CmdPar_psub(NULL, GetResource("Title", NULL), NULL);
	md->i_name = StrPool_xadd(md->sbuf, p);
	memfree(p);
	md->axis = CountPar->axis;
	CountPar->axis = NULL;
	ptr = &md->axis;

	while (*ptr)
		ptr = &(*ptr)->next;

	io = io_cstr(GetResource("Grpdef", NULL));

	while ((*ptr = md_ctabaxis(io, CountPar)) != NULL)
		ptr = &(*ptr)->next;

	io_close(io);
	need_init = md_ctabinit(md, clist);

/*	Ausgabefile bestimmen
*/
	oname = GetResource("Output", NULL);
	
	if	((p = GetResource("Post", NULL)))
	{
		if	(oname && *oname == '|')
			oname = mstrpaste(" ", p, oname);
		else 	oname = mstrpaste(" > ", p, oname);
	}

/*	EDB-File bei RecordMode öffnen
*/
	if	(rec_mode)
	{
		rec_edb = make_edb(md);
		edb_fout(rec_edb, oname, rec_mode);
		md_count_hook = rec_print;

		if	(need_init)
			rec_init = ext_init;
	}
	else
	{
		rec_edb = NULL;
		md_count_hook = std_hook;
	}

/*	Datenbank auszählen
*/
	cnt_counter = 0;
	cnt_rec = 0;
	CountPar->md = rd_refer(md);

	while (edb_read(edb))
	{
		cnt_rec += MdCountPar_count(CountPar);

		if	(cnt_limit && cnt_rec >= cnt_limit)
			break;
	}

	rd_deref(edb);

	if	(rec_edb)
	{
		UnrefObj(rec_obj);
		rd_deref(rec_edb);
	}
	else	md_fsave(oname, md, 0);

	return 0;
}
