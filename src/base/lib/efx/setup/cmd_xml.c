/*
XML-Konstruktor

$Copyright (C) 2008 Erich Frühstück
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

static EnumTypeDef xmltype[] = {
	{ "pi",		xml_pi,		"Process instruction" },
	{ "dtd",	xml_dtd,	"Document type declaration" },

	{ "beg",	xml_beg,	"begin tag" },
	{ "att",	xml_att,	"attribut" },
	{ "data",	xml_data,	"data" },
	{ "cdata",	xml_cdata,	"cdata" },
	{ "end",	xml_end,	"end tag" },

	{ "comm",	xml_comm,	"comment" },
	{ "err",	xml_err,	"error" },
};

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
	{ "b:eautified", xml_print, "Aufgelockert mit weißen Zeichen" },
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


static void f_parse (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_parse(arg[0], Val_io(arg[1]));
}

static void f_beg (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_beg(arg[0], Val_str(arg[1]));
}

static void f_entry (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_entry(arg[0], Val_str(arg[1]), Val_str(arg[2]));
}

static void f_att (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_att(arg[0], Val_str(arg[1]), Val_str(arg[2]));
}

static void f_pi (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_pi(arg[0], Val_str(arg[1]), Val_str(arg[2]));
}

static void do_fmt (XMLBuf *xml, XMLType type,
	const char *fmt, EfiObjList *list)
{
	if	(xml)
	{
		IO *io;
	       
		XMLBuf_start(xml, NULL);
		io = io_strbuf(&xml->sbuf);
		PrintFmtList(io, fmt, list);
		io_close(io);
		XMLBuf_action(xml, type, -1);
	}
}

static void f_rem (EfiFunc *func, void *rval, void **arg)
{
	do_fmt(arg[0], xml_comm, Val_str(arg[1]), Val_list(arg[2]));
}

static void f_err (EfiFunc *func, void *rval, void **arg)
{
	do_fmt(arg[0], xml_err, Val_str(arg[1]), Val_list(arg[2]));
}

static void f_dtd (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_dtd(arg[0], Val_str(arg[1]), Val_str(arg[2]), Val_str(arg[3]));
}

static void f_end (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_end(arg[0]);
}

static void f_data (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_data(arg[0], xml_data, Val_str(arg[1]));
}

static void f_cdata (EfiFunc *func, void *rval, void **arg)
{
	XMLBuf_data(arg[0], xml_cdata, Val_str(arg[1]));
}

static EfiFuncDef fdef_xml[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, XML)", f_fprint },
	{ FUNC_VIRTUAL, &Type_void,
		"XML::open (IO out, str method = NULL)", f_open_io },
	{ 0, &Type_obj, "XML::close ()", f_close },
	{ 0, &Type_void, "XML::parse (IO in)", f_parse },

	{ 0, &Type_void, "XML::dtd (str name, str id, str decl = NULL)", f_dtd },
	{ 0, &Type_void, "XML::pi (str name, str data)", f_pi },
	{ 0, &Type_void, "XML::rem (str fmt, ...)", f_rem },
	{ 0, &Type_void, "XML::err (str fmt, ...)", f_err },

	{ 0, &Type_void, "XML::entry (str tag, str data)", f_entry },
	{ 0, &Type_void, "XML::beg (str tag)", f_beg },
	{ 0, &Type_void, "XML::att (str name, str value)", f_att },
	{ 0, &Type_void, "XML::data (str data)", f_data },
	{ 0, &Type_void, "XML::cdata (str data)", f_cdata },
	{ 0, &Type_void, "XML::end ()", f_end },

};

void CmdSetup_xml(void)
{
	MakeEnumType("XMLType", xmltype, tabsize(xmltype));
	AddType(&Type_XML);
	AddFuncDef(fdef_xml, tabsize(fdef_xml));
}
