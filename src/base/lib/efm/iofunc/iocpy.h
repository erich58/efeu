/*
IO-Kopierhilfsfunktionen

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

#ifndef	EFEU_iocpy_h
#define	EFEU_iocpy_h	1

#include <EFEU/io.h>

/*	Standardkopierroutinen
*/

int iocpy_esc (IO *in, IO *out, int key, const char *arg, unsigned flags);
int iocpy_mark (IO *in, IO *out, int key, const char *arg, unsigned flags);
int iocpy_repl (IO *in, IO *out, int key, const char *arg, unsigned flags);

int iocpy_skip (IO *in, IO *out, int key, const char *arg, unsigned flags);
int iocpy_cskip (IO *in, IO *out, int key, const char *arg, unsigned flags);

int iocpy_value (IO *in, IO *out, int key, const char *arg, unsigned flags);
int iocpy_name (IO *in, IO *out, int key, const char *arg, unsigned flags);
int iocpy_str (IO *in, IO *out, int key, const char *arg, unsigned flags);
int iocpy_xstr (IO *in, IO *out, int key, const char *arg, unsigned flags);

int iocpy_usage (IO *in, IO *out, int key, const char *arg, unsigned flags);

#endif	/* EFEU/iocpy.h */
