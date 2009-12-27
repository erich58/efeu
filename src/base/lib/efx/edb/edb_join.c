/*	Datenbankverknüpfung

$Copyright (C) 2005 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/Op.h>
#include <EFEU/cmdeval.h>
#include <EFEU/parsearg.h>
#include <EFEU/printobj.h>
#include <ctype.h>

#define	ERR_VAR	"[edb:var]$!: invalid declaration $1.\n"
#define	ERR_MEM	"[edb:mem]$!: type $1 has no member $2.\n"
#define	ERR_INC	"[edb:inc]$!: key $1 not compatible.\n"
#define	ERR_KEY	"[edb:key]$!: join key not defined.\n"
#define ERR_USE	"[edb:use]$!: name $1 arready used.\n"
#define	ERR_CMD "[edb:cmd]$!: unknown command $1.\n"
#define	ERR_CMP "[edb:cmp]$!: data file $1 unsorted.\n"
#define	ERR_ORD "[edb:ord]$!: invalid order of parameters.\n"
#define	ERR_DBI "[edb:dbi]$!: invalid data file identification $1.\n"

#define	JOIN_A	0x1
#define	JOIN_B	0x2
#define	JOIN_X	0x4
#define	JOIN_AX	(JOIN_A|JOIN_X)
#define	JOIN_BX	(JOIN_B|JOIN_X)

/*
Parameterstruktur
*/

typedef struct {
	EDB *db;
	EfiObj *key;
	EDBAssign *set_key;
	EDBAssign *set_data;
	EfiVec *vec;
	int save;
} DBDEF;

static void dbdef_init (DBDEF *def, EDB *edb)
{
	def->db = edb;
	def->key = NULL;
	def->set_key = NULL;
	def->set_data = NULL;
	def->vec = NULL;
	def->save = 0;
}

static void dbdef_vec (DBDEF *def)
{
	if	(!def->vec)
		def->vec = NewEfiVec(def->db->obj->type, NULL, 0);
}

static void dbdef_clean (DBDEF *def)
{
	rd_deref(def->db);
	UnrefObj(def->key);
	rd_deref(def->set_key);
	rd_deref(def->set_data);
	rd_deref(def->vec);
}

static void dbdef_kvar (DBDEF *def, EfiVar *var, size_t offset)
{
	def->set_key = NewEDBAssign(def->set_key, var, offset);
}

static int dbdef_read (DBDEF *def)
{
	if	(def->save)
	{
		def->save = 0;
	}
	else if	((edb_read(def->db)))
	{
		EDBAssignData(def->set_key, def->key->data,
			def->db->obj->data);
	}
	else	return 0;

	return 1;
}

static void dbdef_key (DBDEF *def, EfiObj *key)
{
	CleanData(key->type, key->data);
	CopyData(key->type, key->data, def->key->data);
}

static void dbdef_load (DBDEF *def, EfiObj *key, CmpDef *cmp)
{
	CopyData(def->vec->type, vb_next(&def->vec->buf),
		def->db->obj->data);

	while (edb_read(def->db))
	{
		int r;

		EDBAssignData(def->set_key, def->key->data,
			def->db->obj->data);
		r = cmp_data(cmp, key->data, def->key->data);

		if	(r > 0)
		{
			PrintObj(ioerr, key);
			io_puts(" > ", ioerr);
			PrintObj(ioerr, def->key);
			io_puts("\n", ioerr);
			PrintObj(ioerr, def->db->obj);
			io_puts("\n", ioerr);
			dbg_error("edb", ERR_CMP, "m", rd_ident(def->db->ipar));
		}

		if	(r != 0)
		{
			def->save = 1;
			break;
		}

		CopyData(def->vec->type, vb_next(&def->vec->buf),
			def->db->obj->data);
	}
}

typedef struct {
	REFVAR;
	DBDEF db[2];

	EfiVar *kvar;
	EfiObj *key;
	CmpDefEntry *clist;
	CmpDef *cmp;
	EDBAssign *kassign;
	EfiVar *var;
	EfiVec *tg;
	int mode;
	int use;
} JOIN;


