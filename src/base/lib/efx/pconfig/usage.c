/*
Hilfetextformatierung

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

#include <EFEU/ftools.h>
#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	ERR_OUT		io_popen("efeudoc -Tterm - 1>&2", "w")
#define	FMT_IDENT	"$! -- $(Ident:%s)\n"
#define	FMT_USAGE	"|$!|\n"
#define	FMT_HEAD	"\\dhead\t$!(1)\n\\mpage[1] $!\n"

#define	HLP_PFX	"help"
#define	HLP_SFX	"hlp"

char *UsageFmt = "\\Synopsis\n@synopsis\n";

char *HelpFmt = "\\mpage[1] $!\n\\Name\n@ident\n\
@synopsis -h\n\
\\Description\n@arglist 10\n\
";

static void s_head (io_t *io, const char *arg);
static void s_ident (io_t *io, const char *arg);
static void s_name (io_t *io, const char *arg);
static void s_eval (io_t *io, const char *arg);
static void s_synopsis (io_t *io, const char *arg);
static void s_arglist (io_t *io, const char *arg);
static void s_option (io_t *io, const char *arg);
static void s_arg (io_t *io, const char *arg);
static void s_environ (io_t *io, const char *arg);
static void s_proto (io_t *io, const char *arg);

static void show_arg (io_t *io, const char *str, int flag);
static void show_xarg (io_t *io, const char *str, int flag);

static void usage_short (io_t *io, pardef_t *def);
static void usage_long (io_t *io, pardef_t *def);
static void show_pdef (io_t *io, vecbuf_t *vb, void (*show) (io_t *io, pardef_t *def));
static void list_pdef (io_t *io, vecbuf_t *vb, const char *arg);


struct {
	char *name;
	void (*eval) (io_t *io, const char *arg);
} usage_cmd[] = {
	{ "head",	s_head },
	{ "ident",	s_ident },
	{ "name",	s_name },
	{ "eval",	s_eval },
	{ "synopsis",	s_synopsis },
	{ "arglist",	s_arglist },
	{ "environ",	s_environ },
	{ "proto",	s_proto },
	{ "opt",	s_option },
	{ "arg",	s_arg },
};


/*	Filter für Sonderfunktionen
*/

typedef struct {
	io_t *io;	/* Eingabestruktur */
	strbuf_t *sb;	/* Zwischenbuffer */
} UCMD;


static int uc_get (UCMD *uc);
static int uc_ctrl (UCMD *uc, int req, va_list list);
io_t *io_usage(io_t *io);

io_t *io_usage(io_t *io)
{
	if	(io)
	{
		UCMD *uc;

		uc = memalloc(sizeof(UCMD));
		memset(uc, 0, sizeof(UCMD));
		uc->io = io_lnum(io);
		uc->sb = new_strbuf(1024);
		io = io_alloc();
		io->get = (io_get_t) uc_get;
		io->ctrl = (io_ctrl_t) uc_ctrl;
		io->data = uc;
	}

	return io;
}


/*	Zeichen lesen
*/

static int uc_get(UCMD *uc)
{
	int i, c;
	void *cmd;
	char *arg;
	io_t *tmp;

	start:

/*	Gepufferte Zeichen zurückgeben
*/
	if	(uc->sb->nfree < uc->sb->size)
	{
		if	(uc->sb->pos < uc->sb->size - uc->sb->nfree)	
			return (uc->sb->data[uc->sb->pos++]);

		uc->sb->nfree = uc->sb->size;
		uc->sb->pos = 0;
	}

/*	Normale Zeichen zurückgeben
*/
	c = io_getc(uc->io);

	if	(c == '$')
	{
		tmp = io_strbuf(uc->sb);
		iocpy_psub(uc->io, tmp, c, NULL, 0);
		io_close(tmp);
		uc->sb->pos = 0;
		goto start;
	}

	if	(c != '@')	return c;

/*	Steuersequenz interpretieren
*/
	cmd = NULL;
	io_scan(uc->io, SCAN_NAME, &cmd);

	if	(cmd == NULL)
	{
		return io_getc(uc->io);
	}

	io_eat(uc->io, " \t");
	arg = io_mgets(uc->io, "\n");
	c = io_getc(uc->io);

	for (i = 0; i < tabsize(usage_cmd); i++)
	{
		if	(strcmp(usage_cmd[i].name, cmd) == 0)
		{
			tmp = io_strbuf(uc->sb);
			usage_cmd[i].eval(tmp, arg);
			io_close(tmp);
			break;
		}
	}

	memfree(cmd);
	memfree(arg);
	uc->sb->pos = 0;
	goto start;
}


