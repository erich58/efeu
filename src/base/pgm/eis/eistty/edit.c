/*	String Editieren
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include "eistty.h"

char *GetString (char *prompt)
{
	char *p;
	WinSize_t *ws;

	wmove(info_win, LASTLINE, 0);
	wclrtobot(info_win);

	if	(prompt)
		waddstr(info_win, prompt);

	wrefresh(info_win);
	ws = CurrentPos(info_win, 1, COLS - info_win->_curx - 1, W_LEFT);
	p = WinEdit(ws, NULL);
	wmove(info_win, LASTLINE, 0);
	wclrtobot(info_win);
	wrefresh(info_win);
	return p;
}

void WinMessage (char *fmt, ...)
{
	va_list list;
	char *p;

	wmove(info_win, LASTLINE, 0);
	wclrtobot(info_win);
	va_start(list, fmt);
	p = mvsprintf(fmt, list);
	waddstr(info_win, p);
	memfree(p);
	va_end(list);
	wrefresh(info_win);
}
