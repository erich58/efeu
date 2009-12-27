/*
Umgebungswechsel

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

#include <LaTeX.h>

/*	Konfigurationsparameter
*/

typedef struct {
	char *name;
	char *args;
	char *beg;
	char *end;
} ENV;

static char *sec_part = "part";
static char *sec_chapter = "chapter";
static char *sec_manchapter = "chapter";
static char *sec_section = "section";
static char *sec_subsection = "subsection";
static char *sec_paragraph = "paragraph";
static char *sec_manhead = NULL;
static char *sec_mansubhead = NULL;
static char *sec_caption = "caption";
static char *sec_subcaption = NULL;
static char *sec_margin = NULL;
static char *sec_note = "footnote";
static char *sec_fignote = NULL;

static VarDef_t sec_tab[] = {
	{ "part",	&Type_str, &sec_part },
	{ "chapter",	&Type_str, &sec_chapter },
	{ "manchapter",	&Type_str, &sec_manchapter },
	{ "section",	&Type_str, &sec_section },
	{ "subsection",	&Type_str, &sec_subsection },
	{ "paragraph",	&Type_str, &sec_paragraph },
	{ "manhead",	&Type_str, &sec_manhead },
	{ "mansubhead",	&Type_str, &sec_mansubhead },
	{ "caption",	&Type_str, &sec_caption },
	{ "subcaption",	&Type_str, &sec_subcaption },
	{ "margin",	&Type_str, &sec_margin },
	{ "note",	&Type_str, &sec_note },
	{ "fignote",	&Type_str, &sec_fignote },
};

static ENV quote_sgl = { "quote", NULL, "\\lq{}", "\\rq{}" };
static ENV quote_dbl = { "qquote", NULL, "\\lq{}", "\\rq{}" };

static VarDef_t quote_tab[] = {
	{ "lq",		&Type_str, &quote_sgl.beg },
	{ "rq",		&Type_str, &quote_sgl.end },
	{ "lqq",	&Type_str, &quote_dbl.beg },
	{ "rqq",	&Type_str, &quote_dbl.end },
};

static char *att_rm = "\\rm{}";
static char *att_bf = "\\bf{}";
static char *att_it = "\\it{}";
static char *att_tt = "\\tt{}";

static VarDef_t att_tab[] = {
	{ "rm",		&Type_str, &att_rm },
	{ "bf",		&Type_str, &att_bf },
	{ "it",		&Type_str, &att_it },
	{ "tt",		&Type_str, &att_tt },
};

static ENV list_item = { "itemize", NULL,
	"\\begin{itemize}", "\\end{itemize}" };

static ENV list_enum = { "enumerate", NULL,
	"\\begin{enumerate}", "\\end{enumerate}" };

static ENV list_desc = { "description", NULL,
	"\\begin{description}", "\\end{description}" };

static ENV env_code = { "code", NULL,
	NULL, NULL };

static ENV env_hang = { "hangpar", NULL,
	NULL, NULL };

static ENV env_quote = { "quote", NULL,
	"\\begin{quote}", "\\end{quote}" };

static ENV env_intro = { "preface", NULL,
	NULL, NULL };

static ENV env_verbatim = { "verbatim", NULL,
	"\\begin{verbatim}", "\\end{verbatim}" };

static ENV env_manpage = { "manpage", "os",
	NULL, NULL, };

static ENV env_spage = { "spage", "os",
	NULL, NULL, };

static ENV env_bib = { "bib", "s",
	"\\begin{thebibliography}{$1}", "\\end{thebibliography}", };

static ENV env_table = { "table", "o",
	"\\begin{table}$1", "\\end{table}", };

static ENV env_fig = { "fig", "o",
	"\\begin{figure}$1", "\\end{figure}", };

static ENV env_tabular = { "tabular", "os",
	"\\begin{tabular}$1{$2}", "\\end{tabular}\n" };

static ENV env_multicol = { "multicol", "sd",
	"\\multicolumn{$2}{$1}{", "}" };


static ENV *env_tab[] = {
	&list_item, &list_enum, &list_desc,
	&env_code, &env_hang, &env_quote, &env_intro,
	&env_verbatim, &env_manpage,
	&env_spage, &env_bib,
	&env_table, &env_fig, &env_tabular, &env_multicol,
};


static void add_env (VarTab_t *tab, ENV **env, size_t dim)
{
	Var_t *var;

	for (; dim-- > 0; env++)
	{
		var = NewVar(NULL, NULL, 0);
		var->name = mstrcpy((*env)->name);
		var->type = &Type_str;
		var->data = &(*env)->beg;
		AddVar(tab, var, 1);

		var = NewVar(NULL, NULL, 0);
		var->name = mstrpaste(NULL, "end", (*env)->name);
		var->type = &Type_str;
		var->data = &(*env)->end;
		AddVar(tab, var, 1);
	}
}