/*	Kontrollfunktion
*/

static int uc_ctrl(UCMD *uc, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(uc->io);
		del_strbuf(uc->sb);
		memfree(uc);
		return stat;

	case IO_REWIND:

		if	(io_rewind(uc->io) != EOF)
		{
			sb_clear(uc->sb);
			return 0;
		}
		else	return EOF;
		
	default:

		return io_vctrl(uc->io, req, list);
	}
}


/*	Hilfsfunktionen
*/

static void usage_short(io_t *io, pardef_t *def)
{
	if	(!def->name)	return;
	if	(!def->desc)	return;

	switch (def->flag)
	{
	case P_XOPT:
	case P_OPT:
		show_xarg(io, def->name, 1);
		break;
	case P_XARG:
	case P_OPTARG:
		show_xarg(io, def->name, 0);
		break;
	case P_REGEX:
		io_puts("\\{~", io);
		show_arg(io, def->name, 0);
		io_puts("~}", io);
		break;
	default:
		show_arg(io, def->name, 0);
		break;
	}

	io_putc('\n', io);
}


static void usage_long(io_t *io, pardef_t *def)
{
	if	(def->name && def->desc)
	{
		io_t *subio;

		io_puts("[", io);

		switch (def->flag)
		{
		case P_XOPT:
		case P_OPT:
			show_arg(io, def->name, 1);
			break;
		default:
			show_arg(io, def->name, 0);
			break;
		}

		io_puts("]\n", io);
		subio = io_lmark(io_refer(io), "\t", NULL, 0);
		io_psub(subio, def->desc);
		io_close(subio);
		io_putc('\n', io);
	}
}

/*	Argumentstack auswerten
*/

static void show_pdef (io_t *io, vecbuf_t *vb, void (*show) (io_t *io, pardef_t *def))
{
	pardef_t *x;
	size_t n;

	if	(vb == NULL || show == NULL)
		return;

	for (n = vb->used, x = vb->data; n > 0; n--, x++)
		show(io, x);
}


static void list_pdef (io_t *io, vecbuf_t *vb, const char *arg)
{
	show_pdef(io, vb, usage_long);
	/*
	io_puts("\\endlist", io);
	*/
}

static void s_environ(io_t *io, const char *arg)
{
	if	(arg && strchr(arg, 'h'))
		io_puts("\\Environment\n", io);

	list_pdef(io, &PgmDefTab.env, NULL);
}

static void s_arglist(io_t *io, const char *arg)
{
	if	(arg && strchr(arg, 'h'))
		io_puts("\\Description\n", io);

	show_pdef(io, &PgmDefTab.opt, usage_long);
	show_pdef(io, &PgmDefTab.arg, usage_long);
	/*
	io_puts("\\endlist", io);
	*/
}

static void s_proto(io_t *io, const char *arg)
{
	char *ident;
	char *name;
	char *type;
	VirFunc_t *tab;
	Func_t **ftab;
	int i;

	if	(arg == NULL)	return;

	ident = NULL;
	name = (char *) arg;
	type = NULL;

	while (isspace(*name))
		name++;

	for (ident = name; *ident != 0; ident++)
	{
		if	(*ident == ':')
		{
			type = name;
			*ident = 0;
			ident++;

			if	(*ident == ':')	ident++;

			name = ident;
		}

		if	(isspace(*ident))
		{
			*ident = 0;
			ident++;
			break;
		}
	}

	if	(type)
	{
		tab = VirFunc(GetTypeFunc(GetType(type), name));
	}
	else	tab = VirFunc(GetGlobalFunc(name));

	if	(tab == NULL || tab->tab.used == 0)	return;

	ftab = tab->tab.data;

	for (i = 0; i < tab->tab.used; i++)
	{
		io_puts("<|", io);
		ListFunc(io, ftab[i]);
		io_puts("|>\\br\n", io);
	}
}

