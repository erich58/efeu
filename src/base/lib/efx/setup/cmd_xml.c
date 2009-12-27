/*
XML-Konstruktor

$Copyright (C) 2008 Erich Fr�hst�ck
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/xml.h>

static size_t t_read (const EfiType *st, void *data, IO *io)
{
	return 0;
}

static size_t t_write (const EfiType *st, const void *data, IO *io)
{
	return 0;
}

static int t_print (const EfiType *st, const void *data, IO *io)
{
	return 0;
}

static void t_clean (const EfiType *st, void *data, int mode)
{
	XMLBuf *xml = data;
	sb_free(&xml->sbuf);
}

EfiType Type_XML = COMPLEX_TYPE("XML", "XMLBuf",
	sizeof(XMLBuf), 0,
	t_read, t_write, t_print,
	NULL, NULL, t_clean, NULL, TYPE_MALLOC);

static void f_fprint(EfiFunc *func, void *rval, void **arg)
{
	XMLBuf *xml = arg[1];

	Val_int(rval) = io_xprintf(Val_io(arg[0]),
		"XMLBuf(%p, %p, %d, %d)", xml->action, xml->par,
			xml->depth, xml->sbuf.pos);
}

static int xml_io_putc (int c, void *par)
{
	return io_putc(c, Val_io(((EfiObj *) par)->data));
}

static XMLOutput *xml_open_io (IO *io)
{
	XMLOutput *out = memalloc(sizeof *out);
	out->put = xml_io_putc;
	out->par = NewPtrObj(&Type_io, rd_refer(io));
	return out;
}

static struct {
	const char *key;
	XMLAction action;
	const char *desc;
} action_tab[] = {
	{ NULL, xml_print, "Default, entspricht beautified" },
	{ "c:ompact", xml_compact, "Kompakte Datstellung" },
	{ "b:eautified", xml_print, "Aufgelockert mit wei�en Zeichen" },
	{ "s:imple", xml_simple_print, "Einfache XML-Dateien" },
	{ "t:list", xml_tlist, "Tagliste" },
	{ "d:ump", xml_dump, "Dump der Werte mit Hierarchieposition" },
};

static void show_action (void)
{
	int i;

	for (i = 0; i < tabsize(action_tab); i++)
		io_xprintf(ioerr, "%s\t%s\n", action_tab[i].key,
			action_tab[i].desc);
}

static XMLAction get_action (const char *name)
{
	int i;

	if	(name && *name == '?')
	{
		show_action();
		return NULL;
	}

	for (i = 0; i < tabsize(action_tab); i++)
		if (mtestkey(name, NULL, action_tab[i].key))
			return action_tab[i].action;

	return action_tab[0].action;
}

static void f_open_io (EfiFunc *func, void *rval, void **arg)
{
	xml_create(arg[0], get_action(Val_str(arg[2])),
		xml_open_io(Val_io(arg[1])));
}

static void f_close (EfiFunc *func, void *rval, void **arg)
{
	XMLOutput *out = XMLBuf_close(arg[0]);
	Val_obj(rval) = out ? out->par : ptr2Obj(NULL);
	memfree(out);
}


static int32_t xml_get (void *data)
{
	return io_getucs(data);
}

static void f_parse (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[1]);
	XMLBuf_parse(arg[0], xml_get, io);
}

static void f_parse_simple (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[1]);
	xml_simple_parse(arg[0], xml_get, io);
}

static void f_beg (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_beg(arg[0], Val_str(arg[1]), Val_str(arg[2]));
}

static void f_end (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_end(arg[0]);
}

static void f_data (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_data(arg[0], Val_str(arg[1]));
}

static void f_entry (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_entry(arg[0], Val_str(arg[1]), Val_str(arg[2]));
}

static EfiFuncDef fdef_xml[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, XML)", f_fprint },
	{ FUNC_VIRTUAL, &Type_void,
		"XML::open (IO out, str method = NULL)", f_open_io },
	{ 0, &Type_void, "XML::beg (str tag, str opt = NULL)", f_beg },
	{ 0, &Type_void, "XML::data (str data)", f_data },
	{ 0, &Type_void, "XML::entry (str name, str data)", f_entry },
	{ 0, &Type_void, "XML::end ()", f_end },
	{ 0, &Type_obj, "XML::close ()", f_close },
	{ 0, &Type_void, "XML::parse (IO in)", f_parse },
	{ 0, &Type_void, "XML::simple (IO in)", f_parse_simple },
};

void CmdSetup_xml(void)
{
	AddType(&Type_XML);
	AddFuncDef(fdef_xml, tabsize(fdef_xml));
}
