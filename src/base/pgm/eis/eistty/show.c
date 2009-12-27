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

#include "eistty.h"

WINDOW *info_win = NULL;
InfoNode_t *info_node = NULL;
static char *search_key = NULL;

#define	is_key(str)	(search_key && patcmp(search_key, str, NULL))

typedef struct INFOPAR_S INFOPAR;

struct INFOPAR_S {
	INFOPAR *prev;	/* Vorgängerknoten */
	vecbuf_t buf;	/* Buffer */
	int active;	/* Aktiver Index */
	int curline;	/* Aktuelle Zeile */
	int maxline;	/* Maximale Zeilenzahl */
};

static INFOPAR *new_par(INFOPAR *prev)
{
	INFOPAR *x = memalloc(sizeof(INFOPAR));
	memset(x, 0, sizeof(INFOPAR));
	vb_init(&x->buf, 32, sizeof(InfoPart_t));
	x->prev = prev;
	return x;
}

static INFOPAR *IPAR = NULL;

static InfoPart_t def_part;

static void del_part(InfoPart_t *part, size_t dim)
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
	InfoPart_t *part;
	
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

static io_t io_part = STD_IODATA (NULL, part_put, NULL, NULL);

void MakePart(InfoNode_t *info)
{
	IPAR = new_par(IPAR);
	memset(&def_part, 0, sizeof(InfoPart_t));

	maxcols = info_win ? info_win->_maxx - 1 : 70;
	def_part.x = 0;
	def_part.y = 0;
	def_part.att = 0;
	def_part.str = NULL;
	def_part.info = NULL;
	part_pos = 0;

	if	(!info)
		info = GetInfo(NULL, NULL);

	if	(info->load)
		info->load(info);

	def_part.att = A_UNDERLINE;
	def_part.info = NULL;
	io_psub(&io_part, info->label);
	linebreak();
	def_part.att = 0;
	def_part.info = NULL;

	if	(def_part.y)	def_part.y++;

	IPAR->maxline = def_part.y;

	if	(!info->func)
	{
		reg_cpy(1, info->name);
		reg_cpy(2, info->label);
		io_psub(&io_part, info->par);
	}
	else	info->func(&io_part, info);

	linebreak();

	if	(info->list)
	{
		int i = info->list->used;
		InfoNode_t **ip = info->list->data;

		if	(def_part.y > IPAR->maxline)
			def_part.y++;

		while (i > 0)
		{
			flush_buf();
			def_part.att = A_UNDERLINE;
			def_part.info = *ip;
			io_puts((*ip)->name, &io_part);
			flush_buf();
			def_part.att = 0;
			def_part.info = NULL;

			if	((*ip)->label)
			{
				io_putc('\t', &io_part);
				io_psub(&io_part, (*ip)->label);
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
	InfoPart_t *part;
	int i, y; 
	int att;

	if	(!info_win)	return;

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
}

void MovePos (int offset)
{
	IPAR->curline += offset;

	if	(IPAR->curline + LASTLINE > IPAR->maxline)
		IPAR->curline = IPAR->maxline - LASTLINE;

	if	(IPAR->curline < 0)
		IPAR->curline = 0;
}

void MoveRef (int offset)
{
	InfoPart_t *part;
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
	InfoPart_t *part = IPAR->buf.data;

	if	(part[IPAR->active].info)
		MakePart(part[IPAR->active].info);
}

int SearchKey (char *key)
{
	int i, n;
	InfoPart_t *part;
	
	memfree(search_key);
	search_key = key ? msprintf("%s*", key) : NULL;
	memfree(key);

	if	(search_key == NULL)	return 0;

	part = IPAR->buf.data;

	for (i = n = 0; i < IPAR->buf.used; i++)
	{
		if	(is_key(part[i].str))
		{
			if	(n++)	continue;

			if	(IPAR->curline > part[i].y)
				IPAR->curline = part[i].y;

			if	(IPAR->curline + LASTLINE <= i)
				IPAR->curline = part[i].y - LASTLINE + 1;

			if	(IPAR->curline < 0)
				IPAR->curline = 0;
		}
	}

	return n;
}
