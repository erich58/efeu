/*	Hintergrundmuster
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
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
