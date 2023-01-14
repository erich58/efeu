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

#include <EFEU/Debug.h>
#include <EFEU/memalloc.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

#define	MSG_SIZE	256
#define	MSG_LIM1	180
#define	MSG_LIM2	253

typedef struct {
	LogControl *ctrl;	/* Steuerstruktur */
	size_t offset;		/* Offset für Nachricht */
	size_t pos;		/* Aktuelle Position */
	char data[MSG_SIZE];	/* Datenbuffer */
} LOGDATA;

static int log_put (int c, IO *io);
static int log_ctrl (IO *io, int req, va_list list);

IO *log_open (LogControl *ctrl)
{
	LOGDATA *log;
	IO *io;
	
	if	(!LogUpdate(ctrl))	return NULL;

	log = realloc_errcheck(NULL, sizeof *log);
	log->ctrl = ctrl;
	log->offset = snprintf(log->data, sizeof log->data, "%s [%s.%s] ",
		log_timestamp(NULL), ctrl->name, LogLevelName(ctrl->level));
	log->pos = log->offset;

	io = io_alloc();
	io->put = log_put;
	io->ctrl = log_ctrl;
	io->data = log;
	return io;
}

static void log_flush (LOGDATA *log)
{
	log->data[log->pos++] = '\n';
	log->data[log->pos++] = 0;
	log_write(log->ctrl, log->data, log->offset);
	log->pos = log->offset;
}

static int log_put (int c, IO *io)
{
	LOGDATA *log = io->data;

	if	(c == EOF || c == '\n')
	{
		log_flush(log);
		return c;
	}

	if	(log->pos >= MSG_LIM2)
	{
		log->data[log->pos++] = '\\';
		log_flush(log);
	}

	log->data[log->pos++] = c;

	if	((c == ' ' || c == '\t') && log->pos >= MSG_LIM1)
	{
		log->data[log->pos++] = '\\';
		log_flush(log);
	}

	return c;
}

static int log_close (LOGDATA *log)
{
	if	(log->pos > log->offset)
		log_flush(log);

	realloc_errcheck(log, 0);
	return 0;
}

static int log_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:
		return log_close(io->data);
	default:
		return EOF;
	}
}
