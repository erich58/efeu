/*
EFEU-Informationssystem

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/Resource.h>
#include <EFEU/Debug.h>
#include "eistty.h"

#if	USE_EFWIN

#define	ctrl(c)	((c) & 0x1f)

#define	PSIZE	((info_win->_maxy - 2) / 2)

#define	GET()	wgetch(info_win)
#define	WINKEY(x)	case x:

#else

#define	PSIZE	((Lines - 2) / 2)
#define	GET()	io_getc(iostd)
#define	WINKEY(x)

#endif

static int eval(void);

static InfoNode *LoadCommand(InfoNode *info, const char *def)
{
	char *name = strrchr(def, '/');
	char *cmd = msprintf("%s --info=dump:", def);
	IO *io = io_popen(cmd, "r");

	info = AddInfo(info, name ? name + 1 : def, NULL, NULL, NULL);
	IOLoadInfo(info, io);
	memfree(cmd);
	io_close(io);
	return info;
}

static InfoNode *load(InfoNode *info, const char *def)
{
	IO *io;
	char *name;
	
	name = fsearch(InfoPath, NULL, def, "info");

	if	(name == NULL)
		name = fsearch(InfoPath, NULL, def, NULL);

	if	(name == NULL)
		dbg_error(NULL, "[eftools:6]", "s", def);

	io = io_fileopen(name, "rz");
	info = AddInfo(info, def, NULL, NULL, NULL);
	io = io_ptrpreproc(io, &InfoPath, NULL);
	IOLoadInfo(info, io);
	io_close(io);
	memfree(name);
	return info;
}

/*	Hauptprogramm
*/

int main(int narg, char **arg)
{
	int i;
	int flag;
	InfoNode *info;

	SetVersion("$Id: eis.c,v 1.13 2006-01-04 18:28:24 ef Exp $");
	SetupStd();
#if	USE_EFWIN
	SetupWin();
#endif
	ParseCommand(&narg, arg);
	SetInfoPath(GetResource("Path", NULL));

	info = GetInfo(NULL, NULL);
	info->list = NULL;
	info->par = NULL;
	info = NULL;

	flag = GetFlagResource("CommandInfo");

	for (i = 1; i < narg; i++)
		info = flag ? LoadCommand(NULL, arg[i]) : load(NULL, arg[i]);

	if	(narg == 1)	info = load(NULL, GetResource("Top", NULL));
	if	(narg > 2)	info = NULL;
	
	Lines = GetIntResource("Lines", Lines);
	ShowInit();

	MakePart(GetInfo(info, GetResource("Node", NULL)));
	ShowPart();

	while (eval());

	ShowExit();
	exit(EXIT_SUCCESS);
	return 0;
}

static int eval(void)
{
	char *p;
	int key;

	key = GET();

	switch (key)
	{
	case '\f':
#if	USE_EFWIN
		wclear(info_win);
		wrefresh(info_win);
#endif
		break;
#if	!USE_EFWIN
	case EOF:
		io_putc('\n', iostd);
		return 0;
#endif
	case 'q':
		return 0;
	case 'h':
	WINKEY(KEY_HOME)
		GotoHome(); break;
	case 'e':
	WINKEY(KEY_END)
		GotoEnd(); break;
	case 'f':
	case '\n':
	case '\r':
	WINKEY(KEY_RIGHT)
		NextInfo(); break;
	case 'b':
	WINKEY(KEY_LEFT)
		PrevInfo(); break;
	case 'd':
	WINKEY(KEY_DOWN)
		MoveRef(1); break;
	case 'u':
	WINKEY(KEY_UP)
		MoveRef(-1); break;
	case 'n':
	WINKEY(KEY_NPAGE)
		MovePos(PSIZE); break;
	case 'p':
	WINKEY(KEY_PPAGE)
		MovePos(-PSIZE); break;
#if	USE_EFWIN
	case ctrl('H'):
#else
	case '\b':
#endif
	WINKEY(KEY_BREAK)
	WINKEY(KEY_BACKSPACE)
	case 127:
	case 'k':
		MovePos(-1); break;
	case ' ':
	case 'j':
		MovePos(1); break;
	case '?':
		MakePart(load(NULL, "help"));
		break;
	case 't':
		MakePart(NULL);
		break;
	case ':':
		if	((p = GetString("node: ")) != NULL)
		{
			MakePart(GetInfo(NULL, p));
			memfree(p);
		}
		break;
	case '<':
		if	((p = GetString("file: ")) != NULL)
		{
			WinMessage("file: %s", p);
			MakePart(load(0, p));
			memfree(p);
		}

		break;
	case '|':
		if	((p = GetString("command: ")) != NULL)
		{
			WinMessage("|%s --info=dump:", p);
			MakePart(LoadCommand(NULL, p));
			memfree(p);
		}

		break;
	case '/':
		SearchKey(GetString("search: "));
		break;
	default:
#if	USE_EFWIN
		beep();
		return 1;
#else
		io_putc('\a', iostd);
		break;
#endif
	}

	ShowPart();

#if	!USE_EFWIN
	while (key != EOF && key != '\n')
		key = io_getc(iostd);
#endif
	return 1;
}
