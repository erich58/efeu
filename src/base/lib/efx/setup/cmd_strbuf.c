/*
Stringbuffer

$Copyright (C) 1994 Erich Fr�hst�ck
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
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>

EfiType Type_strbuf = REF_TYPE("strbuf", StrBuf *);

#define	SB(x)	((StrBuf **) (x))[0]

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	SB(rval) = sb_create(Val_int(arg[0]));
}

static void f_fprint(EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	StrBuf *sb = SB(arg[1]);

	if	(sb == NULL)
	{
		Val_int(rval) = io_puts("NULL", io);
		return;
	}

	io = io_count(io_refer(io));

	io_printf(io, "strbuf[%d]", sb->blksize);
	io_printf(io, "(%d, %d + %d)", sb->pos,
		sb->size - sb->nfree, sb->nfree);

	if	(sb_size(sb) < 64)
	{
		int n = sb_size(sb);
		unsigned char *p = sb->data;

		io_puts(" = \"", io);

		while (n-- > 0)
			io_xputc(*p++, io, " \t\"");

		io_putc('\"', io);
	}

	Val_int(rval) = io_close(io);
}

static void f_free (EfiFunc *func, void *rval, void **arg)
{
	if	(SB(arg[0]))
	{
		rd_deref(SB(arg[0]));
		SB(arg[0]) = NULL;
	}
}

static void f_getc (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = SB(arg[0]) ? sb_get(SB(arg[0])) : EOF;
}

static void f_putc (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sb_put(Val_int(arg[1]), SB(arg[0]));
}

static void f_puts (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sb_puts(Val_str(arg[1]), SB(arg[0]));
}

static void f_pos (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = SB(arg[0]) ? SB(arg[0])->pos : EOF;
}

static void f_sync (EfiFunc *func, void *rval, void **arg)
{
	if	(SB(arg[0]))	sb_sync(SB(arg[0]));
}

static void f_seek (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;
	int n, s;
	
	sb = SB(arg[0]);

	if	(sb == NULL)
	{
		Val_int(rval) = EOF;
		return;
	}

	n = Val_int(arg[1]);
	s = sb_size(sb) + 1;

	if	(n < -s)	sb->pos = 0;
	else if	(n <  0)	sb->pos = s + n;
	else if	(n <  s)	sb->pos = n;
	else			sb->pos = s - 1;

	Val_int(rval) = sb->pos;
}

static void f_insert (EfiFunc *func, void *rval, void **arg)
{
	if	(SB(arg[0]))	sb_insert(Val_int(arg[1]), SB(arg[0]));
}

static void f_delete (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = SB(arg[0]) ? sb_delete(SB(arg[0])) : EOF;
}

static void f_sb2io (EfiFunc *func, void *rval, void **arg)
{
	Val_io(rval) = io_strbuf(SB(arg[0]));
}

static void f_sb2str (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = SB(arg[0]);

	if	(sb)
	{
		register int n = sb_size(sb);
		register char *p = memalloc(n + 1);
		memcpy(p, sb->data, n);
		p[n] = 0;
		Val_str(rval) = p;
	}
	else	Val_str(rval) = NULL;
}

static void f_index (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = SB(arg[0]);
	int i = Val_int(arg[1]);
	Val_char(rval) = (sb && i < sb_size(sb)) ? sb->data[i] : 0;
}

static void f_read (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = sb_read(Val_int(arg[1]), SB(arg[0])) != NULL;
}

static EfiFuncDef fdef_strbuf[] = {
	{ 0, &Type_strbuf, "strbuf (int bsize = 0)", f_create },
	{ FUNC_VIRTUAL, &Type_void, "free (strbuf & sb)", f_free },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, strbuf)", f_fprint },
	{ 0, &Type_bool, "strbuf::read (int fd)", f_read },
	{ 0, &Type_int, "strbuf::seek (int n)", f_seek },
	{ 0, &Type_int, "strbuf::pos ()", f_pos },
	{ 0, &Type_void, "strbuf::sync ()", f_sync },
	{ 0, &Type_int, "strbuf::putc (char c)", f_putc },
	{ 0, &Type_int, "strbuf::puts (str s)", f_puts },
	{ 0, &Type_int, "strbuf::getc ()", f_getc },
	{ 0, &Type_void, "strbuf::insert (char c)", f_insert },
	{ 0, &Type_int, "strbuf::delete (void)", f_delete },
	{ 0, &Type_io, "strbuf ()", f_sb2io },
	{ 0, &Type_str, "strbuf ()", f_sb2str },
	{ FUNC_VIRTUAL, &Type_char, "operator[] (strbuf, int)", f_index },
};

void CmdSetup_strbuf(void)
{
	AddType(&Type_strbuf);
	AddFuncDef(fdef_strbuf, tabsize(fdef_strbuf));
}
