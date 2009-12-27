/*
:*:EDB interface
:de:EDB-Schnittstelle

$Copyright (C) 2006 Erich Frühstück
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
#include <DB/PGCube.h>
#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/calendar.h>
#include <EFEU/Debug.h>
#include <EFEU/stack.h>

#if	HAS_PQ

#define	E_NAME	"$!: table name must be specified.\n"
#define	M_FLUSH	"$!: PQputline() failed.\n"
#define	M_ECOPY	"$!: PQendcopy() failed\n"
#define	E_FETCH	"$!: could not fetch data type\n"

#define	E2P_DEBUG	0

static Stack *epg_stack = NULL;
static PGCube cube = { NULL, NULL, NULL };

static PG *epg_connect (const char *arg, int force)
{
	PG *x;

	if	(cube.pg && !force && !arg)
		return rd_refer(cube.pg);

	x = PG_connect(arg ? arg : "");

	if	(force)
	{
		if	(cube.pg)
			pushstack(&epg_stack, cube.pg);

		cube.pg = rd_refer(x);
	}

	return x;
}

static void epg_close(void)
{
	rd_deref(cube.pg);
	cube.pg = popstack(&epg_stack, NULL);
}

static void edb_closeall (void *par)
{
	while (cube.pg)
		epg_close();
}

static char *parse_query (IO *io, StrBuf *buf);

static int pfunc_int (IO *out, const EfiType *type, const void *data)
{
	return io_xprintf(out, "%d", Val_int(data));
}

static void conv_int (EfiType *type, void *data, const char *def)
{
	Val_int(data) = strtol(def, NULL, 10);
}

static int pfunc_uint (IO *out, const EfiType *type, const void *data)
{
	return io_xprintf(out, "%u", Val_uint(data));
}

static void conv_uint (EfiType *type, void *data, const char *def)
{
	Val_uint(data) = strtoul(def, NULL, 10);
}

static int pfunc_int64 (IO *out, const EfiType *type, const void *data)
{
	return io_xprintf(out, "%lld", *((int64_t *) data));
}

static int pfunc_uint64 (IO *out, const EfiType *type, const void *data)
{
	return io_xprintf(out, "%llu", *((uint64_t *) data));
}

static int pfunc_str (IO *out, const EfiType *type, const void *data)
{
	char *str = Val_str(data);

	if	(str)
	{
		return io_xputs(str, out, "\";");
	}
	else	return io_puts("NULL", out);
}

static int pfunc_date (IO *out, const EfiType *type, const void *data)
{
	return PrintCalendar(out, "%Y-%m-%d", Val_Date(data));
}

static void conv_date (EfiType *type, void *data, const char *def)
{
	Val_Date(data) = str2Calendar(def, NULL, 0);
}

static void conv_str (EfiType *type, void *data, const char *def)
{
	Val_str(data) = mstrcpy(def);
}

static void conv_any (EfiType *type, void *data, const char *def)
{
	Obj2Data(strterm(def), type, data);
}

static int IsType (const EfiType *type, const EfiType *base)
{
	return (type == base);
}

static int IsAny (const EfiType *type, const EfiType *base)
{
	return 1;
}

static struct {
	EfiType *type;
	int (*test) (const EfiType *type, const EfiType *base);
	char *pg_type;
	int (*pfunc)(IO *out, const EfiType *type, const void *data);
	void (*conv)(EfiType *type, void *data, const char *def);
} conv_tab[] = {
	{ &Type_int, IsType, "integer", pfunc_int, conv_int },
	{ &Type_uint, IsType, "int8", pfunc_uint, conv_uint },
	{ &Type_double, IsType, "float", PrintData, conv_any },
	{ &Type_str, IsType, "text", pfunc_str, conv_str },
	{ &Type_Date, IsType, "date", pfunc_date, conv_date },

	{ &Type_enum, IsTypeClass, "integer", pfunc_int, conv_int },
	{ &Type_bool, IsType, "integer", pfunc_int, conv_int },
	{ &Type_char, IsType, "integer", pfunc_int, conv_int },
	{ &Type_int8, IsType, "integer", PrintData, conv_any },
	{ &Type_int16, IsType, "integer", PrintData, conv_any },
	{ &Type_int32, IsType, "integer", PrintData, conv_any },
	{ &Type_int64, IsType, "int8", pfunc_int64, conv_any },
	{ &Type_varint, IsType, "int8", pfunc_int64, conv_any },
	{ &Type_uint8, IsType, "integer", PrintData, conv_any },
	{ &Type_uint16, IsType, "integer", PrintData, conv_any },
	{ &Type_uint32, IsType, "int8", PrintData, conv_any },
	{ &Type_uint64, IsType, "int8", pfunc_uint64, conv_any },
	{ &Type_varsize, IsType, "int8", pfunc_uint64, conv_any },
	{ &Type_float, IsType, "float", PrintData, conv_any },

	{ NULL, IsAny, "text", PrintData, conv_any },
};

typedef struct {
	char *db;
	char *name;
	char *par;
	int drop;
	int nocon;
} PARG;

static PARG *parg_alloc (const char *opt, const char *arg)
{
	PARG *parg;
	char *db;
	char *p;

	if	(!arg)		return NULL;

	if	(*arg == ':')
	{
		arg++;
		db = NULL;
	}
	else if	((p = strchr(arg, ':')))
	{
		db = mstrncpy(arg, p - arg);
		arg = p + 1;
	}
	else	db = NULL;

	if	(!*arg)		return NULL;

	parg = memalloc(sizeof *parg);
	parg->db = db;

	if	(opt)
	{
		for (; *opt; opt++)
		{
			switch (*opt)
			{
			case 'd':	parg->drop = 1; break;
			case 'n':	parg->nocon = 1; break;
			default:	break;
			}
		}
	}

	if	((p = strchr(arg, ';')))
	{
		parg->name = mstrncpy(arg, p - arg);
		parg->par = mstrcpy(p + 1);
	}
	else
	{
		parg->name = mstrcpy(arg);
		parg->par = NULL;
	}

	return parg;
}

static void parg_free (PARG *parg)
{
	memfree(parg->db);
	memfree(parg->name);
	memfree(parg->par);
	memfree(parg);
}

typedef struct {
	EfiType *type;
	char *name;
	char *pg_type;
	int offset;
	int (*pfunc)(IO *out, const EfiType *type, const void *data);
	void (*conv)(EfiType *type, void *data, const char *def);
	EfiObj *base;
	EfiStruct *st;
} CONVARG;

static void conv_free (void *ptr)
{
	CONVARG *conv = ptr;
	memfree(conv->name);
	UnrefObj(conv->base);
}

#if	E2P_DEBUG
static void put_conv (CONVARG *conv, IO *out)
{
	io_xprintf(out, "%2d %2d", conv->offset,
		conv->base ? conv->base->type->size : conv->type->size);
	io_xprintf(out, " %-12s", conv->type->name);
	io_xprintf(out, " %-12s", conv->name);
	io_xprintf(out, " %-12s", conv->pg_type);

	if	(conv->base)
		io_xprintf(out, " %s", conv->base->type->name);

	io_putc('\n', out);
}
#endif

typedef struct {
	REFVAR;	/* Referenzvariablen */
	PG *pg;	/* Serverzugriff */
	IO *io;	/* COPY-Zugriff */
	VecBuf conv;
	StrBuf buf;
	int endline;
	int copy;
} EDB2PG;

