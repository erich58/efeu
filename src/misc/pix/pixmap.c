/*
Pixelfile generieren

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/oldpixmap.h>
#include <EFEU/parsedef.h>
#include <EFEU/stdtype.h>

/*	Referenztype
*/

static void pix_clean (void *data)
{
	OldPixMap *pix = data;
	memfree(pix->color);
	memfree(pix->pixel);
	memfree(pix);
}

static char *pix_ident (const void *data)
{
	const OldPixMap *pix = data;
	return msprintf("%dx%dx%d", pix->rows, pix->cols, pix->colors);
}

RefType OldPixMap_reftype = REFTYPE_INIT("OldPixMap", pix_ident, pix_clean);


/*	OldPixMap-File generieren
*/

OldPixMap *new_OldPixMap(int rows, int cols)
{
	OldPixMap *pix = memalloc(sizeof(OldPixMap));
	pix->rows = rows;
	pix->cols = cols;
	pix->colors = 0;
	pix->pixel = memalloc(rows * cols);
	memset(pix->color, 0, PIX_CDIM * sizeof(COLOR));
	memset(pix->pixel, 0, rows * cols);
	return rd_init(&OldPixMap_reftype, pix);
}


/*	Datentype
*/

EfiType Type_OldPixMap = REF_TYPE("OldPixMap", OldPixMap *);
EfiType Type_Color = SIMPLE_TYPE("Color", COLOR, NULL, NULL);

COLOR Buf_Color = { 0 };


#define	REF(ptr)	rd_refer(Val_OldPixMap(ptr))
#define	RVPIX		Val_OldPixMap(rval)


/*	Basisfunktionen
*/

static void PF_rule (EfiFunc *func, void *rval, void **arg)
{
	OldPixMap *pix;
	int x0, y0, width, height;
	int color, i, j, n;

	pix = Val_OldPixMap(arg[0]);

	if	(pix == NULL)
	{
		RVPIX = pix;
		return;
	}

	refer_OldPixMap(pix);
	x0 = Val_int(arg[1]);
	y0 = Val_int(arg[2]);
	width = Val_int(arg[3]);
	height = Val_int(arg[4]);
	color = Val_int(arg[5]);

	if	(x0 >= pix->cols || y0 >= pix->rows)
	{
		RVPIX = pix;
		return;
	}

	if	(x0 < 0)	x0 = 0;
	if	(y0 < 0)	y0 = 0;

	y0 = pix->rows - y0;

	if	(width == 0)	width = pix->cols - x0;
	if	(height == 0)	height = pix->rows - y0;

	if	(color >= pix->colors)
		color = pix->colors - 1;

	n = min(pix->cols, x0 + width);

	for (i = max(0, y0 - height); i < y0; i++)
		for (j = x0; j < n; j++)
			pix->pixel[i * pix->cols + j] = color;

	RVPIX = pix;
}

static void PF_cprint (EfiFunc *func, void *rval, void **arg)
{
	register COLOR color = Val_Color(arg[1]);
	Val_int(rval) = io_xprintf(Val_io(arg[0]), "{ %5.3f, %5.3f, %5.3f }",
		color.red / 255., color.green / 255., color.blue / 255.);
}


static void PF_load(EfiFunc *func, void *rval, void **arg)
{
	RVPIX = load_OldPixMap(Val_str(arg[0]));
}

static void PF_create(EfiFunc *func, void *rval, void **arg)
{
	RVPIX = new_OldPixMap(Val_int(arg[1]), Val_int(arg[0]));
}

static void PF_save(EfiFunc *func, void *rval, void **arg)
{
	save_OldPixMap(Val_OldPixMap(arg[0]), Val_str(arg[1]));
	RVPIX = REF(arg[0]);
}

static void PF_toPPM(EfiFunc *func, void *rval, void **arg)
{
	OldPixMapToPPM(Val_OldPixMap(arg[0]), Val_str(arg[1]));
	RVPIX = REF(arg[0]);
}

