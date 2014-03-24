/*
:*:	administration of temporary Files
:de:	Administration von temporären Dateien

$Copyright (C) 2004 Erich Frühstück
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
#include <EFEU/vecbuf.h>
#include <EFEU/patcmp.h>
#include <EFEU/procenv.h>
#include <EFEU/Debug.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int access (const char *path, int amode);

#define	ERR1	"newtemp: no directory available.\n"
#define	ERR2	"newtemp: no unused pathname found.\n"
#define	ERR3	"deltemp: unknown path %s.\n"

#define	PFX	"tmp"

#if	defined(W_OK) && defined(X_OK)
#define	ACC_MODE	(W_OK | X_OK)
#else
#define	ACC_MODE	03
#endif

#define	TMP_FLAGS	(O_RDWR | O_CREAT | O_EXCL)
#define	TMP_MODE	(S_IRUSR | S_IWUSR)
#define	DIR_MODE	S_IRWXU
#define	MAX_TRY		64

#define	TABSIZE	125

typedef struct {
	int (*open) (char *name);
	int (*clean) (char *name);
	char **tab;
	size_t size;
	size_t dim;
} TMPTAB;

static int file_open (char *name)
{
	int fd = open(name, TMP_FLAGS, TMP_MODE);

	if	(fd >= 0)
	{
		close(fd);
		return 1;
	}

	return 0;
}

static int file_clean (char *name)
{
	return (unlink(name) == 0);
}

static int dir_open (char *name)
{
	return (mkdir(name, DIR_MODE) == 0);
}

static int dir_clean (char *name)
{
	char *p = mstrpaste(" ", "rm -rf", name);
	system(p);
	memfree(p);
	return 1;
}

static TMPTAB tmptab = { file_open, file_clean, NULL, 0, 0 };
static TMPTAB dirtab = { dir_open, dir_clean, NULL, 0, 0 };

static void tab_add (TMPTAB *tab, char *path)
{
	if	(tab->size <= tab->dim)
	{
		tab->size += tab->size ? tab->size : TABSIZE;
		tab->tab = lrealloc(tab->tab, tab->size * sizeof tab->tab[0]);
	}

	tab->tab[tab->dim++] = path;
}

void tempstat (void)
{
	size_t n;

	for (n = 0; n < dirtab.dim; n++)
		fprintf(stderr, "%d: dir %s\n", (int) n, dirtab.tab[n]);

	for (n = 0; n < tmptab.dim; n++)
		fprintf(stderr, "%d: file %s\n", (int) n, tmptab.tab[n]);
}

static void tab_clean (void *par)
{
	TMPTAB *tab = par;
	size_t n;

	for (n = 0; n < tab->dim; n++)
	{
		fprintf(stderr, "remove %s\n", tab->tab[n]);
		tab->clean(tab->tab[n]);
		memfree(tab->tab[n]);
	}

	tab->dim = 0;
}

static void setup_temp (void)
{
	static int cleanup_registered = 0;

	if	(cleanup_registered)
		return;

	proc_clean(tab_clean, &dirtab);
	proc_clean(tab_clean, &tmptab);
	cleanup_registered = 1;
}

static int check_dir (const char *path)
{
	struct stat buf;

	if	(stat(path, &buf) != 0)
		return 0;

	if	(!S_ISDIR(buf.st_mode))
		return 0;

	if	(access(path, ACC_MODE) != 0)
		return 0;

	return 1;
}

static void putvalue (StrBuf *buf, int val, const char *digit, size_t mod)
{
	if	(val >= mod)
		putvalue(buf, val / mod, digit, mod);

	sb_putc(digit[val % mod], buf);
}

static char *dig_pid = "0123456789";
static char *dig_num = "abcdefghijklmnopqrstuvwxyz";
static STRBUF(tempbuf, 0);

static char *maketemp (TMPTAB *tab, const char *dir, const char *pfx)
{
	static unsigned num = 0;
	unsigned long pid;
	StrBuf *buf;
	char *p;
	int pos;
	int i;
	
	setup_temp();

	p = getenv("TMPDIR");

	if	(p && check_dir(p))
		dir = p;
	else if	(dir && check_dir(dir))
		;
	else if	((p = "/var/tmp") && check_dir(p))
		dir = p;
	else if	((p = "/tmp") && check_dir(p))
		dir = p;
	else
	{
		fprintf(stderr, ERR1);
		exit(EXIT_FAILURE);
	}

	pid = getpid();
	buf = &tempbuf;
	sb_clean(buf);
	sb_puts(dir, buf);
	sb_putc('/', buf);
	sb_puts(pfx ? pfx : PFX, buf);
	putvalue(buf, pid, dig_pid, strlen(dig_pid));
	pos = sb_getpos(buf);

	for (i = 0; i < MAX_TRY; i++)
	{
		putvalue(buf, num, dig_num, strlen(dig_num));
		sb_putc(0, buf);
		num++;

		if	(tab->open((char *) buf->data))
		{
			p = sb_memcpy(buf);
			tab_add(tab, p);
			return p;
		}

		perror((char *) buf->data);
		sb_setpos(buf, pos);
	}

	fprintf(stderr, ERR2);
	exit(EXIT_FAILURE);
}

char *newtemp (const char *dir, const char *pfx)
{
	return maketemp (&tmptab, dir, pfx);
}

char *newtempdir (const char *dir, const char *pfx)
{
	return maketemp (&dirtab, dir, pfx);
}

static int tab_del (TMPTAB *tab, char *path)
{
	int n;

	if	(!path)
		return 1;

	setup_temp();

	for (n = 0; n < tab->dim; n++)
	{
		if	(mstrcmp(path, tab->tab[n]) == 0)
		{
			char *p;
			int rval;
			
			tab->dim--;
			p = tab->tab[n];
			tab->tab[n] = tab->tab[tab->dim];
			tab->tab[tab->dim] = NULL;

			rval = tab->clean(p);

			if	(!rval)
				perror(p);

			memfree(p);
			return rval;
		}
	}

	fprintf(stderr, ERR3, path);
	return 0;
}

int deltemp (char *name)
{
	return tab_del (&tmptab, name);
}

int deltempdir (char *name)
{
	return tab_del (&dirtab, name);
}