#if	E2P_DEBUG
static void put_pdef (EDB2PG *par, IO *out)
{
	size_t n;
	CONVARG *arg;

	for (n = par->conv.used, arg = par->conv.data; n-- > 0; arg++)
		put_conv(arg, out);
}
#endif

static void edb2pg_clean (void *data)
{
	EDB2PG *par = data;

	if	(par->endline && PQputline(par->pg->conn, "\\.\n"))
		dbg_note("PG", M_FLUSH, NULL);

	if      (par->copy)
	{
		if	(PQendcopy(par->pg->conn) != 0)
			dbg_note("PG", M_ECOPY, NULL);
	}
	else
	{
		PG_command(par->pg, "CLOSE tmpcursor");
		PG_command(par->pg, "COMMIT");
	}

	rd_deref(par->pg);
	io_close(par->io);
	vb_clean(&par->conv, conv_free);
	vb_free(&par->conv);
	sb_free(&par->buf);
	memfree(par);
}

static RefType edb2pg_reftype = REFTYPE_INIT("EDB2PG", NULL, edb2pg_clean);

static EDB2PG *edb2pg_create (void)
{
	EDB2PG *par = memalloc(sizeof *par);
	vb_init(&par->conv, 100, sizeof(CONVARG));
	sb_init(&par->buf, 0);
	par->io = io_strbuf(&par->buf);
	return rd_init(&edb2pg_reftype, par);
}

