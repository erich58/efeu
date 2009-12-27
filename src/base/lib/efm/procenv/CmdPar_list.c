/*
Kommandoparameter auflisten

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/CmdPar.h>
#include <EFEU/strbuf.h>
#include <EFEU/parsub.h>

#define	UsagePrefix(par)	CmdPar_getval(par, "UsagePrefix", "usage: ")
#define	CommandName(par)	CmdPar_getval(par, NULL, "-")

/*
Die globale Variable |$1| steuert die Formatierung der Darstellungsfunktionen
für die Syntax. Ist die Variable verschieden von 0 (Standardeinstellung),
werden |efeudoc|-Steuersequenzen ausgegeben.
Bei der |eis|-Aufbereitung wird |$1| vorübergehend auf 0 gesetzt, da die
Steuerinformationen die Lesbarkeit stark beeinträchtigen.
*/

int CmdPar_docmode = 1;

static void doc_key (const char *key, IO *io)
{
	if	(CmdPar_docmode)
		io_puts(key, io);
}

static void verbatim (const char *str, IO *io, int protect)
{
	if	(!str)	return;

	for (; *str != 0; str++)
	{
		if	(CmdPar_docmode && *str == '|')
			io_putc('|', io);

		if	(protect)
		{
			switch (*str)
			{
			case '"':
				io_putc('\n', io);
				break;
			case '\n':
				io_puts("\\n", io);
				continue;
			case '\t':
				io_puts("\\t", io);
				continue;
			}
		}

		io_putc(*str, io);
	}
}


static void argkey (CmdParKey *key, IO *io)
{
	if	(key->arg)
	{
		doc_key("</", io);
		io_puts(key->arg, io);
		doc_key("/>", io);
	}
}

static void optkey (CmdParKey *key, IO *io)
{
	doc_key("|", io);
	io_putc('-', io);
	verbatim(key->key, io, 0);
	doc_key("|", io);

	switch (key->argtype)
	{
	case ARGTYPE_STD:
		io_putc(CmdPar_docmode ? '~' : ' ', io);
		argkey(key, io);
		break;
	case ARGTYPE_OPT:
		io_putc('[', io);

		if	(key->key[1])
			io_puts(CmdPar_docmode ? "|=|" : "=", io);

		argkey(key, io);
		io_putc(']', io);
		break;
	default:
		break;
	}
}

static void show_opt (CmdParKey *key, IO *io)
{
	io_puts("\n", io);
	io_puts("\\[~", io);
	optkey(key, io);
	io_puts("~]", io);
}

static void show_arg (CmdParKey *key, IO *io)
{
	io_puts("\n", io);

	switch (key->argtype)
	{
	case ARGTYPE_OPT:
	case ARGTYPE_VA0:
		io_puts(CmdPar_docmode ? "\\[~" : "[ ", io);
		argkey(key, io);
		io_puts(CmdPar_docmode ? "~]" : " ]", io);
		break;
	case ARGTYPE_REGEX:
		io_puts(CmdPar_docmode ? "\\{~<|" : "{ ", io);
		io_puts(key->arg, io);
		io_puts(CmdPar_docmode ? "|>~}" : " }", io);
		break;
	default:
		argkey(key, io);
		break;
	}
}

static void show_desc (CmdPar *par, const char *desc, IO *io)
{
	io = io_lmark(io_refer(io), "\t", NULL, 0);
	CmdPar_psubout(par, io, desc, NULL);
	io_close(io);
}

static void list_def (CmdPar *par, CmdParDef *def, IO *io, int showarg)
{
	CmdParKey *key;
	char *pfx;
	int flag;

	if	(!def->desc)	return;

	pfx = CmdPar_docmode ? "[<=" : NULL;
	flag = 1;

	for (key = def->key; key != NULL; key = key->next)
	{
		switch (key->partype) 
		{
		case PARTYPE_ENV:
			continue;
		case PARTYPE_OPT:
			io_puts(pfx, io);
			optkey(key, io);
			break;
		case PARTYPE_ARG:
			if	(!showarg)	continue;

			io_puts(pfx, io);

			if	(key->argtype == ARGTYPE_REGEX)
			{
				doc_key("<|", io);
				io_puts(key->arg, io);
				doc_key("|>", io);
			}
			else	argkey(key, io);

			break;
		}

		flag = 0;
		pfx = ", ";
	}

	if	(flag)	return;

	doc_key("=>]", io);
	io_putc('\n', io);
	show_desc(par, def->desc, io);
}

