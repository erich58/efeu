/*
Curses Schnittstelle

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/pconfig.h>
#include <EFEU/efwin.h>

static int WinMode = 0;

extern void (*callproc_init) (void);
extern void (*callproc_exit) (void);

void (*callproc_init_base) (void);
void (*callproc_exit_base) (void);

static void callproc_init_win()
{
	EndWin();

	if	(callproc_init_base)
		callproc_init_base();
}

static void callproc_exit_win()
{
	if	(callproc_exit_base)
		callproc_exit_base();

	InitWin();
}

void EndWin(void)
{
	if	(WinMode == 1)
	{
		clear();
		move(0, 0);
		refresh();
		endwin();
	}

	WinMode = 2;
}

void InitWin(void)
{
	switch (WinMode)
	{
	case 0:	

		callproc_init_base = callproc_init;
		callproc_init = callproc_init_win;
		callproc_exit_base = callproc_exit;
		callproc_exit = callproc_exit_win;

		atexit(EndWin);
		initscr();
		raw();
		noecho();
		refresh();
		break;

	case 1:

		break;

	default:

		RefreshAll();
		doupdate();
		break;
	}

	WinMode = 1;
}


/*	Initialisieren
*/

void SetupWin(void)
{
	;
}
