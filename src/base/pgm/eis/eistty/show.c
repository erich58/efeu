/*
EFEU-Informationssystem

$Copyright (C) 1998 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <EFEU/patcmp.h>
#include "eistty.h"

int Lines = 24;

#if	USE_EFWIN
WINDOW *info_win = NULL;
InfoNode *info_node = NULL;
#else
static char *smso = NULL;
static char *rmso = NULL;
static int solen = 0;
#endif

static char *search_key = NULL;

#define	is_key(str)	(search_key && patcmp(search_key, str, NULL))

typedef struct INFOPAR_S INFOPAR;

struct INFOPAR_S {
	INFOPAR *prev;	/* Vorgängerknoten */
	VecBuf buf;	/* Buffer */
	int active;	/* Aktiver Index */
	int curline;	/* Aktuelle Zeile */
	int maxline;	/* Maximale Zeilenzahl */
};

static INFOPAR *new_par(INFOPAR *prev)
{
	INFOPAR *x = memalloc(sizeof(INFOPAR));
	memset(x, 0, sizeof(INFOPAR));
	vb_init(&x->buf, 32, sizeof(InfoPart));
	x->prev = prev;
	return x;
}

static INFOPAR *IPAR = NULL;

static InfoPart def_part;

static void del_part(InfoPart *part, size_t dim)
{
	for (; dim > 0; dim--, part++)
		memfree(part->str);
}

static void del_par(INFOPAR *par)
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

static void flush_buf(void)
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

static void linebreak(void)
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

void MakePart(InfoNode *info)
{
	IPAR = new_par(IPAR);
	memset(&def_part, 0, sizeof(InfoPart));
#if	USE_EFWIN
	maxcols = info_win ? info_win->_maxx - 1 : 70;
#endif
	def_part.x = 0;
	def_part.y = 0;
	def_part.att = 0;
	def_part.str = NULL;
	def_part.info = NULL;
	part_pos = 0;

	if	(!info)
		info = GetInfo(NULL, NULL);

#if	USE_EFWIN
	werase(info_win);
	wmove(info_win, LASTLINE, 0);
	wattrset(info_win, 0);
	waddstr(info_win, "eis: ");
	wprintw(info_win, "Knoten laden");
	wrefresh(info_win);
#endif
	SetupInfo(info);

#if	USE_EFWIN
	def_part.att = A_UNDERLINE;
#else
	def_part.att = 0;
#endif
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
#if	USE_EFWIN
			def_part.att = A_UNDERLINE;
#endif
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

void ShowPart (void)
{
	InfoPart *part;
	int i, y; 
#if	USE_EFWIN
	int att;
#else
	int cur_y;
	int cur_x;
#endif

#if	USE_EFWIN
	if	(!info_win)	return;

	werase(info_win);
#else
	y = cur_y = cur_x = 0;

	for (i = 0; i < 25; i++)
		io_putc('\n', iostd);
#endif

	for (i = 0, part = IPAR->buf.data; i < IPAR->buf.used; i++, part++)
	{
		y = part->y - IPAR->curline;

		if	(y < 0)		continue;
		if	(y >= LASTLINE)	break;

#if	USE_EFWIN
		if	(i == IPAR->active)	att = A_STANDOUT;
		else if	(is_key(part->str))	att = A_BOLD;
		else				att = part->att;

		wattrset(info_win, att);
		wmove(info_win, y, part->x);
		waddstr(info_win, part->str);
#else
		while (cur_y < y)
		{
			io_putc('\n', iostd);
			cur_y++;
			cur_x = 0;
		}

		cur_x += io_nputc(' ', iostd, part->x - cur_x);

		if	(i == IPAR->active)
		{
			io_puts(smso, iostd);
			cur_x += io_puts(part->str, iostd);
			io_puts(rmso, iostd);
			cur_x += solen;
		}
		else	cur_x += io_puts(part->str, iostd);
#endif
	}

#if	USE_EFWIN
	wmove(info_win, LASTLINE, 0);
	wattrset(info_win, 0);
	waddstr(info_win, "eis: ");

	y = IPAR->curline + LASTLINE;

	if	(y > IPAR->maxline)	y = IPAR->maxline;

	wprintw(info_win, "Zeile %d von %d", y, IPAR->maxline);
	wrefresh(info_win);
#else
	io_putc('\n', iostd);
	io_printf(iostd, "Zeile %d von %d: ", y, IPAR->maxline);
#endif
}

void MovePos (int offset)
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

void MoveRef (int offset)
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

void GotoHome (void)
{
	IPAR->curline = 0;
}

void GotoEnd (void)
{
	IPAR->curline = IPAR->maxline - LASTLINE;

	if	(IPAR->curline < 0)	IPAR->curline = 0;
}

void PrevInfo (void)
{
	if	(IPAR->prev)
	{
		INFOPAR *save = IPAR;
		IPAR = IPAR->prev;
		del_par(save);
	}
}

void NextInfo (void)
{
	InfoPart *part = IPAR->buf.data;

	if	(part[IPAR->active].info)
		MakePart(part[IPAR->active].info);
}

int SearchKey (char *key)
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

void ShowInit(void)
{
#if	USE_EFWIN
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
#else
	smso = "<";
	rmso = ">";
	solen = 2;
#endif
}

void ShowExit(void)
{
#if	USE_EFWIN
	DelWindow(info_win);
	EndWin();
#else
	;
#endif
}
