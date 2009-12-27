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

#define	PREPROC	"preproc"

extern char *IncPath;

io_t *io_ptrpreproc (io_t *io, char **pptr);
io_t *io_cmdpreproc (io_t *io);

void AddDepend (const char *name);
void MakeDependList (io_t *io, const char *name);

void Func_target (Func_t *func, void *rval, void **arg);
void Func_depend (Func_t *func, void *rval, void **arg);
void Func_dependlist (Func_t *func, void *rval, void **arg);
void Func_targetlist (Func_t *func, void *rval, void **arg);
void Func_makedepend (Func_t *func, void *rval, void **arg);

typedef struct Macro_s Macro_t;

struct Macro_s {
	char *name;		/* Makroname */
	char *repl;		/* Makrodefinition */
	unsigned hasarg : 1;	/* Makro mit Argumenten */
	unsigned lock : 15;	/* Sperrflag gegen rekursive Auflösung */
	unsigned vaarg : 1;	/* Makro mit variabler Argumentzahl */
	unsigned dim : 15;	/* Zahl der Argumente */
	Macro_t **arg;		/* Argumentliste */
	xtab_t *tab;		/* Makrotabelle mit Argumenten */
	int (*sub) (Macro_t *mac, io_t *io, char **arg, int narg);
};


int macsub_repl (Macro_t *mac, io_t *io, char **arg, int narg);
int macsub_subst (Macro_t *mac, io_t *io, char **arg, int narg);
int macsub_def (Macro_t *mac, io_t *io, char **arg, int narg);

size_t macarglist (io_t *io, char ***ptr);

Macro_t *NewMacro (void);
void DelMacro (Macro_t *macro);
Macro_t *GetMacro (const char *name);

void AddMacro (Macro_t *macro);
Macro_t *ParseMacro (io_t *io);


/*	Makrodefinitionen
*/

typedef struct {
	char *def;	/* Makrodefinition */
	int (*sub) (Macro_t *mac, io_t *io, char **arg, int narg);
} MacDef_t;

void AddMacDef (MacDef_t *macdef, size_t dim);


/*	Makrotabellen
*/

extern xtab_t *MacroTab;

xtab_t *NewMacroTab (size_t dim);
void DelMacroTab (xtab_t *tab);
void PushMacroTab (xtab_t *tab);
xtab_t *PopMacroTab (void);

#define	NewMacroTab(dim)	xcreate(dim, skey_cmp)
#define	DelMacroTab(tab)	xdestroy(tab, (clean_t) DelMacro)


/*	Substitutionsfunktionen
*/

int io_macsub (io_t *in, io_t *out, const char *delim);
int io_fmacsub (const char *fmt, io_t *out);
char *macsub (const char *fmt);
int io_egetc(io_t *io, const char *delim);

int iocpy_macsub (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_strmacsub (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

void SetupPreproc (void);

#endif	/* EFEU/preproc.h */
