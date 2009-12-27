/*	Auswerten von Wohnungsvektoren
	(c) 1992 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include <EFEU/pconfig.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include "agf.h"

/*	Variablendefinitionen
*/

char *Output = NULL;
char *Grpdef = NULL;
char *Titel = NULL;
int Type = 0;
int Reduce = 0;
char *List = NULL;
char *DB = NULL;

static Var_t vardef[] = {
	{ "DB", &Type_str, &DB },
	{ "Output", &Type_str, &Output },
	{ "Grpdef", &Type_str, &Grpdef },
	{ "Titel", &Type_str, &Titel },
	{ "Type", &Type_int, &Type },
	{ "Reduce", &Type_bool, &Reduce },
	{ "List", &Type_str, &List },
};


/*	Zaehlerdefinition
*/

#define	COUNT_FUNK(name)	static void name (void *ptr)



COUNT_FUNK(wadd) { *((int *) ptr) += wohnungen; }
COUNT_FUNK(padd) { *((int *) ptr) += personen; }
COUNT_FUNK(sadd) { *((int *) ptr) += arbst; }
COUNT_FUNK(aadd) { *((int *) ptr) += ausl; }


COUNT_FUNK(xadd)
{
	((int *) ptr)[0] += wohnungen;
	((int *) ptr)[1] += personen;
}

COUNT_FUNK(ladd)
{
	((int *) ptr)[0] += personen;
	((int *) ptr)[1] += ausl;
}

COUNT_FUNK(vadd)
{
	((int *) ptr)[0] += wohnungen;
	((int *) ptr)[1] += aufwand * wohnungen;
}

COUNT_FUNK(nadd)
{
	((int *) ptr)[0] += wohnungen;
	((int *) ptr)[1] += nfl;
}


MdCount_t wcounter = { "w", "int", NULL, NULL, NULL, wadd };
MdCount_t pcounter = { "p", "int", NULL, NULL, NULL, padd };
MdCount_t scounter = { "s", "int", NULL, NULL, NULL, sadd };
MdCount_t acounter = { "a", "int", NULL, NULL, NULL, aadd };
MdCount_t xcounter = { "x", "int whg, pers", NULL, NULL, NULL, xadd };
MdCount_t lcounter = { "l", "int ges, aus", NULL, NULL, NULL, ladd };
MdCount_t vcounter = { "v", "int whg, aufwand", NULL, NULL, NULL, vadd };
MdCount_t ncounter = { "n", "int whg, nfl", NULL, NULL, NULL, nadd };


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	io_t *aus;
	mdmat_t *ctab;
	MdCount_t *countdef;
	ushort_t *data;
	DBDEF *def;
	char *name;

	libinit(arg[0]);
	SetupMdMat();
	SetupVZ();
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	def = dbdef(DB);
	def->mksl(&sldef);

	if	(Output == NULL || List != NULL)
	{
		MdShowClass(iostd, &sldef, List);
		return 0;
	}

	if	(Type == 0)	Type = def->stdcount;

	switch (Type)
	{
	case 's':	countdef = &scounter; name = "Anzahl"; break;
	case 'a':	countdef = &acounter; name = "Ausländer"; break;
	case 'b':	countdef = &pcounter; name = "Personen"; break;
	case 'p':	countdef = &pcounter; name = "Personen"; break;
	case 'x':	countdef = &xcounter; name = "Wohnungen und Personen"; break;
	case 'l':	countdef = &lcounter; name = "Insgesamt und Ausländer"; break;
	case 'v':	countdef = &vcounter; name = "Verteilung des Wohnungsaufwandes"; break;
	case 'n':	countdef = &ncounter; name = "Verteilung der Nutzfläche"; break;
	default:	countdef = &wcounter; name = "Wohnungen"; break;
	}

	ctab = md_ctab(NULL, Grpdef, &sldef, countdef);
	ctab->title = Titel ? Titel : mstrcat(": ", def->desc, name, NULL);
	aus = io_fileopen(Output, "w");
	open_db(def);

	while ((data = get_db()) != NULL)
	{
		wohnungen = 0;
		personen = 0;
		arbst = 0;
		ausl = 0;
		def->cval(data);
		md_count(ctab, data);
	}

	if	(Reduce)
	{
		mdaxis_t **x;

		x = &ctab->axis;

		while (*x != NULL)
		{
			if	((*x)->dim == 1)
			{
				*x = (*x)->next;
			}
			else	x = &(*x)->next;
		}
	}

	md_save(aus, ctab, 0);
	io_close(aus);
	close_db();
	return 0;
}
