/*
:*:	debugmode
:de:	Debugmodus

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

#ifndef	EFEU_Debug_h
#define	EFEU_Debug_h	1

#include <EFEU/Resource.h>
#include <EFEU/ArgList.h>
#include <EFEU/io.h>

extern char *FormatTabEntry (const char *name, const char *key);
extern void FormatTabLoad (IO *io, const char *name, int endkey);
extern void FormatTabShow (IO *io, const char *name, const char *fmt);

void log_psub (LogControl *ctrl, const char *fmt, ArgList *list);
void log_psubvarg (LogControl *ctrl, const char *fmt, char *id,
	const char *argdef, va_list);
void log_psubarg (LogControl *ctrl, const char *fmt, char *id,
	const char *argdef, ...);
void log_error (LogControl *ctrl, const char *fmt, const char *argdef, ...);
void log_note (LogControl *ctrl, const char *fmt, const char *argdef, ...);

void filedebug (FILE *file, LogControl *ctrl,
		const char *fmt, const char *def, ...);


void AllocTab_stat (IO *out, AllocTab *tab, int verbosity);

/*
$SeeAlso
\mref{LogConfig(3)}.
*/

#endif	/* EFEU/Debug.h */
