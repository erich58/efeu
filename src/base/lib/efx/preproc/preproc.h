/*
Preprozessor

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_preproc_h
#define	EFEU_preproc_h	1

#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/nkt.h>

#define	PREPROC	"preproc"

extern char *IncPath;

IO *io_ptrpreproc (IO *io, char **pptr, char **lptr);
IO *io_cmdpreproc (IO *io);

void AddDepend (const char *name);
void MakeDependList (IO *io, const char *name);

void Func_target (EfiFunc *func, void *rval, void **arg);
void Func_depend (EfiFunc *func, void *rval, void **arg);
void Func_dependlist (EfiFunc *func, void *rval, void **arg);
void Func_targetlist (EfiFunc *func, void *rval, void **arg);
void Func_makedepend (EfiFunc *func, void *rval, void **arg);

typedef struct PPMacroStruct PPMacro;

struct PPMacroStruct {
	char *name;		/* Makroname */
	char *repl;		/* Makrodefinition */
	unsigned hasarg : 1;	/* Makro mit Argumenten */
	unsigned lock : 15;	/* Sperrflag gegen rekursive Auflösung */
	unsigned vaarg : 1;	/* Makro mit variabler Argumentzahl */
	unsigned dim : 15;	/* Zahl der Argumente */
	PPMacro **arg;		/* Argumentliste */
	NameKeyTab *tab;	/* Makrotabelle mit Argumenten */
	int (*sub) (PPMacro *mac, IO *io, char **arg, int narg);
};


int macsub_repl (PPMacro *mac, IO *io, char **arg, int narg);
int macsub_subst (PPMacro *mac, IO *io, char **arg, int narg);
int macsub_def (PPMacro *mac, IO *io, char **arg, int narg);

PPMacro *NewMacro (void);
void DelMacro (void *ptr);
PPMacro *GetMacro (const char *name);

void AddMacro (PPMacro *macro);
PPMacro *ParseMacro (IO *io);
void ShowMacro (IO *io, PPMacro *macro);


/*	Makrodefinitionen
*/

typedef struct {
	char *def;	/* Makrodefinition */
	int (*sub) (PPMacro *mac, IO *io, char **arg, int narg);
} PPMacDef;

void AddMacDef (PPMacDef *macdef, size_t dim);


/*	Makrotabellen
*/

extern NameKeyTab *MacroTab;

NameKeyTab *NewMacroTab (size_t dim);
void DelMacroTab (NameKeyTab *tab);
void PushMacroTab (NameKeyTab *tab);
NameKeyTab *PopMacroTab (void);

#define	NewMacroTab(dim)	nkt_create("Macro", dim, DelMacro)
#define	DelMacroTab(tab)	rd_deref(tab)


/*	Substitutionsfunktionen
*/

int io_macsub (IO *in, IO *out, const char *delim);
int io_egetc(IO *io, const char *delim);

int iocpy_macsub (IO *in, IO *out, int key, const char *arg, unsigned flags);

void SetupPreproc (void);

#endif	/* EFEU/preproc.h */
