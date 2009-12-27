/*	EFEU-Informationssystem
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pconfig.h>
#include <EFEU/Info.h>
#include <EFEU/efwin.h>
#include <sys/errno.h>


#define	CONFIRM_QUIT	0

#define	ctrl(c)	((c) & 0x1f)

#define	PSIZE ((info_win->_maxy - 2) / 2)

static char *node = NULL;
static char *file = NULL;
static char *cmd = NULL;
static char *fmt_head = "EIS-Informationssystem\n\n";
static char *fmt_foot = NULL;

static char *fmt_failed = "Anfrage nicht beantwortbar:\n\n";

Var_t vardef[] = {
	{ "fmt_head",	&Type_str, &fmt_head },
	{ "fmt_foot",	&Type_str, &fmt_foot },
	{ "fmt_failed",	&Type_str, &fmt_failed },
};


static void html_cmd (const char *cmd, io_t *io)
{
	io_protect(io, 1);
	io_puts(cmd, io);
	io_protect(io, 0);
}

static void cgi_arg (char **ptr, const char *arg)
{
	strbuf_t *sb;

	sb = new_strbuf(0);

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

static void cgi_puts (const char *p, io_t *io)
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

static InfoNode_t *load_cmd (InfoNode_t *base, const char *def)
{
	char *name = msprintf("|%s -dump", def);
	InfoNode_t *info = AddInfo(base, def, NULL, NULL, NULL);
	LoadInfo(info, name);
	memfree(name);
	return info;
}

static InfoNode_t *load_file (InfoNode_t *info, const char *def)
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
		else if	(strncmp("cmd=", *arg, 4) == 0)
			cgi_arg(&cmd, *arg + 4);
		else if	(strncmp("file=", *arg, 5) == 0)
			cgi_arg(&file, *arg + 5);
		else if	(strncmp("node=", *arg, 5) == 0)
			cgi_arg(&node, *arg + 5);
		else	;
	}
}

static void list_node (io_t *io, InfoNode_t *info,
	const char *key, const char *node)
{
	InfoNode_t **ip;
	int i;
	
	html_cmd("<ul>\n", io);

	for (i = info->list->used, ip = info->list->data; i > 0; i--, ip++)
	{
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

static void HTMLInfo (io_t *io, InfoNode_t *info, const char *key, char *node)
{
	if	(info->load)
		info->load(info);

	html_cmd(fmt_head, io);
	html_cmd("<h3>", io);
	io_putc(' ', io);
	io_puts(info->label, io);
	html_cmd("</h3>\n", io);

	if	(info->func || info->par)
	{
		html_cmd("<pre>\n", io);

		if	(!info->func)
		{
			reg_cpy(1, info->name);
			reg_cpy(2, info->label);
			io_psub(io, info->par);
		}
		else	info->func(io, info);

		html_cmd("</pre>\n", io);
	}

	if	(info->list)
		list_node(io, info, key, node);

	html_cmd(fmt_foot, io);
}


int main (int narg, char **arg)
{
	InfoNode_t *info;
	io_t *io;
	int dim;
	char **list;
	char *arglist;

	putenv("PATH=.:/efeu/bin:/usr/bin:/bin");

	libinit(arg[0]);
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	io_puts("Content-type:text/html\n\n", iostd);

	parse_arg(narg - 1, arg + 1);
	dim = strsplit(getenv("QUERY_STRING"), "&", &list);
	parse_arg(dim, list);

	io = io_html(iostd);

	info = GetInfo(NULL, NULL);
	info->list = NULL;
	info->par = NULL;

	if	(cmd != NULL)
	{
		info = load_cmd(NULL, cmd);
		arglist = mstrpaste("\t", "cmd", cmd);
	}
	else if	(file != NULL)
	{
		info = load_file(NULL, file);
		arglist = mstrpaste("\t", "file", file);
	}
	else
	{
		info = NULL;
		arglist = mstrcpy("node\t");
	}

	info = GetInfo(info, node);

	if	(!info)
	{
		html_cmd("<pre>\n", io);
		io_puts(fmt_failed, io);
		io_printf(iostd, "Kommando: %#s\n", cmd);
		io_printf(iostd, "Datei: %#s\n", file);
		io_printf(iostd, "Knoten: %#s\n", node);
		html_cmd("</pre>\n", io);
	}
	else	HTMLInfo(io, info, arglist, node);

	io_close(io);
	libexit(0);
	return 0;
}
