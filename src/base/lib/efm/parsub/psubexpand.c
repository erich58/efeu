/*
Parametersubstitution

$Copyright (C) 1994, 2009 Erich Frühstück
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
#include <EFEU/parsub.h>
#include <EFEU/Debug.h>
#include <EFEU/fmtkey.h>
#include <EFEU/ioscan.h>
#include <ctype.h>

#define	PSUBKEY	'$'

#define E_NOARG	"[efm:pnoarg]$!: parsub($0): arg $1 not defined.\n"
#define E_ILKEY	"[efm:pilkey]$!: parsub($0): illegal format key $1.\n"

static SB_DECL(expand_buf, 0);

/*	Tabelle mit Substitutionsfunktionen
*/

typedef struct {
	int key;
	int (*copy) (IO *in, IO *out, void *par);
	void *par;
} COPYDEF;


static VECBUF(psubtab, 32, sizeof(COPYDEF));

static int pdef_cmp (const void *pa, const void *pb)
{
	const COPYDEF *a = pa;
	const COPYDEF *b = pb;
	return (b->key - a->key);
}

void psubfunc (int key, int (*copy) (IO *in, IO *out, void *par), void *par)
{
	COPYDEF cdef;

	cdef.key = key;
	cdef.copy = copy;
	cdef.par = par;
	vb_search(&psubtab, &cdef, pdef_cmp, copy ? VB_REPLACE : VB_DELETE);
}


/*
Parameterabfrage
*/

typedef struct GETARG GETARG;

struct GETARG {
	char *(*get) (GETARG *arg, int n);
	ArgList *arg;
	int dim;
	char **tab;
};

static char *get_arg (GETARG *arg, int n)
{
	return arg_get(arg->arg, n);
}

static char *get_vec (GETARG *arg, int n)
{
	return n < arg->dim ? arg->tab[n] : NULL;
}


/*	Kopierfunktion
*/

static void psubfmt (StrBuf *buf, IO *in, FmtKey *key, const char *par)
{
	switch (key->mode)
	{
	case 's':
	case 'S':
		fmt_str(buf, key, par);
		break;
	case 'c':
	case 'C':
		fmt_char(buf, key, par ? par[0] : 0);
		break;
	case 'i':
	case 'd':
		fmt_intmax(buf, key, par ? mstr2int64(par, NULL, 10) : 0);
		break;
	case 'b':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
		fmt_uintmax(buf, key, par ? mstr2uint64(par, NULL, 10) : 0);
		break;
	case 'f':
	case 'e':
	case 'E':
	case 'g':
	case 'G':
	case 'a':
	case 'A':
		fmt_double(buf, key, par ? C_strtod(par, NULL) : 0.);
		break;
	default:
		io_note(in, E_ILKEY, "c", key->mode);
		/* FALLTHROUGH */
	case 0:
		sb_puts(par, buf);
		break;
	}
}

static void add_arg (StrBuf *buf, IO *in, const char *fmt, GETARG *arg, int n)
{
	char *p = arg->get(arg, n);
	FmtKey key;

	if	(n >= arg->dim)
		io_note(in, E_NOARG, "d", n);

	if	(fmt && *fmt)
	{
		while (*fmt)
		{
			if	(*fmt == '%')
			{
				fmt++;

				if	(*fmt != '%')
				{
					fmtkey(&key, fmtkey_pgetucs, &fmt);
					psubfmt(buf, in, &key, p);
					continue;
				}
			}

			sb_putc(*fmt, buf);
			fmt++;
		}

		sb_xprintf(buf, fmt, p);
	}
	else	sb_puts(p, buf);
}

static void add_list (StrBuf *buf, IO *in, const char *fmt,
		GETARG *arg, int a, int b)
{
	int d = (a < b) ? 1 : -1;

	add_arg(buf, in, fmt, arg, a);

	while (a != b)
	{
		a += d;
		sb_putc(' ', buf);
		add_arg(buf, in, fmt, arg, a);
	}
}


static int getval (char **ptr, int defval, int dim)
{
	char *p = *ptr;
	int val = strtol(p, ptr, 10);

	if	(p == *ptr)	return defval;
	if	(val >= 0)	return val;

	val += dim;
	return (val > 0) ? val : dim;
}

