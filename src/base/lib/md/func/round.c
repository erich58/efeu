/*	Datenmatrix runden
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdmat.h>
#include <EFEU/efutil.h>

void md_round(mdmat_t *md, double factor, unsigned mask)
{
	Konv_t get;
	Konv_t set;
	double val;

	if	(md == NULL)	return;

	Md_KonvDef(&get, md->type, &Type_double);
	Md_KonvDef(&set, &Type_double, md->type);

	val = factor * MdValSum(md->axis, md->data, &get, mask);
	val = (val < 0.) ? - (int) (-val + 0.5) : (int) (val + 0.5);
	MdRound(md->axis, md->data, &get, &set, val, factor, mask);
}

void md_adjust(mdmat_t *md, double val, unsigned mask)
{
	Konv_t get;
	Konv_t set;
	double factor;

	if	(md == NULL)	return;

	Md_KonvDef(&get, md->type, &Type_double);
	Md_KonvDef(&set, &Type_double, md->type);

	val = (val < 0.) ? - (long) (-val + 0.5) : (long) (val + 0.5);
	factor = MdValSum(md->axis, md->data, &get, mask);
	factor = (factor != 0.) ? val / factor : 1.;
	MdRound(md->axis, md->data, &get, &set, val, factor, mask);
}


double MdValSum(mdaxis_t *x, void *data, Konv_t *get, unsigned mask)
{
	double val;
	int i;

	val = 0.;

	if	(x != NULL)
	{
		for (i = 0; i < x->dim; i++)
		{
			val += MdValSum(x->next, (char *) data + i * x->size, get, mask);
		}
	}
	else	KonvData(get, &val, data);

	return val;
}


void MdRound(mdaxis_t *axis, void *data, Konv_t *get, Konv_t *set, double val,
	double factor, unsigned mask)
{
	double *x;
	char *ptr;
	size_t i;

	if	(axis == NULL)
	{
		KonvData(set, data, &val);
		return;
	}

	x = ALLOC(axis->dim, double);

/*	Werte bestimmen
*/
	ptr = data;

	for (i = 0; i < axis->dim; i++)
	{
		x[i] = factor * MdValSum(axis->next, ptr, get, mask);
		ptr += axis->size;
	}

/*	Vektor runden
*/
	roundvec(x, axis->dim, val);

/*	Ergebnisse setzen
*/
	ptr = data;

	for (i = 0; i < axis->dim; i++)
	{
		MdRound(axis->next, ptr, get, set, x[i], factor, mask);
		ptr += axis->size;
	}

	FREE(x);
}
