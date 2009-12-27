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

#define	XOUT_LIM	(2000 * 1024 * 1024)

static STRBUF(pdef_buf, 0);

char *EDBPrintDef_psub (const char *fmt, const char *opt, const char *arg)
{
	if	(!fmt)	return NULL;

	for (sb_clean(&pdef_buf); *fmt; fmt++)
	{
		if	(fmt[0] == '[' && fmt[1] == ']')
		{
			if	(opt)
			{
				sb_putc('[', &pdef_buf);
				sb_puts(opt, &pdef_buf);
				sb_putc(']', &pdef_buf);
			}

			fmt++;
		}
		else if	(fmt[0] == '=' && fmt[1] == '#')
		{
			if	(arg)
			{
				sb_putc('=', &pdef_buf);
				sb_puts(arg, &pdef_buf);
			}

			fmt++;
		}
		else	sb_putc(*fmt, &pdef_buf);
	}

	sb_putc(0, &pdef_buf);
	return (char *) pdef_buf.data;
}

void EDBPrintDef_alias (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	char *p = mstrcpy(EDBPrintDef_psub(def->par, opt, arg));
	edb_pmode(mode, NULL, p);
	memfree(p);
}

static void set_header (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->header = atoi(arg ? arg : def->par);
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

	lim = C_strtod(arg, &p);

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
	{ "?", NULL, show_def, NULL,
		":*:list print flags"
		":de:Auflisten der Ausgabeflags"
	},
	{ "default", NULL, EDBPrintDef_alias, "plain,verbosity=2,split=512M",
		":*:default flags: $2"
		":de:Standardvorgabe: $2"
	},
	{ "null", NULL, EDBPrint_null, NULL,
		":*:dummy output"
		":de:Leerausgabe"
	},
	{ "binary", NULL, EDBPrint_binary, NULL,
		":*:binary output"
		":de:Binärausgabe"
	},
	{ "plain", NULL, EDBPrint_plain, NULL,
		":*:plain output"
		":de:Textausgabe als Term"
	},
	{ "label", NULL, EDBPrint_label, NULL,
		":*:label output"
		":de:Textausgabe als Zeichenkette"
	},
	{ "data", "[opt]=def", EDBPrint_data, NULL,
		":*:data without declarations"
		":de:Nur Daten ohne Deklarationen ausgeben"
	},
	{ "fmt", "[head]=fmt", EDBPrint_fmt, NULL,
		":*:formatted output of data lines"
		":de:Formatierte Ausgabe von Datenzeilen"
	},

	{ "locale", "=lang", set_locale, NULL,
		":*:set locale for output"
		":de:Lokale für die Ausgabe setzen"
	},

	{ "verbosity", "=level", set_header, "1",
		":*:header verbosity level"
		":de:Wortumfang des Headers"
	},
	{ "nohead", NULL, EDBPrintDef_alias, "verbosity=0",
		":*:no header, alias for $2"
		":de:Kein Header, Kurzform für $2"
	},
	{ "compact", NULL, EDBPrintDef_alias, "verbosity=1",
		":*:compact header\nalias for $2"
		":de:Kompakter Header ohne Kommentare\nKurzform für $2"
	},
	{ "verbose", NULL, EDBPrintDef_alias, "verbosity=2",
		":*:verbose header\nalias for $2"
		":de:Header mit Kommentaren\nKurzform für $2"
	},
	
	{ "prec", "[width]=prec", set_prec, "18g",
		":*:precision of floating point numbers"
		":de:Genauigkeit von Gleitkommazahlen festlegen"
	},

	{ "split", "=limit", set_split, NULL,
		":*:split limit for output"
		":de:Aufspaltungslimit für die Ausgabe"
	},
	{ "nosplit", NULL, set_split, "0",
		":*:do not split output"
		":de:Ausgabe nicht aufspalten"
	},

	{ "export", "[opt]=def", EDBPrintDef_alias, "{data[]=#},nosplit",
		":*:export data\nalias for $2"
		":de:Daten exportieren\nKurzform für $2"
	},
	{ "csv", "=def", EDBPrintDef_alias,
		"{data[crlf]=#},nosplit,nohead,locale",
		":*:export data with CR/LF\nalias for $2"
		":de:Daten mit CR/LF exportieren\nKurzform für $2"
	},

	{ "b", NULL, EDBPrintDef_alias, "binary",
		":*:alias for $2"
		":de:Kurzform für $2"
	},
	{ "raw", NULL, EDBPrintDef_alias, "data=binary,nosplit",
		":*:alias for $2"
		":de:Kurzform für $2"
	},
	{ "l", NULL, EDBPrintDef_alias, "label",
		":*:alias for $2"
		":de:Kurzform für $2"
	},
	{ "x", "[opt]=def", EDBPrintDef_alias, "{export[]=#}",
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
