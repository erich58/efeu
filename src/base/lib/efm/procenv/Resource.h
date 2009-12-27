/*
:*:	command resources
:de:	Programmresourcen

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

#ifndef	EFEU_Resource_h
#define	EFEU_Resource_h	1

#include <EFEU/config.h>

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS	0
#endif

#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE	1
#endif

extern void ParseCommand (int *argc, char **argv);
extern void SetResource (const char *name, const char *val);
extern void SetVersion (const char *val);
extern char *GetResource (const char *name, const char *defval);
extern int GetIntResource (const char *name, int val);
extern int GetFlagResource (const char *name);
extern char *GetFormat (const char *def);

#endif	/* EFEU/Resource.h */
