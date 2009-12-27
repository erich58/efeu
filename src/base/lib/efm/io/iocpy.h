/*
IO-Kopierdefinitionen

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

#ifndef	_EFEU_iocpy_h
#define	_EFEU_iocpy_h	1

#include <EFEU/io.h>

typedef struct {
	char *key;
	char *arg;
	unsigned flags;
	int (*func) (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
} iocpy_t;


int iocpy (io_t *in, io_t *out, iocpy_t *def, size_t dim);
char *miocpy (io_t *in, iocpy_t *def, size_t dim);
int iocpyfmt (const char *fmt, io_t *out, iocpy_t *def, size_t dim);
char *miocpyfmt (const char *fmt, iocpy_t *def, size_t dim);
io_t *cpyfilter (io_t *io, iocpy_t *def, size_t dim);

extern int iocpy_flag;	/* Flag für Sonderzeichen */
extern int iocpy_last;	/* Zuletzt gelesenes Zeichen */


/*	Standardkopierroutinen
*/

int iocpy_esc (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_mark (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_repl (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_skip (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_cskip (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_psub (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_eval (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_term (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_value (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_name (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_str (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_xstr (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_usage (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

#endif	/* EFEU/iocpy.h */