static void join_clean (void *data)
{
	JOIN *join = data;
	dbdef_clean(join->db);
	dbdef_clean(join->db + 1);

	rd_deref(join->cmp);
	UnrefObj(join->key);

	rd_deref(join->tg);
	rd_deref(join->kassign);
	DelVarList(join->kvar);
	DelVarList(join->var);
	memfree(join);
}

static RefType join_reftype = REFTYPE_INIT("EDB_JOIN", NULL, join_clean);

static void *join_alloc (EDB *db1, EDB *db2)
{
	JOIN *join;
	
	join = memalloc(sizeof *join);
	dbdef_init(join->db, db1);
	dbdef_init(join->db + 1, db2);

	join->kvar = NULL;
	join->key = NULL;
	join->clist = NULL;
	join->cmp = NULL;
	join->kassign = NULL;

	join->var = NULL;
	join->tg = NULL;
	join->mode = JOIN_X;
	join->use = 0;
	return rd_init(&join_reftype, join);
}


static EfiVar *get_var (EfiType *type, const char *name)
{
	static EfiVar buf;
	EfiVar *v;

	if	(name == NULL || mstrcmp(name, ".") == 0)
	{
		buf.name = NULL;
		buf.type = type;
		buf.dim = 0;
		buf.offset = 0;
		return &buf;
	}

	for (v = type->list; v; v = v->next)
		if (mstrcmp(name, v->name) == 0)
			return v;

	dbg_error("edb", ERR_MEM, "ss", type->name, name);
	return NULL;
}

static void join_add_var (JOIN *join, EDBAssign **ptr, EfiVar *var, int flag)
{
	EfiVar **p;

	for (p = &join->var; *p; p = &(*p)->next)
	{
		if	(mstrcmp(var->name, (*p)->name) == 0)
		{
			if	(flag)
				dbg_error("edb", ERR_USE, "s", var->name);

			return;
		}
	}

	*p = NewVar(var->type, var->name, var->dim);
	*ptr = NewEDBAssign(*ptr, *p, var->offset);
}

static void join_add_type (JOIN *join, EDBAssign **ptr,
	EfiType *type, const char *name)
{
	EfiVar buf;

	buf.type = type;
	buf.name = (char *) name;
	buf.dim = 0;
	buf.offset = 0;
	join_add_var(join, ptr, &buf, 1);
}

static void join_add_list (JOIN *join, EDBAssign **ptr, EfiVar *list)
{
	for (; list; list = list->next)
		join_add_var(join, ptr, list, 0);
}

static void join_add_dbvar (JOIN *join, DBDEF *def)
{
	join_add_list(join, &def->set_data, def->db->obj->type->list);
}

typedef struct {
	char *name;
	char *args;
	void (*eval) (JOIN *join, const char *opt, const char *arg);
	char *desc;
} JPAR;

static void join_mode (JOIN *join, const char *opt, const char *key)
{
	if	(key == NULL)
	{
		join->mode = JOIN_X;
		return;
	}

	for (join->mode = 0; *key; key++)
	{
		switch (*key)
		{
		case 'a': case 'A': case 1:	join->mode |= JOIN_A; break;
		case 'b': case 'B': case 2:	join->mode |= JOIN_B; break;
		case 'x': case 'X': case 3:	join->mode |= JOIN_X; break;
		}
	}

	join->use = join->mode;
}

static void join_filter (JOIN *join, const char *opt, const char *arg)
{
	int f1, f2;

	if	(join->key || join->kvar)
	{
		dbg_error("edb", ERR_ORD, NULL);
		return;
	}

	if	(!opt)	opt = "b";

	for (f1 = f2 = 0; *opt; opt++)
	{
		switch (*opt)
		{
		case 'a': case 'A': case '1':	f1 = 1; break;
		case 'b': case 'B': case '2':	f2 = 2; break;
		}
	}

	if	(f1)
		join->db[0].db = edb_filter(join->db[0].db, arg);

	if	(f2)
		join->db[1].db = edb_filter(join->db[1].db, arg);
}

