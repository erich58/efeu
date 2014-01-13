/*
Vektoren testen/selektieren

$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/TestDef.h>

int EfiVec_test (EfiVec *vec, TestDef *def)
{
	int i;
	char *p;

	if	(!def || !vec || def->type != vec->type)
		return 0;

	p = vec->buf.data;

	for (i = 0; i < vec->buf.used; i++)
	{
		if	(TestDef_test(def, p))	return 1;

		p += vec->buf.elsize;
	}

	return 0;
}

int EfiVec_select (EfiVec *vec, TestDef *def)
{
	int i, n;
	char *p, *q;

	if	(!vec)
		return 0;

	if	(!def || def->type != vec->type)
	{
		EfiVec_resize(vec, 0);
		return 0;
	}

	p = q = vec->buf.data;

	for (i = n = 0; i < vec->buf.used; i++)
	{
		if	(TestDef_test(def, p))
		{
			if	(p != q)
				memswap(p, q, vec->buf.elsize);

			n++;
			q += vec->buf.elsize;
		}

		p += vec->buf.elsize;
	}

	EfiVec_resize(vec, n);
	return n;
}
