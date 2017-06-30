/*
:*:	process environment
:de:	Prozessumgebung

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

#ifndef	EFEU_procenv_h
#define	EFEU_procenv_h	1

#include <EFEU/config.h>

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS	0
#endif

#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE	1
#endif

extern char *ProgDir;		/* Programmverzeichnis, falls verfügbar */
extern char *ProgName;		/* Programmname */
extern char *ProgIdent;		/* Programmidentifikation */
extern char *ApplPath;		/* Suchpfad für Hilfsdateien */
extern char *InfoPath;		/* Suchpfad für Informationsdateien */
extern char *Pager;		/* Seitenfilter */
extern char *Shell;		/* Shell */
extern char *CurrentLocale;	/* Aktuelle Lokale-Setzung */

/*	Programmnamen initialisieren
*/

void ChangeLocale (const char *def);
void EfeuConfig (const char *name);
void SetProgName (const char *name);
void SetApplPath (const char *path);
void SetInfoPath (const char *path);
void SetInfoPath (const char *path);

char *ExpandPath (const char *name);

/*	Prozesssteuerung
*/

void proc_clean (void (*clean) (void *par), void *par);
void proc_doclean (void);

/*	shared objects
*/

extern char *so_path;
void *so_open (const char *name);
void so_close (void *handle);
void loadlib (const char *name, const char *init);

/*	Systemaufrufe
*/

int callproc (const char *cmd);
extern int callproc_lock;
extern void (*callproc_init) (void);
extern void (*callproc_exit) (void);

#endif	/* EFEU/procenv.h */
