/*	Best�tigung
	(c) 1995 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

#include <EFEU/efwin.h>
#include <EFEU/efutil.h>

void WinConfirm (WinSize_t *ws, char *label)
{
	WINDOW *win;
	int height;
	int i, n;

	if	(ws == NULL)	ws = CenterPos(0, 0);
	if	(label == NULL)	label = "Best�tigen";

	height = 1;

	for (i = 0, n = 1; label[i] != 0; i++)
	{
		if	(label[i] == '\n')
		{
			height++;
			ws->width = max(ws->width, n);
			n = 1;
		}
		else	n++;
	}

	ws->height = max(ws->height, height);
	ws->width = max(ws->width, n);
	win = NewWindow(ws);
	keypad(win, TRUE);
	scrollok(win, TRUE);
	leaveok(win, TRUE);
	ShowWindow(win);
	mvwaddstr(win, 0, 0, label);
	wrefresh(win);
	wgetch(win);
	DelWindow(win);
}
