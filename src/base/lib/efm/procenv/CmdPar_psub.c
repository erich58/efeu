/*
Standardaufbereitung der Argumente für Auswertungsfunktionen

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

#include <EFEU/vecbuf.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <ctype.h>

#define	MODE_STR	0x1
#define	MODE_VERB	0x2

static int do_copy (CmdPar *par, IO *in, IO *out,
	ArgList *args, int mode, int end);
static char *do_vsub (CmdPar *par, IO *in,
	ArgList *args, int mode, int end);
static void do_psub (IO *in, IO *out, ArgList *args, int mode);

/*
Die Funktion |$1| führt eine Parametersubstitution für das Format
<fmt> im Kontext der Befehlsparameter <par> und dem Argument <arg>
durch. Mit einem Dollarsymbol wird eine klassische Parametersubstitution
eingeleitet. Vergleiche dazu \mref{parsub(3)}. Zusätzlich werden noch
Ausdrücke der Form |{|<name>|}| durch den Wert der Resourcedefinition
<name> von <par> ersetzt.
Eine leerer Klammerausdruck wird gegen <arg> ersetzt. Die
Klammerausdrücke können geschachtelt werden, z.B: bewirkt |{.help.{}}|.
das die Variable |.help.|<arg> abgefragt wird.
Klammerausdrücke können eine klassische Parametersubstitution beinhalten,
aber nicht umgekehrt.

Die Funktion |$1| berücksichtigt String- und Attributdefinitionen
und sorgt für eine korrekte Verwendung von benötigten Fluchtsymbolen
im resultierendem Text.

Der Backslash wirkt als Fluchtsymbol für die
Sonderzeichen |$|, |{|, |}|, |"|, ||||, |<| und |\\|.
Die Zeichen |"| und |||| haben keinen Einfluß auf die Analyse
der Formatanweisung.
*/

char *CmdPar_psub (CmdPar *par, const char *fmt, const char *arg)
{
	if	(fmt)
	{
		IO *in;
		char *p;
		ArgList args;

		in = io_cstr(fmt);
		args.data = (char **) &arg;
		args.dim = 1;
		p = do_vsub(CmdPar_ptr(par), in, &args, 0, EOF);
		io_close(in);
		return p;
	}
	else	return mstrcpy(arg);
}

/*
Die Funktion |$1| ist etwas allgemeiner als |CmdPar_psub|.
Das Resultat wird in die Ausgabestruktur <out> geschrieben. Anstelle
eines einzelnen Arguments wird der Pointer auf eine |ArgList| Struktur
übergeben. Bei einem Nullstring als Formatangabe erfolgt keine Ausgabe.
*/

void CmdPar_psubout (CmdPar *par, IO *out, const char *fmt, ArgList *args)
{
	if	(fmt && out)
	{
		IO *in = io_cstr(fmt);
		do_copy(CmdPar_ptr(par), in, out, args, 0, EOF);
		io_close(in);
	}
}

static void f_putc (int c, IO *io, int mode)
{
	if	(c == EOF)	return;

	if	(c == '|' && (mode & MODE_VERB))
		io_putc(c, io);

	if	(mode & MODE_STR)
		io_xputc(c, io, "\"");

	else	io_putc(c, io);
}


static void f_puts (const char *str, IO *io, int mode)
{
	if	(str)
	{
		while (*str)
			f_putc(*str++, io, mode);
	}
}

static int do_copy (CmdPar *par, IO *in, IO *out,
	ArgList *args, int mode, int end)
{
	char *name;
	int c;

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == end)	break;

		switch (c)
		{
		case '\\':
			c = io_getc(in);

			switch (c)
			{
			case '\\':
			case '"':
			case '|':
			case '{':
			case '}':
				break;
			default:
				io_ungetc(c, in);
				c = '\\';
				break;
			}

			break;
		case '<':
			f_putc(c, out, mode);
			c = io_getc(in);

			if	(c == '"' || c == '|')
			{
				f_putc(c, out, mode);
				c = do_copy(par, in, out, args, mode, c);
			}
			else
			{
				io_ungetc(c, in);
				c = do_copy(par, in, out, args, mode, '>');
			}

			break;
		case '$':
			do_psub(in, out, args, mode);
			continue;
		case '{':
			name = do_vsub(par, in, args, 0, '}');
			f_puts(name ? CmdPar_getval(par, name, NULL) :
				args->data[0], out, mode);
			memfree(name);
			continue;
		case '"':
			f_putc(c, out, mode);
			c = do_copy(par, in, out, args, mode | MODE_STR, c);
			break;
		case '|':
			f_putc(c, out, mode);
			c = do_copy(par, in, out, args, mode | MODE_VERB, c);
			break;
		default:
			break;
		}

		f_putc(c, out, mode);
	}

	return c;
}


static void do_psub (IO *in, IO *out, ArgList *args, int mode)
{
	StrBuf *sb = sb_create(0);
	f_puts(args ? psubexpand(sb, in, args->dim, args->data) :
		psubexpand(sb, in, 0, NULL), out, mode);
	sb_destroy(sb);
}

static char *do_vsub (CmdPar *par, IO *in, ArgList *args,
	int mode, int end)
{
	StrBuf *sb;
	IO *out;

	sb = sb_create(0);
	out = io_strbuf(sb);
	do_copy(par, in, out, args, mode, end);
	io_close(out);
	return sb2str(sb);
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
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/

