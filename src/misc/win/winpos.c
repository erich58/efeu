/*	Fensterpositionierung
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/efwin.h>
#include <EFEU/efutil.h>

WinSize_t WinSize_buf = { 0, 0, 0, 0, 0 };

WinSize_t *CheckSize (WinSize_t *wp)
{
	if	(wp)	return wp;

	WinSize_buf.height = LINES;
	WinSize_buf.width = COLS;
	WinSize_buf.flags = W_TOP | W_LEFT;
	WinSize_buf.y = 0;
	WinSize_buf.x = 0;
	return &WinSize_buf;
}

WinSize_t *Frame (WinSize_t *wp)
{
	wp = CheckSize(wp);
	wp->flags |= W_FRAME;
	return wp;
}

WinSize_t *WindowSize(int height, int width, int flags, int y, int x)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = flags;
	WinSize_buf.y = y;
	WinSize_buf.x = x;
	return &WinSize_buf;
}

WinSize_t *CenterPos(int height, int width)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = W_CENTER;
	WinSize_buf.y = 0.5 * LINES;
	WinSize_buf.x = 0.5 * COLS;
	return &WinSize_buf;
}

WinSize_t *Rand48Pos(int height, int width)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = W_CENTER;
	WinSize_buf.y = 0.5 * height + drand48() * (LINES - height) + 0.5;
	WinSize_buf.x = 0.5 * width + drand48() * (COLS - width) + 0.5;
	return &WinSize_buf;
}

WinSize_t *CurrentPos (WINDOW *win, int height, int width, int flags)
{
	WinSize_buf.height = height;
	WinSize_buf.width = width;
	WinSize_buf.flags = flags;
	WinSize_buf.y = win->_begy + win->_cury;
	WinSize_buf.x = win->_begx + win->_curx;
	return &WinSize_buf;
}
