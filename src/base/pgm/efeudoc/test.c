/*
Testdokumentausgabe

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <efeudoc.h>
#include <DocCtrl.h>
#include <DocOut.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>

#define	UNDEF	"!\b!U\bUN\bND\bDE\bEF\bF!\b!"
#define	ERROR	"!\b!E\bER\bRR\bRO\bOR\bR!\b!"

#define	ENV_UNDEF	0xFF

/*	Kodierungstabellen
*/

typedef struct {
	int key;
	char *name;
	char *fmt;
	int lbreak;
} NAME;

static NAME cmd_tab[] = {
	{ DOC_CMD_BREAK, "newline", NULL, 0 },
	{ DOC_CMD_NPAGE, "newpage", NULL, 0 },
	{ DOC_CMD_TOC, "toc", NULL, 0 },
	{ DOC_CMD_ITEM, "item", NULL, 0 },
	{ DOC_CMD_APP, "appendix", "%s", 0 },
	{ DOC_CMD_IDX, "index", "%s %s", 0 },
	{ DOC_CMD_MARK, "mark", "%d", 0 },
	{ DOC_CMD_TEX, "tex", "%s", 0 },

	{ DOC_REF_LABEL, "label", "%s", 0 },
	{ DOC_REF_STD, "ref", "%s", 0 },
	{ DOC_REF_VAR, "vref", "%s", 0 },
	{ DOC_REF_PAGE, "pageref", "%s", 0 },
	{ DOC_REF_MAN, "manref", "%s", 0 },

	{ DOC_TAB_BEG, "tabbeg", NULL, 1 },
	{ DOC_TAB_SEP, "tabseb", NULL, 1 },
	{ DOC_TAB_END, "tabend", NULL, 0 },
	{ DOC_TAB_HLINE, "hline", NULL, 0 },
	{ DOC_TAB_CLINE, "cline", NULL, 0 },
};

static NAME env_tab[] = {
	{ ENV_UNDEF, "_null_", NULL, 1 },
	{ DOC_PAR_STD, "p", NULL, 1 },
	{ DOC_PAR_ITEM, "item", NULL, 1 },
	{ DOC_PAR_TAG, "tag", NULL, 1 },

	{ DOC_SEC_PART, "part", "%s", 1 },
	{ DOC_SEC_CHAP, "chapter", "%s", 1 },
	{ DOC_SEC_MCHAP, "manchapter", "%s", 1 },
	{ DOC_SEC_SECT, "section", "%s", 1 },
	{ DOC_SEC_SSECT, "subsection", "%s", 1 },
	{ DOC_SEC_PARA, "paragraph", "%s", 1 },
	{ DOC_SEC_HEAD, "head", "%s", 1 },
	{ DOC_SEC_SHEAD, "subhead", "%s", 1 },
	{ DOC_SEC_CAPT, "caption", "%s", 1 },
	{ DOC_SEC_SCAPT, "subcaption", "%s", 1 },
	{ DOC_SEC_MARG, "margin", "%s", 1 },
	{ DOC_SEC_NOTE, "note", "%s", 1 },
	{ DOC_SEC_FNOTE, "fignote", "%s", 1 },

	{ DOC_ATT_TT, "tt", NULL, 0 },
	{ DOC_ATT_IT, "it", NULL, 0 },
	{ DOC_ATT_BF, "bf", NULL, 0 },
	{ DOC_ATT_RM, "rm", NULL, 0 },
	{ DOC_QUOTE_SGL, "sq", NULL, 0 },
	{ DOC_QUOTE_DBL, "dq", NULL, 0 },

	{ DOC_LIST_ITEM, "list", NULL, 1 },
	{ DOC_LIST_ENUM, "enum", NULL, 1 },
	{ DOC_LIST_DESC, "desc", NULL, 1 },

	{ DOC_MODE_COPY, "copy", NULL, 0 },
	{ DOC_MODE_SKIP, "skip", NULL, 1 },
	{ DOC_MODE_VERB, "verb", NULL, 1 },
	{ DOC_MODE_MAN, "man", "@", 1 },
	{ DOC_MODE_TEX, "tex", "@", 1 },
	{ DOC_MODE_HTML, "html", "@", 1 },
	{ DOC_MODE_PLAIN, "plain", NULL, 1 },

	{ DOC_ENV_BASE, "base", NULL, 1 },
	{ DOC_ENV_INTRO, "intro", NULL, 1 },
	{ DOC_ENV_QUOTE, "quote", NULL, 0 },
	{ DOC_ENV_FORMULA, "f",	NULL, 1 },
	{ DOC_ENV_SPAGE, "spage", "%#s %#s", 1 },
	{ DOC_ENV_MPAGE, "mpage", "%#s %#s", 1 },
	{ DOC_ENV_BIB, "bib", "%#s", 1 },
	{ DOC_ENV_TABLE, "table", "%#s", 1 },
	{ DOC_ENV_FIG, "fig", "%#s", 1 },
	{ DOC_ENV_TAB, "tab", "option=%s coldef=%s", 1 },
	{ DOC_ENV_URL, "url", "%#s", 1 },
	{ DOC_ENV_CODE, "code", NULL, 0 },
	{ DOC_ENV_HANG, "hang", NULL, 0 },
	{ DOC_ENV_SLOPPY, "sloppy", NULL, 0 },
	{ DOC_ENV_MCOL, "multicol", "%s %d", 0 },
};


