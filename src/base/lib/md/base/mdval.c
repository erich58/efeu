/*	Werte abfragen/setzen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/


#include <EFEU/mdmat.h>


long Md_getlong(Konv_t *konv, void *data)
{
	if	(konv)
	{
		long val;
		KonvData(konv, &val, data);
		return val;
	}
	else	return 0;
}


double Md_getdbl(Konv_t *konv, void *data)
{
	if	(konv)
	{
		double val;
		KonvData(konv, &val, data);
		return val;
	}
	else	return 0.;
}


void Md_setlong(Konv_t *konv, void *data, long val)
{
	KonvData(konv, data, &val);
}


void Md_setdbl(Konv_t *konv, void *data, double val)
{
	KonvData(konv, data, &val);
}
