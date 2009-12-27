/*
Hilfsprogramme für Spreadsheed-Conector

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <EFEU/stdtype.h>
#include <ctype.h>

#define	SC_CMDSIZE	31	/* Bufferlänge für SC-Befehle (ohne Zuweisungswert) */

#define	SC_TYPE		0xF	/* Maske für Datentype */
#define	SC_EMPTY	0x0	/* Leeres Feld */
#define	SC_DATA		0x1	/* Datenfeld */
#define	SC_EXPR		0x2	/* Zuweisungsausdruck */
#define	SC_LABEL	0x3	/* Zentrierter Label */

#define	SC_LEFT		0x10	/* Linksbündiger Text */
#define	SC_RIGHT	0x20	/* Rechtsbündiger Text */

typedef struct {
	int type;	/* Datentype */
	int align;	/* Ausrichtung */
	char *label;	/* Label */
	double value;	/* Wert */
} SCData_t;

typedef struct {
	REFVAR;		/* Referenzzähler */
	int x;		/* X - Position */
	int y;		/* Y - Position */
	int rows;	/* Zahl der Zeilen */
	int cols;	/* Zahl der Spalten */
	int *width;	/* Spaltenbreiten */
	int *prec;	/* Genauigkeit */
	SCData_t *data;	/* Datenvektor */
} SC_t;


/*	SC - Datenstruktur
*/

static char *sc_ident(const SC_t *sc)
{
	return sc ? msprintf("%dx%d", sc->rows, sc->cols) : NULL;
}

static SC_t *sc_admin(SC_t *tg, const SC_t *src)
{
	if	(tg)
	{
		int n = tg->rows * tg->cols;

		while (n-- > 0)
			memfree(tg->data[n].label);

		memfree(tg->width);
		memfree(tg->data);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(SC_t));
}

ADMINREFTYPE(sc_reftype, "SC", sc_ident, sc_admin);

static SC_t *sc_create(int row, int col)
{
	SC_t *sc;
	int i;

	sc = rd_create(&sc_reftype);
	sc->rows = row;
	sc->cols = col;
	sc->width = memalloc(2 * col * sizeof(int));
	sc->prec = sc->width + col;
	sc->data = memalloc(row * col * sizeof(SCData_t));
	memset(sc->data, 0, row * col * sizeof(SCData_t));

	for (i = 0; i < col; i++)
	{
		sc->width[i] = 10;
		sc->prec[i] = 2;
	}

	return sc;
}


/*	SC - Datentype
*/

static Type_t Type_SC = REF_TYPE("SC", SC_t *);

#define	Val_SC(x)	((SC_t **) x)[0]
#define	Ref_SC(x)	rd_refer(Val_SC(x))


/*	Konvertierung der Spaltenbezeichner
*/

static char *label = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *makecol (int col)
{
	static char buf[4];

	memset(buf, 0, 4);

	if	(col >= 26)
	{
		buf[0] = label[(col / 26 - 1) % 26];
		buf[1] = label[col % 26];
	}
	else	buf[0] = label[col];

	return buf;
}

static int getcol (char *buf, char **ptr)
{
	int col;
	char *p;

	if	(buf == NULL)	return 0;

	while (isspace(*buf))
		buf++;

	col = 0;

	if	((p = strchr(label, *buf)))
	{
		col = p - label;
		buf++;

		if	((p = strchr(label, *buf)))
		{
			col = 26 * (col + 1) + p - label;
			buf++;
		}
	}

	if	(ptr)	*ptr = buf;

	return col;
}


/*	Spreadsheet - Daten ausgeben
*/

static void save_entry(io_t *io, SCData_t *x, int row, int col)
{
	char *cs;
	char *align;

	cs = makecol(col);

	if	(x->align < 0)	align = "leftstring";
	else if	(x->align > 0)	align = "rightstring";
	else			align = "label";

	switch (x->type)
	{
	case SC_LABEL:
		io_printf(io, "%s %s%d = %#s\n", align, cs, row, x->label);
		break;
	case SC_EXPR:
		io_printf(io, "let %s%d = %s\n", cs, row, x->label);
		break;
	case SC_DATA:
		io_printf(io, "let %s%d = %.2f\n", cs, row, x->value);
		break;
	default:
		break;
	}
}

