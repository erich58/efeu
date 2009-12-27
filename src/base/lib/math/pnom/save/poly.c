/*	Test der Poloynomfunktionen
	(c) 1992 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <efmain.h>
#include <efio.h>
#include <pnom.h>

#define	FMT	"%8.2lf"
#define	NX	"20"
#define	XMOD	"5"
#define	NY	"70"
#define	YMOD	"10"

#define	RN_FNAME	"File"
#define	RN_FMT		"fmt"
#define	RN_NX		"nx"
#define	RN_NY		"ny"
#define	RN_XMOD		"xmod"
#define	RN_YMOD		"ymod"


fmtdef_t ftab[] = {
	{ 1, "Falsche Eingabe\n" },
	{ 2, "Falsche Parameterzahl\n" },
	{ 5, "Zuwenig Datenwerte\n" },
	{ 3, "Fehler in cdef[] - Struktur\n" },
	{ 4, "Unbekanntes Befehlswort $1\n", },
};

code_t rtab[] = {
	{ RN_IDENT, "Test von Polynomfunktionen" },
	{ RN_VERSION, "2" },
	{ RN_CPYRGHT, "(c) 1992 Erich Frühstück" },
	{ RN_FMT, FMT },
	{ RN_NX, NX },
	{ RN_NY, NY },
	{ RN_XMOD, XMOD },
	{ RN_YMOD, YMOD },
};

pardef_t pdef[] = {
	{ OPT_ANYARG, "x", "set(nx)", NULL, "Plotspalten" },
	{ OPT_ANYARG, "y", "set(ny)", NULL, "Plotzeilen" },
	{ OPT_NOARG, NULL, NULL, NULL, NULL },
	{ P_ARG, NULL, "set(File)", NULL, "Datenfile" },
};
	
/*	Befehlsstruktur
*/

double range[2] = { 0., 1. };

size_t ndat = 0;
size_t nkoef = 0;
size_t ndim = 0;
pnom_t *pn = NULL;
double *xdat = NULL;
double *ydat = NULL;
int dflag = 0;

int v_n = 0;
double v_x = 0.;
double v_y = 0.;
char *v_a = NULL;
char *v_b = NULL;
void *data = NULL;

#define	C_NOOP		0
#define	C_HELP		1
#define	C_QUIT		2
#define	C_PRINT		3
#define	C_LOAD		5
#define	C_DEV		6
#define	C_INT		7
#define	C_SAVE		8
#define	C_EVAL		9
#define	C_PLOT		10
#define	C_RANGE		13
#define	C_LINCMB	14
#define	C_SHOW		15
#define	C_SET		16
#define	C_MULT		17
#define	C_READ		18

#define	C_DATA		20
#define	C_END		21
#define	C_LIST		22
#define	C_CREATE	23

#define	C_LININT	31
#define	C_SPLINE	32
#define	C_CSPLINE	33
#define	C_DSPLINE	34


typedef struct {
	int code;
	char *name;
	char *args;
	char *fmt;
	char *desc;
} cmd_t;


typedef struct {
	double x;
	size_t n;
	double *y;
} data_t;


