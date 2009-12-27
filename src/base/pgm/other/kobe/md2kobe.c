/*	KOBE - Datenbank generieren
	(c) 1992 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 2.0
*/

#include <EFEU/ftools.h>
#include <EFEU/pconfig.h>
#include <EFEU/mdmat.h>

#define	DB_MAGIC	0xEFA1	/* Kennung einer KOBE-Datenbank */
#define	DB_EOF		0xEFAF	/* EOF-Marke fuer KOBE-Datenbank */

static void put_2 (unsigned val, FILE *file);
static void put_4 (unsigned val, FILE *file);
static void puttext (const char *str, size_t size, FILE *file);
static void put_std (int *data, size_t dim, FILE *file);
static void put_class (int *data, size_t dim, FILE *file);

/*	Resourcedefinitionen
*/

static char *Input = NULL;
static char *Output = NULL;
static char DatFmt = 't';
static int Cflag = 0;

static Var_t vardef[] = {
	{ "Input",	&Type_str,	&Input },
	{ "Output",	&Type_str,	&Output },
	{ "DatFmt",	&Type_char,	&DatFmt },
	{ "Cflag",	&Type_bool,	&Cflag },
};


size_t grpsize (mdaxis_t *axis, size_t len);
void grpprint (mdaxis_t *axis, const char *name, FILE *file);


/*	Hauptprogramm
*/

int main(int narg, char **arg)

{
	mdmat_t *md;		/* Multidimensionale Matrix */
	mdaxis_t **ptr;		/* Hilfspointer */
	mdaxis_t *kobe;		/* Kobe - Achsen */
	FILE *file;		/* Ausgabefile */
	size_t gdim;		/* Gruppendimension */
	long offset;		/* Fileposition der Dimensionen */
	long eofpos;		/* Fileposition der EOF - Marke */
	long *pos;		/* Positionspointer */
	size_t n;		/* Hilfsvariable */
	unsigned *stag;		/* Stichtage */
	int *data;		/* Datenpointer */

/*	Aufrufparameter und Formatdefinitionen laden
*/
	libinit(arg[0]);
	pconfig(NULL, vardef, tabsize(vardef));
	SetupMdMat();
	loadarg(&narg, arg);

/*	Datenmatrix laden
*/
	md = md_fload(Input, NULL, NULL);

	if	(md->type != &Type_int)
	{
		reg_cpy(1, md->type->name);
		liberror(NULL, 1);
	}

	if	(md->axis == NULL || md->axis->next == NULL)
		liberror(NULL, 2);

/*	KOBE - Achsen abtrennen und Gruppendimension bestimmen
*/
	ptr = &md->axis;
	gdim = 1;

	while ((*ptr)->next->next != NULL)
	{
		gdim *= (*ptr)->dim;
		ptr = &(*ptr)->next;
	}

	kobe = *ptr;
	*ptr = NULL;

	if	(md->axis == NULL)
		liberror(NULL, 2);

/*	Stichtagsvektor generieren
*/
	stag = ALLOC(kobe->dim, unsigned);

	for (n = 0; n < kobe->dim; n++)
	{
		int x, t, m, j;
		char *ptr;

		switch (DatFmt)
		{
		case 'x':
		case 'X':
			t = 0;
			m = strtol(kobe->idx[n].name, &ptr, 10);
			j = atoi(ptr + 1) - 1900;
			break;
		case 't':
		case 'T':
			x = atoi(kobe->idx[n].name);
			t = x % 100;
			m = (x / 100) % 100;
			j = x / 10000;
			break;
		default:
			x = atoi(kobe->idx[n].name);
			t = 0;
			m = x % 100;
			j = x / 100;
			break;
		}

		stag[n] = (12 * j + m - 1) * 32 + t;
	}

/*	Ausgabefile oeffnen
*/
	file = fileopen(Output, "wz");
	put_2(DB_MAGIC, file);

/*	Titel Ausgeben
*/
	if	(md->title)
	{
		puttext(md->title, strlen(md->title) + 1, file);
	}
	else	put_2(0, file);

/*	Gruppennamen ausgeben
*/
	n = grpsize(md->axis, 0);
	put_2(gdim, file);
	put_2(n + n % 2, file);
	grpprint(md->axis, NULL, file);

	if	(n % 2)	putc(0, file);
	
/*	Dummywerte fuer Filepositionen und Parameter ausgeben
*/
	offset = ftell(file);
	n = 6 * gdim + 12;

	while (n-- > 0)
		putc(0, file);

/*	Daten ausgeben
*/
	pos = ALLOC(gdim, long);
	data = md->data;

	for (n = 0; n < gdim; n++)
	{
		int k, l;
		int kdim;

		pos[n] = ftell(file);
		kdim = kobe->next->dim;

		for (k = 0; k < kobe->dim; k++)
		{
			l = data[0] + 1;
			l = min(l, kdim - 3);
			put_2(stag[k], file);
			put_2(l, file);

			if	(Cflag)
			{
				put_class(data + 3, l, file);
			}
			else	put_std(data + 3, l, file);

			data += kdim;
		}
	}

	eofpos = ftell(file);

/*	Filepositionen und Parameter ausgeben
*/
	fseek(file, offset, SEEK_SET);

	for (n = 0; n < gdim; n++)
	{
		put_2(kobe->dim, file);
		put_4(pos[n], file);
	}

	put_2(kobe->dim, file);
	put_2(kobe->next->dim - 3, file);
	put_2(stag[0], file);
	put_2(stag[kobe->dim - 1], file);
	put_4(eofpos, file);

	fseek(file, eofpos, SEEK_SET);
	put_2(DB_EOF, file);
	fileclose(file);
	return 0;
}


/*	Ausgabe von Text
*/

void puttext(const char *str, size_t n, FILE *file)
{
	put_2(n + n % 2, file);
	fwrite(str, n, 1, file);

	if	(n % 2)	putc(0, file);
}


/*	Ausgabe von Zahlenwerten
*/

void put_2(unsigned x, FILE *file)
{
	putc(((x >> 8) & 0xFF), file);
	putc((x & 0xFF), file);
}

void put_4(unsigned x, FILE *file)
{
	putc(((x >> 24) & 0xFF), file);
	putc(((x >> 16) & 0xFF), file);
	putc(((x >> 8) & 0xFF), file);
	putc((x & 0xFF), file);
}

/*	Bestimmung des Speicherbedarfs fuer Gruppennamen
*/

size_t grpsize(mdaxis_t *x, size_t len)
{
	int i;
	int s;

	if	(len != 0)	len++;
	if	(x == NULL)	return len;

	for (i = s = 0; i < x->dim; i++)
		s += grpsize(x->next, len + strlen(x->idx[i].name));

	return s;
}

void grpprint(mdaxis_t *x, const char *name, FILE *file)
{
	int i;
	char *p;

	if	(x == NULL)
	{
		if	(name)
		{
			fputs(name, file);
			putc(0, file);
		}

		return;
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(name)
		{
			p = mstrcat(".", name, x->idx[i].name, NULL);
			grpprint(x->next, p, file);
			FREE(p);
		}
		else	grpprint(x->next, x->idx[i].name, file);
	}
}

static void put_std (int *data, size_t dim, FILE *file)
{
	int i;

	for (i = 0; i < dim; i++)
		put_4(data[i], file);
}

static void put_class (int *data, size_t dim, FILE *file)
{
	int i;

	dim--;
	put_4(data[0], file);

	for (i = 1; i < dim; i++)
		put_4(data[i] - data[i + 1], file);

	put_4(data[dim], file);
}
