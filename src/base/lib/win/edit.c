/*
Editierfenster

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

#define	ctrl(x)	((x) & 0x1f)

static SB_DECL(sb_win, 0);

char *WinEdit (WinSize *ws, char *defval)
{
	WINDOW *win;
	int i;
	int key;

	if	(ws == NULL)		ws = WindowSize(1, COLS, W_BOT, 0, 0);

	ws->height = max(ws->height, 1);
	ws->width = max(ws->width, 10);
	win = NewWindow(ws);
	keypad(win, TRUE);
	scrollok(win, TRUE);
	leaveok(win, FALSE);
	ShowWindow(win);
	sb_trunc(&sb_win);
	sb_puts(defval, &sb_win);
	sb_win.pos = 0;
	key = '\f';

	for (;;)
	{
		switch (key)
		{
		case '\n':
		case '\r':

			DelWindow(win);
			sb_win.pos = sb_size(&sb_win);
			return sb_strcpy(&sb_win);

		case ctrl('G'):
		case KEY_ESC:

			DelWindow(win);
			return NULL;

		case ctrl('K'):
		
			sb_win.nfree = sb_win.size - sb_win.pos;

		case '\f':

			werase(win);
			wmove(win, 0, 0);

			for (i = 0; i < sb_size(&sb_win); i++)
				waddch(win, sb_win.data[i]);

			wmove(win, 0, sb_win.pos);
			break;

		case KEY_HOME:
		case ctrl('A'):

			sb_win.pos = 0;
			wmove(win, 0, sb_win.pos);
			break;

		case KEY_END:
		case ctrl('E'):
			
			sb_win.pos = sb_size(&sb_win);
			wmove(win, 0, sb_win.pos);
			break;

		case ctrl('B'):
		case KEY_LEFT:

			if	(sb_win.pos > 0)
			{
				sb_win.pos--;
				wmove(win, 0, sb_win.pos);
			} 

			break;

		case ctrl('F'):
		case KEY_RIGHT:

			if	(sb_win.pos < sb_size(&sb_win))
			{
				sb_win.pos++;
				wmove(win, 0, sb_win.pos);
			} 

			break;

		case KEY_BACKSPACE:
		case '\b':

			if	(sb_win.pos > 0)
			{
				sb_win.pos--;
				sb_delete(&sb_win, 1);
				wmove(win, 0, sb_win.pos);
				wdelch(win);
			}

			break;

		case ctrl('D'):

			if	(sb_win.pos < sb_size(&sb_win))
			{
				sb_delete(&sb_win, 1);
				wdelch(win);
			}

			break;

		default:

			if	(key <= 255)
			{
				sb_insert(&sb_win, key, 1);
				winsch(win, key);
				wmove(win, 0, sb_win.pos);
			}
			else	beep();

			break;
		}

		wrefresh(win);
		key = wgetch(win);
	}
}
