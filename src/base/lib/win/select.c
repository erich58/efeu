/*
Selektion

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

static int newpos(int old, int height, int width, int y, int x)
{
	y = (old / width + y + height) % height;
	x = (old % width + x + width) % width;
	return y * width + x;
}

char *WinSelect (WinSize_t *ws, char **list, int dim, int cols)
{
	WINDOW *win;
	int key;
	int width;
	int height;
	int i, n;
	int last;

	if	(list == NULL || dim == 0)	return NULL;

	for (i = width = 0; i < dim; i++)
	{
		n = strlen(list[i]);
		width = max(width, n);
	}

	width += 1;

	if	(cols == 0)
		cols = (dim + LINES - 3) / (LINES - 2);

	height = (dim + cols - 1) / cols;

	ws->height = height;
	ws->width = cols * width;
	win = NewWindow(ws);
	keypad(win, TRUE);
	scrollok(win, FALSE);
	leaveok(win, TRUE);
	n = last = 0;
	ShowWindow(win);
	key = '\f';

	do
	{
		switch (key)
		{
		case KEY_ESC:

			DelWindow(win);
			return NULL;

		case '\f':

			werase(win);

			for (i = 0; i < dim; i++)
			{
				wattrset(win, (i == n) ? A_STANDOUT : 0);
				mvwaddstr(win, i / cols, width * (i % cols), list[i]);
			}

			break;

		case KEY_UP:	n = newpos(n, height, cols, -1, 0); break;
		case KEY_DOWN:	n = newpos(n, height, cols, 1, 0); break;
		case KEY_LEFT:	n = newpos(n, height, cols, 0, -1); break;
		case KEY_RIGHT:	n = newpos(n, height, cols, 0, 1); break;
		case ' ':
		case '\t':	n = (n + 1) % dim; break;
		case '\b':	n = (n + dim - 1) % dim; break;
		default:	continue;
		}

		if	(n >= dim)	n = dim - 1;

		if	(n != last)
		{
			wattrset(win, 0);
			mvwaddstr(win, last / cols, width * (last % cols), list[last]);
			wattrset(win, A_STANDOUT);
			mvwaddstr(win, n / cols, width * (n % cols), list[n]);
			last = n;
		}

		wrefresh(win);
	}
	while ((key = wgetch(win)) != '\n' && key != '\r');

	DelWindow(win);
	return list[n];
}
