/*
Hintergrundmuster

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

WINDOW *StippleBackground(void)
{
	WINDOW *win;

	win = NewWindow(NULL);
	touchwin(win);

	wrefresh(win);
	return win;
}

#else

WINDOW *StippleBackground(void)
{
	WINDOW *win;
	int x, y;

	win = NewWindow(NULL);

	for (y = 0; y < win->_maxy; y++)
	{
		for (x = 0; x < win->_maxx; x++)
		{
			win->_y[y][x] = ((x + y) % 2) ? '-' : '|';
		}
	}

	touchwin(win);
	wrefresh(win);
	return win;
}
#endif
