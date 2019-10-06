/*	Werte abfragen/setzen
	(c) 1994 Erich Frühstück
*/


#include <EFEU/mdmat.h>


long Md_getlong(EfiConv *konv, void *data)
{
	if	(konv)
	{
		long val;
		ConvData(konv, &val, data);
		return val;
	}
	else	return 0;
}


double Md_getdbl(EfiConv *konv, void *data)
{
	if	(konv)
	{
		double val;
		ConvData(konv, &val, data);
		return val;
	}
	else	return 0.;
}


void Md_setlong(EfiConv *konv, void *data, long val)
{
	ConvData(konv, data, &val);
}


void Md_setdbl(EfiConv *konv, void *data, double val)
{
	ConvData(konv, data, &val);
}
