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
	lc->enabled = 0;
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
	case LOGLEVEL_ERR:	return "ERR";
	case LOGLEVEL_WARN:	return "WNG";
	case LOGLEVEL_NOTE:	return "MSG";
	case LOGLEVEL_INFO:	return "INF";
	case LOGLEVEL_DEBUG:	return "DBG";
	case LOGLEVEL_TRACE:	return "TRC";
	default:		break;
	}

	return "XXX";
}

int LogLevelIndex (const char *name)
{
	static struct {
		const char *name;
		int level;
	} map[] = {
		{ "err", LOGLEVEL_ERR },
		{ "warn", LOGLEVEL_WARN },
		{ "note", LOGLEVEL_NOTE },
		{ "info", LOGLEVEL_INFO },
		{ "debug", LOGLEVEL_DEBUG },
		{ "trace", LOGLEVEL_TRACE },
		{ NULL, 0 },
	};

	for (int n = 0; map[n].name; ++n)
		if (strcmp(name, map[n].name) == 0)
			return map[n].level;

	return 0;
}
