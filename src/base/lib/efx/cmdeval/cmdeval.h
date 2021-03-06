/*
Befehlsausführung

$Header <EFEU/$1>

$Copyright (C) 1998 Erich Frühstück
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

#ifndef	EFEU_cmdeval_h
#define	EFEU_cmdeval_h	1

#include <EFEU/object.h>
#include <EFEU/parsedef.h>

#define	CmdEval_Normal	0	/* Normale Verarbeitung */
#define	CmdEval_Cont	1	/* Fortsetzung */
#define	CmdEval_Break	2	/* Abbruch */
#define	CmdEval_Return	3	/* Rücksprung */

extern EfiObj *CmdEval_retval;
extern int CmdEval_stat;

extern void CmdEval_setup (void);

extern IO *CmdEval_cin;
extern IO *CmdEval_cout;

/*	Befehlsinterpreter
*/

void CmdEvalFunc(IO *in, IO *out, int flag);
int CmdPreProc (IO *in, int key);
void CmdEval (IO *in, IO *out);

IO *io_cmdeval (IO *in, const char *delim);

#endif	/* EFEU/cmdeval.h */