static char *edb2pg_get (EDB2PG *par)
{
	io_putc(0, par->io);
	io_rewind(par->io);
	sb_clean(&par->buf);
	return (char *) par->buf.data;
}

static int edb2pg_cmd (EDB2PG *par)
{
	char *p = edb2pg_get(par);
	PG_info(par->pg, p);
	return PG_command(par->pg, p);
}

static void add_conv (EDB2PG *par, char *name, EfiType *type,
	int offset, EfiStruct *st, EfiObj *base)
{
	CONVARG *p;
	size_t n;

	p = vb_next(&par->conv);
	p->name = name ? name : "this";
	p->type = type;
	p->offset = offset;
	p->pfunc = PrintData;
	p->conv = conv_any;
	p->base = base;
	p->st = st;

	for (n = 0; n < tabsize(conv_tab); n++)
	{
		if	(conv_tab[n].test(p->type, conv_tab[n].type))
		{
			p->pg_type = conv_tab[n].pg_type;
			p->pfunc = conv_tab[n].pfunc;
			p->conv = conv_tab[n].conv;
			break;
		}
	}
}

static void add_pdef (EDB2PG *par, EfiType *type, char *name, int offset)
{
	if	(type->list)
	{
		EfiStruct *x;
		EfiObj *base;

		base = LvalObj(&Lval_ptr, type, NULL);

		for (x = type->list; x; x = x->next)
		{
			size_t off;
			char *s;

			off = offset + x->offset;
			s = mstrpaste("_", name, x->name);

			if	(x->member)
			{
				add_conv(par, s, x->type, offset,
					x, RefObj(base));
			}
			else if	(x->dim)
			{
				int n;

				for (n = 1; n <= x->dim; n++)
				{
					char *vn = msprintf("%s_%d", s, n);
					add_pdef(par, x->type, vn, off);
					off += x->type->size;
				}

				memfree(s);
			}
			else	add_pdef(par, x->type, s, off);
		}

		UnrefObj(base);
		memfree(name);
	}
	else	add_conv(par, name, type, offset, NULL, NULL);
}

static size_t write_pg (EfiType *type, void *data, void *p_par)
{
	EDB2PG *par = p_par;
	char *delim = NULL;
	CONVARG *put;
	EfiObj *obj;
	size_t n;
	
	for (n = par->conv.used, put = par->conv.data; n-- > 0; put++)
	{
		sb_puts(delim, &par->buf);

		if	(put->base)
		{
			put->base->data = (char *) data + put->offset;
			obj = put->st->member(put->st, put->base);
			put->pfunc(par->io, put->type,
				obj ? obj->data : put->type->defval);
			UnrefObj(obj);
		}
		else
		{
			put->pfunc(par->io, put->type,
				(char *) data + put->offset);
		}

		delim = "\t";
	}

	sb_putc('\n', &par->buf);

	if	(PQputline(par->pg->conn, edb2pg_get(par)) != 0)
		dbg_note("PG", M_FLUSH, NULL);

	return 1;
}

