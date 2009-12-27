/*
Hilfsprogramme

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

#include <EFEU/pconfig.h>
#include <EFEU/efutil.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/datum.h>
#include <EFEU/calendar.h>
#include <EFEU/patcmp.h>
#include <time.h>

#define	RVINT	Val_int(rval)
#define	RVSTR	Val_str(rval)

#define	STR(n)		Val_str(arg[n])
#define	INT(n)		Val_int(arg[n])
#define	LONG(n)		Val_long(arg[n])
#define	DOUBLE(n)	Val_double(arg[n])


/*	Zifferndarstellung zur Basis 37
*/

CEXPR(f_a37l, RVINT = a37l(STR(0)))
CEXPR(f_l37a, RVSTR = l37a(INT(0)))


/*	Vergleichsfunktionen
*/

static void cmp_str (Func_t *func, void *rval, void **arg)
{
	RVINT = mstrcmp(STR(0), STR(1));
}

static void cmp_lex (Func_t *func, void *rval, void **arg)
{
	RVINT = lexcmp(STR(0), STR(1));
}

static void cmp_int (Func_t *func, void *rval, void **arg)
{
	if	(INT(0) < INT(1))	RVINT = -1;
	else if	(INT(0) > INT(1))	RVINT = 1;
	else				RVINT = 0;
}

static void cmp_long (Func_t *func, void *rval, void **arg)
{
	if	(LONG(0) < LONG(1))	RVINT = -1;
	else if	(LONG(0) > LONG(1))	RVINT = 1;
	else				RVINT = 0;
}

static void cmp_double (Func_t *func, void *rval, void **arg)
{
	if	(DOUBLE(0) < DOUBLE(1))	RVINT = -1;
	else if	(DOUBLE(0) > DOUBLE(1))	RVINT = 1;
	else				RVINT = 0;
}

static void rcmp_str (Func_t *func, void *rval, void **arg)
{
	RVINT = mstrcmp(STR(1), STR(0));
}

static void rcmp_int (Func_t *func, void *rval, void **arg)
{
	if	(INT(1) < INT(0))	RVINT = -1;
	else if	(INT(1) > INT(0))	RVINT = 1;
	else				RVINT = 0;
}

static void rcmp_long (Func_t *func, void *rval, void **arg)
{
	if	(LONG(1) < LONG(0))	RVINT = -1;
	else if	(LONG(1) > LONG(0))	RVINT = 1;
	else				RVINT = 0;
}

static void rcmp_double (Func_t *func, void *rval, void **arg)
{
	if	(DOUBLE(1) < DOUBLE(0))	RVINT = -1;
	else if	(DOUBLE(1) > DOUBLE(0))	RVINT = 1;
	else				RVINT = 0;
}

static void f_inquire(Func_t *func, void *rval, void **arg)
{
	char *p;

	io_puts(Val_str(arg[0]), iostd);
	p = io_mgets(iostd, "\n");
	io_getc(iostd);
	RVSTR = p ? p : mstrcpy(Val_str(arg[1]));
}

static void f_texquote(Func_t *func, void *rval, void **arg)
{
	RVINT = TeXquote(STR(0), Val_io(arg[1]));
}

static void f_texputs(Func_t *func, void *rval, void **arg)
{
	RVINT = TeXputs(STR(0), Val_io(arg[1]));
}

static void f_texputc(Func_t *func, void *rval, void **arg)
{
	RVINT = TeXputc(Val_char(arg[0]), Val_io(arg[1]));
}


/*	Funktionstabelle
*/

static FuncDef_t fdef[] = {
	{ 0, &Type_int, "a37l (str)", f_a37l },
	{ 0, &Type_str, "l37a (int)", f_l37a },

	{ FUNC_VIRTUAL, &Type_int, "lexcmp (str, str)", cmp_lex },
	{ FUNC_VIRTUAL, &Type_int, "cmp (str, str)", cmp_str },
	{ FUNC_VIRTUAL, &Type_int, "cmp (int, int)", cmp_int },
	{ FUNC_VIRTUAL, &Type_int, "cmp (long, long)", cmp_long },
	{ FUNC_VIRTUAL, &Type_int, "cmp (double, double)", cmp_double },
	{ FUNC_VIRTUAL, &Type_int, "rcmp (str, str)", rcmp_str },
	{ FUNC_VIRTUAL, &Type_int, "rcmp (int, int)", rcmp_int },
	{ FUNC_VIRTUAL, &Type_int, "rcmp (long, long)", rcmp_long },
	{ FUNC_VIRTUAL, &Type_int, "rcmp (double, double)", rcmp_double },

	{ 0, &Type_str, "inquire (str p = \">>>\", str def = NULL)",
		f_inquire },

	{ FUNC_VIRTUAL, &Type_int, "TeXput (char c, IO io = iostd)", f_texputc },
	{ FUNC_VIRTUAL, &Type_int, "TeXput (str s, IO io = iostd)", f_texputs },
	{ 0, &Type_int, "TeXquote (str s, IO io = iostd)", f_texquote },
};


/*	Initialisieren
*/

void SetupUtil(void)
{
	AddFuncDef(fdef, tabsize(fdef));
}
