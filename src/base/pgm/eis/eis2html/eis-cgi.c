/*
:*:	cgi interface to eis
:de:	CGI-Schnittstelle zu eis

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

#include <EFEU/Resource.h>
#include <EFEU/LangDef.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/Info.h>

#define	_String(x)	#x
#define	String(x)	_String(x)
#define	TOP		String(EFEUROOT)

static char *lang = NULL;
static char *node = NULL;
static char *file = NULL;
static char *cmd = NULL;

#define	FMT_HEAD	\
	":*:EFEU information system" \
	":de:EIS-Informationssystem"

#define	FMT_FAILURE	\
	":*:request failed:" \
	":de:Anfrage nicht beantwortbar:"

static void html_cmd (const char *cmd, IO *io)
{
	io_protect(io, 1);
	io_puts(cmd, io);
	io_protect(io, 0);
}

static void cgi_arg (char **ptr, const char *arg)
{
	StrBuf *sb;

	sb = sb_create(0);

	for (; *arg != 0; arg++)
	{
		switch (*arg)
		{
		case '%':

			if	(arg[1] && arg[2])
			{
				int c = 0;
				sscanf(arg + 1, "%2x", &c);
				sb_putc(c, sb);
				arg += 2;
			}

			break;

		case '+':	sb_putc(' ', sb); break;
		default:	sb_putc(*arg, sb); break;
		}
	}

	memfree(*ptr);
	*ptr = sb2str(sb);
}

static void cgi_puts (const char *p, IO *io)
{
	for (; *p != 0; p++)
	{
		switch (*p)
		{
		case '\n':
			io_putc('&', io);
			break;
		case '\t':
			io_putc('=', io);
			break;
		case ' ':
			io_putc('+', io);
			break;
		case '&':
		case '=':
		case '+':
		case '%':
		case '<':
		case '>':
			io_printf(io, "%%%2X", *p);
			break;
		default:
			io_putc(*p, io);
			break;
		}
	}
}

static InfoNode *load_cmd (InfoNode *base, const char *def)
{
	char *name = msprintf("|%s --info=dump: 2>/dev/null", def);
	InfoNode *info = AddInfo(base, def, NULL, NULL, NULL);
	LoadInfo(info, name);
	memfree(name);
	return info;
}

static InfoNode *load_file (InfoNode *info, const char *def)
{
	info = AddInfo(info, def, NULL, NULL, NULL);
	LoadInfo(info, def);
	return info;
}

static void parse_arg (int narg, char **arg)
{
	for (; narg-- > 0; arg++)
	{
		if	(*arg == NULL)
			continue;
		else if	(strncmp("lang=", *arg, 5) == 0)
			cgi_arg(&lang, *arg + 5);
		else if	(strncmp("cmd=", *arg, 4) == 0)
			cgi_arg(&cmd, *arg + 4);
		else if	(strncmp("file=", *arg, 5) == 0)
			cgi_arg(&file, *arg + 5);
		else if	(strncmp("node=", *arg, 5) == 0)
			cgi_arg(&node, *arg + 5);
	}
}

static void list_node (IO *io, InfoNode *info,
	const char *key, const char *node)
{
	InfoNode **ip;
	int i;
	
	html_cmd("<ul>\n", io);

	for (i = info->list->used, ip = info->list->data; i > 0; i--, ip++)
	{
		SetupInfo(*ip);
		io_protect(io, 1);
		io_puts("<li><a HREF=\"/cgi-bin/eis-cgi?", io);
		cgi_puts(key, io);
		cgi_puts("\nnode\t", io);

		if	(node)
		{
			cgi_puts(node, io);
			cgi_puts("/", io);
		}

		cgi_puts((*ip)->name, io);
		io_puts("\">", io);
		io_protect(io, 0);

		io_puts((*ip)->name, io);
		io_putc(' ', io);
		io_puts((*ip)->label, io);
		InfoName(io, info, NULL);
		html_cmd("</a>\n", io);
	}
	
	html_cmd("</ul>\n", io);
}

static void HTMLInfo (IO *io, InfoNode *info, const char *key, char *node)
{
	SetupInfo(info);
	html_cmd("<h3 ALIGN=center>", io);
	io_langputs(FMT_HEAD, io);
	html_cmd("</h3>\n", io);
	html_cmd("<h3>", io);
	io_putc(' ', io);
	io_puts(info->label, io);
	html_cmd("</h3>\n", io);

	if	(info->func || info->par)
	{
		html_cmd("<pre>\n", io);

		if	(!info->func)
		{
			io_psubarg(io, info->par, "nss",
				info->name, info->label);
		}
		else	info->func(io, info);

		html_cmd("</pre>\n", io);
	}

	if	(info->list)
		list_node(io, info, key, node);
}


int main (int narg, char **arg)
{
	InfoNode *info;
	IO *io;
	int dim;
	char **list;
	char *pfx;
	char *arglist;

/*	Argumente bestimmen
*/
	SetVersion("$Id: eis-cgi.c,v 1.17 2006-03-01 10:08:19 ef Exp $");
	ParseCommand(&narg, arg);

	parse_arg(narg - 1, arg + 1);
	dim = mstrsplit(getenv("QUERY_STRING"), "&", &list);
	parse_arg(dim, list);
	memfree(list);

	putenv("PATH=" TOP "/bin:/usr/bin:/bin");

	if	(lang)
	{
		SetLangDef(lang);
		putenv(msprintf("LANG=%s", lang));
		pfx = msprintf("lang\t%s\n", lang);
	}
	else	pfx = NULL;

	info = GetInfo(NULL, NULL);
	info->list = NULL;
	info->par = NULL;

/*	HTML-Dokument generieren
*/
	io_puts("Content-type:text/html\n\n", iostd);

	io = io_html(iostd);

	if	(cmd != NULL)
	{
		info = load_cmd(NULL, cmd);
		arglist = msprintf("%scmd\t%s", pfx, cmd);
	}
	else if	(file != NULL)
	{
		info = load_file(NULL, file);
		arglist = msprintf("%sfile\t%s", pfx, file);
	}
	else
	{
		info = NULL;
		arglist = msprintf("%snode\t", pfx);
	}

	info = GetInfo(info, node);

	if	(!info)
	{
		html_cmd("<pre>\n", io);
		io_langputs(FMT_FAILURE, io);
		io_puts("\n\n", io);
		io_printf(iostd, "%7s %#s\n", "cmd:", cmd);
		io_printf(iostd, "%7s %#s\n", "file:", file);
		io_printf(iostd, "%7s %#s\n", "node:", node);
		html_cmd("</pre>\n", io);
	}
	else	HTMLInfo(io, info, arglist, node);

	io_close(io);
	exit(0);
	return 0;
}
