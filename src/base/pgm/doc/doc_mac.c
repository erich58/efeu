/*
Dokumentbefehle ausführen

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

#include <EFEU/parsedef.h>
#include "efeudoc.h"
#include <ctype.h>


/*	Argumentbuffer
*/

static VECBUF(arg_buf, 32, sizeof(char **));

#define nextarg	((char **) vb_next(&arg_buf))[0]


/*	Ausgabefunktionen
*/

typedef void (*PutFunc) (StrBuf *buf, const char *arg, int flag);

static void pf_std (StrBuf *buf, const char *arg, int flag)
{
	if	(flag && arg)
		sb_putc(' ', buf);

	sb_puts(arg, buf);
}

static void pf_list (StrBuf *buf, const char *arg, int flag)
{
	if	(flag)	sb_putc(',', buf);

	if	(!arg)	return;

	for (; *arg != 0; arg++)
	{
		if	(*arg == ',' || *arg == ')')
		{
			sb_putc('\'', buf);
			sb_putc(*arg, buf);
			sb_putc('\'', buf);
		}
		else	sb_putc(*arg, buf);
	}
}

static void pf_str (StrBuf *buf, const char *arg, int flag)
{
	if	(flag)
		sb_puts("\", \"", buf);

	if	(arg)
	{
		IO *out = io_strbuf(buf);
		io_xputs(arg, out, "\"");
		io_close(out);
	}
}

#define	pfunc(q,d)	((q) ? pf_str : ((d > 0) ? pf_list : pf_std))


/*	Argument substituieren
*/

static void arg_sub (StrBuf *buf, IO *io, PutFunc put, int argc, char **argv)
{
	int c = io_getc(io);

	switch (c)
	{
	case EOF:

		return;

	case '#':

		sb_printf(buf, "%d", argc);
		break;

	case '*':

		for (c = 1; c < argc; c++)
			put(buf, argv[c], c > 1);

		break;

	default:

		if	(isdigit(c))
		{
			c -= '0';
			put(buf, (c < argc) ? argv[c] : NULL, 0);
		}
		else	sb_putc(c, buf);

		break;
	}
}

/*	Makro expandieren
*/

static char *mac_expand (const char *fmt, int argc, char **argv)
{
	StrBuf *buf;
	IO *in;
	int c, quote, depth;

	if	(fmt == NULL)	return NULL;

	buf = sb_acquire();
	in = io_cstr(fmt);
	quote = 0;
	depth = 0;

	while ((c = io_getc(in)) != EOF)
	{
		switch (c)
		{
		case '$':
			arg_sub(buf, in, pfunc(quote, depth), argc, argv);
			continue;
		case '\\':
			if	(io_peek(in) != EOF)
			{
				sb_putc(c, buf);
				c = io_getc(in);
			}
			break;
		case '"':
			quote = !quote;
			break;
		case '(':
			if	(!quote)	depth++;
			break;
		case ')':
			if	(!quote)	depth--;
			break;
		default:	
			break;
		}

		sb_putc(c, buf);
	}

	io_close(in);
	return sb_cpyrelease(buf);
}


DocMac *Doc_getmac (Doc *doc, const char *name)
{
	Stack *ptr;
	DocMac *mac;

	if	(!(doc && doc->cmd_stack))	return NULL;

	for (ptr = doc->cmd_stack; ptr != NULL; ptr = ptr->next)
		if ((mac = DocTab_getmac(ptr->data, name)) != NULL)
			return mac;

	return NULL;
}


void Doc_mac (Doc *doc, IO *in)
{
	DocMac *mac;
	char *p;

	p = DocParseName(in, '@');

	if	(io_peek(in) == '=')
	{
		io_getc(in);
		p = mstrcpy(p);	/* Temporärer Buffer !!! */
		DocTab_setmac(doc->cmd_stack->data, p,
			Doc_lastcomment(doc), DocParseExpr(in));
		return;
	}

	mac = Doc_getmac(doc, p);

	if	(mac == NULL)
	{
		io_note(in, "[Doc:13]", "s", p);
		io_puts(p, doc->out);
		Doc_char(doc, ';');
		return;
	}

	if	(io_peek(in) == '(')
	{
		io_getc(in);
		vb_clean(&arg_buf, NULL);
		nextarg = mstrcpy(mac->name);

		do	nextarg = DocParseMacArg(in);
		while	(io_getc(in) == ',');

		p = mac_expand(mac->fmt, arg_buf.used, arg_buf.data);
		vb_clean(&arg_buf, memfree);
	}
	else	p = mac_expand(mac->fmt, 1, &mac->name);

	io_push(in, io_mstr(p));
}
