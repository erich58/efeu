/*
Interpreterstatus

$Copyright (C) 1995 Erich Frühstück
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
#include <EFEU/EfiStat.h>
#include <EFEU/printobj.h>
#include <EFEU/cmdsetup.h>

#define	SB(x)	((StrBuf **) (x))[0]

static void f_src_pop (EfiFunc *func, void *rval, void **arg)
{
	Efi *efi = rd_refer(Val_ptr(arg[0]));
	EfiSrc_pop(efi);
	Val_ptr(rval) = efi;
}

static void f_src_push (EfiFunc *func, void *rval, void **arg)
{
	Efi *efi = rd_refer(Val_ptr(arg[0]));
	StrBuf *sb;
	IO *io;
	EfiSrc *src;

	sb = sb_create(0);
	io = io_strbuf(sb);
	PrintFmtList(io, Val_str(arg[1]), Val_list(arg[2]));
	io_close(io);
	src = EfiSrc_alloc(NULL);
	src->cmd = sb2str(sb);
	EfiSrc_push(efi, src);
	Val_ptr(rval) = efi;
}

static EfiFuncDef fdef_stat[] = {
	{ 0, &Type_efi, "Efi::src_push (str fmt, ...)", f_src_push },
	{ 0, &Type_efi, "Efi::src_pop ()", f_src_pop },
};

void CmdSetup_stat(void)
{
	AddFuncDef(fdef_stat, tabsize(fdef_stat));
}