static void f_sc_save(Func_t *func, void *rval, void **arg)
{
	io_t *io;
	SC_t *sc;
	int i, j;

	sc = Ref_SC(arg[0]);
	io = Val_io(arg[1]);
	Val_SC(rval) = sc;

	if	(sc == NULL || io == NULL)	return;

	for (j = 0; j < sc->cols; j++)
		io_printf(io, "format %s %d %d 0\n",
			makecol(j), sc->width[j], sc->prec[j]);

	for (i = 0; i < sc->rows; i++)
		for (j = 0; j < sc->cols; j++)
			save_entry(io, sc->data + i * sc->cols + j, i, j);
}


/*	Zuweisungstype
*/

static int line_type(char *buf, char **p)
{
	if	(patcmp("leftstring", buf, p))	return SC_LABEL|SC_LEFT;
	if	(patcmp("rightstring", buf, p))	return SC_LABEL|SC_RIGHT;
	if	(patcmp("label", buf, p))	return SC_LABEL;
	if	(patcmp("let", buf, p))		return SC_DATA;

	*p = NULL;
	return 0;
}


/*	Eintrag laden
*/

static void loadentry(io_t *io, int type, SCData_t *entry)
{
	void *p;

	entry->type = type & SC_TYPE;

	if	(type == SC_DATA)
	{
		if	(io_scan(io, SCAN_DOUBLE, &p))
			entry->value = * ((double *) p);
	}
	else if	(io_scan(io, SCAN_STR, &p))
	{
		entry->label = p;
	}

	if	(type & SC_LEFT)	entry->align = -1;
	else if	(type & SC_RIGHT)	entry->align = 1;
	else				entry->align = 0;
}


/*	Voranalyse des SC - Files mit umkopieren und bestimmen der
	Spalten und Zeilenzahl. Zur Weiterverarbeitung der Kopie
	wird der Pointer auf das erste Element positioniert. 
*/

void preview(io_t *io, char *tname, int *rows, int *cols);

void preview(io_t *io, char *tname, int *rows, int *cols)
{
	io_t *save;
	char buf[SC_CMDSIZE + 1];
	int n, c;
	char *p;
	char *t2;

	t2 = mstrcpy(tmpnam(NULL));
	save = io_fileopen(t2, "w");
	n = 0;

	while ((c = io_getc(io)) != EOF)
	{
		io_putc(c, save);

		if	(c == '\n')
		{
			buf[n] = 0;
			n = 0;

			if	(line_type(buf, &p))
			{
				if (*cols <= (c = getcol(p, &p)))	*cols = c + 1;
				if (*rows <= (c = strtol(p, &p, 10)))	*rows = c + 1;
			}
		}
		else if	(n < SC_CMDSIZE)	buf[n++] = c;
	}

	io_puts("goto A0\n", save);
	io_close(save);

/*	Werte Konstant setzen und unter neuen Namen sichern
*/
	p = msprintf("|sc %s > /dev/null", t2);
	save = io_fileopen(p, "w");
	io_printf(save, "%dvcP%s\nq", *cols, tname);
	io_close(save);
	remove(t2);
	memfree(p);
}


/*	Spreadsheet laden
*/

