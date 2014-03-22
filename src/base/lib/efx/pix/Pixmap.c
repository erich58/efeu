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
	return msprintf("%dx%dx3", pix->rows, pix->cols);
}

static RefType Pixmap_reftype = REFTYPE_INIT("Pixmap", pix_ident, pix_clean);

/*	Pixmap-File generieren
*/

EPixmap *NewPixmap (size_t rows, size_t cols, int color)
{
	EPixmap *pix = memalloc(sizeof(EPixmap));
	pix->rows = rows;
	pix->cols = cols;
	pix->data = memalloc(rows * cols * 3);

	if	(color)
	{
		size_t n;
		unsigned char r, g, b, *p;

		r = (color >> 16) & 0xff;
		g = (color >> 8) & 0xff;
		b = color & 0xff;

		for (n = rows * cols, p = pix->data; n-- > 0; p += 3)
			p[0] = r, p[1] = g, p[2] = b;
	}

	return rd_init(&Pixmap_reftype, pix);
}


/*	Datentype
*/

EfiType Type_Pixmap = REF_TYPE("Pixmap", EPixmap *);


static void PF_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = NewPixmap(Val_int(arg[1]), Val_int(arg[0]),
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

static void PF_vadjust (EfiFunc *func, void *rval, void **arg)
{
	Pixmap_vadjust(Val_ptr(arg[0]), Val_int(arg[1]), Val_int(arg[2]));
	Val_ptr(rval) = rd_refer(Val_ptr(arg[0]));
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_Pixmap, "Pixmap (int width, int height, int color = 0)",
		PF_create },
	{ 0, &Type_Pixmap, "LoadPixmap (str name)", PF_load },
	{ 0, &Type_Pixmap, "Pixmap::save (str name)", PF_save },
	{ 0, &Type_Pixmap, "Pixmap::vadjust (int color, int offset)", PF_vadjust },
};

static EfiObj *get_cols (const EfiObj *base, void *data)
{
	EPixmap *pix = base ? Val_ptr(base->data) : NULL;
	int cols = pix ? pix->cols : 0;
	return ConstObj(&Type_int, &cols);
}

static EfiObj *get_rows (const EfiObj *base, void *data)
{
	EPixmap *pix = base ? Val_ptr(base->data) : NULL;
	int rows = pix ? pix->rows : 0;
	return ConstObj(&Type_int, &rows);
}

static EfiMember var_Pixmap[] = {
	{ "cols", &Type_int, get_cols, NULL },
	{ "rows", &Type_int, get_rows, NULL },
};


void SetupPixmap (void)
{
	AddType(&Type_Pixmap);
	AddFuncDef(fdef, tabsize(fdef));
	AddEfiMember(Type_Pixmap.vtab, var_Pixmap, tabsize(var_Pixmap));
}
