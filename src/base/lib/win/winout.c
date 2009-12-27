/*
Filter für Fensterausgabe

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
#include <EFEU/ioctrl.h>

static int win_put (int c, WINDOW *win);
static int win_ctrl (WINDOW *win, int req, va_list list);
static int win_cctrl (WINDOW *win, int req, va_list list);


io_t *io_winopen(WinSize_t *ws)
{
	io_t *io;

	io = io_alloc();
	io->put = (io_put_t) win_put;
	io->ctrl = (io_ctrl_t) win_cctrl;
	io->data = NewWindow(ws);
	scrollok(io->data, TRUE);
	wclear(io->data);
	wrefresh(io->data);
	return io;
}

io_t *io_winout(WINDOW *win)
{
	io_t *io;

	io = io_alloc();
	io->put = (io_put_t) win_put;
	io->ctrl = (io_ctrl_t) win_ctrl;
	io->data = win;
	scrollok(win, TRUE);
	wclear(win);
	wrefresh(win);
	return io;
}


/*	Zeichen ausgeben
*/

static int win_put(int c, WINDOW *win)
{
	waddch(win, c);
	wrefresh(win);
	return 1;
}


/*	Kontrollfunktion
*/

static int win_ctrl(WINDOW *win, int req, va_list list)
{
	switch (req)
	{
	case IO_COLUMNS:	return win->_maxx;
	case IO_LINES:		return win->_maxy;
	case IO_BOLD_FONT:	wattrset(win, A_STANDOUT); return 0;
	case IO_ITALIC_FONT:	wattrset(win, A_UNDERLINE); return 0;
	case IO_ROMAN_FONT:	wattrset(win, 0); return 0;
	default:		return EOF;
	}
}

static int win_cctrl(WINDOW *win, int req, va_list list)
{
	if	(req == IO_CLOSE)
	{
		DelWindow(win);
		return 0;
	}
	else	return win_ctrl(win, req, list);
}
