/*
:*:read tape file
:de:Lesen eines Magnetbandes

$Copyright (C) 1998, 2001 Erich Frühstück
This file is part of EFEU.

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

#ifdef	EFEUROOT
#define	EFEUCFG	1
#define	FDEF(name,def)	GetFormat("[%" name "]" def)
#else
#define	EFEUCFG	0
#define	FDEF(name,def)	def
#endif

#define	ERR1	FDEF("err.1", "%s: wrong block size (%d != %d).\n")
#define	ERR2	FDEF("err.2", "%s: error in block size (%d, %d).\n")
#define	ERR3	FDEF("err.3", "%s: block size not defined.\n")
#define	ERR4	FDEF("err.4", "%s: malloc(%d) failed.\n")
#define	ERR5	FDEF("err.5", "%s: file %s not readable")
#define	MSG1	FDEF("msg.1", "max. block size %d.\n")
#define	MSG2	FDEF("msg.2", "%d+%d blocks read.\n")
#define	MSG3	FDEF("msg.3", "%d bytes write.\n")
#define	MSG4	FDEF("msg.4", "%d defect blocks.\n")

#define _XOPEN_SOURCE 1

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#if	__linux__
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#if	EFEUCFG
#include <EFEU/Resource.h>
#endif

#define	COMPRESS	"gzip -1"

#define	USAGE	"usage: %s [-hvfzx] [-n count] [-b size] dev [file]\n\n"

#define	OPTLIST	"\
\t-h\tshort command information\n\
\t-v\tvariable blocking\n\
\t-f\tfix blocking (default)\n\
\t-z\tcompress output file\n\
\t-x\trecover tape\n\
\t-n count\tread only count blocks\n\
\t-b size\tblock size, default %u\n\
\t-s size\tsplit size, default %u\n\
"

#define	BLKSIZE	0x8000

char *PgmName = NULL;
char *Output = NULL;
char str_buf[1024];
int idev = EOF;

/*	Eingabefunktionen
*/

static int get_stdin (char *buf, size_t size)
{
	return fread(buf, 1, size, stdin);
}

static int get_file (char *buf, size_t size)
{
	return read(idev, buf, size);
}

/*	Ausgabefunktionen
*/

static int put_null (FILE *file, char *buf, size_t n)
{
	return n;
}

static int put_fix (FILE *file, char *buf, size_t n)
{
	return (fwrite(buf, n, 1, file) == 1) ? n : EOF;
}

static int put_var (FILE *file, char *buf, size_t n)
{
	return (fwrite(buf + 4, n - 4, 1, file) == 1) ? n - 4 : EOF;
}

/*	Blocklängenfeld testen
*/

static int test_bsize (char *buf, int n)
{
	int k;

	k = (buf[0] << 8) + buf[1];

	if	(k != n)
	{
		fprintf(stderr, ERR1, PgmName, k, n);
		return 1;
	}

	if	(buf[2] || buf[3])
	{
		fprintf(stderr, ERR2, PgmName, buf[2], buf[3]);
		return 1;
	}

	return 0;
}

/*	Ausgabefile öffnen/schließen
*/

static int open_count = 0;

static FILE *open_output (char *name, int compress, int recover)
{
	FILE *file;

	if	(name == NULL)
	{
		file = compress ? popen(COMPRESS, "w") : stdout;
	}
	else if	(compress)
	{
		if	(recover)
		{
			open_count++;
			sprintf(str_buf, "%s > %s.%02d.gz",
				COMPRESS, name, open_count);
		}
		else	sprintf(str_buf, "%s > %s.gz", COMPRESS, name);

		file = popen(str_buf, "w");
	}
	else if	(recover)
	{
		open_count++;
		sprintf(str_buf, "%s.%02d", name, open_count);
		file = fopen(str_buf, "wb");
	}
	else	file = fopen(name, "wb");

	if	(file == NULL)
	{
		perror(name);
		return NULL;
	}

	return file;
}

static void close_output (FILE *file, int compress)
{
	if	(file == NULL)		;
	else if	(compress)		pclose(file);
	else if	(file != stdout)	fclose(file);
}


static unsigned get_size (const char *str, unsigned defval)
{
	char *p;
	unsigned s;

	if	(!str)	return defval;

	s = strtoul(str, &p, 10);

	if	(p == NULL)	;
	else if	(*p == 'k')	s *= 1024;
	else if	(*p == 'M')	s *= 1024 * 1024;
	else if	(*p == 'G')	s *= 1024 * 1024 * 1024;

	return s;
}

/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	extern char *optarg;
	extern int optind;
	unsigned maxblk;
	char *buf;
	unsigned nblk;
	unsigned nerr;
	unsigned nshort;
	unsigned nout;
	int blksize;
	int compress;
	int varblock;
	unsigned splitsize;
	unsigned blkread, blklim;
	int recover;
	int n;
	FILE *ofile;
	int (*get)(char *buf, size_t size);
	int (*put)(FILE *file, char *buf, size_t size);

	PgmName = arg[0];

