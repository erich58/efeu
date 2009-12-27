/*
mroff-Ausgabefilter

$Copyright (C) 1999 Erich Frühstück
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

#ifndef	mroff_h
#define	mroff_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocDrv.h>

extern void mroff_setup (void);
extern char *mroff_par (const char *name);
extern void mroff_addpar (VarDef_t *def, size_t dim);
extern void mroff_cmdpar (VarTab_t *tab);
extern void mroff_envpar (VarTab_t *tab);


typedef struct mroff_s mroff_t;
typedef struct mroff_var_s mroff_var_t;

struct mroff_var_s {
	mroff_var_t *next;
	void (*caption) (mroff_t *mr, int flag);
};

struct mroff_s {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	char *dochead;		/* Dokumentkopf */
	char *nextpar;		/* Nächster Absatz */
	unsigned copy : 1;	/* Kopiermodus */
	unsigned space : 1;	/* Leerzeichen wird benötigt */
	unsigned nlignore : 1;	/* Nächsten Zeilenvorschub ignorieren */
	unsigned item : 1;	/* Flag für item */
	mroff_var_t var;	/* Umgebungsvariablen */
	stack_t *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribut */
};

extern io_t *DocOut_mroff (io_t *io);

extern void mroff_push (mroff_t *mr);
extern void mroff_pop (mroff_t *mr);

extern void mroff_hdr (mroff_t *mr, int mode);
extern int mroff_putc (mroff_t *mr, int c);
extern int mroff_plain (mroff_t *mr, int c);
extern int mroff_protect (mroff_t *mr, int c);

extern void mroff_cbeg (mroff_t *mr, const char *pfx);
extern void mroff_cend (mroff_t *mr, int flag);

extern void mroff_psub (mroff_t *mr, const char *name);
extern void mroff_string (mroff_t *mr, const char *str);
extern void mroff_newline (mroff_t *mr);
extern void mroff_rem (mroff_t *mr, const char *rem);
extern int mroff_cmd (mroff_t *mr, va_list list);
extern int mroff_env (mroff_t *mr, int flag, va_list list);
extern void mroff_cmdline (mroff_t *mr, const char *name);
extern void mroff_cmdend (mroff_t *mr, const char *name);

#endif	/* roff.h */
