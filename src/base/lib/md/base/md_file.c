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

void md_info (IO *out, mdmat *md, const char *name, const char *mode)
{
	io_printf(out, "%s: ", name);

	if	(mode && strchr(mode, 's'))
		md_show(out, md, "S");
	else
		md_show(out, md, "h");

	if	(mode && strchr(mode, 'x'))
		md_show(out, md, "x");
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
