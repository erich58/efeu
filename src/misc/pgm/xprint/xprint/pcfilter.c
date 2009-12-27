/*	Ausgabefilter
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/object.h>
#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>
#include "printpar.h"

static int base_put (int c, io_t *io);
static int base_ctrl (io_t *io, int req, va_list list);
static int line_put (int c, io_t *io);
static int line_ctrl (io_t *io, int req, va_list list);
static int page_put (int c, io_t *io);
static int page_ctrl (io_t *io, int req, va_list list);

int pc_pos = 0;
int pc_line = 0;
fontdef_t *cur_font = NULL;
fontdef_t *set_font = NULL;
int ulmode = 0;
int fillmode = 0;
int need_init = 0;

static void change_font(io_t *io, fontdef_t *font)
{
	if	(font == set_font)	return;

	if	(set_font)	io_psub(io, set_font->exit);

	set_font = font;

	if	(set_font)	io_psub(io, set_font->init);
}


io_t *pc_filter (const char *type)
{
	io_t *io, *save;

	io = pc_setup(type);
	need_init = 1;

	save = io;
	io = io_alloc();
	io->put = (io_put_t) base_put;
	io->ctrl = (io_ctrl_t) base_ctrl;
	io->data = save;

	save = io;
	io = io_alloc();
	io->put = (io_put_t) line_put;
	io->ctrl = (io_ctrl_t) line_ctrl;
	io->data = save;

	save = io;
	io = io_alloc();
	io->put = (io_put_t) page_put;
	io->ctrl = (io_ctrl_t) page_ctrl;
	io->data = save;

	return io;
}


/*	Zeichen ausgeben
*/

static int base_put(int c, io_t *io)
{
	if	(PageNumber >= FirstPage && PageNumber <= LastPage)
	{
		if	(need_init)
		{
			io_psub(io, pc_init);
			need_init = 0;
		}

		io_putc(c, io);
	}

	return 1;
}

static int page_put(int c, io_t *io)
{
	fontdef_t *save;

	if	(c == '\f')
	{
		if	(pc_pos)
			io_putc('\n', io);

		save = cur_font;
		cur_font = NULL;
		io_nputc('\n', io, PageHeight - pc_line - 1);
		cur_font = getfont(12);
		headline(io, LeftFoot, CenterFoot, RightFoot);
		cur_font = NULL;
		io_psub(io->data, pc_newpage);
		cur_font = save;
		pc_line = 0;
		PageNumber++;
		return 0;
	}

	if	(pc_line >= PageHeight - FootHeight)
	{
		page_put('\f', io);
	}

	if	(pc_line == 0)
	{
		save = cur_font;
		cur_font = NULL;
		io_nputc('\n', io->data, TopMargin);
		io_putc(0, io);
		cur_font = getfont(11);
		headline(io, LeftHead, CenterHead, RightHead);
		cur_font = NULL;
		io_nputc('\n', io, HeadHeight - 1);
		cur_font = save;
	}

	return io_putc(c, io);
}


static int line_put(int c, io_t *io)
{
	switch (c)
	{
	case 0:
		change_font(io, NULL);
		return 0;
	case '\r':
	case '\b':
	case '\f':
		return 0;
	case '\n':
		change_font(io, cur_font);

		if	(fillmode)
			io_nputc(' ', io, pc_pos ? PageWidth - pc_pos : PageWidth + LeftMargin);

		change_font(io, NULL);
		io_psub(io, pc_newline);
		pc_pos = 0;
		pc_line++;
		return 1;
	default:
		break;
	}

/*	Neue Zeile beginnen
*/
	if	(pc_pos == 0)
	{
		io_nputc(' ', io, LeftMargin);
	}

	if	(c == '\t')
	{
		c = 8 - pc_pos % 8;
		io_nputc(' ', io, c);
		pc_pos += c;
		return c;
	}

/*	Font aktualisieren
*/
	change_font(io, cur_font);

	if	(ulmode)
		io_puts(pc_ulfmt, io);

	io_putc(c, io);
	pc_pos++;
	return 1;
}


/*	Kontrollfunktion
*/


static int base_ctrl(io_t *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:
		if	(!need_init)
			io_psub(io, pc_exit);
		else	errmsg(NULL, 2);

		return io_close(io);
	default:
		return io_vctrl(io, req, list);
	}
}


static int line_ctrl(io_t *io, int req, va_list list)
{
	switch (req)
	{
	case PC_FONT:
		cur_font = getfont(va_arg(list, int));
		return 0;
	case IO_CLOSE:
		return io_close(io);
	default:
		return io_vctrl(io, req, list);
	}
}


static int page_ctrl(io_t *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:
		if	(pc_line != 0)
			page_put('\f', io);
		return io_close(io);
	default:
		return io_vctrl(io, req, list);
	}
}
