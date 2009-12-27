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
#include <EFEU/Debug.h>
#include <EFEU/LangDef.h>
#include <ctype.h>

int access (const char *path, int amode);

#ifndef	R_OK
#define	R_OK	04
#endif

#if	HAS_GETPWNAM
#include <pwd.h>
#endif


#define	USE_TERRITORY	0
#define	FS_BSIZE	64

static int do_search (StrBuf *sb, const char *path,
	const char *sub, const char *name, int flag);
static int try_lang (StrBuf *sb, const char *path, const char *sub,
	const char *name);
static int sub_search (StrBuf *sb, const char *name);
static int need_search (const char *name);
static char *add_tilde (StrBuf *sb, const char *path);


/*	Filenamen an Pfad anhängen und auf Lesbarkeit testen
*/

static int sub_search (StrBuf *sb, const char *name)
{
	if	(sb->pos != 0 && sb->data[sb->pos - 1] != '/')
		sb_putc('/', sb);

	sb_puts(name, sb);
	sb_putc(0, sb);
	dbg_message("fsearch", DBG_TRACE, "access(\"$1\")\n", NULL, "s", sb->data);

	if	(access((char *) sb->data, R_OK) == 0)
		return 1;

	return 0;
}


static int need_search (const char *name)
{
	if	(name[0] == '/')	return 0;
	if	(name[0] != '.')	return 1;
	if	(name[1] == '/')	return 0;
	if	(name[1] != '.')	return 1;
	if	(name[2] == '/')	return 0;

	return 1;
}

static char *add_tilde (StrBuf *sb, const char *path)
{
	path++;

#if	HAS_GETPWNAM
	if	(isalpha(*path))
	{
		int save;
		struct passwd *pw;

		save = sb_getpos(sb);
		sb_putc(*path, sb);

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
			sb_puts(pw->pw_dir, sb);

		return (char *) path;
	}
#endif
	sb_puts(getenv("HOME"), sb);
	return (char *) path;
}

static int try_lang (StrBuf *sb, const char *path, const char *sub,
	const char *name)
{
	if	(LangDef.language != NULL)
	{
		int save = sb_getpos(sb);

		sb_puts(LangDef.language, sb);

#if	USE_TERRITORY
		if	(LangDef.territory != NULL)
		{
			int tsave = sb_getpos(sb);

			sb_putc('_', sb);
			sb_puts(LangDef.territory, sb);

			if	(do_search(sb, path, sub, name, 1))
				return 1;

			sb_setpos(sb, tsave);
			sb_sync(sb);
		}
#endif
		if	(do_search(sb, path, sub, name, 1))
			return 1;

		sb_setpos(sb, save);
		sb_sync(sb);
	}

	return 0;
}

static int do_search (StrBuf *sb, const char *path,
	const char *sub, const char *name, int flag)
{
	while (*path != 0)
	{
		switch (*path)
		{
		case '~':
			path = add_tilde(sb, path);
			continue;
		case '%':
			path++;

			switch (*path)
			{
			case 'S':
				sb_puts(sub, sb);
				break;
			case 'T':
				sb_puts(getenv("TERM"), sb);
				break;
			case 'H':
				sb_puts(getenv("HOME"), sb);
				break;
			case 'L':
				if	(try_lang(sb, path + 1, sub, name))
					return 1;

				break;
			default:
				break;
			}

			break;
		case ':':
			if	(sub_search(sb, name))
				return 1;
			if	(flag)
				return 0;

			sb_clean(sb);
			break;

		case '/':
			if	(sb->pos == 0 || sb->data[sb->pos - 1] != '/')
				sb_putc('/', sb);

			break;
		default:

			sb_putc(*path, sb);
			break;
		}

		path++;
	}

	return sub_search(sb, name);
}


/*	Suche nach dem File
*/

char *fsearch (const char *path, const char *sub,
	const char *name, const char *type)
{
	char *tname = NULL;

	if	(name == NULL)
		return NULL;
	
	if	(type)
	{
		char *p = strrchr(name, '.');

		if	(p == NULL || strcmp(p + 1, type) != 0)
		{
			tname = mstrpaste(".", name, type);
			name = tname;
		}
	}

	if	(path && need_search(name))
	{
		StrBuf *sb = sb_acquire();
		int flag = do_search(sb, path, sub, name, 0);

		memfree(tname);

		if	(flag)	return sb_cpyrelease(sb);

		sb_release(sb);
		return NULL;
	}
	else if	(access(name, R_OK) == 0)
	{
		return tname ? tname : mstrcpy(name);
	}

	memfree(tname);
	return NULL;
}
