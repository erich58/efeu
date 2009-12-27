/*	Hilfsprogramme
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
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
