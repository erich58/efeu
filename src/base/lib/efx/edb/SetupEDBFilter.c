/*	Ausgabedefinitionen initialisieren
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/EDBFilter.h>

void SetupEDBFilter (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;
	AddEfiPar(NULL, &EDBFilter_sort);
	AddEfiPar(NULL, &EDBFilter_uniq);
	AddEfiPar(NULL, &EDBFilter_sum);
	AddEfiPar(NULL, &EDBFilter_var);
	AddEfiPar(NULL, &EDBFilter_conv);
	AddEfiPar(NULL, &EDBFilter_trans);
	AddEfiPar(NULL, &EDBFilter_dup);
	AddEfiPar(NULL, &EDBFilter_tee);
	AddEfiPar(NULL, &EDBFilter_clip);
	AddEfiPar(NULL, &EDBFilter_script);
	AddEfiPar(NULL, &EDBFilter_join);
	AddEfiPar(NULL, &EDBFilter_test);
	AddEfiPar(NULL, &EDBFilter_expr);
	AddEfiPar(NULL, &EDBFilter_nest);
	AddEfiPar(NULL, &EDBFilter_sample);
}
