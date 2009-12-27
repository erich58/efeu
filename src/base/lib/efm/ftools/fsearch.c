/*
Suche nach einer Datei

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

#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/extension.h>
#include <ctype.h>

int access (const char *path, int amode);

#ifndef	R_OK
#define	R_OK	04
#endif

#if	HAS_GETPWNAM
#include <pwd.h>
#endif


/*	Statischer Buffer zum zwischenspeichern
*/

#define	FS_BSIZE	64


/*	Test auf relativen Pfad
*/

static int checkpath(const char *path)
{
	if	(path == NULL)		return 0;
	else if	(path[0] == 0)		return 0;
	else if	(path[0] == '/')	return 1;
	else if	(path[0] != '.')	return 0;
	else if	(strchr(path, '/'))	return 1;
	else				return 0;
}


/*	Filenamen an Pfad anhängen und auf Lesbarkeit testen
*/

static char *sub_search(strbuf_t *sb, const char *name)
{
	if	(sb->pos != 0 && sb->data[sb->pos - 1] != '/')
		sb_putc('/', sb);

	sb_puts(name, sb);
	sb_putc(0, sb);

	if	(access((char *) sb->data, R_OK) == 0)
	{
		return sb2str(sb);
	}

	return NULL;
}


/*	Suche nach dem File
*/

char *fsearch(const char *path, const char *subpath,
	const char *name, const char *type)
{
	fname_t *fn;
	char *fname;
	const char *cp;
	strbuf_t *sb;
	char *p;

/*	Kein Filename oder kein Pfad und Filetype
*/
	if	(name == NULL)
	{
		return NULL;
	}
	else if	(path == NULL && type == NULL)
	{
		if	(access(name, R_OK) == 0)
		{
			return mstrcpy(name);
		}
		else	return NULL;
	}

/*	Filename zerlegen und auf vollständige Definition prüfen.
*/
	fn = strtofn(name);

	if	(fn->type == NULL)
	{
		fn->type = (char *) type;
	}

	fname = fntostr(fn);

	if	(path == NULL || checkpath(fn->path))
	{
		memfree(fn);

		if	(access(fname, R_OK) == 0)	return fname;

		memfree(fname);
		return NULL;
	}

/*	Suche nach Datei
*/
	sb = new_strbuf(FS_BSIZE);

	while (*path != 0)
	{
		switch (*path)
		{
		case '~':

#if	HAS_GETPWNAM
			if	(isalpha(path[1]))
			{
				int save;
				struct passwd *pw;

				save = sb_getpos(sb);

				while (*(++path) != 0)
				{
					if	(*path == '/' || *path == ':')
					{
						break;
					}
					else	sb_putc(*path, sb);
				}

				sb_putc(0, sb);
				pw = getpwnam((char *) sb->data + save);
				sb_setpos(sb, save);
				sb_sync(sb);

				if	(pw)
				{
					sb_puts(pw->pw_dir, sb);
				}

				continue;
			}
#endif
			sb_puts(getenv("HOME"), sb);
			break;

		case '%':

			switch (*(++path))
			{
#if	0
			case 'P':	cp = ProgName; break;
#endif
			case 'S':	cp = subpath; break;
			case 'T':	cp = getenv("TERM"); break;
			case 'H':	cp = getenv("HOME"); break;
			case 'L':	cp = getenv("LANG"); break;
			default:	path++; continue;
			}

			sb_puts(cp, sb);
			break;
		case ':':
			if	((p = sub_search(sb, fname)) != NULL)
				return p;

			sb_clear(sb);
			break;

		default:

			sb_putc(*path, sb);
			break;
		}

		path++;
	}

	return sub_search(sb, fname);
}

char *xfsearch(const char *path, const char *subpath,
	const char *name, const char *type)
{
	fname_t *fn;
	char *p, *fname;

	if	(name == NULL)	return NULL;

	fn = strtofn(name);
	fn->type = (char *) type;
	p = fntostr(fn);
	memfree(fn);
	fname = fsearch(path, subpath, p, NULL);
	memfree(p);
	return fname;
}
