/*	Selektion
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <efwin.h>

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
	while ((key = wgetch(win)) != '\n');

	DelWindow(win);
	return list[n];
}
