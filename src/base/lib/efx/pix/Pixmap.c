/*
Pixelfile generieren

$Copyright (C) 2000 Erich Frühstück
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

#include <EFEU/Pixmap.h>
#include <EFEU/object.h>
#include <EFEU/stdtype.h>

/*	Referenztype
*/

static void pix_clean (void *data)
{
	EPixmap *tg = data;
	memfree(tg->data);
	memfree(tg);
}

static char *pix_ident (const void *data)
{
	const EPixmap *pix = data;
	return msprintf("%dx%dx3", pix->width, pix->height);
}

static RefType Pixmap_reftype = REFTYPE_INIT("Pixmap", pix_ident, pix_clean);

/*	Pixmap generieren
*/

EPixmap *NewPixmap (size_t width, size_t height, int color)
{
	EPixmap *pix = memalloc(sizeof(EPixmap));
	pix->height = height;
	pix->width = width;
	pix->data = memalloc(width * height * 3);
	pix->fillMode = 0;

	if	(color)
	{
		size_t n;
		unsigned char r, g, b, *p;

		r = (color >> 16) & 0xff;
		g = (color >> 8) & 0xff;
		b = color & 0xff;

		for (n = width * height, p = pix->data; n-- > 0; p += 3)
			p[0] = r, p[1] = g, p[2] = b;
	}

	return rd_init(&Pixmap_reftype, pix);
}

/*	Pixmap ausgeben
*/

void SavePixmap (EPixmap *pix, IO *io)
{
	if	(!pix || !io)	return;

	io_xprintf(io, "P6\n%d %d\n255\n", pix->width, pix->height);
	io_dbwrite(io, pix->data, 1, pix->height * pix->width * 3);
}


/*	Datentype
*/

EfiType Type_Pixmap = REF_TYPE("Pixmap", EPixmap *);


static void PF_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = NewPixmap(Val_int(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]));
}

static void PF_load (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_fileopen(Val_str(arg[0]), "rz");
	Val_ptr(rval) = LoadPixmap(io);
	io_close(io);
}

static void PF_save (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_fileopen(Val_str(arg[1]), "wz");
	SavePixmap(Val_ptr(arg[0]), io);
	io_close(io);
	Val_ptr(rval) = rd_refer(Val_ptr(arg[0]));
}

static void PF_gray (EfiFunc *func, void *rval, void **arg)
{
	Pix_gray(Val_ptr(arg[0]));
}

static void PF_bw (EfiFunc *func, void *rval, void **arg)
{
	Pix_bw(Val_ptr(arg[0]));
}

static void PF_fill (EfiFunc *func, void *rval, void **arg)
{
	Pix_fill(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]),
		Val_int(arg[3]), Val_int(arg[4]),
		Val_int(arg[5]));
}

static void PF_clip (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = Pix_clip(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]),
		Val_int(arg[3]), Val_int(arg[4]),
		Val_int(arg[5]));
}

static void PF_line (EfiFunc *func, void *rval, void **arg)
{
	Pix_line(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]),
		Val_int(arg[3]), Val_int(arg[4]),
		Val_int(arg[5]), Val_int(arg[6]));
}

static void PF_frame (EfiFunc *func, void *rval, void **arg)
{
	Pix_frame(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]),
		Val_int(arg[3]), Val_int(arg[4]),
		Val_int(arg[5]), Val_int(arg[6]));
}

static void PF_grayhist (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_fileopen(Val_str(arg[1]), "wz");
	Pix_grayhist(Val_ptr(arg[0]), io);
	io_close(io);
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_Pixmap, "Pixmap (int width, int height, "
		"int color = 0xFFFFFF)", PF_create },
	{ 0, &Type_Pixmap, "LoadPixmap (str name)", PF_load },
	{ 0, &Type_Pixmap, "Pixmap::clip (int x0, int y0, "
		"int width, int height, int color = 0xFFFFFF)", PF_clip },
	{ 0, &Type_Pixmap, "Pixmap::save (str name)", PF_save },
	{ 0, &Type_void, "Pixmap::gray ()", PF_gray },
	{ 0, &Type_void, "Pixmap::bw ()", PF_bw },
	{ 0, &Type_void, "Pixmap::grayhist (str save)", PF_grayhist },
	{ 0, &Type_void, "Pixmap::fill (int x0, int y0, int x1, int y1, "
		"int color)", PF_fill },
	{ 0, &Type_void, "Pixmap::line (int x0, int y0, int x1, int y1, "
		"int width, int color)", PF_line },
	{ 0, &Type_void, "Pixmap::frame (int x0, int y0, int x1, int y1, "
		"int width, int color)", PF_frame },
};

static EfiObj *get_width (const EfiObj *base, void *data)
{
	EPixmap *pix = base ? Val_ptr(base->data) : NULL;
	int width = pix ? pix->width : 0;
	return ConstObj(&Type_int, &width);
}

static EfiObj *get_height (const EfiObj *base, void *data)
{
	EPixmap *pix = base ? Val_ptr(base->data) : NULL;
	int height = pix ? pix->height : 0;
	return ConstObj(&Type_int, &height);
}

static EfiMember var_Pixmap[] = {
	{ "width", &Type_int, get_width, NULL },
	{ "height", &Type_int, get_height, NULL },
};


void SetupPixmap (void)
{
	AddType(&Type_Pixmap);
	AddFuncDef(fdef, tabsize(fdef));
	AddEfiMember(Type_Pixmap.vtab, var_Pixmap, tabsize(var_Pixmap));
}
