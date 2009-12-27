/*	Generierung einer Dezilverteilung
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 3.1
*/

#include <EFEU/pconfig.h>
#include <EFEU/mdmat.h>
#include <EFEU/data.h>
#include "dezile.h"


/*	Eingabe - Datenstruktur
*/

typedef struct {
	double anz;
	double val;
} DDATA;

typedef struct {
	int anz;
	int val;
} IDATA;


/*	Resourcedefinitionen
*/

char *Mode = "dez";	/* Verteilungsmodus */
char *Input = NULL;
char *Output = NULL;
char *SelDef = NULL;	/* Selektionsliste für Achsen */
char *LKMask = NULL;	/* Maske für Lorenzkurven */
int Reduce = 0;		/* Flag zur Reduktion singulärer Achsen */
int Spline = 0;		/* Spline-Interpolation verwenden */
int Gini = 0;		/* Gini-Koeffizient berechnen */
int Mittel = 0;		/* Mittelwerte berechnen */

Var_t vardef[] = {
	{ "Mode",	&Type_str, &Mode },
	{ "Input",	&Type_str, &Input },
	{ "Output",	&Type_str, &Output },
	{ "LKMask",	&Type_str, &LKMask },
	{ "SelDef",	&Type_str, &SelDef },
	{ "Reduce",	&Type_bool, &Reduce },
	{ "Spline",	&Type_bool, &Spline },
	{ "Mittel",	&Type_bool, &Mittel },
	{ "Gini",	&Type_bool, &Gini },
};


/*	Achse mit Verteilungsparametern
*/

#define	NAME	"vdat"	/* Name der Verteilungsachse */
#define	LBL_ANZ	"ANZ"	/* Label für Zahl der Personen */
#define	LBL_GMW	"GMW"	/* Label für Mittelwert */
#define	LBL_GNI	"GNI"	/* Label für GINI-Koeffizient */


/*	Globale Variablen
*/

ModeDef_t *mode;	/* Verteilungstype */

mdaxis_t *class = NULL;	/* Klassenachse */
double *anz = NULL;	/* Anzahl */
double *val = NULL;	/* Wertesumme */

char **idx_list = NULL;	/* Indexliste */
size_t idx_dim = 0;	/* Indexdim */

/*	Hilfsfunktionen
*/

void maineval (io_t *io, mdaxis_t *axis, int depth, void *data);
void subeval (io_t *io, size_t dim);
size_t setddata (DDATA *data, size_t dim);
size_t setidata (IDATA *data, size_t dim);
pnom_t *intpol (size_t n, double *x, double *y);

typedef size_t (*setdata_t) (void *data, size_t dim);
setdata_t setdata = NULL;

void putval (io_t *io, double x);
mdaxis_t *make_axis (ModeDef_t *def);



/*	Hauptprogramm
*/

int main(int narg, char **arg)
{
	mdmat_t *md;		/* Multidimensionale Matrix */
	io_t *io;		/* IO-Struktur */
	mdaxis_t **ptr;		/* Achsenpointer */

/*	Aufrufparameter und Formatdefinitionen laden
*/
	libinit(arg[0]);
	SetupMdMat();
	ModeInfo(NULL, "mode", "Verteilungsdefinitionen");
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

/*	Verteilungsdefinition bestimmen
*/
	if	((mode = GetMode(Mode)) == NULL)
	{
		liberror(NULL, 5);
		libexit(0);
	}

/*	Datenmatrix laden
*/
	io = io_fileopen(Input, "rb");
	md = md_load(io, SelDef, NULL);
	io_close(io);

	if	(md->type->size == sizeof(DDATA))
	{
		setdata = (setdata_t) setddata;
	}
	else if	(md->type->size == sizeof(IDATA))
	{
		setdata = (setdata_t) setidata;
	}
	else
	{
		reg_cpy(1, md->type->name);
		liberror(NULL, 1);
	}

	if	(Reduce)
		md_reduce(md, NULL);

	if	(md->axis == NULL)
		liberror(NULL, 2);

/*	Klassenindex bestimmen und gegen Verteilungsindex austauschen
*/
	class = md->axis;
	ptr = &md->axis;
	idx_dim = 0;

	while ((*ptr)->next != NULL)
	{
		idx_dim++;
		ptr = &(*ptr)->next;
	}

	class = *ptr;
	*ptr = make_axis(mode);

	if	(class->dim == 0)
		liberror(NULL, 3);

/*	Hilfsvektoren initialisieren
*/
	idx_list = ALLOC(idx_dim, char *);
	anz = ALLOC(class->dim + 1, double);
	val = ALLOC(class->dim + 1, double);

/*	Dezilverteilung berechnen und ausgeben
*/
	io = io_fileopen(Output, "wb");
	md->type = &Type_double;
	md_puthdr(io, md, 0);
	maineval(io, md->axis, 0, md->data);
	md_puteof(io);
	io_close(io);
	return 0;
}


