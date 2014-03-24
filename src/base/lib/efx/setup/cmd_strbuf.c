/*
Stringbuffer

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
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>

static size_t t_read (const EfiType *st, void *data, IO *io)
{
	return 0;

#if	0
	int byte = io_getc(io);

	if	(byte == EOF)
	{
		Val_str(data) = NULL;
		return 0;
	}
	else if	(byte > 0)
	{
		size_t size = io_getval(io, byte);
		Val_str(data) = io_mread(io, size);
		return size + byte + 1;
	}
	else
	{
		Val_str(data) = NULL;
		return 1;
	}
#endif
}

static size_t t_write (const EfiType *st, const void *data, IO *io)
{
	return 0;
}

static int t_print (const EfiType *st, const void *data, IO *io)
{
	/*
	char *str = Val_str(data);

	if	(str)
	{
		int n = io_puts("\"", io);
		n += io_xputs(str, io, "\"");
		n += io_puts("\"", io);
		return n;
	}
	else	return io_puts("NULL", io);
	*/
	return 0;
}

static void t_clean (const EfiType *st, void *data, int mode)
{
	sb_free(data);
}

static void t_copy (const EfiType *st, void *tg, const void *p_src)
{
	const StrBuf *src = p_src;
	sb_trunc(tg);
	sb_putmem(tg, src->data, src->pos);
}

EfiType Type_strbuf = COMPLEX_TYPE("StrBuf", "StrBuf", sizeof(StrBuf), 0,
	t_read, t_write, t_print,
	NULL, NULL, t_clean, t_copy, TYPE_MALLOC);

#define	SB(x)	((StrBuf *) (x))

static void f_fprint(EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	StrBuf *sb = SB(arg[1]);

	io = io_count(io_refer(io));
	io_xprintf(io, "strbuf[%d]", sb->blksize);
	io_xprintf(io, "(%d, %d + %d)", sb->pos,
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
	sb_free(arg[0]);
}

static void f_getc (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sb_get(SB(arg[0]));
}

static void f_getw (EfiFunc *func, void *rval, void **arg)
{
	*((int32_t *) rval) = sb_getucs(SB(arg[0]));
}

static void f_putc (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sb_put(Val_int(arg[1]), SB(arg[0]));
}

static void f_putw (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sb_putucs(*((int32_t *) arg[1]), SB(arg[0]));
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
	n = Val_int(arg[1]);
	s = sb_size(sb) + 1;

	if	(n < -s)	sb->pos = 0;
	else if	(n <  0)	sb->pos = s + n;
	else if	(n <  s)	sb->pos = n;
	else			sb->pos = s - 1;

	Val_int(rval) = sb->pos;
}

static void f_move (EfiFunc *func, void *rval, void **arg)
{
	sb_move(SB(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]), Val_int(arg[3]));
}

static void f_insert (EfiFunc *func, void *rval, void **arg)
{
	sb_insert(SB(arg[0]), Val_int(arg[1]), Val_int(arg[2]));
}

static void f_delete (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sb_delete(SB(arg[0]), Val_int(arg[1])) != NULL;
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
		int n = sb_size(sb);
		char *p = memalloc(n + 1);
		memcpy(p, sb->data, n);
		p[n] = 0;
		Val_str(rval) = p;
	}
	else	Val_str(rval) = NULL;
}

static void f_str2sb (EfiFunc *func, void *rval, void **arg)
{
	sb_trunc(rval);
	sb_puts(Val_str(arg[0]), rval);
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

static void f_printf (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = StrBufFmtList(SB(arg[0]), Val_str(arg[1]),
		Val_list(arg[2]));
}

static EfiFuncDef fdef_strbuf[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, StrBuf)", f_fprint },
	{ 0, &Type_void, "StrBuf::free ()", f_free },
	{ 0, &Type_bool, "StrBuf::read (int fd)", f_read },
	{ 0, &Type_int, "StrBuf::seek (int n)", f_seek },
	{ 0, &Type_int, "StrBuf::pos ()", f_pos },
	{ 0, &Type_void, "StrBuf::sync ()", f_sync },
	{ 0, &Type_int, "StrBuf::putc (char c)", f_putc },
	{ 0, &Type_int, "StrBuf::putw (int32_t c)", f_putw },
	{ 0, &Type_int, "StrBuf::puts (str s)", f_puts },
	{ 0, &Type_int, "StrBuf::getc ()", f_getc },
	{ 0, &Type_wchar, "StrBuf::getw ()", f_getw },
	{ 0, &Type_int, "StrBuf::printf (str fmt, ...)", f_printf },
	{ 0, &Type_void, "StrBuf::move (int dest, int src, int len)", f_move },
	{ 0, &Type_void, "StrBuf::insert (char c, int count = 1)", f_insert },
	{ 0, &Type_int, "StrBuf::delete (int count = 1)", f_delete },
	{ 0, &Type_io, "StrBuf ()", f_sb2io },
	{ 0, &Type_str, "StrBuf ()", f_sb2str },
	{ 0, &Type_strbuf, "str ()", f_str2sb },
	{ FUNC_VIRTUAL, &Type_char, "operator[] (StrBuf, int)", f_index },
};

void CmdSetup_strbuf(void)
{
	AddType(&Type_strbuf);
	AddFuncDef(fdef_strbuf, tabsize(fdef_strbuf));
}
