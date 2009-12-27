/*	Konvertieren eines Aggregat-Datenfiles
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include <EFEU/pconfig.h>
#include "oestz.h"
#include <EFEU/ftools.h>


/*	Variablen
*/

static char *List = NULL;
static char *DB = NULL;
static char *Header = NULL;
static char *TeXout = NULL;
static char *Kennung = NULL;
int TeXmode = 1;


static Var_t vardef[] = {
	{ "List",	&Type_str,	&List },
	{ "DB",		&Type_str,	&DB },
	{ "Header",	&Type_str,	&Header },
	{ "TeXmode",	&Type_bool,	&TeXmode },
	{ "TeXout",	&Type_str,	&TeXout },
	{ "Kennung",	&Type_str,	&Kennung },
};


int main (int narg, char **arg)
{
	OESTZ_HDR *hdr;
	io_t *io;
	io_t *DB_io;
	char *p;
	double *scale;
	int k, i, n;
	ulong_t count;
	ushort_t value;
	char *filename;

	libinit(arg[0]);
	SetupOestz();
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	if	(narg < 2)
	{
		usage(ioerr, UsageFmt, NULL);
		return EXIT_FAILURE;
	}

	DB_io = DB ? io_fileopen(DB, "wz") : NULL;

	for (k = 1; k < narg; k++)
	{
		filename = argval(arg[k]);

	/*	Aggregatfile oeffnen
	*/
		oestz_open(filename);
		hdr = oestz_hdr();

		if	(Kennung)	hdr->kennung = Kennung;

	/*	TeX - Dokumentation generieren
	*/
		if	(TeXout != NULL)
		{
			char *p;

			p = mstrcat(".", TeXout, TeXmode ? "tex" : "def", NULL);
			io = io_fileopen(p, "w");
			FREE(p);
			TeXout = NULL;
		}
		else	io = NULL;

		oestz_doc(io, hdr, TeXmode);
		io_close(io);

	/*	Gegenstands- und Kriteriennamen bestimmen
	*/
		oestz_names(hdr);

	/*	Headerdatei generieren
	*/
		if	(Header != NULL)
		{
			fname_t *fn;
			char *p;

			fn = strtofn(Header);
			fn->type = fn->type ? fn->type : "h";
			p = fntostr(fn);
			io = io_fileopen(p, "w");
			FREE(p);
			oestz_type(io, hdr, fn->name);
			io_close(io);
			Header = NULL;
		}

	/*	Datenfiles generieren
	*/
		if	(DB == NULL)
		{
			continue;
		}

		scale = oestz_scale(hdr, List);
		count = 0;
		n = 0;
	
		while ((p = oestz_get('E', 0)) == NULL)
		{
			for (i = 0; i < hdr->gdim; i++)
			{
				if	(i % 7 == 0)
				{
					p = oestz_get('G', 1);
					n = 2;
				}
	
				value = atof(oestz_parse(p, n, n + 11))
					* scale[i] + 0.5;
				put_MSBF(DB_io, &value, sizeof(value));
				n += 12;
			}
	
			for (i = 0; i < hdr->kdim; i++)
			{
				if	(i % 17 == 0)
				{
					p = oestz_get('A', 1);
					n = 2;
				}
	
				value = atoi(oestz_parse(p, n, n + 4));
				put_MSBF(DB_io, &value, sizeof(value));
				n += 5;
			}
	
			count++;
		}
	
		reg_set(1, filename);
		reg_set(2, msprintf("%ld", count));
		errmsg(NULL, 1);
		oestz_close();
	}

	io_close(DB_io);
	return 0;
}