static void f_sc_load(Func_t *func, void *rval, void **arg)
{
	SC_t *sc;
	io_t *io;
	int row, col;
	char *tname, *p;
	char buf[SC_CMDSIZE + 1];
	int n, c;

	io = Val_io(arg[0]);
	row = col = 0;
	tname = mstrcpy(tmpnam(NULL));
	preview(io, tname, &row, &col);
	io = io_fileopen(tname, "r");

	sc = sc_create(row, col);
	n = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			n = 0;
		}
		else if	(c == '=')
		{
			buf[n] = 0;
			n = 0;

			if	((c = line_type(buf, &p)))
			{
				col = getcol(p, &p);
				row = strtol(p, &p, 10);

				if	(col < sc->cols && row < sc->rows)
					loadentry(io, c, sc->data + row * sc->cols + col);
			}
		}
		else if	(n < SC_CMDSIZE)	buf[n++] = c;
	}

	io_close(io);
	remove(tname);
	memfree(tname);
	Val_SC(rval) = sc;
}

static void f_sc_create(Func_t *func, void *rval, void **arg)
{
	Val_SC(rval) = sc_create(Val_int(arg[0]), Val_int(arg[1]));
}


static char *sc_index(int row, int col)
{
	char *rs, *cs;

	rs = cs = "";

	if	(row < 0)	rs = "$", row = -row;
	else if	(row == 0)	row = 1;

	if	(col < 0)	cs = "$", col = -col;
	else if	(col == 0)	col = 1;

	return msprintf("%s%s%s%d", cs, makecol(col - 1), rs, row - 1);
}

static void f_sc_index(Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = sc_index(Val_int(arg[0]), Val_int(arg[1]));
}

static void f_sc_range(Func_t *func, void *rval, void **arg)
{
	char *name, *r1, *r2;

	name = Val_str(arg[0]);
	r1 = sc_index(Val_int(arg[1]), Val_int(arg[2]));
	r2 = sc_index(Val_int(arg[3]) ? Val_int(arg[3]) : Val_int(arg[1]),
		Val_int(arg[4]) ? Val_int(arg[4]) : Val_int(arg[2]));
	Val_str(rval) = name ? msprintf("%s(%s:%s)", name, r1, r2)
		: mstrcat(":", r1, r2, NULL);
	memfree(r1);
	memfree(r2);
}


static void f_sc_format(Func_t *func, void *rval, void **arg)
{
	int col;
	SC_t *sc;

	sc = Val_SC(arg[0]);
	col = Val_int(arg[1]) - 1;

	if	(sc && col >= 0 && col < sc->cols)
	{
		sc->width[col] = Val_int(arg[2]);
		sc->prec[col] = Val_int(arg[3]);
	}
}

static SCData_t *sc_data(void **arg)
{
	int r, c;
	SC_t *sc;

	sc = Val_SC(arg[0]);

	if	(sc == NULL)	return NULL;

	r = Val_int(arg[1]) - 1;
	c = Val_int(arg[2]) - 1;

	if	(r < 0 || r >= sc->rows || c < 0 || c >= sc->cols)
		return NULL;

	return sc->data + r * sc->cols + c;
}

static void f_sc_get(Func_t *func, void *rval, void **arg)
{
	SCData_t *data;

	if	((data = sc_data(arg)) != NULL)
	{
		switch (data->type)
		{
		case SC_EXPR:
		case SC_LABEL:
			Val_obj(rval) = LvalObj(&Lval_ref, &Type_str,
				Val_SC(arg[0]), &data->label);
			break;
		case SC_DATA:
			Val_obj(rval) = LvalObj(&Lval_ref, &Type_double,
				Val_SC(arg[0]), &data->value);
			break;
		default:
			Val_obj(rval) = ptr2Obj(NULL);
			break;
		}
	}
}

static void f_sc_data(Func_t *func, void *rval, void **arg)
{
	SCData_t *data;

	if	((data = sc_data(arg)) != NULL)
	{
		data->type = SC_DATA;
		data->align = 0;
		Val_obj(rval) = LvalObj(&Lval_ref, &Type_double,
			Val_SC(arg[0]), &data->value);
	}
	else	Val_obj(rval) = ptr2Obj(NULL);
}

