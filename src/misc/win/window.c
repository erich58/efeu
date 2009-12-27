/*	Fensteroberfläche
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efwin.h>
#include <EFEU/vecbuf.h>

/*	Kombinierte Fenster
*/

typedef struct {
	WINDOW *child;
	WINDOW *parent;
} WinList_t;

static int wlist_cmp(const WinList_t *a, const WinList_t *b)
{
	if	(a->child < b->child)	return -1;
	else if	(a->child > b->child)	return 1;
	else				return 0;
}

static XTAB(winlist, 0, (comp_t) wlist_cmp);


void OverlayWindow(WINDOW *child, WINDOW *parent)
{
	WinList_t *wlist;

	wlist = ALLOC(1, WinList_t);
	wlist->child = child; 
	wlist->parent = parent; 
	xsearch(&winlist, wlist, XS_REPLACE);
}


/*	Sichtbarkeit von Fenstern
*/

static VECBUF(vis_tab, 16, sizeof(WINDOW *));

void HideWindow (WINDOW *win)
{
	WINDOW **ptr;
	int i;

	ptr = vis_tab.data;

	for (i = 0; i < vis_tab.used; i++)
		if (win == ptr[i])	break;

	for (; i < vis_tab.used; i++)
		ptr[i] = ptr[i + 1];
	
	vis_tab.used--;
}

void ShowWindow (WINDOW *win)
{
	WINDOW **ptr;
	int i;

	if	(win == NULL)	return;

	ptr = vis_tab.data;

	for (i = 0; i < vis_tab.used; i++)
		if (win == ptr[i]) return;

	ptr = vb_next(&vis_tab);
	*ptr = win;
	touchwin(win);
	wrefresh(win);
}


/*	Fenster generieren/freigeben
*/

static WINDOW *xnewwin(int maxy, int maxx, int begy, int begx)
{
	WINDOW *win;

	win = newwin(maxy, maxx, begy, begx);

	if	(win == NULL)
	{
		EndWin();
		reg_fmt(1, "%d", maxy);
		reg_fmt(2, "%d", maxx);
		reg_fmt(3, "%d", begy);
		reg_fmt(4, "%d", begx);
		liberror(MSG_EFWIN, 1);
	}

	return win;
}

static WINDOW *ws_newwin(WinSize_t *ws)
{
	int maxy, maxx, begy, begx;

	if	(ws->height > LINES)	maxy = LINES;
	else if	(ws->height > 1)	maxy = ws->height;
	else				maxy = 1;

	if	(ws->width > COLS)	maxx = COLS;
	else if	(ws->width > 1)		maxx = ws->width;
	else				maxx = 1;

	if	(ws->flags & W_TOP)	begy = ws->y;
	else if	(ws->flags & W_BOT)	begy = ws->y - maxy + 1;
	else				begy = ws->y - 0.5 * maxy + 0.5;

	if	(ws->flags & W_LEFT)	begx = ws->x;
	else if	(ws->flags & W_RIGHT)	begx = ws->x - maxx + 1;
	else				begx = ws->x - 0.5 * maxx + 0.5;

	if	(begy < 0)		begy = 0;
	if	(begy + maxy > LINES)	begy = LINES - maxy;
	if	(begx < 0)		begx = 0;
	if	(begx + maxx > COLS)	begx = COLS - maxx;

	return xnewwin(maxy, maxx, begy, begx);
}

WINDOW *NewWindow(WinSize_t *ws)
{
	WINDOW *win;

	ws = CheckSize(ws);

	if	(ws->flags & W_FRAME)
		return FrameWindow(ws);

	win = ws_newwin(ws);
	ShowWindow(win);
	return win;
}

WINDOW *FrameWindow(WinSize_t *ws)
{
	WINDOW *frame;
	WINDOW *win;

	ws = CheckSize(ws);
	ws->width += 2;
	ws->height += 2;

	if	(ws->width < 3)		ws->width = 3;
	if	(ws->height < 3)	ws->height = 3;

	frame = ws_newwin(ws);
	DrawBox(frame);
	ShowWindow(frame);
	win = xnewwin(frame->_maxy - 2, frame->_maxx - 2, frame->_begy + 1, frame->_begx + 1);
	ShowWindow(win);
	OverlayWindow(win, frame);
	return win;
}

void DelWindow(WINDOW *win)
{
	WinList_t key, *list;

	if	(win == NULL)	return;

	HideWindow(win);
	key.child = win;

	list = xsearch(&winlist, &key, XS_DELETE);

	if	(list)
	{
		DelWindow(list->parent);
		memfree(list);
	}

	delwin(win);
	RefreshAll();
}


void RefreshAll()
{
	WINDOW **ptr;
	int i;

	ptr = vis_tab.data;

	for (i = 0; i < vis_tab.used; i++)
	{
		touchwin(ptr[i]);
		wnoutrefresh(ptr[i]);
	}

	doupdate();
}
