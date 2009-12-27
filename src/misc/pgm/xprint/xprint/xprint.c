/*	Druckeraufbereitung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/pconfig.h>
#include <ctype.h>
#include "printpar.h"
#include <EFEU/calendar.h>
#include <time.h>


/*	Programmvariablen
*/

static char *Input = NULL;	/* Eingabefile */
static char *Type = NULL;	/* Druckertype */
static char *Format = NULL;	/* Formatierungstype */
static char *Title = NULL;	/* Titel */
static char *Mode = NULL;	/* Formatierungsmodus */
static char *Range = NULL;	/* Seitenbereich */
char *PCPath = NULL;		/* Suchpfad für Druckerdefinitionen */

int TopMargin = 5;	/* Oberer Rand */
int LeftMargin = 8;	/* Linker Rand */
int PageHeight = 3;	/* Seitenhöhe */
int PageWidth = 3;	/* Seitenbreite */

int HeadHeight = 5;
char *LeftHead = "$N";
char *CenterHead = NULL;
char *RightHead = "$D";

int FootHeight = 5;
char *LeftFoot = NULL;
char *CenterFoot = "- $# -";
char *RightFoot = NULL;

int PageNumber = 1;	/* Aktuelle Seitennummer */
int FirstPage = 1;	/* Erste Seite */
int LastPage = 0xFFFF;	/* Letzte Seite */

Var_t vardef[] = {
	{ "Input",	&Type_str, &Input },
	{ "Title",	&Type_str, &Title },
	{ "Type",	&Type_str, &Type },
	{ "Range",	&Type_str, &Range },
	{ "Mode",	&Type_str, &Mode },
	{ "Format",	&Type_str, &Format },
	{ "PCPath",	&Type_str, &PCPath },

	{ "LeftMargin",	&Type_int, &LeftMargin },
	{ "TopMargin",	&Type_int, &TopMargin },
	{ "PageHeight",	&Type_int, &PageHeight },
	{ "PageWidth",	&Type_int, &PageWidth },

	{ "HeadHeight",	&Type_int, &HeadHeight },
	{ "LeftHead",	&Type_str, &LeftHead },
	{ "CenterHead",	&Type_str, &CenterHead },
	{ "RightHead",	&Type_str, &RightHead },

	{ "FootHeight",	&Type_int, &FootHeight },
	{ "LeftFoot",	&Type_str, &LeftFoot },
	{ "CenterFoot",	&Type_str, &CenterFoot },
	{ "RightFoot",	&Type_str, &RightFoot },

	/*
	{ "FirstPage",	&Type_int, &FirstPage },
	{ "LastPage",	&Type_int, &LastPage },
	*/
};


static int c_pageno(io_t *in, io_t *out, void *arg)
{
	return io_printf(out, "%d", PageNumber);
}

static char *filename = NULL;

static int c_name(io_t *in, io_t *out, void *arg)
{
	return io_puts(Title ? Title : filename, out);
}

static struct tm *tm_buf;

static int c_time(io_t *in, io_t *out, void *arg)
{
	return io_printf(out, "%d:%02d", tm_buf->tm_hour, tm_buf->tm_min);
}

static int c_date(io_t *in, io_t *out, void *arg)
{
	return PrintCalendar(out, "%e %b %Y", CalendarIndex(tm_buf->tm_mday,
		tm_buf->tm_mon + 1, 1900 + tm_buf->tm_year));
}


int main(int narg, char **arg)
{
	io_t *in, *out;
	time_t now;
	int i;
	char *p;
	void (*eval) (io_t *in, io_t *out);

	libinit(arg[0]);
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	if	(Range)
	{
		FirstPage = strtol(Range, &p, 10);

		if	(*(p++) != ':')
		{
			LastPage = FirstPage;
		}
		else if	(*p != 0)
		{
			LastPage = strtol(p, NULL, 10);
		}
		else	LastPage = 0xFFFF;
	}

	if	(mstrcmp(Mode, "ctrl") == 0)
	{
		eval = mode_control;
	}
	else	eval = mode_comment;

	time(&now);
	tm_buf = localtime(&now);
	out = pc_filter(Type);
	psubfunc('#', c_pageno, NULL);
	psubfunc('N', c_name, NULL);
	psubfunc('D', c_date, NULL);
	psubfunc('T', c_time, NULL);

	for (i = 1; i < narg; i++)
	{
		if	(i > 1)
			io_putc('\f', out);

		filename = argval(arg[i]);
		PageNumber = 1;
		in = io_fileopen(filename, "r");
		eval(in, out);
		io_close(in);
	}

	if	(narg == 1)
		eval(iostd, out);

	io_close(out);
	return 0;
}
