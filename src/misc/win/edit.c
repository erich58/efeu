/*	Bestätigung
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/efwin.h>
#include <EFEU/efutil.h>

#define	ctrl(x)	((x) & 0x1f)

char *WinEdit (WinSize_t *ws, char *defval)
{
	WINDOW *win;
	strbuf_t *sb;
	int i;
	int key;

	if	(ws == NULL)		ws = WindowSize(1, COLS, W_BOT, 0, 0);

	ws->height = max(ws->height, 1);
	ws->width = max(ws->width, 10);
	win = NewWindow(ws);
	keypad(win, TRUE);
	scrollok(win, TRUE);
	leaveok(win, FALSE);
	ShowWindow(win);
	sb = new_strbuf(0);
	sb_puts(defval, sb);
	sb->pos = 0;
	key = '\f';

	for (;;)
	{
		switch (key)
		{
		case '\n':
		case '\r':

			DelWindow(win);
			return sb2str(sb);

		case ctrl('G'):
		case KEY_ESC:

			DelWindow(win);
			del_strbuf(sb);
			return NULL;

		case ctrl('K'):
		
			sb->nfree = sb->size - sb->pos;

		case '\f':

			werase(win);
			wmove(win, 0, 0);

			for (i = 0; i < sb_size(sb); i++)
				waddch(win, sb->data[i]);

			wmove(win, 0, sb->pos);
			break;

		case KEY_HOME:
		case ctrl('A'):

			sb->pos = 0;
			wmove(win, 0, sb->pos);
			break;

		case KEY_END:
		case ctrl('E'):
			
			sb->pos = sb_size(sb);
			wmove(win, 0, sb->pos);
			break;

		case ctrl('B'):
		case KEY_LEFT:

			if	(sb->pos > 0)
			{
				sb->pos--;
				wmove(win, 0, sb->pos);
			} 

			break;

		case ctrl('F'):
		case KEY_RIGHT:

			if	(sb->pos < sb_size(sb))
			{
				sb->pos++;
				wmove(win, 0, sb->pos);
			} 

			break;

		case KEY_BACKSPACE:
		case '\b':

			if	(sb->pos > 0)
			{
				sb->pos--;
				sb_delete(sb);
				wmove(win, 0, sb->pos);
				wdelch(win);
			}

			break;

		case ctrl('D'):

			if	(sb->pos < sb_size(sb))
			{
				sb_delete(sb);
				wdelch(win);
			}

			break;

		default:

			if	(key <= 255)
			{
				sb_insert(key, sb);
				winsch(win, key);
				wmove(win, 0, sb->pos);
			}
			else	beep();

			break;
		}

		wrefresh(win);
		key = wgetch(win);
	}
}
