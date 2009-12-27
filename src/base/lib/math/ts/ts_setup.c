/*	Befehlsinterpreter für Zeitreihenanalysen initialisieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <Math/TimeSeries.h>


/*	Funktionen
*/

/*	Initialisieren
*/

static Var_t var[] = {
	{ "TimeSeriesDebug", &Type_bool, &TimeSeries_reftype.debug },
	{ "OLSParDebug", &Type_bool, &OLSPar_reftype.debug },
};

#if	0
static ParseDef_t pdef[] = {
	{ "TS_YEAR", PFunc_int, (void *) TS_YEAR },
	{ "TS_QUART", PFunc_int, (void *) TS_QUART },
	{ "TS_MONTH", PFunc_int, (void *) TS_MONTH },
	{ "TS_WEEK", PFunc_int, (void *) TS_WEEK1900 },
	{ "TS_DAY", PFunc_int, (void *) TS_DAT1900 },
};
#endif

#if	0
static FuncDef_t func[] = {
};
#endif	


void SetupTimeSeries (void)
{
	CmdSetup_TimeIndex();
	CmdSetup_TimeSeries();
	CmdSetup_OLSKoef();
	CmdSetup_OLSPar();
	AddVar(NULL, var, tabsize(var));
#if	0
	AddFuncDef(ts_func, tabsize(ts_func));
	AddParseDef(pdef, tabsize(pdef));
#endif
}