#if	EFEUCFG
	SetVersion("$Id: rawtape.c,v 1.9 2006-03-01 10:00:43 ef Exp $");
	ParseCommand(&narg, arg);
	varblock = GetFlagResource("varblock");
	recover = GetFlagResource("recover");
	compress = GetFlagResource("compress");
	blksize = get_size(GetResource("blksize", NULL), BLKSIZE);
	blklim = get_size(GetResource("blklim", NULL), 0);
	splitsize = get_size(GetResource("splitsize", NULL), 0);
	arg++;
	narg--;
#else

/*	Optionen und Argumente
*/
	compress = 0;
	varblock = 0;
	recover = 0;
	blksize = BLKSIZE;
	blklim = 0;
	splitsize = 0;

	while ((n = getopt(narg, arg, "hvfxzn:b:s:")) != EOF)
	{
		switch (n)
		{
		case 'h':
			fprintf(stderr, USAGE, PgmName);
			fprintf(stderr, OPTLIST, blksize);
			return 1;
		case 'v': varblock = 1; break;
		case 'f': varblock = 0; break;
		case 'x': recover = 1; break;
		case 'z': compress = 1; break;
		case 'b': blksize = get_size(optarg, BLKSIZE); break;
		case 'n': blklim = get_size(optarg, 0); break;
		case 's': splitsize = get_size(optarg, 0); break;
		}
	}

	arg += optind;
	narg -= optind;

/*	Argumentzahl testen
*/
	if	(narg < 1 || narg > 2)
	{
		fprintf(stderr, USAGE, PgmName);
		return 1;
	}

#endif

/*	Blocklänge testen
*/
	if	(blksize == 0)
	{
		fprintf(stderr, ERR3, PgmName);
		return 1;
	}

/*	Datenbuffer
*/
	buf = (char *) malloc(blksize);

	if	(buf == NULL)
	{
		fprintf(stderr, ERR4, PgmName, blksize);
		return 1;
	}

/*	Eingabefile
*/
	if	(arg[0][0] == '-' && arg[0][1] == 0)
	{
		get = get_stdin;
	}
	else if	((idev = open(arg[0], O_RDONLY)) == EOF)
	{
		sprintf(str_buf, ERR5, PgmName, arg[0]);
		perror(str_buf);
		return 1;
	}
	else	get = get_file;

/*	Ausgabefile
*/
	put = varblock ? put_var : put_fix;
	str_buf[0] = 0;

	if	(narg == 2)
	{
		Output = (arg[1][0] == '-' && arg[1][1] == 0) ? NULL : arg[1];
		ofile = open_output(Output, compress, splitsize || recover);

		if	(ofile == NULL)
		{
			close(idev);
			return 1;
		}
	}
	else
	{
		ofile = NULL;
		put = put_null;
	}

	nblk = 0;
	nerr = 0;
	nshort = 0;
	nout = 0;
	maxblk = 0;

	for (blkread = 1; (n = get(buf, blksize)) != 0; blkread++)
	{
		if	(n == EOF)
		{
			sprintf(str_buf, "%s: Eingabefehler bei Block %d",
				PgmName, nblk + 1);
			perror(NULL);
			nerr++;

			if	(!recover)
				break;

			close_output(ofile, compress);
			ofile = open_output(Output, compress, 1);

			if	(!ofile)	break;

			continue;
		}

		if	(varblock && test_bsize(buf, n))
		{
			break;
		}

		if	(splitsize && splitsize < nout + n)
		{
			close_output(ofile, compress);

			if	(str_buf[0])
				fprintf(stderr, "%s: ", str_buf);

			fprintf(stderr, MSG3, nout);
			ofile = open_output(Output, compress, 1);
			nout = 0;

			if	(!ofile)	break;
		}

		if	(maxblk < n)
		{
			nshort += nblk;
			maxblk = n;
			nblk = 1;
		}
		else if	(n < maxblk)
		{
			nshort++;
		}
		else	nblk++;

		if	((n = put(ofile, buf, n)) == EOF)
		{
			sprintf(str_buf, "%s: Schreibfehler", PgmName);
			perror(str_buf);
			break;
		}
		else	nout += n;

		if	(blklim && blkread >= blklim)
			break;
	}

/*	Dateien schließen
*/
	if	(idev != EOF)		close(idev);

	close_output(ofile, compress);

/*	Statistik ausgeben
*/
	if	(str_buf[0])
		fprintf(stderr, "%s: ", str_buf);

	fprintf(stderr, MSG3, nout);
	fprintf(stderr, MSG1, maxblk);
	fprintf(stderr, MSG2, nblk, nshort);

	if	(nerr)
		fprintf(stderr, MSG4, nerr);

	return 0;
}
