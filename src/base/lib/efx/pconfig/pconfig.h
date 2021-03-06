/*
:*:programm configuration
:de:Programmkonfiguration

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

#ifndef	EFEU_pconfig_H
#define	EFEU_pconfig_H	1

#include <EFEU/Resource.h>
#include <EFEU/Info.h>
#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/procenv.h>
#include <EFEU/appl.h>

void SetupReadline (void);
void SetupDebug (void);

extern char *PS1;
extern char *PS2;
extern int float_align;

int EshEval (int *narg, char **arg);
void EshIdent (IO *in);

#endif	/* EFEU/pconfig.h */
