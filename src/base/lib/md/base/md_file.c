/*
:*:display meta information from data cube
:de:Informationen zu einer Multidimensionale Matrix abfragen
*/

#include <EFEU/mdmat.h>
#include <EFEU/MapFile.h>
#include <EFEU/preproc.h>
#include <EFEU/printobj.h>

static int extname (const char *name)
{
	char *p;

	if	(name == NULL || *name == '|' || *name == '&')
		return 1;

	p = strrchr(name, '.');

	if	(!p)	return 0;

	p++;

	if	(strcmp(p, "gz") == 0)	return 1;
	if	(strcmp(p, "bz2") == 0)	return 1;

	return 0;
}

static mdmat *get_head (const char *name)
{
	IO *io;
	mdmat *md;
	MapFile *map;

	if	(extname(name))
	{
		io = io_fileopen(name, "rdz");
	}
	else if	((map = MapFile_open(name)))
	{
		if	((md = md_map(map)))
		{
			rd_deref(map);
			return md;
		}

		io = io_data(map, map->data, map->size);
	}
	else	io = io_fileopen(name, "rdz");

	md = md_gethdr(io, 0);
	io_close(io);
	return md;
}

static void show_struct (IO *out, mdmat *md)
{
	mdaxis *x;

	for (x = md->axis; x != NULL; x = x->next)
		io_xprintf(out, " %s[%zu]",
			StrPool_get(x->sbuf, x->i_name), x->dim);
}

static void show_type (IO *out, mdmat *md, int mode)
{
	size_t size = 1;
	mdaxis *x;

	for (x = md->axis; x != NULL; x = x->next)
		size *= x->dim;

	io_putc(' ', out);
	PrintType(out, md->type, 1);
	io_xprintf(out, "[%zu", size);

	if	(mode)
	{
		int delim = '=';

		for (x = md->axis; x != NULL; x = x->next, delim = '*')
			io_xprintf(out, "%c%zu", delim, x->dim);
	}

	io_putc(']', out);
}

static void show_head (IO *out, mdmat *md)
{
	char *p;

	if	((p = TypeHead(md->type)) != NULL)
	{
		io_puts(p, out);
		io_putc('\n', out);
		memfree(p);
	}

	PrintType(out, md->type, 3);
	io_putc('\n', out);
}

static void show_desc (IO *out, mdmat *md)
{
	char *p;

	io_puts("/*\n", out);

	if	((p = StrPool_get(md->sbuf, md->i_name)))
	{
		io_puts(p, out);
		io_puts("\n", out);
	}

	if	((p = StrPool_get(md->sbuf, md->i_desc)))
	{
		io_puts(p, out);
		io_puts("\n", out);
	}

	io_puts("*/\n", out);
}

static void short_axis_list (IO *out, mdmat *md)
{
	mdaxis *x;
	size_t n, i;

	for (x = md->axis; x; x = x->next)
	{
		n = io_xprintf(out, "%s:", StrPool_get(md->sbuf, x->i_name));

		for (i = 0; i < x->dim; i++)
		{
			if	(n > 70)
			{
				io_puts("\n\t", out);
				n = 8;
			}
			else	n += io_nputc(' ', out, 1);

			n += io_puts(StrPool_get(x->sbuf,
				x->idx[i].i_name), out);
		}

		io_putc('\n', out);
	}
}

static void verbose_axis_list (IO *out, mdmat *md)
{
	mdaxis *x;
	size_t i;

	for (x = md->axis; x; x = x->next)
	{
		io_puts(StrPool_get(md->sbuf, x->i_name), out);
		io_putc('\t', out);
		io_puts(StrPool_get(md->sbuf, x->i_desc), out);
		io_putc('\n', out);

		for (i = 0; i < x->dim; i++)
		{
			io_putc('\t', out);
			io_puts(StrPool_get(x->sbuf, x->idx[i].i_name), out);
			io_putc('\t', out);
			io_puts(StrPool_get(x->sbuf, x->idx[i].i_desc), out);
			io_putc('\n', out);
		}
	}
}


void md_info (IO *out, mdmat *md, const char *name, const char *mode)
{
	if	(!mode)	mode = "";

	io_xprintf(out, "%s:", name);

	if	(strchr(mode, 's'))
	{
		show_struct(out, md);
		show_type(out, md, 0);
	}
	else
	{
		show_type(out, md, 1);

		if	(!strchr(mode, 'd'))
			io_xprintf(out, " %#s",
				StrPool_get(md->sbuf, md->i_name));
	}

	io_putc('\n', out);

	if	(strchr(mode, 'd'))
		show_desc(out, md);

	if	(strchr(mode, 'h'))
		show_head(out, md);

	if	(strchr(mode, 'x'))
		verbose_axis_list(out, md);
	else if	(strchr(mode, 'X'))
		short_axis_list(out, md);
}

int md_file (IO *out, const char *name, const char *mode)
{
	mdmat *md = get_head(name);
	
	if	(md)
	{
		md_info(out, md, name, mode);
		rd_deref(md);
		return 1;
	}
	else	return 0;
}