static void PF_toXPM2(EfiFunc *func, void *rval, void **arg)
{
	OldPixMapToXPM2(Val_OldPixMap(arg[0]), Val_str(arg[1]));
	RVPIX = REF(arg[0]);
}

#define	RED(x)		(Val_Color(x).red / 255.)
#define	GREEN(x)	(Val_Color(x).green / 255.)
#define	BLUE(x)		(Val_Color(x).blue / 255.)

static void PF_skcolor(EfiFunc *func, void *rval, void **arg)
{
	Val_Color(rval) = SetColor(Val_double(arg[0]) * RED(arg[1]),
		Val_double(arg[0]) * GREEN(arg[1]),
		Val_double(arg[0]) * BLUE(arg[1]));
}

static void PF_addcolor(EfiFunc *func, void *rval, void **arg)
{
	Val_Color(rval) = SetColor(RED(arg[0]) + RED(arg[1]),
		GREEN(arg[0]) + GREEN(arg[1]),
		BLUE(arg[0]) + BLUE(arg[1]));
}

static void PF_subcolor(EfiFunc *func, void *rval, void **arg)
{
	Val_Color(rval) = SetColor(RED(arg[0]) - RED(arg[1]),
		GREEN(arg[0]) - GREEN(arg[1]),
		BLUE(arg[0]) - BLUE(arg[1]));
}

static void PF_getcolor(EfiFunc *func, void *rval, void **arg)
{
	OldPixMap *pix;

	if	((pix = Val_OldPixMap(arg[0])) != NULL)
	{
		Val_Color(rval) = pix->color[GetColor(Val_Color(arg[1]), pix->color, pix->colors)];
	}
	else	Val_Color(rval) = SetColor(0., 0., 0.);
}

static void PF_newcolor(EfiFunc *func, void *rval, void **arg)
{
	OldPixMap *pix;

	pix = Val_OldPixMap(arg[0]);

	if	(pix && pix->colors < PIX_CDIM)
	{
		pix->color[pix->colors++] = Val_Color(arg[1]);
	}

	RVPIX = REF(arg[0]);
}


static EfiFuncDef fdef[] = {
	{ 0, &Type_OldPixMap, "LoadOldPixMap (str file)", PF_load },
	{ 0, &Type_OldPixMap, "NewOldPixMap (int width, int height)", PF_create },
	{ 0, &Type_OldPixMap, "OldPixMap::save(str file)", PF_save },
	{ 0, &Type_OldPixMap, "OldPixMap::PPMsave(str file = NULL)", PF_toPPM },
	{ 0, &Type_OldPixMap, "OldPixMap::XPM2save(str file = NULL)", PF_toXPM2 },
	{ 0, &Type_void, "OldPixMap::Histogramm(IO io = iostd)", OldPixMap_Histogramm },
	{ 0, &Type_void, "OldPixMap::Picture(IO = iostd, int = 0)", OldPixMap_TeX },
	{ 0, &Type_OldPixMap, "OldPixMap::rule(int x = 0, int y = 0, int width = 0, \
int height = 0, int color = 0)", PF_rule },
	{ 0, &Type_OldPixMap, "Slice(OldPixMap, ...)", OldPixMap_Slice },
	{ 0, &Type_OldPixMap, "Dither(OldPixMap)", OldPixMap_Dither },
	{ 0, &Type_Color, "OldPixMap::get(Color)", PF_getcolor },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Color)", PF_cprint },
	{ FUNC_VIRTUAL, &Type_OldPixMap, "operator+= (OldPixMap &, Color)", PF_newcolor },
	{ FUNC_VIRTUAL, &Type_Color, "operator* (double, Color)", PF_skcolor },
	{ FUNC_VIRTUAL, &Type_Color, "operator+ (Color, Color)", PF_addcolor },
	{ FUNC_VIRTUAL, &Type_Color, "operator- (Color, Color)", PF_subcolor },
};


#if	0
/*	Konvertierungen
*/

