/*
:*:	command information browser
:de:	Informationseinträge darstellen

$Copyright (C) 2006 Erich Frühstück
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
#include <EFEU/procenv.h>
#include <EFEU/Info.h>

typedef struct {
	int y;		/* Zeilenposition */
	int x;		/* Spaltenposition */
	int att;	/* Attribut */
	char *str;	/* Zeichenkette */
	InfoNode *info;	/* Verzweigungsknoten */
} InfoPart;

typedef struct INFOPAR_S INFOPAR;

struct INFOPAR_S {
	INFOPAR *prev;	/* Vorgängerknoten */
	VecBuf buf;	/* Buffer */
	int active;	/* Aktiver Index */
	int curline;	/* Aktuelle Zeile */
	int maxline;	/* Maximale Zeilenzahl */
};

static INFOPAR *new_par (INFOPAR *prev)
{
	INFOPAR *x = memalloc(sizeof(INFOPAR));
	memset(x, 0, sizeof(INFOPAR));
	vb_init(&x->buf, 32, sizeof(InfoPart));
	x->prev = prev;
	return x;
}

static int Lines = 0;
static WINDOW *info_win = NULL;
static char *search_key = NULL;

#define	LASTLINE 	(info_win->_maxy - 1)
#define	is_key(str)	(search_key && patcmp(search_key, str, NULL))

