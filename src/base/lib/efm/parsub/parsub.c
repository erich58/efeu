/*
Parametersubstitution

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

#include <EFEU/vecbuf.h>
#include <EFEU/parsub.h>
#include <EFEU/Debug.h>
#include <EFEU/fmtkey.h>
#include <EFEU/ioscan.h>
#include <ctype.h>

#define	PSUBKEY	'$'

#define E_NOARG	"[efm:pnoarg]$!: parsub($0): arg $1 not defined.\n"
#define E_ILKEY	"[efm:pilkey]$!: parsub($0): illegal format key $1.\n"

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


/*	Kopierfunktion
*/

static STRBUF(expand_buf, 1024);

static void psubfmt (StrBuf *buf, IO *in, const char *par)
{
	FmtKey key;
	IO *out;

	out = io_strbuf(buf);
	io_fmtkey(in, &key);

	switch (key.mode)
	{
	case 's':
	case 'S':
		fmt_str(out, &key, par);
		break;
	case 'c':
	case 'C':
		fmt_char(out, &key, par ? par[0] : 0);
		break;
	case 'i':
	case 'd':
		fmt_intmax(out, &key, par ? mstr2int64(par, NULL, 10) : 0);
		break;
	case 'b':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
		fmt_uintmax(out, &key, par ? mstr2uint64(par, NULL, 10) : 0);
		break;
	case 'f':
	case 'e':
	case 'E':
	case 'g':
	case 'G':
	case 'a':
	case 'A':
		fmt_double(out, &key, par ? C_strtod(par, NULL) : 0.);
		break;
	default:
		io_note(in, E_ILKEY, "c", key.mode);
		/* FALLTHROUGH */
	case 0:
		sb_puts(par, buf);
		break;
	}

	io_close(out);
}

char *psubexpand (StrBuf *buf, IO *in, int argc, char **argv)
{
	COPYDEF def, *ptr;
	char *par;

	if	(!buf)	buf = &expand_buf;

	sb_setpos(buf, 0);
	def.key = io_getc(in);
	ptr = vb_search(&psubtab, &def, pdef_cmp, VB_SEARCH);

	if	(ptr && ptr->copy)
	{
		IO *tmp = io_strbuf(buf);
		ptr->copy(in, tmp, ptr->par);
		io_close(tmp);
	}
	else if	(isdigit(def.key))
	{
		int n = 0;

		do
		{
			n = 10 * n + def.key - '0';
			def.key = io_getc(in);
		}
		while	(isdigit(def.key));

		if	(n >= argc)
		{
			io_note(in, E_NOARG, "d", n);
			par = NULL;
		}
		else	par = argv[n];

		if	(def.key != '%')
		{
			io_ungetc(def.key, in);
			sb_puts(par, buf);
		}
		else	psubfmt(buf, in, par);
	}
	else if	(def.key != EOF)
	{
		sb_putc(def.key, buf);
	}

	sb_putc(0, buf);
	return (char *) buf->data;
}

char *psubexpandarg (StrBuf *buf, IO *in, ArgList *argl)
{
	COPYDEF def, *ptr;
	char *par;

	if	(!buf)	buf = &expand_buf;

	sb_setpos(buf, 0);
	def.key = io_getc(in);
	ptr = vb_search(&psubtab, &def, pdef_cmp, VB_SEARCH);

	if	(ptr && ptr->copy)
	{
		IO *tmp = io_strbuf(buf);
		ptr->copy(in, tmp, ptr->par);
		io_close(tmp);
	}
	else if	(isdigit(def.key))
	{
		int n = 0;

		do
		{
			n = 10 * n + def.key - '0';
			def.key = io_getc(in);
		}
		while	(isdigit(def.key));

		if	(!arg_test(argl, n))
		{
			io_note(in, E_NOARG, "d", n);
			par = NULL;
		}
		else	par = arg_get(argl, n);

		if	(def.key != '%')
		{
			io_ungetc(def.key, in);
			sb_puts(par, buf);
		}
		else	psubfmt(buf, in, par);
	}
	else if	(def.key != EOF)
	{
		sb_putc(def.key, buf);
	}

	sb_putc(0, buf);
	return (char *) buf->data;
}

