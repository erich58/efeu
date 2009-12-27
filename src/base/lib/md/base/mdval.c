/*	Werte abfragen/setzen
	(c) 1994 Erich Frühstück
*/


#include <EFEU/mdmat.h>


long Md_getlong(EfiKonv *konv, void *data)
{
	if	(konv)
	{
		long val;
		KonvData(konv, &val, data);
		return val;
	}
	else	return 0;
}


double Md_getdbl(EfiKonv *konv, void *data)
{
	if	(konv)
	{
		double val;
		KonvData(konv, &val, data);
		return val;
	}
	else	return 0.;
}


void Md_setlong(EfiKonv *konv, void *data, long val)
{
	KonvData(konv, data, &val);
}


void Md_setdbl(EfiKonv *konv, void *data, double val)
{
	KonvData(konv, data, &val);
}
