/*
Datei öffnen

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/patcmp.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/Debug.h>

#define FMT_1	"[ftools:1]$!: file \"$1\": undefined access mode \"$2\".\n"
#define FMT_2	"[ftools:2]$!: popen(\"$1\", \"$2\"): execution failed.\n"
#define FMT_3	"[ftools:3]$!: fopen(\"$1\", \"$2\"): could not open file.\n"
#define FMT_4	"[ftools:4]$!; fileopen(\"$1\", \"$2\"): execution locked.\n"

/*
$Description
April 1999:
	Flexiblere Kompressionsverfahren.
	Automatische Einträge in Abhängigkeitslisten.
März 2000:
	Filename "-" wird wie Nullpointer behandelt.
	Filedeskriptorkennungen "&0", "&1", "&2".
*/

#define	MODE_READ	0
#define	MODE_WRITE	1
#define	MODE_APPEND	2

static char *ftype[] = { "rb", "wb", "ab" };
static char *ptype[] = { "r", "w", "w" };

int pipe_lock = 0;

/*	Kompressionsfilter
*/

static struct {
	int mode;
	char *ext;
	char *res;
	char *cmd;
} ztab[] = {
	{ 'r',  "gz",	NULL,	"|gzip -cdf $1" },
	{ 'w',  "gz",	"GZIP", "|gzip $2 -c > $1" },
	{ 'a',  "gz",	"GZIP",	"|gzip $2 -c >> $1" },
	{ 'r',  "bz2",	NULL,	"|bzip2 -cd $1" },
	{ 'w',  "bz2",	NULL,	"|bzip2 -c > $1" },
};

#define	zcmp(x,m,e)	((x).mode == (m) && strcmp((x).ext, (e)) == 0)
#define zres(x)		((x).res ? GetResource((x).res, NULL) : NULL)

static char *zcmd (const char *name, const char *mode)
{
	char *ext;

	if	(strchr(mode, 'z') && (ext = strrchr(name, '.')))
	{
		int i;
		ext++;

		for (i = 0; i < tabsize(ztab); i++)
		{
			if	(zcmp(ztab[i], mode[0], ext))
			{
				return mpsubarg(ztab[i].cmd, "nss",
					name, zres(ztab[i]));
			}
		}
	}

	return NULL;
}


/*	Pipeline öffnen
*/

static FILE *open_pipe (const char *name, int mode)
{
	FILE *file = popen((char *) name + 1, ptype[mode]);
	
	if	(file == NULL)
		dbg_error(NULL, FMT_2, "ss", name + 1, ptype[mode]);

	filenotice(name, ptype[mode], file, pclose);
	return file;
}

/*	Datei öffnen
*/

static FILE *open_file (const char *name, int mode)
{
	FILE *file = fopen(name, ftype[mode]);
	
	if	(file == NULL)
		dbg_error(NULL, FMT_3, "ss", name, ftype[mode]);

	filenotice(name, ftype[mode], file, fclose);
	return file;
}


/*
Die Funktion |$1| öffnet die Datei <name> mit Zugriff <mode>
*/

FILE *fileopen (const char *name, const char *mode)
{
	char *fname;
	int omode;

/*	Zugriffstype bestimmen
*/
	switch (mode[0])
	{
	case 'r':	omode = MODE_READ; break;
	case 'w':	omode = MODE_WRITE; break;
	case 'a':	omode = MODE_APPEND; break;
	default:
		dbg_error(NULL, FMT_1, "ss", name, mode);
		return NULL;
	}

/*	Einträge in Abhängigkeitslisten
*/
	if	(strchr(mode, 'd'))
	{
		if	(omode != MODE_READ)
		{
			AddTarget(name);

			if	(MakeDepend)
				name = "/dev/null"; 
		}
		else	AddDepend(name);
	}

/*	Dateien öffnen
*/
	if	(name == NULL || (name[0] == '-' && name[1] == 0))
		return (omode == MODE_READ) ? stdin : stdout;

	if	(*name == '|')
	{
		if	(pipe_lock)
			dbg_error(NULL, FMT_4, "ss", name, omode);

		return open_pipe(name, omode);
	}

	if	(*name == '&')
	{
		switch (atoi(name + 1))
		{
		case  0:	return stdin;
		case  1:	return stdout;
		case  2:	return stderr;
		default:	break;
		}
	}

	if	((fname = zcmd(name, mode)))
	{
		FILE *file = open_pipe(fname, omode);
		memfree(fname);
		return file;
	}
	
	return open_file(name, omode);
}
