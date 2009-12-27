/*	KOBE - Datenbank in multidimensionale Matrix umwandeln
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/ftools.h>
#include <EFEU/pconfig.h>
#include <EFEU/mdmat.h>

#define	DB_MAGIC	0xEFA1	/* Kennung einer KOBE-Datenbank */
#define	DB_EOF		0xEFAF	/* EOF-Marke fuer KOBE-Datenbank */

static unsigned get_2byte (FILE *file);
static unsigned get_4byte (FILE *file);

typedef struct {
	unsigned dim;
	unsigned offset;
} POS;


/*	Resourcedefinitionen
*/

static char *Input = NULL;
static char *Output = NULL;
static char *Name = "sv";
static char *Title = NULL;
static char *DatName = "monat";
static char DatFmt = 't';
static int Cflag = 0;

static Var_t vardef[] = {
	{ "Input",	&Type_str,	&Input },
	{ "Output",	&Type_str,	&Output },
	{ "Name",	&Type_str,	&Name },
	{ "Title",	&Type_str,	&Title },
	{ "DatName",	&Type_str,	&DatName },
	{ "DatFmt",	&Type_char,	&DatFmt },
	{ "Cflag",	&Type_bool,	&Cflag },
};


/*	Hauptprogramm
*/

int main(int narg, char **arg)
{
	FILE *file;
	mdmat_t md;
	unsigned x;
	mdaxis_t *axis;
	char *p;
	POS *pos;
	int i, j, k;

	libinit(arg[0]);
	pconfig(NULL, vardef, tabsize(vardef));
	SetupMdMat();
	loadarg(&narg, arg);

/*	KOBE - Datenbank öffnen und Magic - Nummer testen
*/
	file = fileopen(Input, "rz");
	x = get_2byte(file);

	if	(x != DB_MAGIC)	liberror(NULL, 1);

/*	Datenbanktitel
*/
	x = get_2byte(file);
	md.title = memalloc(x);
	fread(md.title, x, 1, file);

	if	(Title)	md.title = Title;

/*	Datentype
*/
	md.type = &Type_int;
	md.data = NULL;
	md.priv = NULL;

/*	Achsen mit Personengruppen lesen
*/
	x = get_2byte(file);
	md.axis = new_axis(x);
	md.axis->name = Name;

	x = get_2byte(file);
	p = memalloc(x);
	fread(p, x, 1, file);

	for (i = 0; i < md.axis->dim; i++, p++)
	{
		md.axis->idx[i].name = p;

		while (*p != 0)
			p++;
	}

	pos = ALLOC(md.axis->dim, POS);

	for (i = 0; i < md.axis->dim; i++, p++)
	{
		pos[i].dim = get_2byte(file);
		pos[i].offset = get_4byte(file);
	}

/*	Stichtagsdimension
*/
	x = get_2byte(file);
	axis = new_axis(x);
	md.axis->next = axis;
	axis->name = DatName;

	for (i = 0; i < axis->dim; i++)
		axis->idx[i].flags = 0;

/*	Wertedimension
*/
	x = get_2byte(file);

	axis = new_axis(x + 3);
	md.axis->next->next = axis;
	axis->name = "kobe";
	axis->idx[0].name = "MAX";
	axis->idx[1].name = "ZUG";
	axis->idx[2].name = "ABG";
	axis->idx[3].name = "ANZ";

	for (i = 1; i < x; i++)
		axis->idx[i+3].name = msprintf("B%02d", i - 1);

/*	Endekennung testen
*/
	(void) get_2byte(file);
	(void) get_2byte(file);
	x = get_4byte(file);
	fseek(file, x, SEEK_SET);
	x = get_2byte(file);

	if	(x != DB_EOF)	liberror(NULL, 2);

/*	Initialisieren
*/
	md.size = md_size(md.axis, md.type->size);
	md.data = memalloc(md.size);
	memset(md.data, 0, md.size);
	p = md.data;

/*	Daten laden
*/
	axis = md.axis->next;

	for (i = 0; i < md.axis->dim; i++)
	{
		unsigned *data;
		unsigned last;

		if	(fseek(file, pos[i].offset, SEEK_SET) == EOF)
			liberror(NULL, 3);
		
		last = 0;

		for (j = 0; j < pos[i].dim; j++)
		{
			x = get_2byte(file);

			if	(axis->idx[j].flags == 0)
			{
				axis->idx[j].flags = x;
			}
			else if	(axis->idx[j].flags != x)
			{
				liberror(NULL, 4);
			}

			data = (unsigned *) (p + j * axis->size);
			x = get_2byte(file);
			data[0] = x - 1;

			for (k = 0; k < x; k++)
				data[k+3] = get_4byte(file);

			data[1] = data[4];

			if	(j != 0)
			{
				data[2] = last - data[3] + data[1];
			}
			else	data[2] = 0;

			last = data[3];

			if	(Cflag)
				for (k = x + 1; k > 3; k--)
					data[k] += data[k + 1];
		}

		p += md.axis->size;
	}

/*	Ausgabe der Datenmatrix
*/
	axis = md.axis->next;

	for (i = 0; i < axis->dim; i++)
	{
		int j, m, t;

		t = axis->idx[i].flags % 32;
		m = (axis->idx[i].flags / 32) % 12 + 1;
		j = axis->idx[i].flags / (12 * 32);

		switch (DatFmt)
		{
		case 'x':
		case 'X':
			axis->idx[i].name = msprintf("%d.%d", m, 1900 + j);
			break;
		case 't':
		case 'T':
			axis->idx[i].name = msprintf("%02d%02d%02d", j, m, t);
			break;
		default:
			axis->idx[i].name = msprintf("%02d%02d", j, m);
			break;
		}
	}

	md_fsave(Output, &md, 0);
	return 0;
}


static unsigned get_2byte(FILE *file)
{
	unsigned x;

	x = getc(file);
	x = (x << 8) + getc(file);
	return x;
}


static unsigned get_4byte(FILE *file)
{
	unsigned x;

	x = getc(file);
	x = (x << 8) + getc(file);
	x = (x << 8) + getc(file);
	x = (x << 8) + getc(file);
	return x;
}
