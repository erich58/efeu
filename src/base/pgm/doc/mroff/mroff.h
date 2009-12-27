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

void mroff_setup (void);
char *mroff_par (const char *name);
void mroff_addpar (EfiVarDef *def, size_t dim);
void mroff_cmdpar (EfiVarTab *tab);
void mroff_envpar (EfiVarTab *tab);


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

IO *DocOut_mroff (IO *io);

void mroff_push (ManRoff *mr);
void mroff_pop (ManRoff *mr);

void mroff_hdr (void *drv, int mode);
int mroff_putc (void *drv, int c);
int mroff_plain (void *drv, int c);
int mroff_protect (void *drv, int c);

void mroff_cbeg (ManRoff *mr, const char *pfx);
void mroff_cend (ManRoff *mr, int flag);

void mroff_string (ManRoff *mr, const char *str);
void mroff_newline (ManRoff *mr);
void mroff_rem (void *drv, const char *rem);
int mroff_cmd (void *drv, va_list list);
int mroff_env (void *drv, int flag, va_list list);
void mroff_cmdline (ManRoff *mr, const char *name);
void mroff_cmdend (ManRoff *mr, const char *name);

void mroff_tab_beg (ManRoff *mr, const char *opt, const char *def);
void mroff_tab_end (ManRoff *mr);
void mroff_tab_begrow (ManRoff *mr);
void mroff_tab_endrow (ManRoff *mr);
void mroff_tab_mcol (ManRoff *mr, int cdim, const char *def);
void mroff_tab_hline (ManRoff *mr, int n);
void mroff_tab_cline (ManRoff *mr, int n, int p1, int p2);
void mroff_tab_sep (ManRoff *mr);
void mroff_tab_nl (ManRoff *mr);

#endif	/* mroff.h */
