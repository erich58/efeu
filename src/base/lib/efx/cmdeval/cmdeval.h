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

#ifndef	EFEU_CMDEVAL_H
#define	EFEU_CMDEVAL_H	1

#include <EFEU/object.h>
#include <EFEU/parsedef.h>

#define	CmdEval_Normal	0	/* Normale Verarbeitung */
#define	CmdEval_Cont	1	/* Fortsetzung */
#define	CmdEval_Break	2	/* Abbruch */
#define	CmdEval_Return	3	/* Rücksprung */

extern Obj_t *CmdEval_retval;
extern int CmdEval_stat;

extern void CmdEval_setup (void);

extern io_t *CmdEval_cin;
extern io_t *CmdEval_cout;

/*	Befehlsinterpreter
*/

void CmdEvalFunc(io_t *in, io_t *out, int flag);
int CmdPreProc (io_t *in, int key);
void CmdEval (io_t *in, io_t *out);

io_t *io_cmdeval (io_t *in, const char *delim);

#endif	/* EFEU_CMDEVAL_H */
