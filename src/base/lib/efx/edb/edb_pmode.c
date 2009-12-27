/*	Ausgabemodus für EDB-Datenfiles
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/parsearg.h>
#include <EFEU/Resource.h>

#define	FMT_UNDEF	"edb_out: undefined parameter \"$1\" ignored.\n"

EDBPrintMode *edb_pmode (EDBPrintMode *pmode, const char *mode)
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
		edb_pmode(pmode, "default");
	}

	while ((arg = assignarg(mode, (char **) &mode, ",")))
	{
		EDBPrintDef *def = GetEDBPrintDef(arg->name);

		if	(!def)
		{
			if	(arg->name && *arg->name)
				dbg_note("edb", FMT_UNDEF, "s", arg->name);
		}
		else if	(mstrcmp(arg->arg, "?") == 0)
		{
			io_printf(ioerr, "%s\t%s\n", def->name,
				GetFormat(def->desc));
			exit(EXIT_SUCCESS);
		}
		else	def->set(pmode, def, arg->opt, arg->arg);
		
		memfree(arg);
	}

	return pmode;
}

