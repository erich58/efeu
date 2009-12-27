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

#define	DBG_NONE	0	/* Keine Meldungen */
#define	DBG_ERR		1	/* Fehlermeldungen */
#define	DBG_NOTE	2	/* Anmerkungen */
#define	DBG_INFO	3	/* Verarbeitungsinformationen */
#define	DBG_STAT	4	/* Verarbeitungsstatistik */
#define	DBG_DEBUG	5	/* Debug-Informationen */
#define	DBG_TRACE	6	/* Ablaufprotokoll */
#define	DBG_DIM		7	/* Zahl der Debug-Level */

typedef struct DebugClass {
	const char *name;	/* Bezeichnung */
	int level;		/* Debug - Level */
	int sync;		/* Debug - Synchronisation */
	FILE *log;		/* Log - File */
} DebugClass;

#define DEBUG_CLASS_DATA(name, level)	{ name, level, 0, NULL }

extern int DebugKey (const char *name);
extern char *DebugLabel (int type);
extern void DebugMode (const char *def);
extern FILE *LogFile (const char *cl, int level);
extern FILE *DebugClassFile (DebugClass *cl);
extern FILE *ParseLogFile (const char *def);
extern IO *LogOut (const char *cl, int level);
extern IO *ParseLogOut (const char *def);

extern int DebugChangeCount;

extern char *FormatTabEntry (const char *name, const char *key);
extern void FormatTabLoad (IO *io, const char *name, int endkey);
extern void FormatTabShow (IO *io, const char *name, const char *fmt);

void dbg_psub (const char *cl, int level, const char *fmt,
	ArgList *list);
void dbg_vpsub (const char *cl, int level, const char *fmt,
	char *id, const char *argdef, va_list);
void dbg_message (const char *cl, int level, const char *fmt,
	char *id, const char *argdef, ...);
void dbg_error (const char *cl, const char *fmt, const char *argdef, ...);
void dbg_note (const char *cl, const char *fmt, const char *argdef, ...);

/*
$SeeAlso
\mref{Debug(3)},
\mref{Message(3)}.
*/

#endif	/* EFEU/Debug.h */
