/*
Fensterpositionierung

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

WinSize WinSize_buf = { 0, 0, 0, 0, 0 };

WinSize *CheckSize (WinSize *wp)
{
	if	(wp)	return wp;

	WinSize_buf.height = LINES;
	WinSize_buf.width = COLS;
	WinSize_buf.flags = W_TOP | W_LEFT;
	WinSize_buf.y = 0;
	WinSize_buf.x = 0;
	return &WinSize_buf;
}

WinSize *Frame (WinSize *wp)
{
	wp = CheckSize(wp);
	wp->flags |= W_FRAME;
	return wp;
}

WinSize *WindowSize(int height, int width, int flags, int y, int x)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = flags;
	WinSize_buf.y = y;
	WinSize_buf.x = x;
	return &WinSize_buf;
}

WinSize *CenterPos(int height, int width)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = W_CENTER;
	WinSize_buf.y = 0.5 * LINES;
	WinSize_buf.x = 0.5 * COLS;
	return &WinSize_buf;
}

WinSize *Rand48Pos(int height, int width)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = W_CENTER;
	WinSize_buf.y = 0.5 * height + drand48() * (LINES - height) + 0.5;
	WinSize_buf.x = 0.5 * width + drand48() * (COLS - width) + 0.5;
	return &WinSize_buf;
}

WinSize *CurrentPos (WINDOW *win, int height, int width, int flags)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = flags;
	WinSize_buf.y = win->_begy + win->_cury;
	WinSize_buf.x = win->_begx + win->_curx;
	return &WinSize_buf;
}