cmd_t cdefs[] = {
	{ C_NOOP, "?", "", "", "Hilfetext ausgeben" },
	{ C_NOOP, "#", "", "", "Kommentarzeile" },
	{ C_NOOP, "*", "", "", "Text ausgeben" },
	{ C_NOOP, "!cmd", "", "", "Systemaufruf" },
	{ C_QUIT, "q", "", "", "Verarbeitung abbrechen" },
	{ C_PRINT, "print", "", "", "Datenstruktur ausgeben" },
	{ C_LOAD, "load", "a", "file", "Daten aus File laden" },
	{ C_READ, "read", "a", "file", "Polynom aus File lesen" },
	{ C_DEV, "dev", "n", "dev", "Ableitung vom Grad dev berechnen" },
	{ C_INT, "int", "xy", "x y", "Stammfunktion durch Punkt x, y" },
	{ C_SAVE, "save", "a", "file", "Sichern in Datei file" },
	{ C_EVAL, "eval", "x", "step", "Werte berechnen" },
	{ C_PLOT, "plot", "xy", "y0 y1", "Funktion plotten" },
	{ C_RANGE, "range", "xy", "x0 x1", "X-Intervall setzen" },
	{ C_LINCMB, "lincmb", "xayb", "a p b q",
		"Lineare Kombination: a*p + b*q" },
	{ C_MULT, "mult", "xab", "c p q", "Multiplikation: c*p*q" },
	{ C_SHOW, "show", "a", "res", "Resourcedefinition abfragen" },
	{ C_SET, "set", "ab", "res val", "Resouce res auf val setzen" },
	{ C_DATA, "data", "", "", "Beginn der Dateneingabe" },
	{ C_END, "end", "", "", "Ende der Dateneingabe" },
	{ C_LIST, "list", "", "", "Auflisten der Datenwerte" },
	{ C_CREATE, "create", "", "", "Polynom generieren" },

	{ C_LININT, "linint", "", "", "Lineare Interpolation" },
	{ C_SPLINE, "spline", "", "", "Splineinterpolation" },
	{ C_CSPLINE, "cspline", "", "",
		"geschlossene Splineinterpolation" },
	{ C_DSPLINE, "dspline", "xy", "a b",
		"Splineinterpolation mit Ableitungen" },
};


/*	Funktionsprototypen
*/

cmd_t *getcmd (char *name);
int getdata (void *entry);
int getkoef (void *entry);
int maxdim (void *entry);
int showdata (void *entry);
void mkdata (void);
void mkpnom (void);
int datacmp (const void *a, const void *b);
int getargs (cmd_t *cmd, char **list, size_t dim);
void setdata (char **list, size_t dim);
int get (io_t *io);
void plot (FILE *file, pnom_t *pn, double y0, double y1);
void eval (pnom_t *pn, double step);


int datacmp(a, b)

const void *a;
const void *b;

{
	if	(((data_t *) a)->x < ((data_t *) b)->x)
		return -1;
	if	(((data_t *) a)->x > ((data_t *) b)->x)
		return 1;

	return 0;
}


int showdata(entry)

void *entry;

{
	data_t *x;
	char *fmt;
	int i;

	x = (data_t *) entry;
	fmt = getres(RN_FMT, FMT);
	printf(fmt, x->x);
	printf(": ");

	for (i = 0; i < x->n; i++)
	{
		printf(" ");
		printf(fmt, x->y[i]);
	}

	printf("\n");
	return 1;
}


int getdata(x)

void *x;

{
	xdat[ndat] = ((data_t *) x)->x;
	ydat[ndat] = ((data_t *) x)->y[0];
	ndat++;
	return 1;
}

int maxdim(entry)

void *entry;

{
	if	(((data_t *) entry)->n > ndim)
		ndim = ((data_t *) entry)->n;

	return 1;
}


int getkoef(entry)

void *entry;

{
	data_t *x;
	int i;

	x = (data_t *) entry;

	for (i = 0; i < x->n; i++)
		pn->c[nkoef][i] = x->y[i];

	while (i <= pn->deg)
		pn->c[nkoef][i++] = 0.;

	pn->x[nkoef++] = x->x;
	return 1;
}


void mkdata()

{
	if	((ndat = xwalk(data, NULL)) == 0)
	{
		return;
	}

	FREE(xdat);
	xdat = ALLOC(ndat, double);
	FREE(ydat);
	ydat = ALLOC(ndat, double);
	ndat = 0;
	xwalk(data, getdata);
}


void mkpnom()

{
	ndim = 0;

	if	((nkoef = xwalk(data, maxdim)) == 0)
	{
		return;
	}

	FREE(pn);
	pn = pnalloc(nkoef, ndim - 1);
	nkoef = 0;
	xwalk(data, getkoef);
}


cmd_t *getcmd(name)

char *name;

{
	int i;

	for (i = 0; i < tabsize(cdefs); i++)
	{
		if	(strcmp(name, cdefs[i].name) == 0)
		{
			return cdefs + i;
		}
	}

	return NULL;
}


void setdata(list, dim)

char **list;
size_t dim;

