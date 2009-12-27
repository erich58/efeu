/*
Öffnen einer Efeu-Datenbank
$Copyright (C) 2006 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/preproc.h>
#include <ctype.h>

EDB *edb_open (IO *io)
{
	char *p = rd_ident(io);
	char *path = (p && *p != '<' && *p != '|') ? mdirname(p, 1) : NULL;
	memfree(p);
	return edb_meta(io, path);
}

EDB *edb_popen (const char *cmd)
{
	return edb_meta(io_popen(cmd, "r"), NULL);
}

static char *subsearch (const char *path, const char *name, const char *ext)
{
	char *p = mstrpaste(".", name, ext);
	char *fname = fsearch(path, NULL, p, NULL);
	memfree(p);
	return fname;
}


static EDB *edb_import (EfiType *type, const char *path, const char *def)
{
	EDB *edb;
	char *scan;
	IO *io;

	edb = edb_create(type);

	if	(*def == '[')
	{
		int depth;
		int n;

		depth = 0;
		def++;

		for (n = 0; def[n]; n++)
		{
			if	(def[n] == '[')
			{
				depth++;
			}
			else if	(def[n] == ']')
			{
				if	(depth)
					depth--;
				else	break;
			}
		}

		scan = mstrncpy(def, n); 
		def += n;

		if	(*def)	def++;
	}
	else	scan = NULL;

	if	(*def == ':')	def++;

	if	(*def == 0)
	{
		io = NULL;
	}
	else if	(strcmp("-", def) == 0)
	{
		io = io_fileopen(NULL, "rz");
	}
	else if	(strchr("|&./", *def))
	{
		io = io_fileopen(def, "rz");
	}
	else	io = io_findopen(path, def, NULL, "rz");

	edb_data(edb, io, scan);
	memfree(scan);
	return edb;
}

static char *type_end (const char *name)
{
	char *p;

	if	(!isalpha((unsigned char) *name) && *name != '_')
		return NULL;

	for (p = (char *) name; *p != 0; p++)
	{
		if	(*p == ':' || *p == '[')
			return p;

		if	(!isalnum((unsigned char) *p) && *p != '_')
			break;
	}
	
	return NULL;
}

static char *filterpos (const char *name)
{
	for (; *name != 0; name++)
		if	(isspace(*name))
			return (char *) name;

	return NULL;
}

EDB *edb_fopen (const char *path, const char *name)
{
	char *p;
	char *buf;
	char *filter;
	EDB *edb;

	if	(!name || *name == '|' || *name == '&')
		return edb_meta(io_fileopen(name, "rz"), mstrcpy(path));

	if	(*name == '{')
	{
		IO *io = io_cstr(name + 1);
		EfiType *type = MakeStruct(NULL, NULL, GetStruct(io, '}'));
		unsigned pos;

		io_ctrl(io, IO_GETPOS, &pos);
		edb = edb_import(type, path, name + pos + 1);
		io_close(io);
		return edb;
	}

	if	((p = type_end(name)))
	{
		char *tname = mstrncpy(name, p - name);
		EfiType *type = edb_type(tname);
		memfree(tname);
		return edb_import(type, path, p);
	}

	buf = NULL;
	filter = NULL;

	if	((p = filterpos(name)))
	{
		buf = mstrncpy(name, p - name);
		name = buf;
		filter = p + 1;
	}

	p = fsearch(path, NULL, name, NULL);

	if	(!p)	p = subsearch(path, name, "gz");
	if	(!p)	p = subsearch(path, name, "edb");
	if	(!p)	p = subsearch(path, name, "db");

	edb = edb_meta(io_fileopen(p ? p : name, "rz"),
		p ? mdirname(p, 1) : NULL);
	memfree(p);
	memfree(buf);
	return edb_filter(edb, filter);
}
