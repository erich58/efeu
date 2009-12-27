/*
Fenster mit Spuranzeige

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

extern io_t *wio;

static char *list[] = {
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"
};

static WinMenu_t menu[] = {
	{ 0, "<Sonderzeichen>", NULL },
	{ '*', "<*> Stern", "*" },
	{ '+', "<+> Plus", "+" },
	{ '.', "<.> Punkt", "." },
	{ 0, "<Buchstaben>", NULL },
	{ 'a', "Buchstabe <A>", "A" },
	{ 'b', "Buchstabe <B>", "B" },
	{ 'c', "Buchstabe <C>", "C" },
	{ 'x', NULL, NULL },
	{ 'd', "Buchstabe <D>", "D" },
	{ 'e', "Buchstabe <E>", "E" },
	{ 'f', "Buchstabe <F>", "F" },
	{ 'g', "Buchstabe <G>", "G" },
	{ 'h', "Buchstabe <H>", "H" },
	{ 'i', "Buchstabe <I>", "I" },
};

void WinTrace (int rows, int cols)
{
	WINDOW *win;
	char *p;
	int key;
	int c;
	int x, y;
	int flags;

	if	(rows == 0)	rows = 0.5 * LINES;
	if	(cols == 0)	cols = 0.5 * COLS;

	win = FrameWindow(Rand48Pos(rows, cols));

	io_puts("Neues ", wio);
	io_ctrl(wio, IO_BOLD_FONT);
	io_puts("Fenster", wio);
	io_ctrl(wio, IO_ROMAN_FONT);
	io_puts(" ", wio);
	io_ctrl(wio, IO_ITALIC_FONT);
	io_puts("bei", wio);
	io_ctrl(wio, IO_ROMAN_FONT);
	io_printf(wio, " %d %d\n", win->_begy , win->_begx);

	keypad(win, TRUE);
	scrollok(win, FALSE);
	x = y = 0;
	c = '*';
	flags = 0;
	mvwaddch(win, y, x, c);
	wmove(win, y, x);
	wrefresh(win);

	while ((key = wgetch(win)) != KEY_ESC)
	{
		switch (key)
		{
		case '!':

			EndWin();
			system(Shell);
			InitWin();
			continue;

		case 'z':
		
			WinConfirm(CurrentPos(win, 0, 0, flags), "Bitte um eine\nBestätigung");
			break;

		case 'e':
		
			p = WinEdit(NULL, "Eingabe");
			io_printf(wio, "%#s\n", p);
			break;

		case 'm':

			p = WinMenu(CurrentPos(win, 0, 0, flags), menu, tabsize(menu), 2);
			c = p ? *p : c;
			break;

		case 's':

			p = WinSelect(CurrentPos(win, 0, 0, flags), list, tabsize(list), 3);
			c = p ? *p : '*';
			break;

		case 't':	flags &= ~W_BOT; flags |= W_TOP; break;
		case 'b':	flags &= ~W_TOP; flags |= W_BOT; break;
		case 'l':	flags &= ~W_RIGHT; flags |= W_LEFT; break;
		case 'r':	flags &= ~W_LEFT; flags |= W_RIGHT; break;
		case 'c':	flags &= ~0xf; break;
		case ' ':	flags &= ~W_FRAME; break;
		case '#':	flags |= W_FRAME; break;

		case 'x':
		
			io_printf(wio, "%4b: %d %d\n", flags, y, x);

			switch (WinQuestion(CurrentPos(win, 0, 0, flags), "Zeichen", ".", "+"))
			{
			case  1:	c = '.'; break;
			case  2:	c = '+'; break;
			default:	break;
			}

			break;

		case 'w':
		
			WinTrace(0, 0);
			continue;

		case '\n':
		case '\r':

			if	(WinQuestion(CurrentPos(win, 0, 0, flags),
				"Ende", "ja", "nein") == 1)
			{
				DelWindow(win);
				return;
			}
			else	break;

		case '\f':	werase(win); break;
		case KEY_LEFT:	x = max(0, x - 1); break;
		case KEY_RIGHT:	x = min(cols - 1, x + 1); break;
		case KEY_UP:	y = max(0, y - 1); break;
		case KEY_DOWN:	y = min(rows - 1, y + 1); break;
		default:	continue;
		}

		mvwaddch(win, y, x, c);
		wmove(win, y, x);
		wrefresh(win);
	}

	DelWindow(win);
}
