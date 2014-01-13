/*
:*:	application files
:de:	Applikationsdateien

$Header	<EFEU/$1>

$Copyright (C) 2001 Erich Frühstück
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

#ifndef	EFEU_appl_h
#define	EFEU_appl_h	1

#include <EFEU/procenv.h>
#include <EFEU/io.h>

extern char *ApplPath;		/* Suchpfad für Hilfsdateien */

/*	Hilfsdateitypen
*/

#define	APPL_CNF	1
#define	APPL_MSG	2
#define	APPL_HLP	3

extern IO *io_applfile (const char *name, int type);

#endif	/* EFEU/appl.h */