static NAME *get_def (int key, NAME *tab, size_t dim)
{
	static char str_buf[32];
	static NAME name_buf = { 0, str_buf, NULL, 1 };

	for (; dim-- > 0; tab++)
		if (key == tab->key) return tab;

	name_buf.key = key;
	sprintf(str_buf, "%#x", key);
	return &name_buf;
}

char *DocCmdName (int key)
{
	return get_def(key, cmd_tab, tabsize(cmd_tab))->name;
}

char *DocEnvName (int key)
{
	return get_def(key, env_tab, tabsize(env_tab))->name;
}


/*	Darstellungsfunktionen
*/

static void show_var (IO *out, EfiVarTab *tab)
{
	VarTabEntry *p;
	size_t n;

	if	(!tab)	return;

	io_xprintf(out, "\t%s: \\\n", tab->name);

	for (p = tab->tab.data, n = tab->tab.used; n-- > 0; p++)
	{
		EfiObj *obj = p->get ? p->get(NULL, p->data) : RefObj(p->obj);

		io_xprintf(out, "\t%s = ", p->name);
		PrintObj(out, obj);
		UnrefObj(obj);
		io_puts(" \\\n", out);
	}
}

/*	Ausgabestruktur
*/

typedef struct {
	IO *out;
	Stack *env;
	int plain;
} TPAR;


/*	Parameter ausgeben
*/

static void show_par (TPAR *par, const char *fmt, va_list list)
{
	if	(fmt == NULL)	return;

	io_putc(' ', par->out);

	if	(*fmt == '@')
	{
		io_puts(DocEnvName(va_arg(list, int)), par->out);
	}
	else	io_vxprintf(par->out, fmt, list);
}

/*	Steuerzeichen ausgeben
*/

static int test_ctrl (IO *io, int req, va_list list)
{
	TPAR *par = io->data;
	NAME *name;
	int last, lbreak;

	switch (req)
	{
	case IO_CLOSE:
		return io_close(par->out);
	case IO_IDENT:
		return io_vctrl(par->out, req, list);
	}
	
	io_puts("<\b<", par->out);
	lbreak = 0;

	switch (req)
	{
	case DOC_HEAD:
		io_puts("@\b@", par->out);
		show_var(par->out, LocalVar);
		lbreak = 1;
		break;
	case DOC_REM:
		io_puts("!\b!", par->out);
		io_xputs(va_arg(list, char *), par->out, "<!>");
		lbreak = 1;
		break;
	case DOC_SYM:
		io_xprintf(par->out, "&\b&%s", va_arg(list, char *));
		break;
	case DOC_CMD:
		name = get_def(va_arg(list, int), cmd_tab, tabsize(cmd_tab));
		io_puts(name->name, par->out);
		show_par(par, name->fmt, list);
		lbreak = name->lbreak;
		break;
	case DOC_BEG:
		io_puts("+\b+", par->out);
		name = get_def(va_arg(list, int), env_tab, tabsize(env_tab));
		io_puts(name->name, par->out);
		show_par(par, name->fmt, list);
		pushstack(&par->env, (void *) (size_t) name->key);
		lbreak = name->lbreak;
		break;
	case DOC_END:
		io_puts("-\b-", par->out);
		name = get_def(va_arg(list, int), env_tab, tabsize(env_tab));
		io_puts(name->name, par->out);
		last = (int) (size_t) popstack(&par->env, (void *) 0xFF);

		if	(name->key != last)
			io_xprintf(par->out, " != %s %s",
				DocEnvName(last), ERROR);

		lbreak = name->lbreak;
		break;
	default:
		io_xprintf(par->out, "%c-%02x %s",
			(req >> 8), req & 0xFF, UNDEF);
		break;
	}

	io_puts(">\b>", par->out);

	if	(lbreak)
		io_puts("\\\n", par->out);
	
	return 0;
}


/*	Zeichen ausgeben
*/

static int test_put (int c, IO *io)
{
	TPAR *par = io->data;

	if	(par->plain)
		return io_putc(c, par->out);

	switch (c)
	{
	case '\t':	io_puts("\\t", par->out); break;
	case '<':	io_puts("\\<", par->out); break;
	case '|':	io_puts("\\|", par->out); break;
	case '>':	io_puts("\\>", par->out); break;
	case '\\':	io_puts("\\\\", par->out); break;
	default:	io_putc(c, par->out); break;
	}

	return c;
}

static int test_putucs (int32_t c, IO *io)
{
	TPAR *par = io->data;

	if	(par->plain)
		return io_putucs(c, par->out);

	if	(c >= 127)
	{
		io_printf(par->out, "&#%d;", c);
		return c;
	}

	switch (c)
	{
	case '\t':	io_puts("\\t", par->out); break;
	case '<':	io_puts("\\<", par->out); break;
	case '|':	io_puts("\\|", par->out); break;
	case '>':	io_puts("\\>", par->out); break;
	case '\\':	io_puts("\\\\", par->out); break;
	default:	io_putc(c, par->out); break;
	}

	return c;
}

IO *DocOut_test (IO *io)
{
	if	(io)
	{
		TPAR *par = memalloc(sizeof(TPAR));
		par->out = io;

		io = io_alloc();
		io->put = test_put;
		io->putucs = test_putucs;
		io->ctrl = test_ctrl;
		io->data = par;
	}

	return io;
}
