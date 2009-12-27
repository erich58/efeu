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

static int label_size(char *label)
{
	int i, n;

	if	(label == NULL)	return 0;

	for (i = n = 0; label[i] != 0; i++)
	{
		switch (label[i])
		{
		case '<':
		case '>':	break;
		default:	n++;
		}
	}

	return n;
}

static void show_label(WINDOW *win, int y, int x, char *label, int flag)
{
	int i;

	if	(label == NULL)	return;

	wattrset(win, flag ? A_STANDOUT : 0);
	wmove(win, y, x);

	for (i = 0; label[i] != 0; i++)
	{
		switch (label[i])
		{
		case '<':	wattron(win, A_UNDERLINE); break;
		case '>':	wattroff(win, A_UNDERLINE); break;
		default:	waddch(win, label[i]); break;
		}
	}

}

static int valpos(WinMenu_t *win, int dim, int pos, int step)
{
	for (;;)
	{
		while (pos < 0)
			pos += dim;

		pos = pos % dim;

		if	(win[pos].label && win[pos].data)	return pos;

		pos += step;
	}
}

static int newpos(WinMenu_t *menu, int dim, int old, int height, int width, int y, int x)
{
	int pos;

	y = (old / width + y + height) % height;
	x = (old % width + x + width) % width;
	pos = y * width + x;

	if	(pos >= dim)		return valpos(menu, dim, dim - 1, -1);
	else if	(y < 0 || x < 0)	return valpos(menu, dim, pos, -1);
	else				return valpos(menu, dim, pos, 1);
}

void *WinMenu (WinSize_t *ws, WinMenu_t *menu, int dim, int cols)
{
	WINDOW *win;
	int key;
	int width;
	int height;
	int i, n;
	int last;
	int flag;

	if	(menu == NULL || dim == 0)	return NULL;

	for (i = width = flag = 0; i < dim; i++)
	{
		if	(menu[i].label && menu[i].data)	flag = 1;

		n = label_size(menu[i].label);
		width = max(width, n);
	}

	if	(!flag)	return NULL;

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
	n = last = valpos(menu, dim, 0, 1);
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
				show_label(win, i / cols, width * (i % cols), menu[i].label, i == n);

			break;

		case KEY_UP:	n = newpos(menu, dim, n, height, cols, -1, 0); break;
		case KEY_DOWN:	n = newpos(menu, dim, n, height, cols, 1, 0); break;
		case KEY_LEFT:	n = newpos(menu, dim, n, height, cols, 0, -1); break;
		case KEY_RIGHT:	n = newpos(menu, dim, n, height, cols, 0, 1); break;
		case ' ':
		case '\t':	n = valpos(menu, dim, n + 1, 1); break;
		case KEY_BACKSPACE:
		case '\b':	n = valpos(menu, dim, n + dim - 1, -1); break;
		case 0:		continue;
		default:
		
			for (i = 0; i < dim; i++)
			{
				if	(menu[i].key == key && menu[i].label && menu[i].data)
				{
					n = i;
					break;
				}
			}
			
			break;
		}

		if	(n != last)
		{
			show_label(win, last / cols, width * (last % cols), menu[last].label, 0);
			show_label(win, n / cols, width * (n % cols), menu[n].label, 1);
			last = n;
		}

		wrefresh(win);
	}
	while ((key = wgetch(win)) != '\n' && key != '\r');

	DelWindow(win);
	return menu[n].data;
}