static void init_pg (EDB *edb, EDBPrintMode *mode, IO *io)
{
	PARG *arg = mode->par;

	if	(!arg->nocon)
	{
		EfiType *type = edb->obj->type;
		EDB2PG *par = edb2pg_create();
		CONVARG *p;
		char *cmd;
		size_t n;

		par->pg = epg_connect(arg->db, 0);

		if	(!par->pg)
			exit(EXIT_FAILURE);

		add_pdef(par, type, NULL, 0);

#if	E2P_DEBUG
		put_pdef(par, ioerr);
#endif

		if	(arg->drop)
		{
			io_xprintf(par->io, "DROP TABLE %s", arg->name);
			edb2pg_cmd(par);
		}

		io_xprintf(par->io, "CREATE TABLE %s (", arg->name);

		for (n = 0, p = par->conv.data; n < par->conv.used; n++)
			io_xprintf(par->io, "%s%s %s", n ? ", " : "",
				p[n].name, p[n].pg_type);
		
		if	(arg->par)
		{
			io_puts(", ", par->io);
			io_puts(arg->par, par->io);
		}

		io_puts(")", par->io);

		if	(!edb2pg_cmd(par))
			exit(EXIT_FAILURE);

		io_xprintf(par->io, "COPY %s FROM stdin", arg->name);
		cmd = edb2pg_get(par);
		PG_info(par->pg, cmd);
		PG_exec(par->pg, cmd, PGRES_COPY_IN);
		par->endline = 1;
		par->copy = 1;
		edb->write = write_pg;
		edb->opar = par;
	}

	edb_head(edb, io, mode->header);
	io_puts("@head\n", io);
	io_puts("#include <pg.hdr>\n", io);
	io_puts("@pg_copy ", io);

	if	(arg->db)
	{
		io_puts(arg->db, io);
		io_putc(':', io);
	}

	io_puts(arg->name, io);
	io_putc('\n', io);

	parg_free(mode->par);
	mode->par = NULL;
}

static void pset_pg (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	if	((mode->par = parg_alloc(opt, arg)))
	{
		mode->name = def->name;
		mode->init = init_pg;
		mode->split = 0;
	}
	else	dbg_error(NULL, E_NAME, NULL);
}

static EDBPrintDef pdef_pg = { "pg", "[flag]=db:name", pset_pg, NULL,
	":*:write out as PostrgeSQL table"
	":de:Ausgabe in eine PostgreSQL-Tabelle"
};


static int read_pg (EfiType *type, void *data, void *p_par)
{
	EDB2PG *par = p_par;
	CONVARG *get;
	EfiObj *obj;
	size_t n;
	char *p;
	int c;
	
	sb_clean(&par->buf);

	if	(!par->buf.nfree)
		sb_expand(&par->buf, 2);

	while ((c = PQgetline(par->pg->conn, 
		(char *) par->buf.data + par->buf.pos, par->buf.nfree)) == 1)
	{
		par->buf.pos += par->buf.nfree - 1;
		par->buf.nfree = 1;
		sb_expand(&par->buf, 2);
	}

	if	(c == EOF)	return 0;

	p = (char *) par->buf.data;

	if	(p[0] == '\\' && p[1] == '.')
		return 0;

	n = 0;
	get = par->conv.data;

	while (*p && n < par->conv.used)
	{
		char *arg = p;

		for (; *p; p++)
		{
			if	(*p == '\t')
			{
				*p = 0;
				p++;
				break;
			}
		}

		if	(get[n].base)
		{
			get[n].base->data = (char *) data + get[n].offset;
			obj = get[n].st->member(get[n].st, get[n].base);

			if	(obj)
			{
				get[n].conv(get[n].type, obj->data, arg);
				SyncLval(obj);
				UnrefObj(obj);
			}
		}
		else
		{
			get[n].conv(get[n].type,
				(char *) data + get[n].offset, arg);
		}

		n++;
	}

	for (; n < par->conv.used; n++)
		CleanData(get[n].type, (char *) data + get[n].offset, 0);

	return 1;
}

