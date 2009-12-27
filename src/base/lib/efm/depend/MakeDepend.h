/*
Abhängigkeitslisten

$Header	<EFEU/$1>

$Copyright (C) 1999 Erich Frühstück
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

#ifndef	EFEU_MakeDepend_h
#define	EFEU_MakeDepend_h	1

#include <EFEU/mstring.h>
#include <EFEU/vecbuf.h>
#include <EFEU/io.h>

extern int MakeDepend;
extern VecBuf DependList;
extern VecBuf TargetList;

void AddTarget (const char *name);
void AddDepend (const char *name);
void MakeDependRule (IO *io);
void MakeTargetRule (IO *io, const char *cmd);
void MakeTaskRule (IO *io, const char *name);
void MakeCleanRule (IO *io, const char *name);

#endif	/* EFEU/MakeDepend.h */
