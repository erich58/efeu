/*	Eingabekonvertierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/vecbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/preproc.h>

#define	FILE_EXT	"enc"

#define	TEST	0

static VECBUF(enc_tab, 16, sizeof(MergeEnc_t));
static VECBUF(def_tab, 128, sizeof(MergeEncDef_t));

static int cmp_enc (MergeEnc_t *a, MergeEnc_t *b)
{
	return mstrcmp(a->name, b->name);
}

static MergeEnc_t *load_enc (const char *encname)
{
	int c;
	io_t *io;
	MergeEnc_t *enc;
	MergeEncDef_t *def;
	char *name, *value;

	io = io_findopen(IncPath, encname, FILE_EXT, "r");

	if	(io == NULL)
	{
		message(NULL, MSG_DOC, 41, 2, encname, FILE_EXT);
		return NULL;
	}

	io = io_lnum(io);
	enc = vb_next(&enc_tab);
	enc->name = mstrcpy(encname);
	def_tab.used = 0;

	while ((c = io_eat(io, "%s")) != EOF)
	{
		name = io_xgets(io, " \t");
		io_eat(io, " \t");
		value = io_xgets(io, "\n");

		def = vb_next(&def_tab);
		def->name = (uchar_t *) name;
		def->value = (uchar_t *) value;
	}

	enc->tab = def_tab.data;
	enc->dim = def_tab.used;
	def_tab.data = NULL;
	def_tab.size = 0;
	def_tab.used = 0;
	MergeEnc_sort(enc);
	return enc;
}

MergeEnc_t *MergeEnc_get (const char *name)
{
	MergeEnc_t key, *enc;

	if	(name == NULL)	return NULL;

	key.name = (char *) name;
	enc = vb_search(&enc_tab, &key, (comp_t) cmp_enc, VB_SEARCH);

	if	(enc == NULL)
		enc = load_enc(name);

	return enc;
}


/*	Konvertierungstabelle ausgeben
*/

void MergeEnc_print (io_t *io, MergeEnc_t *enc)
{
	MergeEncDef_t *tab;
	size_t n;

	if	(!enc)	return;

	io_printf(io, "MergeEnc_t %s[%d] = {\n", enc->name, enc->dim);

	for (n = enc->dim, tab = enc->tab; n-- > 0; tab++)
		io_printf(io, "\t%#s\t%#s\n", tab->name, tab->value);

	io_printf(io, "}\n");
}

/*	Konvertierungstabelle sortieren
*/

static int cmp (MergeEncDef_t *ma, MergeEncDef_t *mb)
{
	register uchar_t *a = ma->name;
	register uchar_t *b = mb->name;

	if	(a == NULL)	return (b ? -1 : 0);
	if	(b == NULL)	return 1;

	for (;;)
	{
		if	(*a < *b)	return -1;
		if	(*a > *b)	return 1;
		if	(*a == 0)	break;

		a++;
		b++;
	}
	
	return 0;
}

void MergeEnc_sort (MergeEnc_t *enc)
{
	if	(enc && enc->dim > 1)
		qsort(enc->tab, enc->dim, sizeof(MergeEncDef_t), (comp_t) cmp);
}