int io_pcopy (IO *in, IO *out, int delim, int argc, char **argv)
{
	StrBuf *buf;
	char *arg;
	int c, n, flag;

	n = 0;
	flag = 0;
	buf = NULL;

	while ((c = io_mgetc(in, 1)) != delim && c != EOF)
	{
		if	(c == PSUBKEY)
		{
			if	(!buf)	buf = sb_create(1024);

			arg = psubexpand(buf, in, argc, argv);

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

	rd_deref(buf);
	return n;
}

int io_pcopyarg (IO *in, IO *out, int delim, ArgList *args)
{
	StrBuf *buf;
	char *arg;
	int c, n, flag;

	n = 0;
	flag = 0;
	buf = NULL;

	while ((c = io_mgetc(in, 1)) != delim && c != EOF)
	{
		if	(c == PSUBKEY)
		{
			if	(!buf)	buf = sb_create(1024);

			arg = psubexpandarg(buf, in, args);

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

	rd_deref(buf);
	return n;
}

char *mpcopy (IO *in, int delim, int argc, char **argv)
{
	StrBuf *sb = sb_create(0);
	IO *out = io_strbuf(sb);
	io_pcopy(in, out, delim, argc, argv);
	io_close(out);
	return sb2str(sb);
}


int io_psubvec (IO *out, const char *fmt, int argc, char **argv)
{
	if	(fmt)
	{
		IO *in = io_cstr(fmt);
		int n = io_pcopy(in, out, EOF, argc, argv);
		io_close(in);
		return n;
	}

	return 0;
}

int io_psub (IO *out, const char *fmt, ArgList *args)
{
	if	(fmt)
	{
		IO *in = io_cstr(fmt);
		int n = io_pcopyarg(in, out, EOF, args);
		io_close(in);
		return n;
	}

	return 0;
}

int io_psubvarg (IO *out, const char *fmt, const char *argdef, va_list list)
{
	int n;

	if	(argdef)
	{
		ArgList *args = arg_create();
		arg_append(args, argdef, list);
		n = io_psub(out, fmt, args);
		rd_deref(args);
	}
	else	n = io_psub(out, fmt, NULL);

	return n;
}

int io_psubarg (IO *out, const char *fmt, const char *argdef, ...)
{
	va_list list;
	int n;

	va_start(list, argdef);
	n = io_psubvarg(out, fmt, argdef, list);
	va_end(list);
	return n;
}

char *mpsubvec (const char *fmt, int argc, char **argv)
{
	if	(fmt)
	{
		StrBuf *sb = sb_create(0);
		IO *in = io_cstr(fmt);
		IO *out = io_strbuf(sb);
		io_pcopy(in, out, EOF, argc, argv);
		io_close(out);
		io_close(in);
		return sb2str(sb);
	}
	else	return NULL;
}

char *mpsub (const char *fmt, ArgList *argl)
{
	if	(fmt)
	{
		StrBuf *sb = sb_create(0);
		IO *out = io_strbuf(sb);
		io_psub(out, fmt, argl);
		io_close(out);
		return sb2str(sb);
	}

	return NULL;
}

char *mpsubvarg (const char *fmt, const char *argdef, va_list list)
{
	char *p;

	if	(argdef)
	{
		ArgList *args = arg_create();
		arg_append(args, argdef, list);
		p = mpsub(fmt, args);
		rd_deref(args);
	}
	else	p = mpsub(fmt, NULL);

	return p;
}

char *mpsubarg (const char *fmt, const char *argdef, ...)
{
	va_list list;
	char *p;

	va_start(list, argdef);
	p = mpsubvarg(fmt, argdef, list);
	va_end(list);
	return p;
}
