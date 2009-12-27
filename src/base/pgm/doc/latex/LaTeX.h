/*
LaTeX-Ausgabefilter

$Copyright (C) 2001 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	_EFEU_LaTeX_h
#define	_EFEU_LaTeX_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocDrv.h>


typedef struct {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	int ignorespace;	/* Leerzeichen ignorieren */
	int space;		/* Leerzeichen vor Text */
} LaTeX_t;

extern io_t *DocOut_latex (io_t *io);

extern int LaTeX_indexmode;
extern int LaTeX_putc (LaTeX_t *ltx, int c);
extern int LaTeX_xputc (LaTeX_t *ltx, int c);
extern int LaTeX_plain (LaTeX_t *ltx, int c);
extern void LaTeX_puts (LaTeX_t *ltx, const char *str);
extern void LaTeX_iputs (LaTeX_t *ltx, const char *str);
extern void LaTeX_newline (LaTeX_t *ltx);
extern void LaTeX_rem (LaTeX_t *ltx, const char *rem);
extern int LaTeX_cmd (LaTeX_t *ltx, va_list list);
extern int LaTeX_env (LaTeX_t *ltx, int flag, va_list list);

extern void LaTeX_setup (void);
extern void LaTeX_SetupEnv (void);
extern void LaTeX_SetupCmd (void);
void LaTeX_ShowEnv (io_t *io);
void LaTeX_ShowCmd (io_t *io);

extern char *LaTeX_var (const char *name);
extern void LaTeX_psub (LaTeX_t *ltx, const char *name);

#endif	/* EFEU/LaTeX.h */
