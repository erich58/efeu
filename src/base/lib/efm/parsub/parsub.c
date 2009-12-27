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

char *mpcopy (IO *in, int delim, int argc, char **argv)
{
	StrBuf *sb = sb_acquire();
	IO *out = io_strbuf(sb);
	io_pcopy(in, out, delim, argc, argv);
	io_close(out);
	return sb_cpyrelease(sb);
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
		StrBuf *sb = sb_acquire();
		IO *in = io_cstr(fmt);
		IO *out = io_strbuf(sb);
		io_pcopy(in, out, EOF, argc, argv);
		io_close(out);
		io_close(in);
		return sb_cpyrelease(sb);
	}
	else	return NULL;
}

char *mpsub (const char *fmt, ArgList *argl)
{
	if	(fmt)
	{
		StrBuf *sb = sb_acquire();
		IO *out = io_strbuf(sb);
		io_psub(out, fmt, argl);
		io_close(out);
		return sb_cpyrelease(sb);
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
