/*
Tabelle mit gemapten Matrizen

$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include "CubeHandle.h"
#include <EFEU/printobj.h>
#include <EFEU/parsearg.h>
#include <ctype.h>

static NameKeyTab MapTab = NKT_DATA("MapTab", 32, rd_deref);

void MapTab_clean (void)
{
	nkt_clean (&MapTab);
}

void MapTab_add (const char *name, const char *path, const char *arg)
{
	mdmat *md;
	AssignArg *x;
	char *p;
	
	md = md_fload(path, NULL, NULL);
	nkt_insert (&MapTab, mstrcpy(name), md);

	while ((x = assignarg(arg, &p, " ,;\t\n")))
	{
		if	(test_key("t[ime]", x->name))
		{
			md_setflag(md, x->arg, 0, mdsf_mark,
				MDXFLAG_TIME, NULL, 0);
		}
		else if	(test_key("c[olumn]", x->name))
		{
			md_setflag(md, x->arg, 0, mdsf_mark,
				MDXFLAG_MARK, NULL, 0);
		}

		arg = p;
		memfree(x);
	}
}

static void parse_line (char *p)
{
	char *name;
	char *path;
	char *arg;

	name = p;

	while (isspace(*name))
		name++;

	if	(*name == 0 || *name == '#')
		return;

	path = name;

	while (*path && !isspace(*path))
		path++;

	if	(*path == 0)
		return;

	*path = 0;
	path++;

	while (isspace(*path))
		path++;

	arg = path;

	while (*arg && !isspace(*arg))
		arg++;

	if	(*arg)
	{
		*arg = 0;
		arg++;

		while (isspace(*arg))
			arg++;
	}
	else	arg = NULL;

	MapTab_add(name, path, arg);
}

void MapTab_load (IO *io)
{
	static STRBUF(buf, 1000);
	int c;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '\n')
		{
			sb_putc(0, &buf);
			parse_line((char *) buf.data);
			sb_begin(&buf);
		}
		else	sb_putc(c, &buf);
	}

	sb_free(&buf);
}

static int map_visit (const char *name, void *data, void *par)
{
	CubeHandle *handle = par;
	mdmat *md = data;

	CubeHandle_string(handle, name);
	io_putc(';', handle->out);
	io_putc('"', handle->out);
	PrintType(handle->out, md->type, 1);
	io_putc('"', handle->out);
	io_putc(';', handle->out);
	io_printf(handle->out, "%llu;", (uint64_t) md_dim(md->axis));
	io_printf(handle->out, "%llu;", (uint64_t) md->size / md->type->size);
	io_printf(handle->out, "%llu;", (uint64_t) md->type->size);
	CubeHandle_stridx(handle, md->sbuf, md->i_name);
	io_putc(';', handle->out);
	CubeHandle_stridx(handle, md->sbuf, md->i_desc);
	io_putc('\n', handle->out);
	return 0;
}

void MapTab_list (CubeHandle *handle)
{
	if	(handle->headline)
		io_puts("#name;type;dim;nel;size;head;desc\n", handle->out);

	nkt_walk(&MapTab, map_visit, handle);
}

mdmat *MapTab_get (const char *name)
{
	return rd_refer(nkt_fetch(&MapTab, name, NULL));
}
