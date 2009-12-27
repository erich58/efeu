/*
Hilfsprogramme

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

#if	__linux__

void DrawBox(WINDOW *win)
{
	box(win, '|', '-');
}

#else

void DrawBox (WINDOW *win)
{
	int x, y;

	win->_y[0][0] = box_chars_1[0]  | A_ALTCHARSET;
	win->_y[0][win->_maxx - 1] = box_chars_1[2]  | A_ALTCHARSET;

	for (x = 1; x < win->_maxx - 1; x++)
	{
		win->_y[0][x] = box_chars_1[1]  | A_ALTCHARSET;
		win->_y[win->_maxy - 1][x] = box_chars_1[1]  | A_ALTCHARSET;
	}

	for (y = 1; y < win->_maxy - 1; y++)
	{
		win->_y[y][0] = box_chars_1[3]  | A_ALTCHARSET;
		win->_y[y][win->_maxx - 1] = box_chars_1[3]  | A_ALTCHARSET;
	}

	win->_y[win->_maxy - 1][0] = box_chars_1[5]  | A_ALTCHARSET;
	win->_y[win->_maxy - 1][win->_maxx - 1] = box_chars_1[4]  | A_ALTCHARSET;
	touchwin(win);
}
#endif
