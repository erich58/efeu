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

#ifndef	_EFEU_Debug_h
#define	_EFEU_Debug_h	1

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

extern int DebugKey (const char *name);
extern char *DebugLabel (int type);
extern void DebugMode (const char *def);
extern FILE *LogFile (const char *class, int level);
extern FILE *ParseLogFile (const char *def);
extern io_t *LogOut (const char *class, int level);
extern io_t *ParseLogOut (const char *def);

extern void Message (const char *class, int level,
	const char *fmt, ArgList_t *args);
extern void VMessage (const char *class, int level,
	const char *fmt, int narg, ...);

extern int DebugChangeCount;

/*
$SeeAlso
\mref{Debug(3)},
\mref{Message(3)}.
*/

#endif	/* EFEU/Debug.h */
