/*	Globale Variablen fuer hwz - Bibliothek
	(c) 1991 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 1.1
*/

#include <EFEU/pconfig.h>
#include "agf.h"


/*	Globale Variablen
*/

XTAB(sldef, 0, skey_cmp);

ushort_t personen = 0;
ushort_t wohnungen = 0;
ushort_t arbst = 0;
ushort_t ausl = 0;
int aufwand = 0;
int nfl = 0;


static void f_dblist (Func_t *func, void *rval, void **arg)
{
	dblist();
	libexit(EXIT_SUCCESS);
}

static FuncDef_t fdef[] = {
	{ 0, &Type_void, "dblist ()", f_dblist },
};


/*	Globale Variablen
*/

void SetupVZ(void)
{
	AddFuncDef(fdef, tabsize(fdef));
}
