/*	Metadefinitionen initialisieren
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDBMeta.h>
#include <EFEU/LangDef.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>

#define	FMT_UNIT "edb_meta: parameter \"$1\": undefined unit key $2.\n"
#define	FMT_XARG "edb_meta: parameter \"$1\": argument \"$2\" ignored.\n"

#define	TEST	1	/* Testkommando */

#if	TEST
static void meta_test (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	char *desc = mlangcpy(def->desc, NULL);
	io_xprintf(ioerr, "/* %s */\n", desc);
	io_xprintf(ioerr, "@%s: %#s\n", def->name, arg);
	memfree(desc);
}
#endif

static void meta_head (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	IO *cin = io_cmdpreproc(io_cstr(EDBMeta_par(meta, 0)));
	CmdEval(cin, NULL);
	io_close(cin);
}

void EDBMeta_type (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	EfiType *type =	edb_type(EDBMeta_par(meta, 0));

	if	(type)
	{
		meta->prev = edb_paste(meta->prev, meta->cur);
		meta->cur = edb_create(type);
		meta->cur->desc = meta->desc;
		meta->desc = NULL;
	}
}

static void meta_expr (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	IO *io = io_cmdpreproc(io_cstr(EDBMeta_par(meta, 0)));
	EfiObj *obj = meta->cur->obj;

	PushVarTab(RefVarTab(obj->type->vtab), RefObj(obj));
	obj = Parse_block(io, EOF);
	PopVarTab();

	if	(obj && meta->base)
	{
		EfiBlock *block = obj->data;
		VarTab_xadd(block->tab, "data", NULL,
			io2Obj(rd_refer(meta->base)));
	}

	io_close(io);
	edb_expr(meta->cur, obj);
}

static void meta_data (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	io_close(meta->ctrl);
	meta->ctrl = NULL;

	if	(!meta->cur->read)
		edb_data(meta->cur, rd_refer(meta->base), arg);
}

static void meta_join (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	meta->cur = edb_join(meta->prev, meta->cur, EDBMeta_par(meta, 0));
	meta->prev = NULL;
}

static void meta_filter (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	meta->cur = edb_script(meta->cur, io_cstr(EDBMeta_par(meta, 0)));
}

static EDBMetaDef mdef[] = {
	{ "end", NULL, 0,
		":*:empty statement"
		":de:Leerkommando"
	},
#if	TEST
	{ "test", meta_test, 0,
		":*:test command"
		":de:Testkommando"
	},
#endif
	{ "head", meta_head, 0,
		":*:header definition"
		":de:Headerdefinitionen laden"
	},
	{ "type", EDBMeta_type, 0,
		":*:data type definition"
		":de:Datentypdefinition"
	},
	{ "expr", meta_expr, 1,
		":*:expression"
		":de:Ausdruck zur Datensatzgenerierung"
	},
	{ "data", meta_data, 1,
		":*:start of data section"
		":de:Start des Datenteils"
	},
	{ "file", EDBMeta_file, 0,
		":*:file registration"
		":de:Datei registrieren"
	},
	{ "import", EDBMeta_import, 1,
		":*:import external data files"
		":de:Externe Datenfiles einbinden"
	},
	{ "paste", EDBMeta_paste, 0,
		":*:paste data files"
		":de:Datenfiles zusammenhängen"
	},
	{ "join", meta_join, 0,
		":*:join data files"
		":de:Datenfiles verknüpfen"
	},
	{ "filter", meta_filter, 0,
		":*:append filter to data base"
		":de:Filter auf Datenbank anwenden"
	},
};

void SetupEDBMeta (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;
	AddEDBMetaDef(mdef, tabsize(mdef));
}
