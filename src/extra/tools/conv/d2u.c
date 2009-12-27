/*
:*:	newline conversion between UNIX and DOS
:de:	Zeilenumbrüche zwischen UNIX und DOS konvertieren

$Copyright (C) 2004 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#define _XOPEN_SOURCE 1

#include <EFEU/Resource.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/strbuf.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#include <errno.h>
#include <signal.h>

#define	NTRY	10	/* Maximalzahl der Versuche für temporäre Datei */

#define	CONV	"[d2u:conv]$1: file converted.\n"
#define	CHECK	"[d2u:check]$1: conversion proposed.\n"
#define	ISUNIX	"[d2u:isunix]$1: already a UNIX text file.\n"
#define	ISDOS	"[d2u:isdos]$1: already a DOS text file.\n"
#define	ISDIR	"[d2u:isdir]$1: directory skipped.\n"
#define	BINARY	"[d2u:binary]$1: binary file skiped.\n"
#define	ERROUT	"[d2u:errout]$1: output error, file not changed.\n"
#define	NOTMP	"[d2u:notmp]$1: no temporary name availabel.\n"

static int create_file (char *name)
{
	int fd = open(name, O_WRONLY | O_CREAT | O_EXCL, 0666);

	if	(fd >= 0)
	{
		close(fd);
		return 1;
	}

	return 0;
}

static char *tmp = NULL;
static int zflag = 0;

static void clfunc (void *par)
{
	if	(tmp)
	{
		remove(tmp);
		tmp = NULL;
	}
}

static STRBUF(buf, 0);

static char *savename (const char *path)
{
	const char *name;
	int pos;
	int i;

	if	(path == NULL)	return NULL;

	sb_clean(&buf);
	name = strrchr(path, '/');

	if	(name)
	{
		name++;

		for (i = 0; path + i < name; i++)
			sb_putc(path[i], &buf);
	}
	else	name = path;
	
	pos = sb_getpos(&buf);

	for (i = 0; i < NTRY; i++)
	{
		sb_setpos(&buf, pos);
		sb_sync(&buf);
		sb_printf(&buf, ".#%.0d%s", i, name);
		sb_putc(0, &buf);

		if	(create_file((char *) buf.data))
		{
			return (char *) buf.data;
		}
		else if	(errno != EEXIST)
		{
			perror(path);
			return NULL;
		}
		else	perror((char *) buf.data);
	}

	log_note(NULL, NOTMP, "s", path);
	return NULL;
}

static char *to_unix (FILE *in, FILE *out, int flag)
{
	int c;

	while ((c = getc(in)) != EOF)
	{
		if	(c == '\r')
		{
			if	((c = getc(in)) != '\n')
				return BINARY;
		}
		else if	(c == '\n')
		{
			if	(!flag)
				return ISUNIX;
		}
		else if	(c == '\t' || (zflag && c == 0))
		{
			;
		}
		else if	(c < 0x20 || c == 127 || c >= 0xF0)
		{
			return BINARY;
		}

		putc(c, out);

		if	(ferror(out))
			return ERROUT;
	}

	return NULL;
}

static char *to_dos (FILE *in, FILE *out, int flag)
{
	int c;

	while ((c = getc(in)) != EOF)
	{
		if	(c == '\r')
		{
			if	((c = getc(in)) != '\n')
				return BINARY;

			if	(!flag)
				return ISDOS;

			putc('\r', out);
		}
		else if	(c == '\n')
		{
			putc('\r', out);
		}
		else if	(c == '\t' || (zflag && c == 0))
		{
			;
		}
		else if	(c < 0x20 || c == 127 || c >= 0xF0)
		{
			return BINARY;
		}

		putc(c, out);

		if	(ferror(out))
			return ERROUT;
	}

	return NULL;
}

int main (int argc, char **argv)
{
	int i;
	char *src;
	struct stat statbuf;
	FILE *in;
	FILE *out;
	char *msg;
	char *(*copy) (FILE *in, FILE *out, int flag);
	int check;
	int stamp;

	SetProgName(argv[0]);
	SetVersion(EFEU_VERSION);
	ParseCommand(&argc, argv);
	check = GetFlagResource("CheckOnly");
	stamp = GetFlagResource("Timestamp");
	copy = GetFlagResource("TODOS") ? to_dos : to_unix;
	zflag = GetFlagResource("Zero");
	proc_clean(clfunc, NULL);

	for (i = 1; i < argc; i++)
	{
		if	(argv[i][0] == '-' && argv[i][1] == 0)
		{
			if	(check)
			{
				out = fileopen("/dev/null", "w");
				copy(stdin, out, 1);
				fileclose(out);
			}
			else	copy(stdin, stdout, 1);

			continue;
		}

		src = argv[i];

		if	(stat(src, &statbuf) != 0)
		{
			perror(src);
			continue;
		}

		if	(S_ISDIR(statbuf.st_mode))
		{
			log_note(NULL, ISDIR, "s", src);
			continue;
		}

		tmp = savename(src);

		if	(!tmp)
			continue;

		in = fileopen(src, "rz");
		out = fileopen(tmp, "wz");
		msg = copy(in, out, 0);
		fileclose(in);
		fileclose(out);

		if	(msg)
		{
			log_note(NULL, msg, "s", src);
			remove(tmp);
			tmp = NULL;
		}
		else if	(check)
		{
			log_note(NULL, CHECK, "s", src);
			remove(tmp);
			tmp = NULL;
		}
		else if	(rename(tmp, src) == 0)
		{
			log_note(NULL, CONV, "s", src);
			tmp = NULL;
		}
		else	perror(src);

		if	(stamp)
		{
			struct utimbuf ubuf;
			ubuf.actime = statbuf.st_atime;
			ubuf.modtime = statbuf.st_mtime;

			if	(utime(src,  &ubuf) != 0)
				perror(src);
		}
	}

	return EXIT_SUCCESS;
}
