/*
Kommandoparameter in Datei schreiben

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
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

/**/

static void put_name (const char *name, IO *io)
{
	if	(!name)	return;

	switch (*name)
	{
	case '@':
	case '$':
	case '*':
	case '+':
	case '/':
		io_putc('\\', io);
		break;
	default:
		break;
	}

	for (; *name != 0; name++)
	{
		switch (*name)
		{
		case '\t':
			io_puts("\\t", io);
			continue;
		case '\n':
			io_puts("\\n", io);
			continue;
		case ' ':
		case ':':
		case '\\':
		case '[':
		case '=':
			io_putc('\\', io);
			break;
		default:
			break;
		}

		io_putc(*name, io);
	}
}

/**/

static void put_desc (const char *desc, IO *io)
{
	int last;

	if	(!desc)	return;

	for (last = '\n'; *desc != 0; desc++)
	{
		if	(last == '\n')
			io_putc('\t', io);

		last = *desc;
		io_putc(last, io);
	}

	if	(last != '\n')
		io_putc('\n', io);
}

/*
Die Funktion |$1| gibt eine Parameterkenung aus
*/

void CmdParKey_print (IO *io, CmdParKey *key)
{
	if	(!key)	return;

	switch (key->partype)
	{
	case PARTYPE_ENV:
		io_putc('@', io);
		put_name(key->key, io);
		return;
	case PARTYPE_OPT:
		put_name(key->key, io);
		break;
	default:
		break;
	}

	switch (key->argtype)
	{
	case ARGTYPE_OPT:
		io_putc(':', io);
		/*FALLTHROUGH*/
	case ARGTYPE_STD:
		io_putc(':', io);
		break;
	case ARGTYPE_LAST:
		io_putc('$', io);
		break;
	case ARGTYPE_VA0:
		io_putc('*', io);
		break;
	case ARGTYPE_VA1:
		io_putc('+', io);
		break;
	case ARGTYPE_REGEX:
		io_putc('/', io);
		io_xputs(key->key, io, "/");
		io_putc('/', io);
		break;
	default:
		break;
	}

	put_name(key->arg, io);

	if	(key->val)
	{
		io_putc('[', io);
		io_xputs(key->val, io, "[]");
		io_putc(']', io);
	}
}

/*
Die Funktion |$1| gibt eine Parameterauswertung aus
*/

void CmdParCall_print (IO *io, CmdParCall *call)
{
	if	(!call)	return;

	if	(call->name)
		put_name(call->name, io);

	if	(call->eval)
	{
		io_puts(": ", io);
		put_name(call->eval->name, io);

		if	(call->par)
			io_xprintf(io, " %#s", call->par);
	}
	else if	(call->par)
	{
		io_xprintf(io, " = %#s", call->par);
	}
}

/*
Die Funktion |$1| gibt eine Parameterdefinition aus
*/

void CmdParDef_print (IO *io, CmdParDef *def)
{
	CmdParKey *key;
	CmdParCall *call;

	if	(!def)	return;

	for (key = def->key; key != NULL; key = key->next)
	{
		CmdParKey_print(io, key);
		io_putc('|', io);
	}

	for (call = def->call; call != NULL; call = call->next)
	{
		CmdParCall_print(io, call);

		if	(call->next)	io_puts("; ", io);
	}

	io_putc('\n', io);
	put_desc(def->desc, io);
}


/*
Die Funktion |$1| schreibt Kommandoparameter in die IO-Struktur <io>.
*/

void CmdPar_write (CmdPar *par, IO *io)
{
	size_t n;
	CmdParDef **ptr;
	CmdParVar *var;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);

	for (n = par->var.used, var = par->var.data; n-- > 0; var++)
	{
		put_name(var->name, io);
		io_xprintf(io, " = %#s\n", var->name, var->value);
		put_desc(var->desc, io);
	}

	io_putc('\n', io);

	for (n = par->def.used, ptr = par->def.data; n-- > 0; ptr++)
		CmdParDef_print(io, *ptr);
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
\mref{CmdPar_list(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/


