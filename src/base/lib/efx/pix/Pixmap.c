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

static Pixmap_t *PixmapAdmin (Pixmap_t *tg, const Pixmap_t *src)
{
	if	(tg)
	{
		memfree(tg->data);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(Pixmap_t));
}

static char *PixmapIdent (Pixmap_t *pix)
{
	return msprintf("%dx%dx3", pix->rows, pix->cols);
}

ADMINREFTYPE(Pixmap_reftype, "Pixmap", PixmapIdent, PixmapAdmin);


/*	Pixmap-File generieren
*/

Pixmap_t *NewPixmap (size_t rows, size_t cols, int color)
{
	Pixmap_t *pix = rd_create(&Pixmap_reftype);
	pix->rows = rows;
	pix->cols = cols;
	pix->data = memalloc(rows * cols * 3);

	if	(color)
	{
		size_t n;
		uchar_t r, g, b, *p;

		r = (color >> 16) & 0xff;
		g = (color >> 8) & 0xff;
		b = color & 0xff;

		for (n = rows * cols, p = pix->data; n-- > 0; p += 3)
			p[0] = r, p[1] = g, p[2] = b;
	}

	return pix;
}


/*	Datentype
*/

Type_t Type_Pixmap = REF_TYPE("Pixmap", Pixmap_t *);


static void PF_create (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = NewPixmap(Val_int(arg[1]), Val_int(arg[0]),
		Val_int(arg[2]));
}

static void PF_load (Func_t *func, void *rval, void **arg)
{
	io_t *io = io_fileopen(Val_str(arg[0]), "rz");
	Val_ptr(rval) = LoadPixmap(io);
	io_close(io);
}

static void PF_save (Func_t *func, void *rval, void **arg)
{
	io_t *io = io_fileopen(Val_str(arg[1]), "wz");
	SavePixmap(Val_ptr(arg[0]), io);
	io_close(io);
	Val_ptr(rval) = rd_refer(Val_ptr(arg[0]));
}

static void PF_vadjust (Func_t *func, void *rval, void **arg)
{
	Pixmap_vadjust(Val_ptr(arg[0]), Val_int(arg[1]), Val_int(arg[2]));
	Val_ptr(rval) = rd_refer(Val_ptr(arg[0]));
}

static FuncDef_t fdef[] = {
	{ 0, &Type_Pixmap, "Pixmap (int width, int height, int color = 0)",
		PF_create },
	{ 0, &Type_Pixmap, "LoadPixmap (str name)", PF_load },
	{ 0, &Type_Pixmap, "Pixmap::save (str name)", PF_save },
	{ 0, &Type_Pixmap, "Pixmap::vadjust (int color, int offset)", PF_vadjust },
};


static int *m_cols (Pixmap_t **pix)
{
	Buf_int = (*pix != NULL) ? (*pix)->cols : 0;
	return &Buf_int;
}

static int *m_rows (Pixmap_t **pix)
{
	Buf_int = (*pix != NULL) ? (*pix)->rows : 0;
	return &Buf_int;
}

static MemberDef_t var_Pixmap[] = {
	{ "cols", &Type_int, ConstMember, m_cols },
	{ "rows", &Type_int, ConstMember, m_rows },
};


void SetupPixmap (void)
{
	AddType(&Type_Pixmap);
	AddFuncDef(fdef, tabsize(fdef));
	AddMember(Type_Pixmap.vtab, var_Pixmap, tabsize(var_Pixmap));
}
