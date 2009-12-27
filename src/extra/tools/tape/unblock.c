/*
:*:	remove block size of a variable blocked file
:de:	Blocklängenfeld eines variabel geblockten Datenfiles auflösen

$Copyright (C) 1999, 2002 Erich Frühstück
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

#define	E_BLOCK	FDEF("err.beof", "eof in block size field.")
#define	E_XCHAR	FDEF("err.binv", "invalid block size field.")
#define	E_READ	FDEF("err.read", "unexcpected end of file.")
#define	E_WRITE	FDEF("err.write", "output error.")

#define	S_NBLK	FDEF("msg.nblk", "%d blocks read (maxblksize = %d).\n")
#define	S_NOUT	FDEF("msg.nout", "%d chars written.\n")

#define _XOPEN_SOURCE 1

#include <stdio.h>

#if	EFEUCFG
#include <EFEU/Resource.h>
#include <EFEU/ftools.h>
#endif

#define	USAGE	"usage: %s in [out]\n\n"

char *PgmName = NULL;

/*	get block size
*/

static int get_size (FILE *file)
{
	unsigned char buf[4];
	int n;

	n = fread(buf, 1, 4, file);

	if	(n == 0)	return 0;

	if	(n != 4)
	{
		fprintf(stderr, "%s: %s\n", PgmName, E_BLOCK);
		return 0;
	}

	n = (buf[0] << 8) + buf[1] - 4;

	if	(n <= 0 || buf[2] != 0 || buf[3] != 0)
	{
		fprintf(stderr, "%s: %s\n", PgmName, E_XCHAR);
		return 0;
	}

	return n;
}

/*	copy data
*/

static void copy_block (FILE *in, FILE *out, int n)
{
	while (n-- > 0)
	{
		register int c;

		if	((c = getc(in)) == EOF)
		{
			fprintf(stderr, "%s: %s\n", PgmName, E_READ);
			exit(1);
		}
	
		if	(out && putc(c, out) == EOF)
		{
			fprintf(stderr, "%s: %s\n", PgmName, E_WRITE);
			exit(1);
		}
	}
}

static FILE *f_open (const char *name, const char *mode)
{
#if	EFEUCFG
	return fileopen(name, *mode == 'r' ? "rz" : "wz");
#else
	if	(name && strcmp(name, "-") != 0)
	{
		FILE *file = fopen(name, mode);

		if	(!file)
		{
			perror(name);
			exit(1);
		}

		return file;
	}

	return *mode == 'r' ? stdin : stdout;
#endif
}

static void f_close (FILE *file)
{
#if	EFEUCFG
	fileclose(file);
#else
	if	(file && file != stdin && file != stdout)
		fclose(file);
#endif
}

int main (int narg, char **arg)
{
	FILE *ifile;
	FILE *ofile;
	unsigned maxblk;
	unsigned nblk;
	unsigned nout;
	int n;

	PgmName = arg[0];

#if	EFEUCFG
	SetVersion("$Id: unblock.c,v 1.3 2002-04-03 09:14:13 ef Exp $");
	ParseCommand(&narg, arg);
#else
	if	(narg < 2 || narg > 3)
	{
		fprintf(stderr, USAGE, PgmName);
		return 1;
	}
#endif

	ifile = f_open(arg[1], "rb");
	ofile = (narg > 2) ? f_open(arg[2], "wb") : NULL;

	nblk = 0;
	nout = 0;
	maxblk = 0;

	while ((n = get_size(ifile)) > 0)
	{
		if	(maxblk < n)
			maxblk = n;

		copy_block(ifile, ofile, n);
		nout += n;
		nblk++;
	}

	fprintf(stderr, S_NBLK, nblk, maxblk + 4);

	if	(ofile)
		fprintf(stderr, S_NOUT, nout);

	f_close(ifile);
	f_close(ofile);
	return 0;
}
