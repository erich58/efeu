/*	Datenmatrix korrigieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/efutil.h>

static void do_korr(mdaxis *axis, void *data, EfiKonv *get, EfiKonv *set, double val, int round);

void md_korr(mdmat *md, int round)
{
	EfiKonv get;
	EfiKonv set;
	double val;

	if	(md == NULL)	return;

	Md_KonvDef(&get, md->type, &Type_double);
	Md_KonvDef(&set, &Type_double, md->type);
	val = MdValSum(md->axis, md->data, &get, 0);

	if	(val < 0.)	val = 0.;

	do_korr(md->axis, md->data, &get, &set, val, round);
}

static void do_korr(mdaxis *axis, void *data, EfiKonv *get, EfiKonv *set,
	double val, int round)
{
	double *dat;
	char *ptr;
	size_t i;
	double z;

	if	(axis == NULL)
	{
		KonvData(set, data, &val);
		return;
	}

	dat = memalloc(axis->dim * sizeof(double));

/*	Werte bestimmen
*/
	ptr = data;
	z = 0.;

	for (i = 0; i < axis->dim; i++)
	{
		dat[i] = MdValSum(axis->next, ptr, get, 0);

		if	(dat[i] < 0.)
		{
			dat[i] = 0.;
		}
		else	z += dat[i];

		ptr += axis->size;
	}

/*	Korrekturwerte bestimmen
*/
	z = (z ? val / z : 0.);

	for (i = 0; i < axis->dim; i++)
		dat[i] *= z;

	if	(round)
		roundvec(dat, axis->dim, val);

/*	Ergebnisse setzen
*/
	ptr = data;

	for (i = 0; i < axis->dim; i++)
	{
		do_korr(axis->next, ptr, get, set, dat[i], round);
		ptr += axis->size;
	}

	memfree(dat);
}
