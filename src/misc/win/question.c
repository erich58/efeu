/*	Bestätigung
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/efwin.h>
#include <EFEU/efutil.h>

#define	VMARGIN	1
#define	HMARGIN	2

#define	ctrl(x)	((x) & 0x1f)

int WinQuestion (WinSize_t *ws, char *label, char *s1, char *s2)
{
	WINDOW *win;
	int key;
	int n, n1, n2;
	int cols;
	int flag;
	int stat;

	if	(label == NULL)	label = "Antwort";
	if	(s1 == NULL)	s1 = "ja";
	if	(s2 == NULL)	s2 = "nein";

	n = strlen(label);
	n1 = strlen(s1);
	n2 = strlen(s2);
	cols = max(n, n1 + n2 + 1) + 2 * HMARGIN;

	if	(ws == NULL)
	{
		ws = CenterPos(3 + 2 * VMARGIN, cols);
		ws->flags |= W_FRAME;
	}

	ws->width = max(ws->width, cols);
	ws->height = max(ws->height, 3);
	win = NewWindow(ws);
	keypad(win, TRUE);
	scrollok(win, FALSE);
	leaveok(win, TRUE);
	mvwaddstr(win, VMARGIN, (cols - n) / 2, label);
	ShowWindow(win);
	flag = 1;
	stat = 1;

	while (stat)
	{
		wattrset(win, flag == 1 ? A_STANDOUT : 0);
		mvwaddstr(win, VMARGIN + 2, HMARGIN, s1);
		wattrset(win, flag == 2 ? A_STANDOUT : 0);
		mvwaddstr(win, VMARGIN + 2, cols - n2 - HMARGIN, s2);
		wrefresh(win);

		switch (key = wgetch(win))
		{
		case ctrl('G'):	
		case KEY_ESC:	flag = 0; stat = 0; break;
		case KEY_LEFT:	flag = 1; break;
		case KEY_RIGHT:	flag = 2; break;
		case ' ':
		case '\t':	flag = (flag == 1 ? 2 : 1); break;
		case '\r':
		case '\n':	stat = 0; break;
		default:	beep(); break;
		}
	}

	DelWindow(win);
	return flag;
}
