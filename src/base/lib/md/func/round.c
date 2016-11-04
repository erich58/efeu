/*	Datenmatrix runden
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/efutil.h>

void md_round(mdmat *md, double factor, unsigned mask)
{
	EfiConv get;
	EfiConv set;
	double val;

	if	(md == NULL)	return;

	Md_ConvDef(&get, md->type, &Type_double);
	Md_ConvDef(&set, &Type_double, md->type);

	val = factor * MdValSum(md->axis, md->data, &get, mask);
	val = (val < 0.) ? - (int) (-val + 0.5) : (int) (val + 0.5);
	MdRound(md->axis, md->data, &get, &set, val, factor, mask);
}

void md_adjust(mdmat *md, double val, unsigned mask)
{
	EfiConv get;
	EfiConv set;
	double factor;

	if	(md == NULL)	return;

	Md_ConvDef(&get, md->type, &Type_double);
	Md_ConvDef(&set, &Type_double, md->type);

	val = (val < 0.) ? - (long) (-val + 0.5) : (long) (val + 0.5);
	factor = MdValSum(md->axis, md->data, &get, mask);
	factor = (factor != 0.) ? val / factor : 1.;
	MdRound(md->axis, md->data, &get, &set, val, factor, mask);
}


double MdValSum(mdaxis *x, void *data, EfiConv *get, unsigned mask)
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
	else	ConvData(get, &val, data);

	return val;
}


void MdRound(mdaxis *axis, void *data, EfiConv *get, EfiConv *set, double val,
	double factor, unsigned mask)
{
	double *x;
	char *ptr;
	size_t i;

	if	(axis == NULL)
	{
		ConvData(set, data, &val);
		return;
	}

	x = memalloc(axis->dim * sizeof(double));

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

	memfree(x);
}
