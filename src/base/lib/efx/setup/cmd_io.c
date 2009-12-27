/*	IO - Strukturen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/pconfig.h>


/*	Funktionsmakros
*/

#define	RVIO	Val_io(rval)
#define	RVINT	Val_int(rval)
#define	RVSTR	Val_str(rval)

#define	IO(n)	Val_io(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	STR(n)	Val_str(arg[n])
#define	CHAR(n)	Val_char(arg[n])

CEXPR(k_str2io, RVIO = io_mstr(mstrcpy(STR(0))))
CEXPR(f_open, RVIO = io_fileopen(STR(0), STR(1)))
CEXPR(f_popen, RVIO = io_popen(STR(0), STR(1)))
CEXPR(f_findopen, RVIO = io_findopen(STR(0), STR(1), STR(2), STR(3)))
CEXPR(f_iaopen, RVIO = io_interact(STR(0), STR(1)))

CEXPR(f_bigbuf, RVIO = io_bigbuf(INT(0), STR(1)))
CEXPR(f_tmpfile, RVIO = io_tmpfile())
CEXPR(f_lmark, RVIO = io_lmark(io_refer(IO(0)), STR(1), STR(2), INT(3)))
CEXPR(f_indent, RVIO = io_indent(io_refer(IO(0)), INT(1), INT(2)))
CEXPR(f_pushio, io_push(IO(0), io_refer(IO(1))))
CEXPR(f_popio, RVIO = io_pop(IO(0)))

CEXPR(f_close, RVINT = io_close(IO(0)); IO(0) = NULL)
CEXPR(f_rewind, RVINT = io_rewind(IO(0)))

CEXPR(f_getc, RVINT = io_getc(IO(0)))
CEXPR(f_ungetc, RVINT = io_ungetc(INT(1), IO(0)))
CEXPR(f_putc, RVINT = io_putc(INT(1), IO(0)))
CEXPR(f_puts, RVINT = io_puts(STR(1), IO(0)))
CEXPR(f_xputs, RVINT = io_xputs(STR(1), IO(0), STR(2)))
CEXPR(f_mputs, RVINT = io_mputs(STR(1), IO(0), STR(2)))
CEXPR(f_protect, io_protect(IO(0), INT(1)))
CEXPR(f_submode, io_submode(IO(0), INT(1)))

CEXPR(f_xgetc, RVINT = io_xgetc(IO(0), STR(1)))
CEXPR(f_mgetc, RVINT = io_mgetc(IO(0), INT(1)))

CEXPR(f_xgets, RVSTR = io_xgets(IO(0), STR(1)))
CEXPR(f_mgets, RVSTR = io_mgets(IO(0), STR(1)))

CEXPR(f_remove, RVINT = remove(STR(0)))

static ObjList_t **end_scan(strbuf_t *sb, ObjList_t **ptr)
{
	char *p;
	int n;

	n = sb_size(sb);
	p = memalloc(n + 1);
	memcpy(p, sb->data, n);
	p[n] = 0;
	sb_setpos(sb, 0);
	sb_sync(sb);

	*ptr = NewObjList(str2Obj(p));
	return &(*ptr)->next;
}

static void f_scanline (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, **ptr;
	char *delim, *end;
	strbuf_t *sb;
	char *p;
	io_t *io;
	int c;

	io = IO(0);
	delim = STR(1);
	end = STR(2);
	Val_list(rval) = NULL;

	for (;;)
	{
		c = io_eat(io, delim);

		if	(c == EOF)		return;
		else if	(listcmp(end, c))	io_getc(io);
		else				break;
	}

	list = NULL;
	ptr = &list;
	sb = new_strbuf(0);

	for (;;)
	{
		c = io_getc(io);

		if	(c == EOF || listcmp(end, c))
		{
			ptr = end_scan(sb, ptr);
			break;
		}
		else if	(listcmp(delim, c))
		{
			ptr = end_scan(sb, ptr);
			c = io_eat(io, delim);

			if	(listcmp(end, c))	break;
		}
		else if	(c == '\\')
		{
			c = io_getc(io);

			if	(c == EOF)	break;
			else if	(c == '\n')	io_linemark(io);
			else			sb_putc(c, sb);
		}
		else if	(c == '"')
		{
			p = io_xgets(io, "\"");
			io_getc(io);
			sb_puts(p, sb);
		}
		else if	(c == '\'')
		{
			p = io_mgets(io, "'");
			io_getc(io);
			sb_puts(p, sb);
		}
		else	sb_putc(c, sb);
	}

	del_strbuf(sb);
	Val_list(rval) = list;
}

static void f_getline (Func_t *func, void *rval, void **arg)
{
	io_t *io;
	ObjList_t *list;
	Obj_t *obj;
	char *p;
	int c;

	io = IO(0);
	list = Val_list(arg[1]);

	for (;;)
	{
		c = io_eat(io, "%s");

		if	(c == EOF)
		{
			Val_bool(rval) = 0;
			return;
		}
		else if	(c == '#')
		{
			io_mcopy(io, NULL, "\n");
		}
		else	break;
	}

	if	(list == NULL)
		io_mcopy(io, NULL, "\n");

	while (list != NULL)
	{
		io_eat(io, " \t");

		p = io_xgets(io, list->next ? "%s" : "\n");

		if	(list->obj == NULL || list->obj->lref == NULL)
		{
			reg_cpy(1, func->name);
			errmsg(MSG_EFMAIN, 88);
			FREE(p);
			continue;
		}

		CleanData(list->obj->type, list->obj->data);
		obj = p ? EvalObj(str2Obj(p), list->obj->type) : NULL;

		if	(obj)
		{
			CopyData(list->obj->type, list->obj->data, obj->data);
			UnrefObj(obj);
		}

		list = list->next;
	}

	Val_bool(rval) = 1;
}

