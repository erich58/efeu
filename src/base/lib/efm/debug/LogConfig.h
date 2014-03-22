/*
:*:	Log configuration
:de:	Protokollkonfiguration

$Header	<EFEU/$1>
$Copyright (C) 2009 Erich Frühstück
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

#ifndef	EFEU_LogConfig_h
#define	EFEU_LogConfig_h	1

#include <EFEU/refdata.h>

#define	LOGLEVEL_ERR	1	/* Fehlermeldungen */
#define	LOGLEVEL_WARN	2	/* Warnungen */
#define	LOGLEVEL_NOTE	3	/* Verarbeitungsinformationen */
#define	LOGLEVEL_INFO	4	/* Zusätzliche Informationen */
#define	LOGLEVEL_DEBUG	5	/* Debug-Informationen */
#define	LOGLEVEL_TRACE	6	/* Ablaufverfolgung */

char *LogLevelName (int level);
int LogLevelIndex (const char *name);

void LogConfig (const char *def);

typedef struct LogOutput LogOutput;

struct LogOutput {
	const char *name;
	size_t refcount;
	void (*write) (LogOutput *out, const char *msg);
	void (*close) (LogOutput *out);
	void *data;
};

void LogOutputStat (void);
LogOutput *NewLogOutput (const char *name);
void DelLogOutput (LogOutput *out);

typedef struct LogControlEntry LogControlEntry;

#define	LOGFLAG_UTF8	0x1	/* UTF8-Konvertierung wird benötigt */
#define	LOGFLAG_LATIN9	0x2	/* LATIN9-Konvertierung wird benötigt */

#define	LOGFLAG_TIME	0x10	/* Zeitstempel wird ausgegeben */
#define	LOGFLAG_TYPE	0x20	/* Typ der Logmeldung wird ausgegeben */

struct LogControlEntry {
	LogOutput *out;
	LogControlEntry *next;
	unsigned int flags;
};


typedef struct LogControl LogControl;

struct LogControl {
	REFVAR;			/* Referenzvariablen */
	const char *name;	/* Bezeichnung */
	unsigned int level;	/* Debug - Level */
	unsigned int sync;	/* Synchronisation */
	LogControlEntry *entry;	/* Tabelle mit Protokollausgaben */
};

extern RefType LogControl_reftype;
LogControl *LogControl_create (const char *name);

#define	LOG_CONTROL(name, level)	\
	{ REFDATA(&LogControl_reftype), name, level, 0, NULL }

extern LogControl *ErrLog;
extern LogControl *WarnLog;
extern LogControl *NoteLog;
extern LogControl *InfoLog;
extern LogControl *DebugLog;
extern LogControl *TraceLog;

int LogUpdate (LogControl *ctrl);

void log_error (LogControl *ctrl, const char *fmt, const char *argdef, ...);
void log_note (LogControl *ctrl, const char *fmt, const char *argdef, ...);

/*
$SeeAlso
\mref{Message(3)}.
*/

#endif	/* EFEU/LogConfig.h */
