/*
Bestätigung

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/efwin.h>
#include <EFEU/efutil.h>

void WinConfirm (WinSize *ws, char *label)
{
	WINDOW *win;
	int height;
	int i, n;

	if	(ws == NULL)	ws = CenterPos(0, 0);
	if	(label == NULL)	label = "Bestätigen";

	height = 1;

	for (i = 0, n = 1; label[i] != 0; i++)
	{
		if	(label[i] == '\n')
		{
			height++;
			ws->width = max(ws->width, n);
			n = 1;
		}
		else	n++;
	}

	ws->height = max(ws->height, height);
	ws->width = max(ws->width, n);
	win = NewWindow(ws);
	keypad(win, TRUE);
	scrollok(win, TRUE);
	leaveok(win, TRUE);
	ShowWindow(win);
	mvwaddstr(win, 0, 0, label);
	wrefresh(win);
	wgetch(win);
	DelWindow(win);
}
