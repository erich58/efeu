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

#ifndef	LaTeX_h
#define	LaTeX_h	1

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
} LaTeX;

extern IO *DocOut_latex (IO *io);

extern int LaTeX_indexmode;
extern int LaTeX_putc (void *drv, int c);
extern int LaTeX_vputc (void *drv, int c);
extern int LaTeX_putucs (void *drv, int32_t c);
extern int LaTeX_vputucs (void *drv, int32_t c);
extern int LaTeX_xputc (void *drv, int c);
extern int LaTeX_plain (void *drv, int c);
extern void LaTeX_puts (LaTeX *ltx, const char *str);
extern void LaTeX_iputs (LaTeX *ltx, const char *str);
extern void LaTeX_newline (LaTeX *ltx);
extern void LaTeX_rem (void *drv, const char *rem);
extern int LaTeX_cmd (void *drv, va_list list);
extern int LaTeX_env (void *drv, int flag, va_list list);

extern void LaTeX_setup (void);
extern void LaTeX_SetupEnv (void);
extern void LaTeX_SetupCmd (void);
void LaTeX_ShowEnv (IO *io);
void LaTeX_ShowCmd (IO *io);

#endif	/* LaTeX.h */