static char *GetString (char *prompt)
{
	char *p;
	WinSize *ws;

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

#if	0
static void WinMessage (char *fmt, ...)
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
#endif

#if	0
static void ShowInit(void);
static void ShowExit(void);

static int SearchKey (char *key);
static void PrevInfo (void);
static void NextInfo (void);

static void HelpWindow ();
static char *GetString (char *prompt);
static void WinMessage (char *fmt, ...);
#endif

static INFOPAR *IPAR = NULL;

static InfoPart def_part;

static void del_part (InfoPart *part, size_t dim)
{
	for (; dim > 0; dim--, part++)
		memfree(part->str);
}

static void del_par (INFOPAR *par)
{
	if	(par)
	{
		del_part(par->buf.data, par->buf.used);
		vb_free(&par->buf);
		memfree(par);
	}
}

#define	PARTBUFSIZE	64

static char part_buf[PARTBUFSIZE];
static int part_pos = 0;
static int maxcols = 70;

static void flush_buf (void)
{
	InfoPart *part;
	
	if	(part_pos == 0)	return;

	part = vb_next(&IPAR->buf);

	if	(def_part.x + part_pos > maxcols)
	{
		def_part.x = 0;
		def_part.y++;
	}

	*part = def_part;
	def_part.info = NULL;
	part->str = mstrncpy(part_buf, part_pos);
	def_part.x += part_pos;
	part_pos = 0;
}

static void linebreak (void)
{
	flush_buf();

	if	(def_part.x)
	{
		def_part.y++;
		def_part.x = 0;
	}
}

static int part_put (int c, void *ptr)
{
	switch (c)
	{
	case '\f':
	case '\n':
		flush_buf();
		def_part.x = 0;
		def_part.y++;
		return 1;
	case ' ':
		flush_buf();
		def_part.x++;
		return 1;
	case '\t':
		flush_buf();
		def_part.x = 8 * (1 + def_part.x / 8);
		return 1;
	default:
		break;
	}

	if	(part_pos >= PARTBUFSIZE)
		flush_buf();

	part_buf[part_pos++] = c;
	return 1;
}

static IO io_part = STD_IODATA (NULL, part_put, NULL, NULL);

static void MakePart (InfoNode *info)
{
	IPAR = new_par(IPAR);
	memset(&def_part, 0, sizeof(InfoPart));
	maxcols = info_win ? info_win->_maxx - 1 : 70;
	def_part.x = 0;
	def_part.y = 0;
	def_part.att = 0;
	def_part.str = NULL;
	def_part.info = NULL;
	part_pos = 0;

	if	(!info)
		info = GetInfo(NULL, NULL);

	werase(info_win);
	wmove(info_win, LASTLINE, 0);
	wattrset(info_win, 0);
	waddstr(info_win, "eis: ");
	wprintw(info_win, "Knoten laden");
	wrefresh(info_win);
	SetupInfo(info);

	def_part.att = A_UNDERLINE;
	def_part.info = NULL;
	io_psubarg(&io_part, info->label, "ns", info->name);
	linebreak();
	def_part.att = 0;
	def_part.info = NULL;

	if	(def_part.y)	def_part.y++;

	IPAR->maxline = def_part.y;

	if	(!info->func)
	{
		io_psubarg(&io_part, info->par, "nss", info->name, info->label);
	}
	else	info->func(&io_part, info);

	linebreak();

	if	(info->list)
	{
		int i = info->list->used;
		InfoNode **ip = info->list->data;

		if	(def_part.y > IPAR->maxline)
			def_part.y++;

		while (i > 0)
		{
			flush_buf();
			SetupInfo(*ip);
			def_part.att = A_UNDERLINE;
			def_part.info = *ip;
			io_puts((*ip)->name, &io_part);
			flush_buf();
			def_part.att = 0;
			def_part.info = NULL;

			if	((*ip)->label)
			{
				io_putc('\t', &io_part);
				io_psubarg(&io_part, (*ip)->label, NULL);
			}

			linebreak();
			i--;
			ip++;
		}
	}

	IPAR->active = 0;
	IPAR->curline = 0;
	IPAR->maxline = def_part.y;
}

static void ShowPart (void)
{
	InfoPart *part;
	int i, y; 
	int att;

	if	(!info_win)	return;

	werase(info_win);

	for (i = 0, part = IPAR->buf.data; i < IPAR->buf.used; i++, part++)
	{
		y = part->y - IPAR->curline;

		if	(y < 0)		continue;
		if	(y >= LASTLINE)	break;

		if	(i == IPAR->active)	att = A_STANDOUT;
		else if	(is_key(part->str))	att = A_BOLD;
		else				att = part->att;

		wattrset(info_win, att);
		wmove(info_win, y, part->x);
		waddstr(info_win, part->str);
	}

	wmove(info_win, LASTLINE, 0);
	wattrset(info_win, 0);
	waddstr(info_win, "eis: ");

	y = IPAR->curline + LASTLINE;

	if	(y > IPAR->maxline)	y = IPAR->maxline;

	wprintw(info_win, "Zeile %d von %d", y, IPAR->maxline);
	wrefresh(info_win);
}

static void MovePos (int offset)
{
	InfoPart *part;
	int i;

	part = IPAR->buf.data;
	IPAR->curline += offset;

	if	(IPAR->curline + LASTLINE > IPAR->maxline)
		IPAR->curline = IPAR->maxline - LASTLINE;

	if	(IPAR->curline < 0)
		IPAR->curline = 0;

	if	(offset > 0)
	{
		for (i = IPAR->active; i < IPAR->buf.used; i++)
		{
			if	(part[i].info)
			{
				IPAR->active = i;

				if	(part[i].y >= IPAR->curline)
					break;
			}
		}
	}
	else if	(offset < 0)
	{
		int pos = IPAR->curline + LASTLINE - 1;

		for (i = IPAR->active; i >= 0; i--)
		{
			if	(part[i].info)
			{
				IPAR->active = i;

				if	(part[i].y <= pos)
					break;
			}
		}
	}
}

static void MoveRef (int offset)
{
	InfoPart *part;
	int i;

	part = IPAR->buf.data;

	if	(IPAR->active >= IPAR->buf.used)
	{
		IPAR->active = IPAR->buf.used;
		offset = -1;
	}
	else if	(IPAR->active < 0)	IPAR->active = 0;

	if	(offset > 0)
	{
		for (i = IPAR->active + 1; i < IPAR->buf.used; i++)
		{
			if	(part[i].info)
			{
				IPAR->active = i;
				break;
			}
		}
	}
	else if	(offset < 0)
	{
		for (i = IPAR->active - 1, IPAR->active = 0; i >= 0; i--)
		{
			if	(part[i].info)
			{
				IPAR->active = i;
				break;
			}
		}
	}

	i = part[IPAR->active].y;

	if	(IPAR->curline > i)	
		IPAR->curline = i;

	if	(IPAR->curline + LASTLINE <= i)
		IPAR->curline = i - LASTLINE + 1;
}

static void GotoHome (void)
{
	IPAR->curline = 0;
}

static void GotoEnd (void)
{
	IPAR->curline = IPAR->maxline - LASTLINE;

	if	(IPAR->curline < 0)	IPAR->curline = 0;
}

static void PrevInfo (void)
{
	if	(IPAR->prev)
	{
		INFOPAR *save = IPAR;
		IPAR = IPAR->prev;
		del_par(save);
	}
}

static void NextInfo (void)
{
	InfoPart *part = IPAR->buf.data;

	if	(part[IPAR->active].info)
		MakePart(part[IPAR->active].info);
}

static int SearchKey (char *key)
{
	int i, n, ref;
	InfoPart *part;
	
	memfree(search_key);
	search_key = key ? msprintf("%s*", key) : NULL;
	memfree(key);

	if	(search_key == NULL)	return 0;

	part = IPAR->buf.data;
	ref = IPAR->active;

	for (i = n = 0; i < IPAR->buf.used; i++)
	{
		if	(part[i].info)
			ref = i;

		if	(is_key(part[i].str))
		{
			if	(n++)	continue;

			if	(IPAR->curline > part[i].y)
				IPAR->curline = part[i].y;

			IPAR->active = ref;

			if	(IPAR->curline + LASTLINE <= i)
				IPAR->curline = part[i].y - LASTLINE + 1;

			if	(IPAR->curline < 0)
				IPAR->curline = 0;
		}
	}

	return n;
}

static void ShowInit(void)
{
	InitWin();

	if	(Lines)
	{
		info_win = NewWindow(WindowSize(Lines, COLS, 0, 0, 0));
	}
	else	info_win = NewWindow(NULL);

	keypad(info_win, TRUE);
	scrollok(info_win, FALSE);
	leaveok(info_win, TRUE);
	ShowWindow(info_win);
}

static void ShowExit(void)
{
	DelWindow(info_win);
	EndWin();
}

#define	ctrl(c)	((c) & 0x1f)

#define	PSIZE	((info_win->_maxy - 2) / 2)

#define	GET()	wgetch(info_win)
#define	WINKEY(x)	case x:

#if	0
static InfoNode *LoadCommand(InfoNode *info, const char *def)
{
	char *name = strrchr(def, '/');
	char *cmd = msprintf("%s --info=dump:", def);
	IO *io = io_popen(cmd, "r");

	info = AddInfo(info, name ? name + 1 : def, NULL, NULL, NULL);
	IOLoadInfo(info, io);
	memfree(cmd);
	io_close(io);
	return info;
}
#endif

IO *io_ptrpreproc (IO *io, char **pptr, char **lptr);

static InfoNode *load(InfoNode *info, const char *def)
{
	IO *io;
	char *name;
	
	name = fsearch(InfoPath, NULL, def, "info");

	if	(name == NULL)
		name = fsearch(InfoPath, NULL, def, NULL);

	if	(name == NULL)
		dbg_error(NULL, "[eftools:6]", "s", def);

	io = io_fileopen(name, "rz");
	info = AddInfo(info, def, NULL, NULL, NULL);
	io = io_ptrpreproc(io, &InfoPath, NULL);
	IOLoadInfo(info, io);
	io_close(io);
	memfree(name);
	return info;
}

static int eval(void)
{
	char *p;
	int key;

	key = GET();

	switch (key)
	{
	case '\f':
		wclear(info_win);
		wrefresh(info_win);
		break;
	case 'q':
		return 0;
	case 'h':
	WINKEY(KEY_HOME)
		GotoHome(); break;
	case 'e':
	WINKEY(KEY_END)
		GotoEnd(); break;
	case 'f':
	case '\n':
	case '\r':
	WINKEY(KEY_RIGHT)
		NextInfo(); break;
	case 'b':
	WINKEY(KEY_LEFT)
		PrevInfo(); break;
	case 'd':
	WINKEY(KEY_DOWN)
		MoveRef(1); break;
	case 'u':
	WINKEY(KEY_UP)
		MoveRef(-1); break;
	case 'n':
	WINKEY(KEY_NPAGE)
		MovePos(PSIZE); break;
	case 'p':
	WINKEY(KEY_PPAGE)
		MovePos(-PSIZE); break;
	case ctrl('H'):
	WINKEY(KEY_BREAK)
	WINKEY(KEY_BACKSPACE)
	case 127:
	case 'k':
		MovePos(-1); break;
	case ' ':
	case 'j':
		MovePos(1); break;
	case '?':
		MakePart(load(NULL, "help"));
		break;
	case 't':
		MakePart(NULL);
		break;
	case ':':
		if	((p = GetString("node: ")) != NULL)
		{
			MakePart(GetInfo(NULL, p));
			memfree(p);
		}
		break;
#if	0
	case '<':
		if	((p = GetString("file: ")) != NULL)
		{
			WinMessage("file: %s", p);
			MakePart(load(0, p));
			memfree(p);
		}

		break;
	case '|':
		if	((p = GetString("command: ")) != NULL)
		{
			WinMessage("|%s --info=dump:", p);
			MakePart(LoadCommand(NULL, p));
			memfree(p);
		}

		break;
#endif
	case '/':
		SearchKey(GetString("search: "));
		break;
	default:
		beep();
		return 1;
	}

	ShowPart();

	return 1;
}

void WinInfo (const char *name)
{
	SetupWin();
	ShowInit();
	MakePart(GetInfo(NULL, name));
	ShowPart();

	while (eval());

	ShowExit();
}