static void f_fgets (Func_t *func, void *rval, void **arg)
{
	io_t *in;
	strbuf_t *sb;
	int c, end;

	in = IO(0);
	sb = new_strbuf(0);
	end = Val_int(arg[1]);

	while ((c = io_mgetc(in, 1)) != EOF)
	{
		if	(c == end)
		{
			sb_putc(0, sb);
			RVSTR = sb2mem(sb);
			return;
		}

		sb_putc(c, sb);
	}

	RVSTR = sb2str(sb);
}

static void f_ngets (Func_t *func, void *rval, void **arg)
{
	io_t *in = IO(0);
	int n = Val_int(arg[1]);
	char *buf = memalloc(n);

	if	((RVSTR = io_gets(buf, n, in)) == NULL)
		memfree(buf);
}

static void f_copy (Func_t *func, void *rval, void **arg)
{
	int c, n;
	io_t *in, *out;

	in = IO(0);
	out = IO(1);
	n = 0;

	while ((c = io_getc(in)) != EOF)
		if (io_putc(c, out) != EOF) n++;

	RVINT = n;
}

static void f_filter (Func_t *func, void *rval, void **arg)
{
	io_t *io;
	char *name;
	char *cmd;
	strbuf_t *buf;
	int flag, c;

	if	((cmd = STR(0)) == NULL)
	{
		Val_str(rval) = mstrcpy(STR(1));
		return;
	}

	name = tmpnam(NULL);
	io = io_fileopen(name, "w");
	io_puts(STR(1), io);
	io_putc('\n', io);
	io_close(io);

	cmd = mstrcat(" < ", cmd, name, NULL);
	io = io_popen(cmd, "r");
	buf = new_strbuf(0);
	flag = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(flag)	sb_putc('\n', buf);

		if	(c != '\n')
		{
			sb_putc(c, buf);
			flag = 0;
		}
		else	flag = 1;
	}

	io_close(io);
	FREE(cmd);
	remove(name);
	Val_str(rval) = sb2str(buf);
}

/*	Dokumentkontrolle
*/


static FuncDef_t fdef_io[] = {
	{ FUNC_RESTRICTED, &Type_io, "str ()", k_str2io },
	{ 0, &Type_io, "tmpfile ()", f_tmpfile },
	{ 0, &Type_io, "open (str name, str mode)", f_open },
	{ 0, &Type_io, "popen (str name, str mode)", f_popen },
	{ 0, &Type_io, "iaopen (str prompt, str hist = NULL)", f_iaopen },
	{ 0, &Type_io, "bigbuf (int size, str pfx = NULL)", f_bigbuf },
	{ 0, &Type_io, "findopen (str path, str name, str type = NULL, \
str mode = \"r\")", f_findopen },
	{ 0, &Type_io, "linemark (IO io, str pre = \"%4d\t\", \
str post = NULL, bool flag = false)", f_lmark },
	{ 0, &Type_io, "indent (IO io, char c = '\t', int n = 1)", f_indent },
	{ 0, &Type_void, "IO::push (IO io)", f_pushio },
	{ 0, &Type_io, "IO::pop (void)", f_popio },
	{ 0, &Type_int, "close (IO & io)", f_close },
	{ 0, &Type_int, "rewind (IO io)", f_rewind },
	{ 0, &Type_int, "copy (IO in, IO out)", f_copy },

	{ 0, &Type_int, "IO::getc ()", f_getc },
	{ 0, &Type_int, "IO::ungetc (int x)", f_ungetc },
	{ 0, &Type_int, "IO::putc (int x)", f_putc },

	{ 0, &Type_int, "IO::mgetc (bool flag = true)", f_mgetc },
	{ 0, &Type_int, "IO::xgetc (str delim = NULL)", f_xgetc },

	{ 0, &Type_str, "IO::mgets (str delim = NULL)", f_mgets },
	{ 0, &Type_str, "IO::xgets (str delim = NULL)", f_xgets },

	{ 0, &Type_int, "IO::puts (str s)", f_puts },
	{ 0, &Type_int, "IO::mputs (str s, str delim = NULL)", f_mputs },
	{ 0, &Type_int, "IO::xputs (str s, str delim = NULL)", f_xputs },
	{ 0, &Type_void, "IO::protect (bool mode)", f_protect },
	{ 0, &Type_void, "IO::submode (bool mode)", f_submode },

	{ 0, &Type_list, "scanline (IO io, str delim = \"%s\", \
str end = \"\\n\")", f_scanline },
	{ 0, &Type_bool, "getline (IO io, ...)", f_getline },
	{ 0, &Type_str, "fgets (IO io, int end = '\n')", f_fgets },
	{ 0, &Type_str, "ngets (IO io, int n)", f_ngets },

	{ 0, &Type_int, "remove (str name)", f_remove },
	{ 0, &Type_str, "filter (str cmd, str par = NULL)", f_filter },
};


/*	Initialisierung
*/

void CmdSetup_io(void)
{
	AddFuncDef(fdef_io, tabsize(fdef_io));
}
