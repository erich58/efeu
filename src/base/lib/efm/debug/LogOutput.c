/*
:*:logging output
:de:Log-Ausgabe

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

#include <EFEU/extension.h>
#include <EFEU/LogConfig.h>
#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>

#if	HAS_SYSLOG

#include <syslog.h>

static void slog_err (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	syslog(LOG_ERR, "%s", msg + pfx);
}

static void slog_warn (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	syslog(LOG_WARNING, "%s", msg + pfx);
}

static void slog_note (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	syslog(LOG_NOTICE, "%s", msg + pfx);
}

static void slog_info (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	syslog(LOG_INFO, "%s", msg + pfx);
}

static void slog_debug (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	syslog(LOG_DEBUG, "%s", msg + pfx);
}
#endif

static void w_stderr (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	write(2, msg, len);
}

static LogOutput logout_tab[] = {
	{ "stderr", w_stderr, NULL },
#if	HAS_SYSLOG
	{ "syslog", slog_err, NULL },
	{ "syslog.err", slog_err, NULL },
	{ "syslog.warning", slog_warn, NULL },
	{ "syslog.notice", slog_note, NULL },
	{ "syslog.info", slog_info, NULL},
	{ "syslog.debug", slog_debug, NULL },
#endif
	{ NULL, NULL, NULL },
};

static void write_file (LogOutput *out, const char *msg, size_t pfx, size_t len)
{
	FILE *file = fopen(out->name, "a");

	if	(file)
	{
		fputs(msg, file);
		fclose(file);
	}
}

static void close_file (LogOutput *out)
{
	realloc_errcheck(out, 0);
}

LogOutput *NewLogOutput (const char *name)
{
	LogOutput *out;
	size_t n;

	if	(!name || !*name)
		return NULL;

	for (out = logout_tab; out->name; ++out)
	{
		if	(strcmp(out->name, name) == 0)
			return out;
	}

	n = strlen(name) + 1;
	out = realloc_errcheck(NULL, sizeof *out + n);
	out->name = memcpy(out + 1, name, n);
	out->write = write_file;
	out->close = close_file;
	return out;
}

void DelLogOutput (LogOutput *out)
{
	if	(out && out->close)
		out->close(out);
}