static void show_def (CmdPar *par, CmdParDef *def, IO *io)
{
	CmdParKey *key;

	for (key = def->key; key != NULL; key = key->next)
	{
		switch (key->partype) 
		{
		case PARTYPE_OPT:	show_opt(key, io); break;
		case PARTYPE_ARG:	show_arg(key, io); break;
		default:		break;
		}
	}
}

/*
Die Funktion |$1| generiert eine Aufrufsyntax aus den Kommandoparametern <par>
und gibt sie nach <io> aus. Falls <flag> ungleich 0 ist, werden die Optionen
und Argumente in der Reihenfolge der Definition ausgegeben.
*/

void CmdPar_synopsis (CmdPar *par, IO *io, int flag)
{
	size_t n;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);
	io_puts("\\hang\n|", io);
	verbatim(par->name, io, 0);
	io_puts("|", io);
	
	if	(flag)
	{
		CmdParDef **def;

		for (n = par->def.used, def = par->def.data; n-- > 0; def++)
			if ((*def)->desc) show_def(par, *def, io);
	}
	else
	{
		CmdParKey **p;

		for (n = par->opt.used, p = par->opt.data; n-- > 0; p++)
			if ((*p)->def->desc) show_opt(*p, io);

		for (n = par->arg.used, p = par->arg.data; n-- > 0; p++)
			if ((*p)->def->desc && (*p)->argtype != ARGTYPE_LAST)
				show_arg(*p, io);

		for (n = par->arg.used, p--; n-- > 0; p--)
			if ((*p)->def->desc && (*p)->argtype == ARGTYPE_LAST)
				show_arg(*p, io);
	}

	io_puts("\n\\end\n", io);
}

/*
Die Funktion |$1| generiert eine Liste der Optionen und Argumente aus den
Kommandoparametern <par> und gibt sie nach <io> aus.
*/

void CmdPar_arglist (CmdPar *par, IO *io)
{
	size_t n;
	CmdParDef **def;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);
	
	for (n = par->def.used, def = par->def.data; n-- > 0; def++)
		if ((*def)->desc) list_def(par, *def, io, 1);

	io_puts("\\end\n", io);
}

/*
Die Funktion |$1| generiert eine Liste der Optionen aus den
Kommandoparametern <par> und gibt sie nach <io> aus.
*/

void CmdPar_options (CmdPar *par, IO *io)
{
	size_t n;
	CmdParDef **def;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);
	
	for (n = par->def.used, def = par->def.data; n-- > 0; def++)
		if ((*def)->desc) list_def(par, *def, io, 0);
}

/*
Die Funktion |$1| generiert eine Liste der Umgebungsvariablen aus den
Kommandoparametern <par> und gibt sie nach <io> aus.
*/

void CmdPar_environ (CmdPar *par, IO *io)
{
	size_t n;
	CmdParKey **key;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);
	
	for (n = par->env.used, key = par->env.data; n-- > 0; key++)
	{
		if	((*key)->def->desc)
		{
			doc_key("[|", io);
			verbatim((*key)->key, io, 0);
			doc_key("|]", io);
			io_putc('\n', io);
			show_desc(par, (*key)->def->desc, io);
		}
	}
}

void CmdPar_resource (CmdPar *par, IO *io)
{
	size_t n;
	CmdParVar *var;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);
	
	for (n = par->var.used, var = par->var.data; n-- > 0; var++)
	{
		if	(var->desc)
		{
			doc_key("[|", io);
			verbatim(var->name, io, 0);

			if	(var->value)
			{
				io_puts(" = \"", io);
				verbatim(var->value, io, 1);
				io_putc('"', io);
			}

			doc_key("|]", io);
			io_putc('\n', io);
			show_desc(par, var->desc, io);
		}
	}
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
\mref{CmdParKey(3)},
\mref{CmdParVar(3)},
\mref{CmdPar_eval(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/