static void print_var (io_t *io, VarDef_t *var, size_t dim)
{
	io_putc('\n', io);

	for (; dim-- > 0; var++)
		io_printf(io, "%s = %#s;\n", var->name, Val_str(var->data));
}

static void print_env (io_t *io, ENV **p, size_t dim)
{
	for (; dim-- > 0; p++)
	{
		io_puts("\n// ", io);
		io_printf(io, "%s: args=%#s\n", (*p)->name, (*p)->args);
		io_printf(io, "%s = %#s;\n", (*p)->name, (*p)->beg);
		io_printf(io, "end%s = %#s;\n", (*p)->name, (*p)->end);
	}
}

void LaTeX_ShowEnv (io_t *io)
{
	print_var(io, sec_tab, tabsize(sec_tab));
	print_var(io, quote_tab, tabsize(quote_tab));
	print_var(io, att_tab, tabsize(att_tab));
	print_env(io, env_tab, tabsize(env_tab));
}

void LaTeX_SetupEnv (void)
{
	AddVarDef(LocalVar, sec_tab, tabsize(sec_tab));
	AddVarDef(LocalVar, quote_tab, tabsize(quote_tab));
	AddVarDef(LocalVar, att_tab, tabsize(att_tab));
	add_env(LocalVar, env_tab, tabsize(env_tab));
}


/*	Ausgabefunktionen
*/

static int env_par (const char *fmt, va_list list)
{
	char *p;
	int n;

	for (n = 1; *fmt != 0; fmt++, n++)
	{
		switch (*fmt)
		{
		case 'o':
			p = va_arg(list, char *);
			reg_set(n, p ? msprintf("[%s]", p) : NULL);
			break;
		case 's':
			reg_cpy(n, va_arg(list, char *));
			break;
		case 'c':
			reg_fmt(n, "%c", va_arg(list, int));
			break;
		case 'd':
			reg_fmt(n, "%d", va_arg(list, int));
			break;
		default:
			break;
		}
	}

	return n;
}

static void put_env (LaTeX_t *ltx, int mode, ENV *env, va_list list)
{
	if	(mode)
	{
		if	(env->args)	env_par(env->args, list);
		if	(env->beg)	io_psub(ltx->out, env->beg);
	}
	else
	{
		if	(env->end)	io_puts(env->end, ltx->out);
	}
}

static void put_xenv (LaTeX_t *ltx, int mode, ENV *env, va_list list)
{
	LaTeX_newline(ltx);

	if	(mode)
	{
		int n = env->args ? env_par(env->args, list) : 0;

		if	(env->beg)
		{
			io_psub(ltx->out, env->beg);
		}
		else
		{
			int i;

			io_printf(ltx->out, "%% beg{%s}", env->name);

			for (i = 1; i < n; i++)
				io_printf(ltx->out, " #%d=%#s", i, reg_get(i));
		}
	}
	else
	{
		if	(env->end)
			io_puts(env->end, ltx->out);
		else	io_printf(ltx->out, "%% end{%s}", env->name);
	}

	io_putc('\n', ltx->out);
}

static void put_att (LaTeX_t *ltx, int mode, char *att)
{
	if	(mode)
	{
		io_putc('{', ltx->out);
		io_puts(att, ltx->out);
	}
	else	io_puts("\\/}", ltx->out);
}

static void put_sec (LaTeX_t *ltx, int mode, const char *name, va_list list)
{
	if	(mode)
	{
		char *p;

		if	(name)
		{
			io_putc('\\', ltx->out);
			io_puts(name, ltx->out);

			p = va_arg(list, char *);

			if	(p)
			{
				io_puts("[{", ltx->out);
				io_puts(p, ltx->out);
				io_puts("}]", ltx->out);
			}
		}

		io_putc('{', ltx->out);
	}
	else
	{
		io_putc('}', ltx->out);
	}
}

static void put_xsec (LaTeX_t *ltx, int mode, const char *name, va_list list)
{
	if	(mode)
	{
		LaTeX_newline(ltx);
		put_sec(ltx, mode, name, list);
	}
	else
	{
		put_sec(ltx, mode, name, list);
		io_putc('\n', ltx->out);
		ltx->last = '\n';
		ltx->ignorespace = 1;
	}
}

/*	Umgebung beginnen/beenden
*/

