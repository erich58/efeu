/*	Ausgabedefinitionen initialisieren
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>

static EDB *fdef_show (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	ListEDBFilter(ioerr, 0);
	return NULL;
}

static EDBFilter filter_show = {
	"?", NULL, fdef_show, NULL,
	":*:list available filters"
	":de:Auflisten der verfügbaren Datenbankfilter"
};

void SetupEDBFilter (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;
	AddEDBFilter(&filter_show);
	AddEDBFilter(&EDBFilter_sort);
	AddEDBFilter(&EDBFilter_uniq);
	AddEDBFilter(&EDBFilter_sum);
	AddEDBFilter(&EDBFilter_cut);
	AddEDBFilter(&EDBFilter_var);
	AddEDBFilter(&EDBFilter_trans);
	AddEDBFilter(&EDBFilter_dup);
	AddEDBFilter(&EDBFilter_tee);
	AddEDBFilter(&EDBFilter_clip);
	AddEDBFilter(&EDBFilter_script);
	AddEDBFilter(&EDBFilter_join);
	AddEDBFilter(&EDBFilter_test);
}
