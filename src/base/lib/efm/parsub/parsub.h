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

#ifndef	_EFEU_parsub_h
#define	_EFEU_parsub_h	1

#include <EFEU/iocpy.h>

typedef int (*iocopy_t) (io_t *in, io_t *out, void *par);

typedef struct {
	int key;
	iocopy_t copy;
	void *par;
} copydef_t;

int iocopystr (const char *fmt, io_t *out, iocopy_t copy, void *arg);
char *miocopy (io_t *in, iocopy_t copy, void *arg);
char *miocopystr (const char *fmt, iocopy_t copy, void *arg);

char *parsub (const char *fmt);
void psubfunc (int key, iocopy_t copy, void *par);
int io_pcopy (io_t *in, io_t *out, void *par);
char *mpcopy (io_t *in);
int io_psub (io_t *io, const char *fmt);

extern int psub_key;

char *reg_get (int n);
char *reg_set (int n, char *arg);
char *reg_cpy (int n, const char *arg);
char *reg_str (int n, const char *arg);
char *reg_fmt (int n, const char *fmt, ...);
void reg_clear (void);

#endif	/* EFEU/parsub.h */
