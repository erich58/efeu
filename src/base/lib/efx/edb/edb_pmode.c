/*	Ausgabemodus für EDB-Datenfiles
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/parsearg.h>
#include <EFEU/Resource.h>
#include <EFEU/EfiPar.h>

#define	FMT_UNDEF	"edb_out: undefined parameter \"$1\" ignored.\n"

EDBPrintMode *edb_pmode (EDBPrintMode *pmode, EDB *edb, const char *mode)
{
	AssignArg *arg;
	static EDBPrintMode pmode_buf;

	if	(pmode == NULL)
	{
		pmode = &pmode_buf;
		pmode->name = NULL;
		pmode->init = NULL;
		pmode->header = 0;
		pmode->split = 0;
		pmode->par = NULL;
		edb_pmode(pmode, edb, "default");
	}

	if	(mode == NULL && edb && edb->obj)
		mode = GetEfiControl(edb->obj->type, "EDBPrintMode");

	while ((arg = assignarg(mode, (char **) &mode, ",")))
	{
		EDBPrintDef *def = GetEDBPrintDef(arg->name);

		if	(!def)
		{
			pmode->init = EDBPrint_data_init;
			pmode->par = rd_wrap(arg, memfree);
			arg = NULL;
		}
		else if	(mstrcmp(arg->arg, "?") == 0)
		{
			ShowEDBPrintDef(ioerr, def, 1);
			exit(EXIT_SUCCESS);
		}
		else	def->set(pmode, def, arg->opt, arg->arg);
		
		memfree(arg);
	}

	return pmode;
}

