/*
Umgebungswechsel

$Copyright (C) 1999 Erich Frühstück
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

#include <HTML.h>

/*	Hilfskommandos
*/

static void put_att (HTML *html, int flag, char *att)
{
	if	(html->copy)	att = NULL;
	if	(html->att)	io_xprintf(html->out, "</%s>", html->att);

	if	(flag)
	{
		pushstack(&html->s_att, html->att);
		html->att = att;
	}
	else	html->att = popstack(&html->s_att, NULL);

	if	(html->att)	io_xprintf(html->out, "<%s>", html->att);
}

static void put_env (HTML *html, int flag, const char *name, int nl)
{
	HTML_newline(html, 0);
	io_putc('<', html->out);

	if	(!flag)
		io_putc('/', html->out);

	io_puts(name, html->out);
	html->last = io_putc('>', html->out);

	if	(!flag || nl)
		html->last = io_putc('\n', html->out);
}

static void put_sect (HTML *html, int flag, int type)
{
	HTML_newline(html, 0);

	if	(flag)
	{
		io_puts("<P>\n", html->out);
		sb_trunc(&html->buf);
		io_push(html->out, io_strbuf(&html->buf));
		HTML_cpush(html, NULL, 0);
	}
	else
	{
		io_close(io_pop(html->out));
		io_ctrl(html->out, type, sb_nul(&html->buf));
		io_puts("<BR>\n", html->out);

		while (html->s_cmd)
			HTML_cpop(html);
	}
}

static void list_env (HTML *html, int flag, const char *name)
{
	HTML_newline(html, 0);

	if	(!flag)
	{
		io_puts(html->enditem, html->out);
		html->enditem = NULL;
		html->nextpar = "\n";
	}

	put_env(html, flag, name, 1);
}

static char *get_align(const char *def)
{
	char *align = "left";

	if	(!def)	return align;

	for (; *def; def++)
	{
		switch (*def)
		{
		case 'l':	align = "left"; break;
		case 'c':	align = "center"; break;
		case 'r':	align = "right"; break;
		default:	break;
		}
	}

	return align;
}

int HTML_env (void *drv, int flag, va_list list)
{
	static int sec_flag = 0;
	HTML *html = drv;
	char *p;

	int cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_PAR_STD:
		HTML_newline(html, 0);

		if	(flag)
		{
			io_puts(html->nextpar, html->out);
			html->nextpar = "<P>";
		}
		else	io_putc('\n', html->out);

		break;
	case DOC_PAR_TAG:
		if	(flag)
		{
			io_puts(html->enditem, html->out);
			io_puts("<DT>", html->out);
		}
		else
		{
			io_puts("</DT><DD>", html->out);
			html->enditem = "</DD>";
		}
		break;

	case DOC_QUOTE_SGL:
		io_puts(flag ? "`" : "'", html->out);
		break;
	case DOC_QUOTE_DBL:
		io_puts(flag ? "&raquo;" : "&laquo;", html->out);
		break;

	case DOC_SEC_PART:
		break;
	case DOC_SEC_MCHAP:
	case DOC_SEC_CHAP:
		if	(flag)
		{
			p = va_arg(list, char *);

			if	(mstrcmp(p, "*") == 0)
			{
				HTML_newline(html, 0);
				io_puts("\n<P><B>", html->out);
				sec_flag = 1;
				break;
			}
		}
		else if	(sec_flag)
		{
			sec_flag = 0;
			HTML_newline(html, 0);
			io_puts("</B><BR>\n", html->out);
			break;
		}

		put_sect(html, flag, HTML_CHAP);
		break;
	case DOC_SEC_SECT:
	case DOC_SEC_SSECT:
		put_sect(html, flag, HTML_SEC);
		break;
	case DOC_SEC_HEAD:
	case DOC_SEC_PARA:
	case DOC_SEC_SHEAD:
	case DOC_SEC_MARG:
	case DOC_SEC_NOTE:
		HTML_newline(html, 0);
		io_puts(flag ? "\n<P><B>" : "</B><BR>\n", html->out);
		break;
	case DOC_ENV_MPAGE:
		if	(flag)
		{
			char *p = va_arg(list, char *);
			p = msprintf("%s(%s)", va_arg(list, char *), p);
			io_ctrl(html->out, HTML_SEC, p);
			memfree(p);
		}

		break;
	case DOC_ATT_RM:	put_att(html, flag, NULL); break;
	case DOC_ATT_BF:	put_att(html, flag, "B"); break;
	case DOC_ATT_IT:	put_att(html, flag, "I"); break;
	case DOC_ATT_TT:	put_att(html, flag, "CODE"); break;

	case DOC_LIST_ITEM:	list_env(html, flag, "UL"); break;
	case DOC_LIST_ENUM:	list_env(html, flag, "OL"); break;
	case DOC_LIST_DESC:	list_env(html, flag, "DL"); break;
		
	case DOC_ENV_INTRO:
		HTML_newline(html, 0);
		break;
	case DOC_ENV_QUOTE:
		if	(!flag)
		{
			HTML_newline(html, 0);
			io_puts("</BLOCKQUOTE>\n", html->out);
		}
		else	html->nextpar = "<BLOCKQUOTE>";

		break;
	case DOC_MODE_COPY:
		html->copy = flag;
		break;
	case DOC_MODE_SKIP:
		html->skip = flag;
		break;
	case DOC_MODE_HTML:
	case DOC_MODE_PLAIN:
		html->put = flag ? DocDrv_plain : HTML_putc;
		break;
	case DOC_MODE_TEX:
	case DOC_MODE_MAN:
		HTML_env(html, flag, list);
		break;
	case DOC_MODE_VERB:
		put_env(html, flag, "PRE", 1);
		html->put = flag ? HTML_plain : HTML_putc;
		break;
	case DOC_ENV_FORMULA:
		break;
	case DOC_ENV_TAB:
		HTML_newline(html, 0);

		if	(flag)
		{
			char *def;

			va_arg(list, char *);
			def = va_arg(list, char *);
			HTML_coldef(html, def);
			io_printf(html->out,
				"<TABLE frame=\"box\" rules=\"all\">\n");
		}
		else
		{
			io_puts("</TABLE>\n", html->out);
		}

		html->last = '\n';
		break;
	case DOC_ENV_MCOL:
		if	(flag)
		{
			char *def = va_arg(list, char *);
			int cdim = va_arg(list, int);
			io_printf(html->out, "<TD colspan=\"%d\" align=\"%s\">",
				cdim, get_align(def));
			html->hmode = NULL;
			html->cpos += cdim - 1;
		}
		break;
	default:
		return EOF;
	}

	return 0;
}
