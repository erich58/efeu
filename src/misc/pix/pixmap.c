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

static OldPixMap_t *OldPixMapAdmin(OldPixMap_t *tg, const OldPixMap_t *src)
{
	if	(tg)
	{
		memfree(tg->color);
		memfree(tg->pixel);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(OldPixMap_t));
}

static char *OldPixMapIdent(OldPixMap_t *pix)
{
	return msprintf("%dx%dx%d", pix->rows, pix->cols, pix->colors);
}

ADMINREFTYPE(OldPixMap_reftype, "OldPixMap", OldPixMapIdent, OldPixMapAdmin);


/*	OldPixMap-File generieren
*/

OldPixMap_t *new_OldPixMap(int rows, int cols)
{
	OldPixMap_t *pix;

	pix = rd_create(&OldPixMap_reftype);
	pix->rows = rows;
	pix->cols = cols;
	pix->colors = 0;
	pix->pixel = memalloc(rows * cols);
	memset(pix->color, 0, PIX_CDIM * sizeof(Color_t));
	memset(pix->pixel, 0, rows * cols);
	return pix;
}


/*	Datentype
*/

Type_t Type_OldPixMap = REF_TYPE("OldPixMap", OldPixMap_t *);
Type_t Type_Color = SIMPLE_TYPE("Color", Color_t, NULL);

Color_t Buf_Color = { 0 };


#define	REF(ptr)	rd_refer(Val_OldPixMap(ptr))
#define	RVPIX		Val_OldPixMap(rval)


/*	Variablen
*/

static VarDef_t vardef[] = {
	{ "pixdebug", &Type_bool, &OldPixMap_reftype.debug },
};


/*	Basisfunktionen
*/

static void PF_rule (Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix;
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
		Val_OldPixMap(rval) = arg[0];
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

	RVPIX = arg[0];
}

static void PF_cprint (Func_t *func, void *rval, void **arg)
{
	register Color_t color = Val_Color(arg[1]);
	Val_int(rval) = io_printf(Val_io(arg[0]), "{ %5.3f, %5.3f, %5.3f }",
		color.red / 255., color.green / 255., color.blue / 255.);
}


static void PF_load(Func_t *func, void *rval, void **arg)
{
	RVPIX = load_OldPixMap(Val_str(arg[0]));
}

static void PF_create(Func_t *func, void *rval, void **arg)
{
	RVPIX = new_OldPixMap(Val_int(arg[1]), Val_int(arg[0]));
}

static void PF_save(Func_t *func, void *rval, void **arg)
{
	save_OldPixMap(Val_OldPixMap(arg[0]), Val_str(arg[1]));
	RVPIX = REF(arg[0]);
}

static void PF_toPPM(Func_t *func, void *rval, void **arg)
{
	OldPixMapToPPM(Val_OldPixMap(arg[0]), Val_str(arg[1]));
	RVPIX = REF(arg[0]);
}

static void PF_toXPM2(Func_t *func, void *rval, void **arg)
{
	OldPixMapToXPM2(Val_OldPixMap(arg[0]), Val_str(arg[1]));
	RVPIX = REF(arg[0]);
}

#define	RED(x)		(Val_Color(x).red / 255.)
#define	GREEN(x)	(Val_Color(x).green / 255.)
#define	BLUE(x)		(Val_Color(x).blue / 255.)

static void PF_skcolor(Func_t *func, void *rval, void **arg)
{
	Val_Color(rval) = SetColor(Val_double(arg[0]) * RED(arg[1]),
		Val_double(arg[0]) * GREEN(arg[1]),
		Val_double(arg[0]) * BLUE(arg[1]));
}

static void PF_addcolor(Func_t *func, void *rval, void **arg)
{
	Val_Color(rval) = SetColor(RED(arg[0]) + RED(arg[1]),
		GREEN(arg[0]) + GREEN(arg[1]),
		BLUE(arg[0]) + BLUE(arg[1]));
}

static void PF_subcolor(Func_t *func, void *rval, void **arg)
{
	Val_Color(rval) = SetColor(RED(arg[0]) - RED(arg[1]),
		GREEN(arg[0]) - GREEN(arg[1]),
		BLUE(arg[0]) - BLUE(arg[1]));
}