static void double2Color (const EfiKonv *p, COLOR *y, double *x)
{
	y->red = ColorValue(*x);
	y->green = y->blue = y->red;
}

static void Color2double (const EfiKonv *p, double *y, COLOR *x)
{
	*y = 0.299 * RED(x) + 0.587 * GREEN(x) + 0.114 * BLUE(x);
}


static void List2Color (const EfiKonv *p, COLOR *y, EfiObjList **x)
{
	y->red = 0;
	y->green = 0;
	y->blue = 0;
	
	if	((*x))
	{
		y->red = ColorValue(Obj2double(RefObj((*x)->obj)));

		if	((*x)->next)
		{
			y->green = ColorValue(Obj2double(RefObj((*x)->next->obj)));

			if	((*x)->next->next)
				y->blue = ColorValue(Obj2double(RefObj((*x)->next->next->obj)));
		}
	}
}

static void Color2List (const EfiKonv *p, EfiObjList **y, COLOR *x)
{
	(*y) = NewObjList(Obj_double(x->red / 255.));
	(*y)->next = NewObjList(Obj_double(x->green / 255.));
	(*y)->next->next = NewObjList(Obj_double(x->blue / 255.));
}
#endif

/*
static EfiKonv konv[] = {
	{ &Type_null, &Type_OldPixMap, Konv_Goodkonv, Null2Any },
	{ &Type_Color, &Type_double, Konv_Badkonv, (KonvEfiFunc) Color2double },
	{ &Type_double, &Type_Color, Konv_Goodkonv, (KonvEfiFunc) double2Color },
	{ &Type_list, &Type_Color, Konv_Goodkonv, (KonvEfiFunc) List2Color },
	{ &Type_Color, &Type_list, Konv_Goodkonv, (KonvEfiFunc) Color2List },
};
*/


/*	Konstanten
*/

static EfiObj *Const_Color (IO *io, void *data)
{
	unsigned val = (size_t) data;
	Buf_Color.idx = 0;
	Buf_Color.red = (val & 0xFF0000) >> 16;
	Buf_Color.green = (val & 0x00FF00) >> 8;
	Buf_Color.blue = (val & 0x0000FF);
	return ConstObj(&Type_Color, &Buf_Color);
}

static EfiParseDef pdef[] = {
	{ "White", Const_Color, (char *) 0xFFFFFF },
	{ "Black", Const_Color, (char *) 0x000000 },
	{ "Red", Const_Color, (char *) 0xFF0000 },
	{ "Green", Const_Color, (char *) 0x00FF00 },
	{ "Blue", Const_Color, (char *) 0x0000FF },
	{ "Cyan", Const_Color, (char *) 0x00FFFF },
	{ "Magenta", Const_Color, (char *) 0xFF00FF },
	{ "Yellow", Const_Color, (char *) 0xFFFF00 },
};

static EfiObj *get_color (const EfiObj *obj, void *data)
{
	OldPixMap *pix = obj ? Val_OldPixMap(obj->data) : NULL;
	return pix ? EfiVecObj(&Type_Color, pix->color, pix->colors) : NULL;
}

static EfiObj *m_cols (const EfiObj *obj, void *data)
{
	int x = obj ? Val_OldPixMap(obj->data)->cols : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *m_rows (const EfiObj *obj, void *data)
{
	int x = obj ? Val_OldPixMap(obj->data)->rows : 0;
	return NewObj(&Type_int, &x);
}

static EfiMember member[] = {
	{ "color", &Type_vec, get_color, NULL },
	{ "width", &Type_int, m_cols, NULL },
	{ "height", &Type_int, m_rows, NULL },
};


void SetupOldPixMap(void)
{
	AddType(&Type_Color);
	AddType(&Type_OldPixMap);
	AddFuncDef(fdef, tabsize(fdef));
	AddParseDef(pdef, tabsize(pdef));
	AddEfiMember(Type_OldPixMap.vtab, member, tabsize(member));
	/*
	AddParseDef(konv, tabsize(konv));
	*/
}
