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
extern void mroff_addpar (EfiVarDef *def, size_t dim);
extern void mroff_cmdpar (EfiVarTab *tab);
extern void mroff_envpar (EfiVarTab *tab);


typedef struct ManRoffStruct ManRoff;
typedef struct ManRoffVarStruct ManRoffVar;

struct ManRoffVarStruct {
	ManRoffVar *next;
	void (*caption) (ManRoff *mr, int flag);
};

struct ManRoffStruct {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	char *dochead;		/* Dokumentkopf */
	char *nextpar;		/* Nächster Absatz */
	unsigned copy : 1;	/* Kopiermodus */
	unsigned space : 1;	/* Leerzeichen wird benötigt */
	unsigned nlignore : 1;	/* Nächsten Zeilenvorschub ignorieren */
	unsigned item : 1;	/* Flag für item */
	ManRoffVar var;	/* Umgebungsvariablen */
	Stack *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribut */
};

extern IO *DocOut_mroff (IO *io);

extern void mroff_push (ManRoff *mr);
extern void mroff_pop (ManRoff *mr);

extern void mroff_hdr (void *drv, int mode);
extern int mroff_putc (void *drv, int c);
extern int mroff_plain (void *drv, int c);
extern int mroff_protect (void *drv, int c);

extern void mroff_cbeg (ManRoff *mr, const char *pfx);
extern void mroff_cend (ManRoff *mr, int flag);

extern void mroff_string (ManRoff *mr, const char *str);
extern void mroff_newline (ManRoff *mr);
extern void mroff_rem (void *drv, const char *rem);
extern int mroff_cmd (void *drv, va_list list);
extern int mroff_env (void *drv, int flag, va_list list);
extern void mroff_cmdline (ManRoff *mr, const char *name);
extern void mroff_cmdend (ManRoff *mr, const char *name);

#endif	/* mroff.h */