static void meta_copy (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	EDB2PG *par = edb2pg_create();
	char *p, *db;

	if	((p = strrchr(arg, ':')) != 0)
	{
		db = mstrncpy(arg, p - arg);
		arg = p + 1;
	}
	else	db = NULL;

	par->pg = epg_connect(db, 0);
	memfree(db);

	io_xprintf(par->io, "COPY %S TO stdin", arg);
	PG_exec(par->pg, edb2pg_get(par), PGRES_COPY_OUT);
	par->endline = 0;
	par->copy = 1;

	add_pdef(par, meta->cur->obj->type, NULL, 0);
#if	E2P_DEBUG
	put_pdef(par, ioerr);
#endif
	edb_input(meta->cur, read_pg, par);
}

static void set_data (EDB2PG *par, EfiType *type, void *data)
{
	PGresult *res; 
	CONVARG *get;
	int i, n;

	res = par->pg->res;
	get = par->conv.data;
	n = PQnfields(res);

	for (i = 0; i < n && i < par->conv.used; i++)
		get[i].conv(get[i].type, (char *) data + get[i].offset,
			PQgetvalue(res, 0, i));

	for (; i < par->conv.used; i++)
		CleanData(get[i].type, (char *) data + get[i].offset, 0);
}

static int read_query (EfiType *type, void *data, void *p_par)
{
	EDB2PG *par = p_par;
	PGresult *res;

	PG_query(par->pg, "FETCH 1 IN tmpcursor");
	res = par->pg->res;

	if	(!res || PQntuples(res) == 0)
		return 0;

	set_data(par, type, data);
	return 1;
}

static char *parse_query (IO *io, StrBuf *buf)
{
	int newline = 1;
	int c;

	while ((c = io_skipcom(io, NULL, newline)) != EOF)
	{
		if	(c == '@' && newline)
		{
			io_ungetc(c, io);
			break;
		}

		newline = (c == '\n');
		sb_putc(newline ? ' ' : c, buf);
	}

	sb_putc(0, buf);
	buf->pos--;

	while (buf->pos && buf->data[buf->pos - 1] == ' ')
		buf->data[--buf->pos] = 0;

	return (char *) buf->data;
}

static void meta_query (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	EDB2PG *par = edb2pg_create();
	void *tinfo;
	
	par->pg = epg_connect(arg, 0);

	if	(meta->cur && meta->cur->read)
	{
		meta->prev = edb_paste(meta->prev, meta->cur);
		meta->cur = NULL;
	}

	tinfo = meta->cur ? NULL : PGType_create(par->pg);

	PG_command(par->pg, "BEGIN");
	par->endline = 0;
	sb_clean(&meta->buf);
	sb_puts("DECLARE tmpcursor CURSOR FOR ", &meta->buf);
	PG_command(par->pg, parse_query(meta->ctrl, &meta->buf));

	if	(!meta->cur)
	{
		PGresult *res;
		int i, nfields;
		EfiStruct *var, **ptr;
		CONVARG *conv;
		EfiType *type;
		IO *log;

		PG_query(par->pg, "FETCH 0 IN tmpcursor");
		res = par->pg->res;
		log = LogOut(NULL, DBG_INFO);

		if	(!res)
		{
			dbg_error(NULL, E_FETCH, NULL);
			return;
		}

		nfields = PQnfields(res);
		var = NULL;
		ptr = &var;

		for (i = 0; i < nfields; i++)
		{
			conv = vb_next(&par->conv);
			conv->name = mstrcpy(PQfname(res, i));
			conv->pg_type = PGType_name(tinfo, PQftype(res, i));
			conv->type = &Type_str;
			conv->offset = 0;
			conv->pfunc = PrintData;
			conv->conv = conv_str;

			if	(mstrcmp(conv->pg_type, "int4") == 0)
			{
				conv->type = &Type_int;
				conv->conv = conv_int;
			}
			else if	(mstrcmp(conv->pg_type, "int8") == 0)
			{
				conv->type = &Type_varint;
				conv->conv = conv_any;
			}
			else if	(mstrcmp(conv->pg_type, "date") == 0)
			{
				conv->type = &Type_Date;
				conv->conv = conv_date;
			}
			else if	(strncmp(conv->pg_type, "float", 5) == 0)
			{
				conv->type = &Type_double;
				conv->conv = conv_any;
			}
			else if	(mstrcmp(conv->pg_type, "text") == 0)
			{
				conv->type = &Type_str;
				conv->conv = conv_any;
			}

			io_xprintf(log, "%s: %s: %s -> %s\n", def->name,
				conv->name, conv->pg_type, conv->type->name);
			*ptr = NewEfiStruct(conv->type, conv->name, 0);
			ptr = &(*ptr)->next;
		}

		type = MakeStruct(NULL, NULL, var);
		conv = par->conv.data;

		for (var = type->list; var; var = var->next, conv++)
			conv->offset = var->offset;

		meta->cur = edb_create(type);

		if	(meta->cur)
		{
			meta->cur->desc = meta->desc;
			meta->desc = NULL;
		}
	}
	else	add_pdef(par, meta->cur->obj->type, NULL, 0);

	PGType_clean(tinfo);
	edb_input(meta->cur, read_query, par);
}