static void join_key (JOIN *join, const char *opt, const char *vdef)
{
	char **list;
	char *a, *b, *name;
	size_t dim, i;
	int inv;
	EfiVar *v, **p;
	CmpDefEntry **cp;

	dim = mstrsplit(vdef, "%s,", &list);

	for (p = &join->kvar; *p; )
		p = &(*p)->next;

	for (cp = &join->clist; *cp; )
		cp = &(*cp)->next;

	for (i = 0; i < dim; i++)
	{
		name = list[i];

		if	(*name == '+')	inv = 0, name++;
		else if	(*name == '-')	inv = 1, name++;
		else			inv = 0;

		if	((a = strchr(name, ':')))
		{
			*a++ = 0;
		}
		else	a = name;

		if	((b = strchr(a, '=')))
		{
			*b++ = 0;
		}
		else	b = a;

		v = get_var(join->db[0].db->obj->type, a);
		*p = NewVar(v->type, name, v->dim);
		*cp = cmp_entry(v->type, 0, v->dim, inv);
		dbdef_kvar(join->db, *p, v->offset);

		v = get_var(join->db[1].db->obj->type, b);

		if	(v->type != (*p)->type || v->dim != (*p)->dim)
			dbg_error("edb", ERR_INC, "s", name);

		dbdef_kvar(join->db + 1, rd_refer(*p), v->offset);
		p = &(*p)->next;
		cp = &(*cp)->next;
	}

	memfree(list);
}

static void join_auto (JOIN *join)
{
	EfiVar *p1, *p2;
	EfiVar **kp;
	CmpDefEntry **cp;

	kp = &join->kvar;
	cp = &join->clist;

	for (p1 = join->db[0].db->obj->type->list; p1; p1 = p1->next)
	{
		for (p2 = join->db[1].db->obj->type->list; p2; p2 = p2->next)
		{
			if	(mstrcmp(p1->name, p2->name) != 0)
				continue;

			if	(p1->type != p2->type ||
					p1->dim != p2->dim)
			{
				dbg_error("edb", ERR_INC, "s", p1->name);
				continue;
			}

			*kp = NewVar(p1->type, p1->name, p1->dim);
			dbdef_kvar(join->db, *kp, p1->offset);
			dbdef_kvar(join->db + 1, *kp, p2->offset);
			*cp = cmp_entry(p1->type, 0, p1->dim, 0);
			kp = &(*kp)->next;
			cp = &(*cp)->next;
			break;
		}
	}
}


static void join_make_key (JOIN *join)
{
	EfiType *type;
	EfiVar *vp;
	CmpDefEntry *cp;

	if	(join->key)			return;

	if	(!join->kvar)
		join_auto(join);

	if	(!join->kvar)
		dbg_error("edb", ERR_KEY, NULL);
	
	type = MakeStruct(NULL, NULL, RefVarList(join->kvar));

	cp = join->clist;
	vp = join->kvar;

	while (cp)
	{
		cp->offset = vp->offset;
		cp = cp->next;
		vp = vp->next;
	}

	join->cmp = cmp_alloc(type, join->clist);
	join->key = LvalObj(NULL, type);
	join->db[0].key = LvalObj(NULL, type);
	join->db[1].key = LvalObj(NULL, type);
}

static void join_use (JOIN *join, const char *opt, const char *arg)
{
	if	(arg && arg[0] && !arg[1])
	{
		switch (arg[0])
		{
		case 'a': case 'A': case '1':	join->use = 1; return;
		case 'b': case 'B': case '2':	join->use = 2; return;
		case 'x': case 'X':		join->use = 0; return;
		default:			break;
		}
	}

	dbg_error("edb", ERR_DBI, "s", arg);
}

