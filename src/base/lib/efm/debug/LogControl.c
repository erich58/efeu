/*
:*:logging control
:de:Steuerstruktur für Protokolle

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

#include <EFEU/LogConfig.h>
#include <EFEU/procenv.h>
#include <EFEU/mstring.h>

static void lc_clean (void *ptr)
{
	LogControl *lc = ptr;
	memfree((char *) lc->name); /* cast wegen const-Qualifizierung */
	memfree(lc);
}

static char *lc_ident (const void *ptr)
{
	const LogControl *lc = ptr;
	return msprintf("%s.%s", lc->name ? lc->name : "",
		LogLevelName(lc->level));
}

RefType LogControl_reftype = REFTYPE_INIT("LogControl", lc_ident, lc_clean);

LogControl *LogControl_create (const char *name)
{
	LogControl *lc;
	int level; 
	int namelen;

	level = LOGLEVEL_NOTE;

	if	(name)
	{
		for (namelen = 0; name[namelen]; namelen++)
		{
			if	(name[namelen] == '.')
			{
				level = LogLevelIndex(name + namelen + 1);
				break;
			}
		}
	}
	else	namelen = 0;

	lc = memalloc(sizeof *lc);
	lc->name = namelen ? mstrncpy(name, namelen) : NULL;
	lc->level = level;
	lc->sync = 0;
	lc->entry = NULL;
	return rd_init(&LogControl_reftype, lc);
}

static LogControl ErrLogData = LOG_CONTROL(NULL, LOGLEVEL_ERR);
static LogControl WarnLogData = LOG_CONTROL(NULL, LOGLEVEL_WARN);
static LogControl NoteLogData = LOG_CONTROL(NULL, LOGLEVEL_NOTE);
static LogControl InfoLogData = LOG_CONTROL(NULL, LOGLEVEL_INFO);
static LogControl DebugLogData = LOG_CONTROL(NULL, LOGLEVEL_DEBUG);
static LogControl TraceLogData = LOG_CONTROL(NULL, LOGLEVEL_TRACE);

LogControl *ErrLog = &ErrLogData;
LogControl *WarnLog = &WarnLogData;
LogControl *NoteLog = &NoteLogData;
LogControl *InfoLog = &InfoLogData;
LogControl *DebugLog = &DebugLogData;
LogControl *TraceLog = &TraceLogData;

char *LogLevelName (int level)
{
	switch (level)
	{
	case LOGLEVEL_ERR:	return "err";
	case LOGLEVEL_WARN:	return "warn";
	case LOGLEVEL_NOTE:	return "note";
	case LOGLEVEL_INFO:	return "info";
	case LOGLEVEL_DEBUG:	return "debug";
	case LOGLEVEL_TRACE:	return "trace";
	default:		break;
	}

	return "none";
}

int LogLevelIndex (const char *name)
{
	if	(strcmp(name, "err") == 0)	return LOGLEVEL_ERR;
	if	(strcmp(name, "warn") == 0)	return LOGLEVEL_WARN;
	if	(strcmp(name, "note") == 0)	return LOGLEVEL_NOTE;
	if	(strcmp(name, "info") == 0)	return LOGLEVEL_INFO;
	if	(strcmp(name, "debug") == 0)	return LOGLEVEL_DEBUG;
	if	(strcmp(name, "trace") == 0)	return LOGLEVEL_TRACE;

	return 0;
}