/*	Wert ausgeben
*/

void putval(io_t *io, double d)
{
	io_dbwrite(io, &d, sizeof(double), sizeof(double), 1);
}


/*	Datenmatrix durchwandern
*/

void maineval(io_t *io, mdaxis_t *axis, int depth, void *data)
{
	if	(axis->next != NULL)
	{
		int i;

		for (i = 0; i < axis->dim; i++)
		{
			idx_list[depth] = axis->idx[i].name;
			maineval(io, axis->next, depth + 1, data);
			data = ((char *) data) + axis->size;
		}
	}
	else	subeval(io, setdata(data, class->dim));
}


/*	Gruppe auswerten
*/

void subeval(io_t *io, size_t n)
{
	double gmw;
	double ganz;
	double z1, z2;
	pnom_t *lk;
	int i;

/*	Daten für Lorenzkurve aufbereiten
*/
	if	(n > 1)
	{
		ganz = anz[n - 1];
		gmw = val[n - 1];

		for (i = 0; i < n; i++)
			anz[i] /= ganz;

		if	(gmw != 0)
		{
			for (i = 0; i < n; i++)
				val[i] /= gmw;
		}

		gmw /= ganz;

		if	(Spline)
		{
			lk = spline(n, anz, val);
		}
		else
		{
			pnom_t *p = intpol(n, anz, val);
			lk = pnint(p, 0., 0.);
			unref_pnom(p);
		}
	}
	else
	{
		ganz = gmw = 0.;
		lk = pnconst(0., 0.);
	}


/*	Anzahl, Mittelwert und Gini-Koeffizient
*/
	putval(io, ganz);

	if	(Mittel)
		putval(io, gmw);

	if	(Gini)
		putval(io, 1. - 2. * pnarea(lk, 0., 1.));

/*	Dezile
*/
	for (i = 0; i < mode->dim; i++)
		putval(io, gmw * pneval(lk, 1, mode->tab[i].x));

/*	Mittelwerte
*/
	if	(Mittel)
	{
		z1 = 0.;
		gmw *= (mode->dim + 1.);

		for (i = 0; i < mode->dim; i++)
		{
			z2 = gmw * pneval(lk, 0, mode->tab[i].x);
			putval(io, z2 - z1);
			z1 = z2;
		}

		putval(io, gmw - z1);
	}

/*	Lorenzkurve ausgeben
*/
	if	(LKMask)
	{
		char *p;

		reg_set(1, listcat(".", idx_list, idx_dim));
		p = parsub(LKMask);
		pnsave(lk, p);
		FREE(p);
	}

	FREE(lk);
}


/*	Datenmatrix setzen
*/

size_t setidata(IDATA *data, size_t dim)
{
	size_t i, j;

	anz[0] = val[0] = 0.;

	for (i = 0, j = 1; i < dim; i++)
	{
		if	(data[i].anz != 0)
		{
			anz[j] = anz[j - 1] + data[i].anz;
			val[j] = val[j - 1] + data[i].val;
			j++;
		}
	}

	return j;
}

size_t setddata(DDATA *data, size_t dim)
{
	size_t i, j;

	anz[0] = val[0] = 0.;

	for (i = 0, j = 1; i < dim; i++)
	{
		if	(data[i].anz != 0.)
		{
			anz[j] = anz[j - 1] + data[i].anz;
			val[j] = val[j - 1] + data[i].val;
			j++;
		}
	}

	return j;
}


/*	Ausgabeachse generieren
*/

mdaxis_t *make_axis (ModeDef_t *mode)
{
	mdaxis_t *axis;
	char *mfmt;
	int i, n;

	mfmt = (mode->dim >= 10) ? "M%02d" : "MW%d";
	n = 1 + mode->dim;

	if	(Gini)		n++;
	if	(Mittel)	n += mode->dim + 2;

	axis = new_axis(n);
	axis->name = NAME;
	axis->size = sizeof(double);
	n = 0;
	axis->idx[n++].name = LBL_ANZ;

	if	(Mittel)
		axis->idx[n++].name = LBL_GMW;
	
	if	(Gini)
		axis->idx[n++].name = LBL_GNI;

	for (i = 0; i < mode->dim; i++)
		axis->idx[n++].name = mstrcpy(mode->tab[i].name);

	if	(Mittel)
	{
		for (i = 0; i <= mode->dim; i++)
			axis->idx[n++].name = msprintf(mfmt, i);
	}

	axis->dim = n;
	return axis;
}