static char *expand (StrBuf *base, IO *in, GETARG *arg)
{
	COPYDEF def, *ptr;
	StrBuf *buf;
	char *par;
	int offset;

/*	Falls base NULL ist, wird der statische Buffer expand_buf verwendet.
	Damit expand rekursiv aufgerufen werden kann, wird dieser immer
	ab der aktuellen Position verwendet. Diese muss am Ende der
	Verarbeitung zurückgesetzt werden.
*/
	buf = base ? base : &expand_buf;
	offset = buf->pos;
	def.key = io_getc(in);
	ptr = vb_search(&psubtab, &def, pdef_cmp, VB_SEARCH);

	if	(ptr && ptr->copy)
	{
		IO *tmp = io_strbuf(buf);
		ptr->copy(in, tmp, ptr->par);
		io_close(tmp);
	}
	else if	(def.key == '[')
	{
		static SB_DECL(format_buf, 0);

		int c;
		int first;
		int last;

		sb_trunc(&format_buf);

		while ((c = io_getc(in)) != ']')
		{
			if	(c == EOF)
				return sb_nul(buf);

			if	(c == '\\')
			{
				int d = io_peek(in);

				if	(d == '[' || d == ']' || d == '\\')
					c = io_getc(in);
			}

			sb_putc(c, &format_buf);
		}

		par = sb_nul(&format_buf);

		if	(*par != 0)
		{
			first = getval(&par, 1, arg->dim);

			if	(*par == ':')
			{
				par++;
				last = getval(&par, arg->dim - 1, arg->dim);
			}
			else	last = first;

			if	(*par == ';')
				par++;

			add_list(buf, in, par, arg, first, last);
		}
	}
	else if	(def.key == '*')
	{
		if	(arg->dim > 1)
			add_list(buf, in, NULL, arg, 1, arg->dim - 1);
	}
	else if	(isdigit(def.key))
	{
		add_arg(buf, in, NULL, arg, def.key - '0');
	}
	else if	(def.key != EOF)
	{
		sb_putc(def.key, buf);
	}

	if	(base)
		return sb_nul(buf) + offset;

/*	Falls expand_buf verwendet wurde, muss dieser auf den Anfangszustand
	zurückgestellt werden. Die zurückgegebene Zeichenkette bleibt bis
	zum nächsten Schreibzugriff gültig.
*/
	sb_putc(0, buf);
	sb_setpos(buf, offset);
	sb_sync(buf);
	return (char *) buf->data + offset;
}

static int pcopy (IO *in, IO *out, int delim, GETARG *get)
{
	char *arg;
	int32_t c, n, flag;

	n = 0;
	flag = 0;

	while ((c = io_mgetc(in, 1)) != delim && c != EOF)
	{
		if	(c == PSUBKEY)
		{
			arg = expand(NULL, in, get);

			if	(flag)
				n += io_xputs(arg, out, "\"");
			else	n += io_puts(arg, out);
		}
		else
		{
			if	(c == '"')	flag = !flag;

			n += io_nputc(c, out, 1);
		}
	}

	return n;
}

char *psubexpand (StrBuf *buf, IO *in, int argc, char **argv)
{
	GETARG arg;
	arg.get = get_vec;
	arg.arg = NULL;
	arg.dim = argc;
	arg.tab = argv;
	return expand(buf, in, &arg);
}

char *psubexpandarg (StrBuf *buf, IO *in, ArgList *argl)
{
	GETARG arg;
	arg.get = get_arg;
	arg.arg = argl;
	arg.dim = argl ? argl->dim : 0;
	arg.tab = NULL;
	return expand(buf, in, &arg);
}

int io_pcopy (IO *in, IO *out, int delim, int argc, char **argv)
{
	GETARG arg;
	arg.get = get_vec;
	arg.arg = NULL;
	arg.dim = argc;
	arg.tab = argv;
	return pcopy(in, out, delim, &arg);
}

int io_pcopyarg (IO *in, IO *out, int delim, ArgList *argl)
{
	GETARG arg;
	arg.get = get_arg;
	arg.arg = argl;
	arg.dim = argl ? argl->dim : 0;
	arg.tab = NULL;
	return pcopy(in, out, delim, &arg);
}

