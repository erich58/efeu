/*
:*:	file tools
:de:	Filehilfsprogramme

$Header	<EFEU/$1>
$Copyright (C) 1996, 2001 Erich Frühstück
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

#ifndef	EFEU_ftools_h
#define	EFEU_ftools_h	1

#include <stdio.h>
#include <EFEU/mstring.h>

#ifndef	SEEK_SET
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

extern int pipe_lock;

/*	Dateien suchen
*/

extern char *fsearch (const char *path, const char *pfx,
	const char *name, const char *ext);

/*	Datenfiles öffnen
*/

FILE *fileopen (const char *name, const char *mode);
FILE *pipeopen (const char *name, const char *mode);
FILE *tempopen (void);
FILE *findopen (const char *path, const char *pfx,
	const char *name, const char *ext, const char *mode);
int fileclose (FILE *file);
char *fileident (FILE *file);
void filenotice (const char *name, const char *mode,
	FILE *file, int (*close) (FILE *file));
FILE *filerefer (FILE *file);
void fileerror (FILE *file, const char *fmt,
	const char *argdef, ...);

/*
temporay files
*/

char *newtemp (const char *dir, const char *pfx);
char *newtempdir (const char *dir, const char *pfx);
int deltemp (char *path);
int deltempdir (char *path);
void tempstat (void);

/*	Binäre Ein/Ausgabe von Datenfiles
*/

size_t rfread (void *ptr, size_t size, size_t nitems, FILE *file);
size_t rfwrite (const void *ptr, size_t size, size_t nitems, FILE *file);

#if	REVBYTEORDER
#define	fread_compat	rfread
#define	fwrite_compat	rfwrite
#else
#define	fread_compat	fread
#define	fwrite_compat	fwrite
#endif

int fpeek (FILE *file);

unsigned get1byte (FILE *file);
unsigned get2byte (FILE *file);
unsigned get3byte (FILE *file);
unsigned get4byte (FILE *file);

void put1byte (unsigned val, FILE *file);
void put2byte (unsigned val, FILE *file);
void put3byte (unsigned val, FILE *file);
void put4byte (unsigned val, FILE *file);

char *loadstr (FILE *file);
void savestr (const char *str, FILE *file);

size_t dbread (FILE *file, void *dp, size_t size, size_t n);
size_t dbwrite (FILE *file, const void *dp, size_t size, size_t n);

#endif	/* EFEU/ftools.h */
