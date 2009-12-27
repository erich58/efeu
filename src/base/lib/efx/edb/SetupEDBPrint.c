/*	Ausgabedefinitionen initialisieren
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/LangDef.h>
#include <EFEU/locale.h>
#include <EFEU/printobj.h>

#define	FMT_UNIT	"edb_out: parameter \"$1\": undefined unit key $2.\n"
#define	FMT_XARG	"edb_out: parameter \"$1\": argument \"$2\" ignored.\n"

#define	XOUT_LIM	(512 * 1024 * 1024)

void EDBPrintDef_alias (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	edb_pmode(mode, def->par);
}

static void set_header (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->header = atoi(def->par);
}

static void set_prec (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	if	(opt)
		PrintFieldWidth = strtol(opt, NULL, 0);

	SetFloatPrec(arg ? arg : def->par);
}

static void set_locale (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	SetLocale(LOC_PRINT, arg ? arg : LangDef.language);
}

static void set_split (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	char *p;
	double lim;

	if	(def->par)
	{
		if	(arg)
			dbg_note("edb", FMT_XARG, "ss", def->name, arg);

		arg = def->par;
	}
	else if	(!arg)
	{
		mode->split = XOUT_LIM;
		return;
	}

	lim = strtod(arg, &p);

	switch (*p)
	{
	case  0:	break;
	case 'k':	lim *= 1024.; break;
	case 'M':	lim *= 1024. * 1024.; break;
	case 'G':	lim *= 1024. * 1024. * 1024.; break;
	default:	dbg_note("edb", FMT_UNIT, "sc", def->name, *p); break;
	}

	mode->split = lim + 0.5;
}

static void show_def (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	ListEDBPrintDef(ioerr);
	exit(EXIT_SUCCESS);
}


static EDBPrintDef pdef[] = {
	{ "?", show_def, NULL,
		":*:list print flags"
		":de:Auflisten der Ausgabeflags"
	},
	{ "default", EDBPrintDef_alias, "plain,verbose,split=512M",
		":*:default flags: $2"
		":de:Standardvorgabe: $2"
	},
	{ "binary", EDBPrint_binary, NULL,
		":*:binary output"
		":de:Binärausgabe"
	},
	{ "plain", EDBPrint_plain, NULL,
		":*:plain output"
		":de:Textausgabe als Term"
	},
	{ "label", EDBPrint_label, NULL,
		":*:label output"
		":de:Textausgabe als Zeichenkette"
	},
	{ "data", EDBPrint_data, NULL,
		":*:data without declarations"
		":de:Nur Daten ohne Deklarationen ausgeben"
	},
	{ "fmt", EDBPrint_fmt, NULL,
		":*:formatted output of data lines"
		":de:Formatierte Ausgabe von Datenzeilen"
	},

	{ "locale", set_locale, NULL,
		":*:set locale for output"
		":de:Lokale für die Ausgabe setzen"
	},
	{ "nohead", set_header, "0",
		":*:no header"
		":de:Kein Header"
	},
	{ "compact", set_header, "1",
		":*:compact header"
		":de:Kompakter Header ohne Kommentare"
	},
	{ "verbose", set_header, "2",
		":*:verbose header"
		":de:Header mit Kommentaren"
	},
	{ "prec", set_prec, "18g",
		":*:precision of floating point numbers"
		":de:Genauigkeit von Gleitkommazahlen festlegen"
	},

	{ "split", set_split, NULL,
		":*:split limit for output"
		":de:Aufspaltungslimit für die Ausgabe"
	},
	{ "nosplit", set_split, "0",
		":*:do not split output"
		":de:Ausgabe nicht aufspalten"
	},

	{ "export", EDBPrintDef_alias, "data,nosplit",
		":*:export data, synonymous to $2"
		":de:Daten exportieren, entspricht $2"
	},
	{ "csv", EDBPrintDef_alias, "data=crlf,nosplit,nohead,locale",
		":*:export data with CR/LF, synonymous to $2"
		":de:Daten mit CR/LF exportieren, enspricht $2"
	},

	{ "b", EDBPrintDef_alias, "binary",
		":*:alias for $2"
		":de:Kurzform für $2"
	},
	{ "l", EDBPrintDef_alias, "label",
		":*:alias for $2"
		":de:Kurzform für $2"
	},
	{ "x", EDBPrintDef_alias, "export",
		":*:alias for $2"
		":de:Kurzform für $2"
	},
};

void SetupEDBPrint (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;
	AddEDBPrintDef(pdef, tabsize(pdef));
}