static void s_option(io_t *io, const char *arg)
{
	list_pdef(io, &PgmDefTab.opt, arg);
}

static void s_arg(io_t *io, const char *arg)
{
	list_pdef(io, &PgmDefTab.arg, arg);
}

static void s_head(io_t *io, const char *arg)
{
	io_psub(io, FMT_HEAD);
}

static void s_ident(io_t *io, const char *arg)
{
	io_psub(io, FMT_IDENT);
}

static void s_name(io_t *io, const char *arg)
{
	io_psub(io, FMT_HEAD);
	io_puts("\\Name\n", io);
	io_psub(io, FMT_IDENT);
}

static void s_eval(io_t *io, const char *arg)
{
	streval(arg);
}


static void s_synopsis(io_t *io, const char *arg)
{
	if	(arg && strchr(arg, 'h'))
		io_puts("\\Synopsis\n", io);

	io_puts("\\hang\n", io);
	io_psub(io, FMT_USAGE);
	show_pdef(io, &PgmDefTab.opt, usage_short);
	io_puts("\\[~|--|~]\n", io);
	show_pdef(io, &PgmDefTab.arg, usage_short);
	io_puts("\\end\n", io);
}

#define	ATT_RM	0
#define	ATT_IT	1
#define	ATT_TT	2

static char *att_beg[3] = { NULL, "<", "|" };
static char *att_end[3] = { NULL, ">", "|" };

static int set_att(io_t *io, int set_att, int att)
{
	if	(set_att != att)
	{
		io_puts(att_end[set_att], io);
		set_att = att;
		io_puts(att_beg[set_att], io);
	}

	return set_att;
}

static void show_arg (io_t *io, const char *str, int flag)
{
	int att, cur_att, escape;

	if	(str == NULL)	return;

	cur_att = ATT_RM;

	if	(flag)
	{
		cur_att = set_att(io, cur_att, ATT_TT);
		io_putc('-', io);

		if	(*str == ' ')
		{
			str++;
			flag = 0;
		}
	}

	for (; *str != 0; str++)
	{
		att = flag ? ATT_TT : ATT_IT;
		escape = 0;

		switch (*str)
		{
		case ' ':
			flag = 0;
			att = 0;
			cur_att = set_att(io, cur_att, ATT_RM);
			io_putc('~', io);
			continue;
		case '=':
			flag = 0;
			att = ATT_TT;
			break;
		case '.':
			att = 0;
			break;
		case '|':
		case '[':
		case ']':
			escape = 1;
			att = 0;
			break;
		case '<':
		case '>':
		case '~':
		case '\\':
		case '$':
		case '&':
		case '@':
			escape = 1;
			break;
		default:
			break;
		}

		cur_att = set_att(io, cur_att, att);

		if	(escape)
			io_putc('\\', io);

		io_putc(*str, io);
	}

	cur_att = set_att(io, cur_att, ATT_RM);
}

static void show_xarg(io_t *io, const char *str, int flag)
{
	io_puts("\\[~", io);
	show_arg(io, str, flag);
	io_puts("~]", io);
}


/*	Formatierungsfunktionen
*/

void cp_usage(io_t *in, io_t *out)
{
	int c;

	if	(in == NULL)
	{
		char *p;

		p = xfsearch(ApplPath, HLP_PFX, ProgName, HLP_SFX);
		in = p ? io_fileopen(p, "r") : io_cstr(HelpFmt);
		memfree(p);
	}
	else	in = io_refer(in);

	in = io_usage(in);
	out = (out == ioerr) ? ERR_OUT : io_refer(out);

	while ((c = io_getc(in)) != EOF)
		io_putc(c, out);

	io_close(in);
	io_close(out);
}


void usage (const char *fmt, io_t *out)
{
	io_t *in;

	in = fmt ? io_cstr(fmt) : NULL;
	cp_usage(in, out);
	io_close(in);
}