static void join_var (JOIN *join, const char *opt, const char *vdef)
{
	char **list;
	char *name;
	char *vname;
	size_t dim, i;
	EfiType *type;
	EfiVar *st, **p;
	EDBAssign **x;

	dim = mstrsplit(vdef, "%s,", &list);
	join_make_key(join);

	for (p = &join->var; *p; )
		p = &(*p)->next;

	for (i = 0; i < dim; i++)
	{
		if	((name = strchr(list[i], '=')))
		{
			vname = list[i];
			*name++ = 0;
		}
		else
		{
			vname = NULL;
			name = list[i];
		}

		if	(name[0] == 'k' && (name[1] == '.' || name[1] == 0))
		{
			name++;
			type = join->key->type;
			x = &join->kassign;
		}
		else if	(name[0] == 'a' && (name[1] == '.' || name[1] == 0))
		{
			name++;
			type = join->db[0].db->obj->type;
			x = &join->db[0].set_data;
		}
		else if	(name[0] == 'b' && (name[1] == '.' || name[1] == 0))
		{
			name++;
			type = join->db[1].db->obj->type;
			x = &join->db[1].set_data;
		}
		else
		{
			dbg_error("edb", ERR_VAR, "s", name);
			continue;
		}

		if	(*name == 0)
		{
			join_add_type(join, x, type, list[i]);
			continue;
		}

		name++;

		if	(*name == '*')
		{
			join_add_list(join, x, type->list);
			continue;
		}

		for (st = type->list; st; st = st->next)
		{
			if	(mstrcmp(st->name, name) == 0)
			{
				EfiVar buf;
				buf.type = st->type;
				buf.name = vname ? vname : name;
				buf.dim = st->dim;
				buf.offset = st->offset;
				join_add_var(join, x, &buf, 1);
				name = NULL;
				break;
			}
		}

		if	(name)
			dbg_error("edb", ERR_VAR, "s", name);
	}

	memfree(list);
}

static JPAR jpar[] = {
	{ "mode", "=flags", join_mode,
		":*:evaluation mode:\n"
		"a,1\trecords uniq in file 1\n"
		"b,2\trecords uniq in file 2\n"
		"x\trecords in both files\n"
		":de:Auswertungsmodus:\n"
		"a,1\tDatensätze nur in Datei 1\n"
		"b,2\tDatensätze nur in Datei 2\n"
		"x\tDatensätze in beiden Dateien)\n"
	}, { "filter", "[db]=fdef", join_filter,
		":*:applay filter <fdef> to given data base <db>:\n"
		"a,1\tfirst data base\n"
		"b,2\tsecond data base (default)\n"
		":de:Filter <fdef> auf gegebene Datenbank <db> geben:\n"
		"a,1\terste Datenbank\n"
		"b,2\tzweite Datenbank (Vorgabe)\n"
	}, { "key", "=list", join_key,
		":*:list of key variables\n"
		":de:Liste der Schlüsselvariablen\n"
	}, { "use", "=db", join_use,
		":*:use data from given input file <db>:\n"
		"a,1\tfirst data base\n"
		"b,2\tsecond data base\n"
		":de:Verwende Daten der gegebenen Eingabedatei <db>:\n"
		"a,1\terste Datenbank\n"
		"b,2\tzweite Datenbank\n"
	}, { "var", "=list", join_var,
		":*:list of output variables\n"
		":de:Liste der Ausgabevariablen\n"
	}
};

static void jpar_list (IO *out)
{
	int i;

	for (i = 0; i < tabsize(jpar); i++)
	{
		IO *io;

		io_puts(jpar[i].name, out);
		io_puts(jpar[i].args, out);
		io_putc('\n', out);

		io = io_lmark(io_refer(out), "\t", NULL, 0);
		io_langputs(jpar[i].desc, io);
		io_close(io);
	}
}


static void jpar_eval (JOIN *join, AssignArg *arg)
{
	int i;

	for (i = 0; i < tabsize(jpar); i++)
	{
		if	(mstrcmp(jpar[i].name, arg->name) == 0)
		{
			jpar[i].eval(join, arg->opt, arg->arg);
			return;
		}
	}

	dbg_error("edb", ERR_CMD, "s", arg->name);
}


static int join_load (JOIN *join)
{
	int s1, s2;
	int r;

	s1 = dbdef_read(join->db);
	s2 = dbdef_read(join->db + 1);

	if	(s1 && s2)
	{
		r = cmp_data(join->cmp, join->db[0].key->data,
			join->db[1].key->data);

		if	(r < 0)
		{
			join->db[1].save = 1;
			s2 = 0;
		}
		else if	(r > 0)
		{
			join->db[0].save = 1;
			s1 = 0;
		}
	}

	if	(s1)	dbdef_key(join->db, join->key);
	else if	(s2)	dbdef_key(join->db + 1, join->key);
	else		return 0;

	EfiVec_resize(join->db[0].vec, 0);
	EfiVec_resize(join->db[1].vec, 0);

	if	(s1)
		dbdef_load(join->db, join->key, join->cmp);

	if	(s2)
		dbdef_load(join->db + 1, join->key, join->cmp);

	if	(s1)
		return s2 ? JOIN_X : JOIN_A;

	return JOIN_B;
}

