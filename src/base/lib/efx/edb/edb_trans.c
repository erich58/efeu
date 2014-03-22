/*	Datenbanktransformation

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
#include <EFEU/EDBFilter.h>
#include <EFEU/dl.h>
#include <ctype.h>

#define	ERR_CMD "[edb:cmd]$!: unknown command $1.\n"
#define	ERR_VEC	"[edb:vec]$!: vector could only used on the whole.\n"
#define	ERR_ORD "[edb:ord]$!: invalid order of parameters.\n"
#define	ERR_NOC "[edb:noc]$!: use of C-expression not available.\n"

#define	TRANS_VEC	0x1
#define	TRANS_COPY	0x2
#define	TRANS_CEXPR	0x4
#define	TRANS_REDUCE	0x8

#define EXTNAME "custom_trans_func"

typedef struct {
	REFVAR;
	EDB *base;
	EfiFunc *cfunc;
	EfiObj *expr;
	EfiObj *obj;
	EfiVarTab *tab;
	EfiVec *src;
	EfiVec *tg;
	CmpDef *cmp;
	unsigned flags;
	int (*func) (EfiObj *tg, EfiObj *src);
	int (*vfunc) (EDB *base, EfiVec *tg, EfiVec *src);
} TRANS;

static void trans_clean (void *data)
{
	TRANS *trans = data;
	rd_deref(trans->base);
	rd_deref(trans->cfunc);
	UnrefObj(trans->expr);
	UnrefObj(trans->obj);
	DelVarTab(trans->tab);
	rd_deref(trans->src);
	rd_deref(trans->tg);
	rd_deref(trans->cmp);
	memfree(trans);
}

static RefType trans_reftype = REFTYPE_INIT("EDB_TRANS", NULL, trans_clean);

static void *trans_alloc (EDB *edb)
{
	TRANS *trans;
	
	trans = memalloc(sizeof *trans);
	trans->base = edb;
	trans->expr = NULL;
	trans->obj = NULL;
	trans->tab = NULL;
	trans->src = NULL;
	trans->tg = NULL;
	trans->cmp = NULL;
	trans->flags = 0;
	return rd_init(&trans_reftype, trans);
}

static int can_reduce (EfiType *type)
{
	if	(!type->list)		return 0;
	if	(type->list->next)	return 0;
	if	(type->list->dim)	return 0;

	return 1;
}

static void make_type (TRANS *trans)
{
	EfiType *type;

	if	(trans->obj)
		return;

	if	(trans->cfunc)
	{
		type = trans->cfunc->type;
	}
	else	type = trans->base->obj->type;

	if	(trans->flags & TRANS_REDUCE && can_reduce(type))
	{
		type = type->list->type;
	}

	trans->obj = LvalObj(NULL, type);
}

static void make_tab (TRANS *trans)
{
	if	(trans->tab)	return;

	make_type(trans);
	trans->tab = VarTab(NULL, 0);

	if	(trans->flags & TRANS_VEC)
	{
		VarTab_xadd(trans->tab, "base", NULL,
			NewPtrObj(&Type_EDB, rd_refer(trans->base)));
		trans->src = NewEfiVec(trans->base->obj->type, NULL, 0);
		VarTab_xadd(trans->tab, "src", NULL,
			NewPtrObj(&Type_vec, rd_refer(trans->src)));
		trans->tg = NewEfiVec(trans->obj->type, NULL, 0);
		VarTab_xadd(trans->tab, "tg", NULL,
			NewPtrObj(&Type_vec, rd_refer(trans->tg)));
	}
	else
	{
		VarTab_xadd(trans->tab, "src", NULL, RefObj(trans->base->obj));
		VarTab_xadd(trans->tab, "tg", NULL, RefObj(trans->obj));
	}
}

typedef struct {
	char *name;
	char *args;
	void (*eval) (TRANS *trans, const char *opt, const char *arg);
	char *desc;
} TPAR;

static void tpar_list (IO *out);


static void trans_var (TRANS *trans, const char *opt, const char *list)
{
	if	(trans->obj)
		log_error(edb_err, ERR_ORD, NULL);

	if	(!opt || *opt == 'r')
		trans->flags |= TRANS_REDUCE;

	trans->cfunc = ConstructObjFunc(NULL, list, trans->base->obj);
}

static void trans_vec (TRANS *trans, const char *opt, const char *cmp)
{
	if	(trans->tab)
		log_error(edb_err, ERR_ORD, NULL);

	trans->flags |= TRANS_VEC;

	if	(opt && *opt == 'c')
		trans->flags |= TRANS_COPY;

	if	(cmp)
		trans->cmp = cmp_create(trans->base->obj->type, cmp, NULL);
}

static void set_func (TRANS *trans, IO *io, int delim)
{
#if	HAS_DLFCN
	static int lfnr = 0;

	char *dir;
	char *name;
	char *p;
	void *handle;
	void *ptr;
	IO *out;
	int c;
	int newline;
	int quote;
	int depth;

	dir = newtempdir(NULL, NULL);
	name = msprintf("%s_%d", EXTNAME, lfnr++);

	p = mstrpaste("/", dir, "trans.c");
	out = io_fileopen(p, "w");
	memfree(p);

	io_puts("#include <EFEU/EDB.h>\n\n", out);
	io_puts("#include <EFEU/cmdeval.h>\n\n", out);

	if	(trans->flags & TRANS_VEC)
	{
		io_xprintf(out, "int %s (EDB *base, "
			"EfiVec *tg, EfiVec *src);\n\n", name);
		io_xprintf(out, "int %s (EDB *base, "
			"EfiVec *tg, EfiVec *src)\n", name);
	}
	else
	{
		io_xprintf(out, "int %s (EfiObj *tg, EfiObj *src);\n\n", name);
		io_xprintf(out, "int %s (EfiObj *tg, EfiObj *src)\n", name);
	}

	io_puts("{\n", out);
	newline = 1;
	quote = 0;
	depth = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			io_putc(c, out);
			newline = 1;
			continue;
		}

		if	(newline && !quote)
		{
			io_putc('\t', out);
			newline = 0;
		}

		if	(c == '"')	quote = !quote;

		if	(!quote)
		{
			if	(c == '{')		depth++;
			else if	(depth && c == '}')	depth--;
			else if	(c == delim)		break;
		}

		if	(c == '$')
		{
			char *p = psubexpand(NULL, io, 0, NULL);

			if	(quote)
				io_xputs(p, out, "\"");
			else	io_puts(p, out);
		}
		else	io_putc(c, out);
	}

	if	(!newline)	io_putc('\n', out);

	io_puts("\treturn 0;\n}\n", out);
	io_close(out);

	p = mstrpaste("/", dir, "run.sh");
	out = io_fileopen(p, "w");
	memfree(p);

	io_xprintf(out, "#!/bin/sh\n");
	io_xprintf(out, "cd %s\n", dir);
	io_xprintf(out, "efeucc -o trans.so trans.c -shared -nostdlib");
	io_xprintf(out, " || nl -ba trans.c\n");
	io_close(out);

	p = msprintf("/bin/sh %s/run.sh >&2", dir);
	callproc(p);
	memfree(p);

	p = msprintf("%s/trans.so", dir);
	handle = dlopen(p, RTLD_GLOBAL|RTLD_NOW);
	memfree(p);

	if	(handle == NULL)
	{
		io_xprintf(ioerr, "dlopen: %s\n", dlerror());
	}
	else if	((ptr = dlsym(handle, name)) == NULL)
	{
		io_xprintf(ioerr, "dlsym: %s\n", dlerror());
		dlclose(handle);
	}
	else if	(trans->flags & TRANS_VEC)
	{
		trans->vfunc = ptr;
	}
	else	trans->func = ptr;

	memfree(name);
	deltempdir(dir);
#else
	log_error(edb_err, ERR_NOC, NULL);
#endif
}

static void trans_expr (TRANS *trans, const char *opt, const char *def)
{
	IO *io;
	int delim;

	make_tab(trans);

	if	(!def)	return;

	if	(*def == '{')
	{
		delim = '}';
		def++;
	}
	else	delim = EOF;

	PushVarTab(RefVarTab(trans->tab), NULL);
	io = io_cstr(def);

	if	(mstrcmp(opt, "C") == 0)
	{
		set_func(trans, io, delim);
	}
	else
	{
		trans->expr = Parse_block(io, delim);
	}

	io_close(io);
	PopVarTab();
}


static TPAR tpar[] = {
	{ "var", "[r]=list", trans_var,
		":*:list of output variables. If flag 'r' is given\n"
		"a structure with a single componend is reduced.\n"
		":de:Liste der Ausgabevariablen. Bei Flag 'r' wird eine\n"
		"Datenstruktur mit nur einer Komponente reduziert.\n"
	}, { "vec", "[flags]=cmp", trans_vec,
		":*:evaluate data in vector mode\n"
		"cmp determines related records\n"
		"available flags: copy\n"
		":de:Datensätze im Vektormodus verarbeiten\n"
		"cmp bestimmt die zusammengehörigen Datensätze\n"
		"Verfügbare Flags: copy\n"
	}, { "expr", "[lang]={exp}", trans_expr,
		":*:expression to process the records\n"
		"alternative language: C\n"
		":de:Ausdruck zur Überarbeitung der Datensätze\n"
		"Alternative Sprachen: C\n"
	}
};

static void tpar_list (IO *out)
{
	int i;

	for (i = 0; i < tabsize(tpar); i++)
	{
		IO *io;

		io_puts(tpar[i].name, out);
		io_puts(tpar[i].args, out);
		io_putc('\n', out);

		io = io_lmark(io_refer(out), "\t", NULL, 0);
		io_langputs(tpar[i].desc, io);
		io_close(io);
	}
}

static void tpar_eval (TRANS *trans, AssignArg *arg)
{
	int i;

	if	(arg->name == NULL || arg->name[0] == 0)
		return;

	if	(arg->name[0] == '?')
	{
		tpar_list (ioerr);
		exit(EXIT_SUCCESS);
	}

	for (i = 0; i < tabsize(tpar); i++)
	{
		if	(mstrcmp(tpar[i].name, arg->name) == 0)
		{
			tpar[i].eval(trans, arg->opt, arg->arg);
			return;
		}
	}

	log_error(edb_err, ERR_CMD, "s", arg->name);
}

static void copy_data (TRANS *trans, void *tg, void *src)
{
	if	(trans->cfunc)
	{
		trans->cfunc->eval(trans->cfunc, tg, &src);
	}
	else	AssignData(trans->obj->type, tg, src);
}

static int std_read (EfiType *type, void *data, void *par)
{
	TRANS *trans = par;

	while (edb_read(trans->base))
	{
		copy_data(trans, data, trans->base->obj->data);
		CmdEval_stat = 0;

		if	(trans->func)
		{
			CmdEval_stat = trans->func(trans->obj,
				trans->base->obj);
		}
		else if	(trans->expr)
		{
			PushVarTab(RefVarTab(trans->base->obj->type->vtab),
				RefObj(trans->base->obj));
			PushVarTab(RefVarTab(trans->obj->type->vtab),
				RefObj(trans->obj));
			PushVarTab(RefVarTab(trans->tab), NULL);
			UnrefEval(RefObj(trans->expr));
			PopVarTab();
			PopVarTab();
			PopVarTab();
		}

		switch (CmdEval_stat)
		{
		case 0:
			return 1;
		case CmdEval_Cont:
			continue;
		case CmdEval_Break:
		case CmdEval_Return:
		default:
			return 0;
		}
	}

	return 0;
}

static int vec_load (TRANS *trans)
{
	if (!edb_read(trans->base))
		return 0;

	EfiVec_resize(trans->src, 0);
	CopyData(trans->src->type, vb_next(&trans->src->buf),
		trans->base->obj->data);

	if	(!trans->cmp)
		return 1;

	while (edb_read(trans->base))
	{
		if	(cmp_data(trans->cmp, trans->src->buf.data,
				trans->base->obj->data) != 0)
		{
			edb_unread(trans->base);
			break;
		}

		CopyData(trans->src->type, vb_next(&trans->src->buf),
			trans->base->obj->data);
	}

	return 1;
}

static int vec_read (EfiType *type, void *data, void *par)
{
	TRANS *trans = par;

	while (trans->tg->buf.used == 0)
	{
		if	(!vec_load(trans))
			return 0;

		if	(trans->flags & TRANS_COPY)
		{
			char *sp, *tp;
			size_t n;

			n = trans->src->buf.used;
			EfiVec_resize(trans->tg, n);
			sp = trans->src->buf.data;
			tp = trans->tg->buf.data;

			while (n-- > 0)
			{
				copy_data(trans, tp, sp);
				sp += trans->src->buf.elsize;
				tp += trans->tg->buf.elsize;
			}
		}

		CmdEval_stat = 0;

		if	(trans->vfunc)
		{
			CmdEval_stat = trans->vfunc(trans->base,
				trans->tg, trans->src);
		}
		else if	(trans->expr)
		{
			PushVarTab(RefVarTab(trans->tab), NULL);
			UnrefEval(RefObj(trans->expr));
			PopVarTab();
		}

		switch (CmdEval_stat)
		{
		case 0:
			break;
		case CmdEval_Cont:
			EfiVec_resize(trans->tg, 0);
			break;
		case CmdEval_Break:
		case CmdEval_Return:
		default:
			return 0;
		}
	}

	memcpy(data, vb_delete(&trans->tg->buf, 0, 1), trans->tg->buf.elsize);
	return 1;
}


static EDB *trans_edb (TRANS *trans, char *desc)
{
	EDB *edb;
	
	make_tab(trans);

	if	(trans->tg)
		trans->tg->buf.used = 0;

	edb = edb_alloc(RefObj(trans->obj), desc);
	edb->read = (trans->flags & TRANS_VEC) ? vec_read : std_read;
	edb->ipar = trans;

	return edb;
}

EDB *edb_trans (EDB *base, const char *def)
{
	AssignArg *arg;
	TRANS *trans;

	if	(def == NULL || *def == 0)
	{
		tpar_list(ioerr);
		exit(EXIT_SUCCESS);
	}

	trans = trans_alloc(base);

	while ((arg = assignarg(def, (char **) &def, " \t\n;")))
	{
		tpar_eval(trans, arg);
		memfree(arg);
	}

	return trans_edb(trans, mstrcpy(base->desc));
}


static EDB *fdef_trans (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	return edb_trans(base, opt);
}

EDBFilter EDBFilter_trans = EDB_FILTER(NULL,
	"trans", "[par]", fdef_trans, NULL, 
	":*:transforming records.\n"
	"trans[?] gives a list of availabel parameters.\n"
	":de:Transformieren von Datensätzen.\n"
	"Die Angabe trans[?] liefert die verfügbaren Parameter.\n"
);
