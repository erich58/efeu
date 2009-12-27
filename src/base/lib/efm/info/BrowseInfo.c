/*
Beschreibungstext durchblättern

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/dl.h>
#include <ctype.h>

#define	ERR_MODE "[efm:imode]$!: info mode $1 not defined.\n"


typedef struct {
	char *name;
	void (*eval) (const char *name);
	char *desc;
} MDEF;

static void mdef_list (IO *out);

void (*InfoBrowser) (const char *name) = NULL;
void (*XInfoBrowser) (const char *name) = NULL;

static void mdef_auto (const char *name)
{
	int use_x11 = (getenv("DISPLAY") != NULL);

#if	HAS_DLFCN
	if	(use_x11)
		loadlib("ETK", "SetupETK");
	else	loadlib("efwin", "SetupWin");
#endif

	if	(use_x11 && XInfoBrowser)
	{
		XInfoBrowser(name);
	}
	else if	(InfoBrowser)
	{
		InfoBrowser(name);
	}
	else	StdInfoBrowser(name);
}

static void mdef_print (const char *name)
{
	PrintInfo(iostd, GetInfo(NULL, name));
}

static void mdef_dump (const char *name)
{
	DumpInfo(iostd, name, 0);
}

static void mdef_xdump (const char *name)
{
	DumpInfo(iostd, name, 1);
}

static void mdef_std (const char *name)
{
	StdInfoBrowser(name);
}

#if	HAS_DLFCN

static void mdef_extern (const char *lib, const char *browser, const char *name)
{
	void *handle;
	void (*eval) (const char *name);
	
	if	(!(handle = so_open(lib)))
		return;

	eval = dlsym(handle, browser);

	if	(eval)
	{
		eval(name);
	}
	else	io_xprintf(ioerr, "dlsym: %s\n", dlerror());

	so_close(handle);
}

static void mdef_win (const char *name)
{
	mdef_extern("efwin", "WinInfo", name);
}

static void mdef_tk (const char *name)
{
	mdef_extern("ETK", "ETKInfo", name);
}

#endif

static MDEF mdef[] = {
	{ NULL, mdef_auto,
		NULL
	}, { "auto", mdef_auto,
		":*:automatic browser selection (default)\n"
		":de:Automatische Browser-Auswahl (default)\n"
	}, { "print", mdef_print,
		":*:print info entry\n"
		":de:Info-Eintrag darstellen\n"
	}, { "dump", mdef_dump,
		":*:dump info entry\n"
		":de:INFO-Einträge ausgeben\n"
	}, { "xdump", mdef_xdump,
		":*:complete dump of info entries\n"
		":de:INFO-Einträge vollständig ausgeben\n"
	}, { "std", mdef_std,
		":*:use standard browser\n"
		":de:Verwende den Standardbrowser\n"
#if	HAS_DLFCN
	}, { "win", mdef_win,
		":*:use curses based browser\n"
		":de:Verwende den Curses-basierenden Browser\n"
	}, { "tk", mdef_tk,
		":*:use Tk based browser\n"
		":de:Verwende den Tk-basierenden Browser\n"
#endif
	},
};

static void mdef_list (IO *out)
{
	int i;

	for (i = 0; i < tabsize(mdef); i++)
	{
		IO *io;

		if	(!mdef[i].desc)	continue;

		io_puts(mdef[i].name, out);
		io_putc('\n', out);

		io = io_lmark(io_refer(out), "\t", NULL, 0);
		io_langputs(mdef[i].desc, io);
		io_close(io);
	}
}

static void eval_info (const char *mode, const char *name)
{
	int n;

	for (n = 0; n < tabsize(mdef); n++)
	{
		if	(mstrcmp(mode, mdef[n].name) == 0)
		{
			mdef[n].eval(name);
			return;
		}
	}

	log_note(NULL, ERR_MODE, "s", mode);
}

void BrowseInfo (const char *name)
{
	int n;
	char *mode;

	if	(!name)
	{
		mdef_auto(NULL);
		return;
	}

	if	(name[0] == '?')
	{
		mdef_list(iostd);
		return;
	}

	mode = NULL;

	for (n = 0; name[n]; n++)
	{
		if	(name[n] == '/')	break;

		if	(name[n] == ':')
		{
			mode = mstrncpy(name, n);
			name += n + 1;
			break;
		}
	}

	eval_info(mode, name);
	memfree(mode);
}