static void join_copy (JOIN *join, DBDEF *def)
{
	char *data = def->vec->buf.data;
	size_t n = def->vec->buf.used;

	while (n-- > 0)
	{
		void *p = vb_next(&join->tg->buf);
		EDBAssignData(join->kassign, p, join->key->data);
		EDBAssignData(def->set_data, p, data);
		data += def->vec->buf.elsize;
	}
}

static void join_set (JOIN *join)
{
	char *p1 = join->db[0].vec->buf.data;
	size_t n1 = join->db[0].vec->buf.used;

	while (n1-- > 0)
	{
		char *p2 = join->db[1].vec->buf.data;
		size_t n2 = join->db[1].vec->buf.used;

		while (n2-- > 0)
		{
			void *p = vb_next(&join->tg->buf);
			EDBAssignData(join->kassign, p, join->key->data);
			EDBAssignData(join->db[0].set_data, p, p1);
			EDBAssignData(join->db[1].set_data, p, p2);
			p2 += join->db[1].vec->buf.elsize;
		}

		p1 += join->db[0].vec->buf.elsize;
	}
}

static int join_read (EfiType *type, void *data, void *par)
{
	JOIN *join = par;

	while (join->tg->buf.used == 0)
	{
		int mode = join_load(join);

		if	(mode == 0)
			return 0;

		if	(!(mode & join->mode))
		{
			EfiVec_resize(join->db[0].vec, 0);
			EfiVec_resize(join->db[1].vec, 0);
		}
		else if	(join->use)
		{
			;
		}
		else if	(mode == JOIN_X)
		{
			join_set(join);
		}
		else if	(mode == JOIN_A)
		{
			join_copy(join, join->db);
		}
		else if	(mode == JOIN_B)
		{
			join_copy(join, join->db + 1);
		}
	}

	memcpy(data, vb_delete(&join->tg->buf, 0, 1), join->tg->buf.elsize);
	return 1;
}

EDB *edb_join (EDB *edb1, EDB *edb2, const char *def)
{
	AssignArg *arg;
	JOIN *join;
	EDB *edb;
	EfiType *type;

	if	(!edb1)	return edb2;
	if	(!edb2)	return edb1;

	join = join_alloc(edb1, edb2);

	while ((arg = assignarg(def, (char **) &def, " \t\n;")))
	{
		if	(*arg->name)
			jpar_eval(join, arg);

		memfree(arg);
	}

	join_make_key(join);
	type = NULL;
	dbdef_vec(join->db);
	dbdef_vec(join->db + 1);

	if	(join->var)
	{
		join->use = 0;
	}
	else if	(join->use == JOIN_A)
	{
		type = join->db[0].vec->type;
		join->tg = rd_refer(join->db[0].vec);
		join->use = JOIN_A;
	}
	else if	(join->use == JOIN_B)
	{
		type = join->db[1].vec->type;
		join->tg = rd_refer(join->db[1].vec);
		join->use = JOIN_B;
	}
	else
	{
		join_add_list(join, &join->kassign, join->key->type->list);
		join_add_dbvar(join, join->db);
		join_add_dbvar(join, join->db + 1);
		join->use = 0;
	}

	if	(join->var)
	{
		type = MakeStruct(NULL, NULL, RefVarList(join->var));
		join->tg = NewEfiVec(type, NULL, 0);
	}

	edb = edb_create(LvalObj(NULL, type), NULL);
	edb->read = join_read;
	edb->ipar = join;
	return edb;
}


static EDB *fdef_join (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	if	(mstrcmp(opt, "?") == 0)
	{
		jpar_list(ioerr);
		exit(EXIT_SUCCESS);
	}

	return edb_join(base, edb_fopen(NULL, arg), opt);
}

EDBFilter EDBFilter_join = {
	"join", "[par]=file", fdef_join, NULL, 
	":*:join with database <file> and parameters <par>.\n"
	"join[?] gives a list of availabel parameters.\n"
	":de:Verknüpfen mit Datenbank <file> und Parameter <par>.\n"
	"Die Angabe join[?] liefert die verfügbaren Parameter.\n"
};
