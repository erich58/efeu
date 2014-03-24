/*
Konfiguration der Initialisierung

$Header <EFEU/$1>

$Copyright (C) 1996 Erich Frühstück
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

#ifndef	EFEU_cmdconfig_h
#define	EFEU_cmdconfig_h	1

#include <EFEU/cmdsetup.h>

#define	MAKE_ASSIGN_OP	1

#define	CEXPR(name, expr)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ expr; }

#define	LEXPR(name, expr)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ expr; Val_ptr(rval) = arg[0]; }

#endif	/* EFEU/cmdconfig.h */
