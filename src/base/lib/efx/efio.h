/*
IO-Definitionen für efmain Programmbibliothek

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

#ifndef	EFEU_efio_h
#define	EFEU_efio_h	1

#include <EFEU/efmain.h>
#include <EFEU/refdata.h>
#include <EFEU/io.h>
#include <EFEU/ioscan.h>
#include <EFEU/parsub.h>
#include <EFEU/iocpy.h>


/*	Hilfsprogramme zur TeX - Ausgabe
*/

int TeXputs (const char *str, IO *io);
int TeXquote (const char *str, IO *io);
int TeXputc (int c, IO *io);


/*	Makros für binäre Ein/Ausgabe
*/

#if	REVBYTEORDER
#define	put_MSBF	io_rwrite
#define	put_LSBF	io_write
#define	get_MSBF	io_rread
#define	get_LSBF	io_read
#else
#define	put_MSBF	io_write
#define	put_LSBF	io_rwrite
#define	get_MSBF	io_read
#define	get_LSBF	io_rread
#endif


/*	Zwischenspeichern von Strings
*/

size_t io_split (IO *io, const char *delim, char ***ptr);
size_t io_savestr (IO *io, const char *str);
char *io_loadstr (IO *io, char **ptr);


void io_eval (IO *io, const char *delim);


/*	Filterprogramme
*/

IO *io_crfilter (IO *io);
IO *io_pgfmt (IO *io);
IO *io_termout (IO *io, const char *term);


#endif	/* EFEU/efio.h */
