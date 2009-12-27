/*	Hilfsroutinen fuer Aggregatfile-Datenformat
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 2.0
*/

#include <EFEU/pconfig.h>
#include "oestz.h"


/*	Variablendefinitionen
*/

char *OestzHflag = NULL;
char *OestzSflag = NULL;
char *OestzGflag = NULL;
char *OestzKflag = NULL;


static Var_t vardef[] = {
	{ "OestzHflag",  &Type_str, &OestzHflag },
	{ "OestzSflag",  &Type_str, &OestzSflag },
	{ "OestzGflag",  &Type_str, &OestzGflag },
	{ "OestzKflag",  &Type_str, &OestzKflag },
};


void SetupOestz(void)
{
	AddVar(NULL, vardef, tabsize(vardef));
}