static void meta_connect (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	rd_deref(epg_connect(arg, 1));
}

static void meta_group (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	char *p = EDBMeta_par(meta, 0);

	if	(!cube.grp)
		cube.grp = NewEfiVec(&Type_PG_Grp, NULL, 0);

	PG_Grp_parse(cube.grp, p);
}

static void meta_cursor (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	char *cmd;
	memfree(cube.cname);
	cube.cname = mstrcpy(arg ? arg : "tmpcursor");
	cmd = msprintf("declare %s cursor for\n%s", cube.cname,
		EDBMeta_par(meta, 0));
	PG_serialize(cube.pg);
	PG_command(cube.pg, cmd);
	memfree(cmd);
}

static void meta_create (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	PGCube_mktype(&cube);
	PGCube_create(&cube, arg);
	meta->prev = edb_paste(meta->prev, meta->cur);
	meta->cur = cube.edb;

	if	(meta->cur)
	{
		meta->cur->desc = meta->desc;
		meta->desc = NULL;
	}

	cube.edb = NULL;
}

static void meta_close (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	epg_close();
}

static void meta_exec (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	PG *pg = epg_connect(arg, 0);
	sb_clean(&meta->buf);
	PG_command(pg, parse_query(meta->ctrl, &meta->buf));
	rd_deref(pg);
}

static EDBMetaDef mdef[] = {
	{ "pg_copy", meta_copy, 1,
		":*:read data from postgresql table"
		":de:Daten aus PostgreSQL tabelle lesen"
	},
	{ "pg_query", meta_query, 0,
		":*:read data from postgresql query"
		":de:Daten über SQL-Abfrage holen"
	},
	{ "pg_connect", meta_connect, 0,
		":*:create persistent connection to postgresql server"
		":de:Beständige Verbindung zum PostgeSQL-Server aufbauen"
	},
	{ "pg_close", meta_close, 0,
		":*:close persistent connection to postgresql server"
		":de:Beständige Verbindung zum PostgeSQL-Server schließen"
	},
	{ "pg_exec", meta_exec, 0,
		":*:send SQL-query to server"
		":de:SQL-Kommando zum Server senden"
	},
	{ "pg_group", meta_group, 0,
		":*:define grouping for cube generation"
		":de:Gruppen für die Würfelkonstruktion festlegen"
	},
	{ "pg_cursor", meta_cursor, 0,
		":*:define cursor for data query"
		":de:Cursor für Datenabfrage definieren"
	},
	{ "pg_create", meta_create, 0,
		":*:Create EDB-Datafile with given cursor and group"
		":de:Erzeuge ein EDB-Datenfile mit entsprechender"
		"Cursor und Gruppendefinition"
	},
};

void PG_edb (void)
{
	PGCube_setup();
	AddEDBPrintDef(&pdef_pg, 1);
	AddEDBMetaDef(mdef, tabsize(mdef));
	proc_clean(edb_closeall, NULL);
}

#endif
