/*
Parametersubstitution

$Header	<EFEU/$1>

$Copyright (C) 1997 Erich Frühstück
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

#ifndef	EFEU_parsub_h
#define	EFEU_parsub_h	1

#include <EFEU/iocpy.h>
#include <EFEU/strbuf.h>
#include <EFEU/ArgList.h>

void psubfunc (int key, int (*copy) (IO *in, IO *out, void *par), void *par);

char *psubexpand (StrBuf *buf, IO *in, int argc, char **argv);
int io_psubvec (IO *io, const char *fmt, int argc, char **argv);
char *mpsubvec (const char *fmt, int argc, char **argv);
int io_pcopy (IO *in, IO *out, int delim, int argc, char **argv);
char *mpcopy (IO *in, int delim, int argc, char **argv);

char *psubexpandarg (StrBuf *buf, IO *in, ArgList *argl);

int io_pcopyarg (IO *in, IO *out, int delim, ArgList *argl);

int io_psubvarg (IO *io, const char *fmt, const char *argdef, va_list list);
int io_psubarg (IO *io, const char *fmt, const char *argdef, ...);
int io_psub (IO *io, const char *fmt, ArgList *argl);

char *mpsubvarg (const char *fmt, const char *argdef, va_list list);
char *mpsubarg (const char *fmt, const char *argdef, ...);
char *mpsub (const char *fmt, ArgList *argl);

IO *psubfilter (IO *io, ArgList *list);

#endif	/* EFEU/parsub.h */