static void f_sc_label(Func_t *func, void *rval, void **arg)
{
	SCData_t *data;

	if	((data = sc_data(arg)) != NULL)
	{
		data->type = SC_LABEL;
		data->align = Val_int(arg[3]);
		Val_obj(rval) = LvalObj(&Lval_ref, &Type_str,
			Val_SC(arg[0]), &data->label);
	}
	else	Val_obj(rval) = ptr2Obj(NULL);
}

static void f_sc_expr(Func_t *func, void *rval, void **arg)
{
	SCData_t *data;

	if	((data = sc_data(arg)) != NULL)
	{
		data->type = SC_EXPR;
		data->align = 0;
		Val_obj(rval) = LvalObj(&Lval_ref, &Type_str,
			Val_SC(arg[0]), &data->label);
	}
	else	Val_obj(rval) = ptr2Obj(NULL);
}

static void f_sc_type(Func_t *func, void *rval, void **arg)
{
	SCData_t *data;

	data = sc_data(arg);
	Val_int(rval) = data ? data->type : 0;
}

/*	Memberfunktionen
*/

static int buf_int;
static int *m_rows(SC_t **ptr)
{
	buf_int = *ptr ? (*ptr)->rows : 0;
	return &buf_int;
}

static int *m_cols(SC_t **ptr)
{
	buf_int = *ptr ? (*ptr)->cols : 0;
	return &buf_int;
}

static int *m_x(SC_t **ptr)
{
	return *ptr ? &(*ptr)->x : NULL;
}

static int *m_y(SC_t **ptr)
{
	return *ptr ? &(*ptr)->y : NULL;
}

static MemberDef_t var_sc[] = {
	{ "rows", &Type_int, ConstMember, m_rows },
	{ "cols", &Type_int, ConstMember, m_cols },
	{ "x", &Type_int, LvalMember, m_x },
	{ "y", &Type_int, LvalMember, m_y },
};


/*	Initialisieren
*/

static VarDef_t sc_var[] = {
	{ "SC_Debug", &Type_bool, &sc_reftype.debug,
		":*:flag to control debuging of SC-structures\n"
		":de:Flag zum Debuggen von SC-Strukturen\n" },
};

static ParseDef_t sc_pdef[] = {
	{ "SC_DATA", PFunc_int, (void *) SC_DATA },
	{ "SC_EXPR", PFunc_int, (void *) SC_EXPR },
	{ "SC_LABEL", PFunc_int, (void *) SC_LABEL },
};

static FuncDef_t sc_func[] = {
	{ 0, &Type_SC, "SC (int rows = 1024, int cols = 32)", f_sc_create },
	{ 0, &Type_SC, "SC (IO io)", f_sc_load },
	{ 0, &Type_SC, "SC::save (IO io)", f_sc_save },
	{ 0, &Type_obj, "SC::get(int row, int col)", f_sc_get },
	{ 0, &Type_obj, "SC::data(int row, int col)", f_sc_data },
	{ 0, &Type_obj, "SC::expr(int row, int col)", f_sc_expr },
	{ 0, &Type_obj, "SC::label(int row, int col, int align = 0)",
		f_sc_label },
	{ 0, &Type_int, "SC::type(int row, int col)", f_sc_type },
	{ 0, &Type_void, "SC::format(int col, int width = 10, int prec = 2)",
		f_sc_format },
	{ 0, &Type_str, "SCIndex(int row, int col)", f_sc_index },
	{ 0, &Type_str, "SCRange(str cmd, int y0, int x0, \
int y1 = 0, int x1 = 0)", f_sc_range },
};


void SetupSC(void);

void SetupSC(void)
{
	static int need_init = 1;

	if	(need_init)
	{
		AddParseDef(sc_pdef, tabsize(sc_pdef));
		AddType(&Type_SC);
		AddFuncDef(sc_func, tabsize(sc_func));
		AddVarDef(NULL, sc_var, tabsize(sc_var));
		AddMember(Type_SC.vtab, var_sc, tabsize(var_sc));
		need_init = 0;
	}
}