static void PF_getcolor(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix;

	if	((pix = Val_OldPixMap(arg[0])) != NULL)
	{
		Val_Color(rval) = pix->color[GetColor(Val_Color(arg[1]), pix->color, pix->colors)];
	}
	else	Val_Color(rval) = SetColor(0., 0., 0.);
}

static void PF_newcolor(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix;

	pix = Val_OldPixMap(arg[0]);

	if	(pix && pix->colors < PIX_CDIM)
	{
		pix->color[pix->colors++] = Val_Color(arg[1]);
	}

	RVPIX = REF(arg[0]);
}


static FuncDef_t fdef[] = {
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

static void double2Color (const Konv_t *p, Color_t *y, double *x)
{
	y->red = ColorValue(*x);
	y->green = y->blue = y->red;
}

static void Color2double (const Konv_t *p, double *y, Color_t *x)
{
	*y = 0.299 * RED(x) + 0.587 * GREEN(x) + 0.114 * BLUE(x);
}


static void List2Color (const Konv_t *p, Color_t *y, ObjList_t **x)
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

static void Color2List (const Konv_t *p, ObjList_t **y, Color_t *x)
{
	(*y) = NewObjList(Obj_double(x->red / 255.));
	(*y)->next = NewObjList(Obj_double(x->green / 255.));
	(*y)->next->next = NewObjList(Obj_double(x->blue / 255.));
}
#endif

/*
static Konv_t konv[] = {
	{ &Type_null, &Type_OldPixMap, Konv_Goodkonv, Null2Any },
	{ &Type_Color, &Type_double, Konv_Badkonv, (KonvFunc_t) Color2double },
	{ &Type_double, &Type_Color, Konv_Goodkonv, (KonvFunc_t) double2Color },
	{ &Type_list, &Type_Color, Konv_Goodkonv, (KonvFunc_t) List2Color },
	{ &Type_Color, &Type_list, Konv_Goodkonv, (KonvFunc_t) Color2List },
};
*/


/*	Konstanten
*/

static Obj_t *Const_Color (io_t *io, void *data)
{
	unsigned val = (size_t) data;
	Buf_Color.idx = 0;
	Buf_Color.red = (val & 0xFF0000) >> 16;
	Buf_Color.green = (val & 0x00FF00) >> 8;
	Buf_Color.blue = (val & 0x0000FF);
	return ConstObj(&Type_Color, &Buf_Color);
}

static ParseDef_t pdef[] = {
	{ "White", Const_Color, (char *) 0xFFFFFF },
	{ "Black", Const_Color, (char *) 0x000000 },
	{ "Red", Const_Color, (char *) 0xFF0000 },
	{ "Green", Const_Color, (char *) 0x00FF00 },
	{ "Blue", Const_Color, (char *) 0x0000FF },
	{ "Cyan", Const_Color, (char *) 0x00FFFF },
	{ "Magenta", Const_Color, (char *) 0xFF00FF },
	{ "Yellow", Const_Color, (char *) 0xFFFF00 },
};

static Obj_t *get_color (const Var_t *st, const Obj_t *obj)
{
	OldPixMap_t *pix;
	
	pix = (obj ? Val_OldPixMap(obj->data) : NULL);

	if	(pix == NULL)	return NULL;

	Buf_vec.type = &Type_Color;
	Buf_vec.dim = pix->colors;
	Buf_vec.data = pix->color;
	return NewObj(&Type_vec, &Buf_vec);
}

static int *m_cols(OldPixMap_t **pix)
{
	Buf_int = (*pix != NULL) ? (*pix)->cols : 0;
	return &Buf_int;
}

static int *m_rows(OldPixMap_t **pix)
{
	Buf_int = (*pix != NULL) ? (*pix)->rows : 0;
	return &Buf_int;
}

static MemberDef_t var_OldPixMap[] = {
	{ "color", &Type_vec, get_color, NULL },
	{ "width", &Type_int, ConstMember, m_cols },
	{ "height", &Type_int, ConstMember, m_rows },
};


void SetupOldPixMap(void)
{
	AddType(&Type_Color);
	AddType(&Type_OldPixMap);
	AddFuncDef(fdef, tabsize(fdef));
	AddParseDef(pdef, tabsize(pdef));
	AddVarDef(NULL, vardef, tabsize(vardef));
	AddMember(Type_OldPixMap.vtab, var_OldPixMap, tabsize(var_OldPixMap));
	/*
	AddParseDef(konv, tabsize(konv));
	*/
}
