/*
Steuerbefehle

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

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <DocCtrl.h>
#include <HTML.h>
#include <efeudoc.h>


int HTML_cmd (HTML_t *html, va_list list)
{
	int cmd;
	int n;
	
	cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_BREAK:
		io_puts("<BR>", html->out);
		break;
#if	0
	case DOC_CMD_NPAGE:
	case DOC_CMD_TITLE:
#endif
	case DOC_CMD_TOC:
		io_puts("<P><A HREF=\"toc.html\">", html->out);
		io_puts("Inhalt", html->out);
		io_puts("</A><BR>\n", html->out);
		break;
	case DOC_CMD_LOF:
		io_puts("<P><A HREF=\"lof.html\">", html->out);
		io_puts("Verzeichnis der Übersichten", html->out);
		io_puts("</A><BR>\n", html->out);
		break;
	case DOC_CMD_ITEM:
		html->nextpar = NULL;
		io_puts(html->enditem, html->out);
		io_puts("<LI>", html->out);
		html->enditem = "</LI>\n";
		break;
	case DOC_CMD_APP:
		return io_vctrl(html->out, cmd, list);
	case DOC_CMD_IDX:
	case DOC_REF_LABEL:
		io_vctrl(html->out, cmd, list);

		if	(html->last == '\n');
			io_putc('\n', html->out);
		break;
	case DOC_CMD_MARK:
		n = va_arg(list, int);

		if	(n)
			io_printf(html->out, "<SUP>%d</SUP>", n);
		else	io_puts("<SUP>*</SUP>", html->out);

		break;
#if	0
	case DOC_CMD_TEX:
#endif
	case DOC_REF_STD:
	case DOC_REF_PAGE:
	case DOC_REF_VAR:
		return io_vctrl(html->out, cmd, list);
#if	0
	case DOC_REF_MAN:
#endif
	case DOC_TAB_BEG:
		HTML_newline(html, 0);
		io_puts("<TR>\n", html->out);
		io_puts(html->bline ? "<TH>" : "\n<TD>", html->out);
		break;
	case DOC_TAB_SEP:
		io_puts(html->bline ? "</TH>\n<TH>" : "</TD>\n<TD>", html->out);
		break;
	case DOC_TAB_END:
		io_puts(html->bline ? "</TH>" : "</TD>", html->out);
		io_puts("\n</TR>\n", html->out);
		html->last = '\n';
		html->bline = 0;
		break;
	case DOC_TAB_HEIGHT:
		break;
	case DOC_TAB_BLINE:
		html->bline = 1;
		break;
	case DOC_TAB_BRULE:
		break;
	case DOC_TAB_INDENT:
		break;
	default:
		return EOF;
	}

	return 0;
}
