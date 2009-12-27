/*
String Editieren

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

#include "eistty.h"

char *GetString (char *prompt)
{
	char *p;
	WinSize *ws;

	wmove(info_win, LASTLINE, 0);
	wclrtobot(info_win);

	if	(prompt)
		waddstr(info_win, prompt);

	wrefresh(info_win);
	ws = CurrentPos(info_win, 1, COLS - info_win->_curx - 1, W_LEFT);
	p = WinEdit(ws, NULL);
	wmove(info_win, LASTLINE, 0);
	wclrtobot(info_win);
	wrefresh(info_win);
	return p;
}

void WinMessage (char *fmt, ...)
{
	va_list list;
	char *p;

	wmove(info_win, LASTLINE, 0);
	wclrtobot(info_win);
	va_start(list, fmt);
	p = mvsprintf(fmt, list);
	waddstr(info_win, p);
	memfree(p);
	va_end(list);
	wrefresh(info_win);
}
