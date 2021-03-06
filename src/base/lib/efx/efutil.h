/*
Hilfsprogramme

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

#ifndef	EFEU_efutil_h
#define	EFEU_efutil_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/ftools.h>
#include <EFEU/dbutil.h>

#ifndef	MSG_EFUTIL
#define	MSG_EFUTIL	"efutil"	/* Name für Fehlermeldungen */
#endif

/*	String/Zeile laden
*/

int io_egetc (IO *io, const char *delim);
char *io_lgets (IO *io, const char *endkey);
int iocpy_brace (IO *in, IO *out, int key, const char *arg, unsigned flags);
char *getstring (IO *io);


/*	Zufallszahlengenereator
*/

double drand48 (void);
long lrand48 (void);
void srand48 (long seedval);

void SetupUtil (void);

/*	Rundungshilfsprogramm
*/

void roundvec(double *x, size_t dim, double val);


/*	Hilfsprogramme zur TeX - Ausgabe
*/

int TeXputs(const char *str, IO *io);
int TeXquote(const char *str, IO *io);
int TeXputc(int c, IO *io);

#endif	/* EFEU/efutil.h */