int LaTeX_env (LaTeX_t *ltx, int flag, va_list list)
{
	int cmd;

	switch ((cmd = va_arg(list, int)))
	{
	case DOC_PAR_STD:
		LaTeX_newline(ltx);

		if	(flag)	ltx->ignorespace = 0;

		break;
	case DOC_PAR_TAG:
		LaTeX_newline(ltx);

		if	(flag)
		{
			io_puts("\\item[{", ltx->out);
			/*
			ltx->put = (DocDrvPut_t) LaTeX_xputc;
			*/
			ltx->last = 0;
			ltx->ignorespace = 1;
		}
		else
		{
			io_puts("}]\n", ltx->out);
			/*
			ltx->put = (DocDrvPut_t) LaTeX_putc;
			*/
			ltx->last = '\n';
			ltx->space = 0;
			ltx->ignorespace = 0;
		}

		break;
	case DOC_SEC_PART:
		put_xsec(ltx, flag, sec_part, list);
		break;
	case DOC_SEC_CHAP:
		put_xsec(ltx, flag, sec_chapter, list);
		break;
	case DOC_SEC_MCHAP:
		put_xsec(ltx, flag, sec_manchapter, list);
		break;
	case DOC_SEC_SECT:
		put_xsec(ltx, flag, sec_section, list);
		break;
	case DOC_SEC_SSECT:
		put_xsec(ltx, flag, sec_subsection, list);
		break;
	case DOC_SEC_PARA:
		put_xsec(ltx, flag, sec_paragraph, list);
		break;
	case DOC_SEC_HEAD:
		put_xsec(ltx, flag, sec_manhead, list);
		break;
	case DOC_SEC_SHEAD:
		put_xsec(ltx, flag, sec_mansubhead, list);
		break;
	case DOC_SEC_CAPT:
		put_xsec(ltx, flag, sec_caption, list);
		break;
	case DOC_SEC_SCAPT:
		put_xsec(ltx, flag, sec_subcaption, list);
		break;
	case DOC_SEC_MARG:
		put_xsec(ltx, flag, sec_margin, list);
		break;
	case DOC_SEC_NOTE:
		put_sec(ltx, flag, sec_note, list);
		break;
	case DOC_SEC_FNOTE:
		put_xsec(ltx, flag, sec_fignote, list);
		break;
	case DOC_QUOTE_SGL:
		put_env(ltx, flag, &quote_sgl, list);
		break;
	case DOC_QUOTE_DBL:
		put_env(ltx, flag, &quote_dbl, list);
		break;
	case DOC_ATT_RM:
		put_att(ltx, flag, att_rm);
		break;
	case DOC_ATT_BF:
		put_att(ltx, flag, att_bf);
		break;
	case DOC_ATT_IT:
		put_att(ltx, flag, att_it);
		break;
	case DOC_ATT_TT:
		put_att(ltx, flag, att_tt);
		break;
	case DOC_MODE_SKIP:
		ltx->skip = flag;
		break;
	case DOC_MODE_TEX:
	case DOC_MODE_PLAIN:
		ltx->put = flag ? DocDrv_plain : (DocDrvPut_t) LaTeX_putc;
		break;
	case DOC_MODE_HTML:
	case DOC_MODE_MAN:
		LaTeX_env (ltx, flag, list);
		break;
	case DOC_MODE_VERB:
		put_xenv(ltx, flag, &env_verbatim, list);
		ltx->put = flag ? DocDrv_plain : (DocDrvPut_t) LaTeX_putc;
		break;
	case DOC_LIST_ITEM:
		put_xenv(ltx, flag, &list_item, list);
		break;
	case DOC_LIST_ENUM:
		put_xenv(ltx, flag, &list_enum, list);
		break;
	case DOC_LIST_DESC:
		put_xenv(ltx, flag, &list_desc, list);
		break;
	case DOC_ENV_CODE:
		put_xenv(ltx, flag, &env_code, list);
		break;
	case DOC_ENV_HANG:
		put_xenv(ltx, flag, &env_hang, list);
		break;
	case DOC_ENV_QUOTE:
		put_xenv(ltx, flag, &env_quote, list);
		break;
	case DOC_ENV_INTRO:
		put_xenv(ltx, flag, &env_intro, list);
		break;
	case DOC_ENV_FORMULA:
		io_putc('$', ltx->out);
		break;
	case DOC_ENV_MPAGE:
		put_xenv(ltx, flag, &env_manpage, list);
		break;
	case DOC_ENV_SPAGE:
		put_xenv(ltx, flag, &env_spage, list);
		break;
	case DOC_ENV_TABLE:
		put_xenv(ltx, flag, &env_table, list);
		break;
	case DOC_ENV_FIG:
		put_xenv(ltx, flag, &env_fig, list);
		break;
	case DOC_ENV_BIB:
		put_xenv(ltx, flag, &env_bib, list);
		break;
	case DOC_ENV_TAB:
		put_xenv(ltx, flag, &env_tabular, list);
		break;
	case DOC_ENV_MCOL:
		put_env(ltx, flag, &env_multicol, list);
		break;
	default:
		return EOF;
	}

	return 0;
}
