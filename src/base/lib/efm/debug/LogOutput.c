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

static void write_syslog (LogOutput *out, const char *msg)
{
	syslog((int) (size_t) out->data, "%s", msg);
}

#endif

static void write_stderr (LogOutput *out, const char *msg)
{
	fputs(msg, stderr);
}

static LogOutput logout_tab[] = {
	{ "stderr", 0, write_stderr, NULL, NULL },
#if	HAS_SYSLOG
	{ "syslog", 0, write_syslog, NULL, (void *) LOG_ERR },
	{ "syslog.err", 0, write_syslog, NULL, (void *) LOG_ERR },
	{ "syslog.warn", 0, write_syslog, NULL, (void *) LOG_WARNING },
	{ "syslog.notice", 0, write_syslog, NULL, (void *) LOG_NOTICE },
	{ "syslog.info", 0, write_syslog, NULL, (void *) LOG_INFO },
	{ "syslog.debug", 0, write_syslog, NULL, (void *) LOG_DEBUG },
#endif
};

LogOutput StdLogOutput = { "stderr", 0, write_stderr, NULL, NULL };

static VECBUF(buf, 32, sizeof(LogOutput *));

static void setup_logout (void)
{
	int i;

	if	(buf.used)	return;

	for (i = 0; i < tabsize(logout_tab); i++)
		*((LogOutput **) vb_next(&buf)) = logout_tab + i;
}


void LogOutputStat (void)
{
	LogOutput **ptr;
	size_t n;

	setup_logout();

	for (n = buf.used, ptr = buf.data; n--; ptr++)
	{
		fprintf(stderr, "%s %zd\n",
			(*ptr)->name, (*ptr)->refcount);
	}
}

static void write_file (LogOutput *out, const char *msg)
{
	FILE *file = fopen(out->name, "a");

	if	(file)
	{
		fputs(msg, file);
		fclose(file);
	}
}

static void write_pipe (LogOutput *out, const char *msg)
{
	if	(!out->data)
		out->data = popen(out->name + 1, "w");

	if	(out->data)
	{
		fputs(msg, out->data);
		fflush(out->data);
	}
}

static void close_pipe (LogOutput *out)
{
	if	(out->data)
		pclose(out->data);
}

LogOutput *NewLogOutput (const char *name)
{
	LogOutput **ptr;
	LogOutput *out;
	size_t n;

	if	(!name)		return &StdLogOutput;
	if	(!name[0])	return NULL;

	setup_logout();

	for (n = buf.used, ptr = buf.data; n--; ptr++)
	{
		if	(strcmp((*ptr)->name, name) == 0)
		{
			if	((*ptr)->refcount)
			{
				(*ptr)->refcount++;
			}

			return *ptr;
		}
	}

	n = strlen(name) + 1;
	out = memalloc(sizeof *out + n);
	out->name = memcpy(out + 1, name, n);
	out->refcount = 1;

	if	(name[0] == '|')
	{
		out->write = write_pipe;
		out->close = close_pipe;
	}
	else
	{
		out->write = write_file;
		out->close = NULL;
	}

	out->data = NULL;
	*((LogOutput **) vb_next(&buf)) = out;
	return out;
}

void DelLogOutput (LogOutput *out)
{
	LogOutput **ptr;
	size_t n;

	if	(!out || !out->refcount)
		return;

	setup_logout();

	for (n = 0, ptr = buf.data; n < buf.used; n++)
	{
		if	(ptr[n] == out)
		{
			if	(ptr[n]->refcount <= 1)
			{
				if	(ptr[n]->close)
					ptr[n]->close(ptr[n]);

				ptr = vb_delete(&buf, n, 1);
				memfree(*ptr);
			}
			else	ptr[n]->refcount--;

			return;
		}
	}
}