{
	data_t *x, *y;
	int i;

	if	(dim < 2)	message(NULL, 5);

	dim--;
	x = (data_t *) memalloc((unsigned long) sizeof(data_t)
		+ dim * sizeof(double));
	x->y = (double *) (x + 1);
	x->x = atof(list[0]);
	x->n = dim;

	for (i = 0; i < dim; i++)
		x->y[i] = atof(list[i + 1]);

	y = (data_t *) xsearch(data, x, XS_REPLACE);

	if	(x != y)	FREE(y);
}


int getargs(cmd, list, dim)

cmd_t *cmd;
char **list;
size_t dim;

{
	int i;

	if	(cmd == NULL)	return C_NOOP;

	if	(strlen(cmd->args) != dim)
	{
		message(NULL, 2);
		return C_NOOP;
	}

	for (i = 0; cmd->args[i] != 0; i++)
	{
		switch (cmd->args[i])
		{
		case 'n':	v_n = atoi(list[i]); break;
		case 'x':	v_x = atof(list[i]); break;
		case 'y':	v_y = atof(list[i]); break;
		case 'a':	v_a = argval(list[i]); break;
		case 'b':	v_b = argval(list[i]); break;
		default:	liberror(NULL, 3); break;
		}
	}

	return cmd->code;
}


/*	Befehlszeile einlesen
*/

int get(io)

io_t *io;

{
	char buf[100];
	char **cmdlist = NULL;
	size_t cmddim = 0;
	cmd_t *cmd;
	int i, c;

	for (i = 0; (c = io_getc(io)) != '\n'; i++)
	{
		if	(c == EOF)
		{
			return C_QUIT;
		}
		else	buf[i] = (char) c;
	}

	buf[i] = 0;

	switch (buf[0])
	{
	case 0:
	case '#':
		return C_NOOP;
	case '?':
		return C_HELP;
	case '!':
		system(buf + 1);
		printf("\n");
		return C_NOOP;
	case '*':
		printf("%s\n", buf + 1);
		return C_NOOP;
	default:
		break;
	}

	cmddim = strsplit(buf, " \t\n", &cmdlist); 
	cmd = getcmd(cmdlist[0]);
	i = C_NOOP;

	if	(dflag && cmd == NULL)
	{
		setdata(cmdlist, cmddim);
	}
	else if	(cmd == NULL)
	{
		cpy_arg(1, cmdlist[0]);
		message(NULL, 4);
	}
	else
	{
		i = getargs(cmd, cmdlist + 1, cmddim - 1);
	}

	FREE(cmdlist);
	return i;
}


void eval(p, step)

pnom_t *p;
double step;

{
	double x;
	char *fmt;
	int j;

	fmt = getres(RN_FMT, FMT);

	if	(step <= 0.)	step = (range[1] - range[0]) / 10.;

	for (x = range[0]; x <= range[1]; x += step)
	{
		putchar(' ');
		printf(fmt, x);

		for (j = 0; j <= p->deg; j++)
		{
			putchar(' ');
			printf(fmt, pneval(p, j, x));
		}

		putchar('\n');
	}
}


void plot(file, p, y0, y1)

FILE *file;
pnom_t *p;
double y0, y1;

{
	int k, i, j;
	double xs, yf;
	int nx, xmod;
	int ny, ymod;
	char *fmt;
	double x;

	fmt = getres(RN_FMT, FMT);
	nx = atoi(getres(RN_NX, NX));
	ny = atoi(getres(RN_NY, NY));
	xmod = atoi(getres(RN_XMOD, XMOD));
	ymod = atoi(getres(RN_YMOD, YMOD));

	fprintf(file, "\fX-Achse: ");
	fprintf(file, fmt, range[0]);
	fprintf(file, ", ");
	fprintf(file, fmt, range[1]);
	fprintf(file, "; Y-Achse: ");
	fprintf(file, fmt, y0);
	fprintf(file, ", ");
	fprintf(file, fmt, y1);
	fprintf(file, "\n\n");
	xs = (range[1] - range[0]) / nx;
	yf = ny / (y1 - y0);
	x = range[0];

	for (i = 0; i <= nx; ++i)
	{
		k = (int) ((pneval(p, 0, x) - y0) * yf + .5);
		x += xs;

		for (j = 0; j <= ny; ++j)
		{
			if (k == j)
				putc('*', file);

			else if	(i % xmod == 0 && j % ymod == 0)
				putc('+', file);

			else if	(i % xmod == 0)
				putc('-', file);

			else if	(j % ymod == 0)
				putc('|', file);

			else	putc(' ', file);
		}

		putc('\n', file);
	}

	putc('\n', file);
}


