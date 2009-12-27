/*
:*:open protokoll chanel
:de:Protokollausgabe öffnen

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

#define	_ISOC99_SOURCE

#include <EFEU/extension.h>
#include <EFEU/LogConfig.h>
#include <EFEU/procenv.h>
#include <EFEU/Resource.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <EFEU/ioctrl.h>
#include <stdio.h>
#include <time.h>

typedef struct {
	LogControl *ctrl;	/* Steuerstruktur */
	StrBuf *buf;	/* Ausgabebuffer */
	time_t sec;	/* Sekunde */
	unsigned usec;	/* Mikrosekunde */
} LOGOUT;

static int log_putucs (int32_t c, IO *io);
static int log_put (int c, IO *io);
static int log_ctrl (IO *io, int req, va_list list);

#if	HAS_TIMEVAL
#include <sys/time.h>
#else
struct timeval {
	time_t tv_sec;
	unsigned tv_usec;
};
#endif

#define	FMT_DATE	"%04d-%02d-%02d"
#define	VAR_DATE	tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday

#define	FMT_TIME	"%02d:%02d:%02d"
#define	VAR_TIME	tm->tm_hour, tm->tm_min, tm->tm_sec

#if	HAS_GETTIMEOFDAY
#define	FMT	FMT_DATE " " FMT_TIME ".%06d"
#define	VAR	VAR_DATE, VAR_TIME, (int) log->usec
#else
#define	FMT	FMT_DATE " " FMT_TIME
#define	VAR	VAR_DATE, VAR_TIME
#endif

static char *reorg (LOGOUT *log, unsigned flags);

#define	U1(c)	((c & 0x80) == 0)	/* Test auf 1-Byte Sequenz */
#define	U2(c)	((c & 0xe0) == 0xc0)	/* Test auf 2-Byte Sequenz */
#define U3(c)	((c & 0xf0) == 0xe0)	/* Test auf 3-Byte Sequenz */
#define	U4(c)	((c & 0xf8) == 0xf0)	/* Test auf 4-Byte Sequenz */

#define	UF(c)	((c & 0xc0) == 0x80)	/* Test auf Folgezeichen */

IO *LogOpen (LogControl *ctrl)
{
	LOGOUT *log;
	IO *io;
	struct timeval tv;
	
	if	(!LogUpdate(ctrl))	return NULL;

#if	HAS_GETTIMEOFDAY
	gettimeofday(&tv, NULL);
#else
	tv.tv_sec = time(NULL);
	tv.tv_usec = 0;
#endif

	log = memalloc(sizeof *log);
	log->ctrl = ctrl;
	log->sec = tv.tv_sec;
	log->usec = tv.tv_usec;
	log->buf = sb_acquire();

	io = io_alloc();
	io->putucs = log_putucs;
	io->put = log_put;
	io->ctrl = log_ctrl;
	io->data = log;
	return io;
}

static void set_buf (IO *out, int c, int n)
{
	int i;

	for (i = 1; i < IO_MAX_SAVE; i++)
		out->save_buf[i] = (i < n);

	out->save_buf[0] = c;
	out->nsave = 1;
}

static void log_flush (IO *out)
{
	LOGOUT *log = out->data;
	int i;

	if	(!out->nsave)	return;

	if	(out->save_buf[out->nsave]) /* Unvollständig */
	{
		for (i = 0; i < out->nsave; i++)
			sb_putucs(latin9_to_ucs(out->save_buf[i]), log->buf);
	}
	else
	{
		for (i = 0; i < out->nsave; i++)
			sb_putc(out->save_buf[i], log->buf);
	}

	out->nsave = 0;
}

static int log_putucs (int32_t c, IO *io)
{
	LOGOUT *log = io->data;
	log_flush(io);
	return sb_putucs(c, log->buf);
}

static int log_put (int c, IO *io)
{
	LOGOUT *log = io->data;

	if	(c == EOF)
	{
		log_flush(io);
		return c;
	}

	if	(UF(c) && io->nsave && io->save_buf[io->nsave])
	{
		io->save_buf[io->nsave++] = c;
		return c;
	}

	log_flush(io);

	if	(U1(c))	return sb_putc(c, log->buf);
	else if	(U2(c))	set_buf(io, c, 2);
	else if	(U3(c))	set_buf(io, c, 3);
	else if	(U4(c))	set_buf(io, c, 4);
	else	sb_putucs(latin9_to_ucs(c), log->buf);

	return c;
}

static int log_close (LOGOUT *log)
{
	LogControlEntry *entry;

	if	(!log->buf->pos || log->buf->data[log->buf->pos - 1] != '\n')
		sb_putc('\n', log->buf);

	sb_putc(0, log->buf);

	for (entry = log->ctrl->entry; entry; entry = entry->next)
		entry->out->write(entry->out, reorg(log, entry->flags));

	sb_release(log->buf);
	memfree(log);
	return 0;
}

static int log_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:
		log_flush(io);
		return log_close(io->data);
	default:
		return EOF;
	}
}

/*
Die Hilfsfunktion |$1| fügt je nach Bedarf zu Beginn jeder Zeile
einen Zeitstempel ein und führt eine Konvertierung nach latin9 durch.
Das Ergebnis der Konvertierung wird an den Stringbuffer angehängt,
der ursprüngliche Inhalt wird nicht verändert.
*/

static char *reorg (LOGOUT *log, unsigned flags)
{
	StrBuf *sb;
	int start;
	int pfx;
	int i, j;
	char *p;
	int last;

	sb = log->buf;

	if	(!(flags & (LOGFLAG_TIME | LOGFLAG_TYPE | LOGFLAG_LATIN9)))
		return (char *) sb->data;

	start = sb->pos;
	sb_sync(sb);

	if	(flags & LOGFLAG_TIME)
	{
		struct tm *tm = localtime(&log->sec);
		sb_printf(sb, FMT, VAR);
		sb_putc(' ', sb);
	}

	if	(flags & LOGFLAG_TYPE)
	{
		sb_printf(sb, "[%s.%s] ",
			log->ctrl->name ? log->ctrl->name : "",
			LogLevelName(log->ctrl->level));
	}

	pfx = sb->pos;

	for (i = last = 0; sb->data[i]; i++)
	{
		if	(last == '\n')
		{
			for (j = start; j < pfx; j++)
				sb_putc(sb->data[j], sb);
		}

		sb_putc(sb->data[i], sb);
		last = sb->data[i];
	}

	p = sb_nul(sb) + start;
	sb->pos = start;
	sb_sync(sb);

	if	(flags & LOGFLAG_LATIN9)
		ucscopy_latin9(p, ~0, p);

	return p;
}
