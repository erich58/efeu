/*	Ausgabekontrolle für mdprint
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pctrl.h>

#define	CR_NL	"\r\n"

/*	Statische Variablen und Initialisierer
*/

int pctrl_fsize = 10;
char *pctrl_pgfmt = "a4";


/*	Globale Variablen
*/

static PrCtrl_t pctrl[] = {
	{ NULL, "Standardausgabefilter", " ", "\n", "\f", std_ctrl },
	{ "test", "Testausgabefilter", " ", "<NL>\n", "<FF>\f", tst_ctrl },
	{ "ascii", "Ascii Ausgabe", "\t", "\n", "\f", std_ctrl },
	{ "struct", "Formatierung als Liste", ", ", " },\n", NULL, struct_ctrl },
	{ "csv", "Formatierung als csv - Datei", ";", CR_NL, NULL, csv_ctrl },
	{ "data", "Nur Datenwerte ausgeben", "\t", "\n", NULL, data_ctrl },
	{ "sc", "Aufbereitung für SC", "\n", "\n", NULL, sc_ctrl },
	{ "tex", "LaTeX-Dokument", "&", "~\\\\\n", "\\newpage\n", tex_ctrl },
	{ "tab", "LaTeX-Tabelle", "&", "~\\\\\n", "\\newpage\n", tab_ctrl },
};


PrCtrl_t *PrCtrl(const char *name)
{
	int i;

	for (i = 0; i < tabsize(pctrl); i++)
		if (mstrcmp(pctrl[i].name, name) == 0)
			return pctrl + i;

	return pctrl;
}