int main(narg, arg)

int narg;
char **arg;

{
	char *p;
	io_t *io;
	pnom_t *q1, *q2;
	int cmd;
	int i;
	FILE *file;

	libinit(arg[0], use_readline, pnom, NULL);
	addmsg(NULL, ftab, tabsize(ftab));
	addres(rtab, tabsize(rtab));
	xloadpar(&narg, arg, pdef, tabsize(pdef),
		XLP_LOAD_ALL|XLP_DUMP_ALL);

	p = getres(RN_FNAME, NULL);

	if	(p != NULL)
	{
		io = io_fileopen(p, "r");
		io = io_lcount(io);
	}
	else	io = iostd;

	while ((cmd = get(io)) != C_QUIT)
	{
		switch (cmd)
		{
		case C_HELP:

			for (i = 0; i < tabsize(cdefs); i++)
			{
				printf("%s\t%s\t%s\n", cdefs[i].name,
					cdefs[i].fmt, cdefs[i].desc);
			}

			break;

		case C_RANGE:

			range[0] = v_x;
			range[1] = v_y;
			break;

		case C_LOAD:

			FREE(pn);
			pn = (v_a != NULL ? pnload(v_a) : NULL);
			break;

		case C_READ:

			FREE(pn);
			file = fileopen(v_a, "r");
			pn = pnread(file);
			fileclose(file);
			break;

		case C_DEV:

			q1 = pndev(pn, v_n);
			FREE(pn);
			pn = q1;
			break;

		case C_INT:

			q1 = pnint(pn, v_x, v_y);
			FREE(pn);
			pn = q1;
			break;

		case C_PRINT:

			pnprint(stdout, pn, "%7.2lf");
			break;

		case C_SAVE:

			pnsave(pn, v_a);
			break;

		case C_EVAL:

			eval(pn, v_x);
			break;

		case C_PLOT:

			plot(stdout, pn, v_x, v_y);
			break;

		case C_LININT:

			mkdata();
			FREE(pn);
			pn = linint(ndat, xdat, ydat);
			break;
		
		case C_SPLINE:

			mkdata();
			FREE(pn);
			pn = spline(ndat, xdat, ydat);
			break;

		case C_CSPLINE:

			mkdata();
			FREE(pn);
			pn = cspline(ndat, xdat, ydat);
			break;

		case C_DSPLINE:

			mkdata();
			FREE(pn);
			pn = dspline(ndat, xdat, ydat, v_x, v_y);
			break;

		case C_LINCMB:

			FREE(pn);
			q1 = (v_a != NULL ? pnload(v_a) : NULL);
			q2 = (v_b != NULL ? pnload(v_b) : NULL);
			pn = pnlincmb(q1, v_x, q2, v_y, NULL);
			FREE(q1);
			FREE(q2);
			break;

		case C_MULT:

			q1 = (v_a != NULL ? pnload(v_a) : NULL);
			q2 = (v_b != NULL ? pnload(v_b) : NULL);
			FREE(pn);
			pn = pnmult(v_x, q1, q2);
			FREE(q1);
			FREE(q2);
			break;

		case C_SHOW:

			printf("%s=%s\n", v_a, getres(v_a, "NULL"));
			break;

		case C_SET:

			setres(v_a, v_b);
			break;

		case C_DATA:

			xdestroy(data, memfree);
			data = xcreate(0, 0, datacmp);
			dflag = 1;
			break;

		case C_END:

			dflag = 0;
			break;

		case C_LIST:

			xwalk(data, showdata);
			break;

		case C_CREATE:

			mkpnom();
			break;

		default:

			break;
		}
	}

	io_close(io);
	return 0;
}
