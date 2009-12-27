/*	Zeichenkonvertierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/EisDoc.h>

#define	FILE_EXT	"enc"

#define	TEST	0

static VECBUF(enc_tab, 16, sizeof(EisDocEnc_t));
static VECBUF(konv_tab, 128, sizeof(EisDocKonv_t));

static int cmp_enc (EisDocEnc_t *a, EisDocEnc_t *b)
{
	return mstrcmp(a->name, b->name);
}

static EisDocEnc_t *load_enc (const char *encname)
{
	int c, flag;
	io_t *io;
	EisDocEnc_t *enc;
	char *name, *value;

	io = io_findopen(EisDocPath, encname, FILE_EXT, "r");

	if	(io == NULL)
	{
		message(NULL, MSG_EISDOC, 11, 1, encname);
		return NULL;
	}

	io = io_lnum(io);
	enc = vb_next(&enc_tab);
	enc->name = mstrcpy(encname);
	enc->compose = NULL;
	konv_tab.used = 0;

	while ((c = io_eat(io, "%s")) != EOF)
	{
		flag = 0;

		switch (c)
		{
		case '@':
			io_getc(io);
			flag = 1;
			break;
		case '\\':
			io_getc(io);
			c = io_getc(io);
			io_ungetc(c, io);

			if	(c != '@')
				io_ungetc('\\', io);

			break;
		default:
			break;
		}

		name = io_xgets(io, " \t");
		io_eat(io, " \t");
		value = io_xgets(io, "\n");

		if	(flag)
		{
			if	(mstrcmp(name, "compose") == 0)
			{
				memfree(enc->compose);
				enc->compose = value;
			}
			else
			{
				io_error(io, MSG_EISDOC, 12, 1, name);
				memfree(value);
			}

			memfree(name);
		}
		else
		{
			EisDocKonv_t *konv = vb_next(&konv_tab);
			konv->name = name;
			konv->value = value;
		}
	}

	enc->konv = konv_tab.data;
	enc->dim = konv_tab.used;
	konv_tab.data = NULL;
	konv_tab.size = 0;
	konv_tab.used = 0;
	EisDocEnc_sort(enc);
	return enc;
}

EisDocEnc_t *EisDocEnc (const char *name)
{
	EisDocEnc_t key, *enc;

	if	(name == NULL)	return NULL;

	key.name = (char *) name;
	enc = vb_search(&enc_tab, &key, (comp_t) cmp_enc, VB_SEARCH);

	if	(enc == NULL)
		enc = load_enc(name);

#if	TEST
	if	(enc)
		EisDocEnc_print(ioerr, enc);
#endif
	return enc;
}

static int cmp (EisDocKonv_t *ma, EisDocKonv_t *mb)
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

void EisDocEnc_sort (EisDocEnc_t *enc)
{
	if	(enc && enc->dim > 1)
		qsort(enc->konv, enc->dim, sizeof(EisDocKonv_t), (comp_t) cmp);
}

void EisDocEnc_print (io_t *io, EisDocEnc_t *enc)
{
	EisDocKonv_t *konv;
	size_t n;

	if	(!enc)	return;

	io_printf(io, "%s: compose = %#s konv = {\n", enc->name, enc->compose);

	for (n = enc->dim, konv = enc->konv; n-- > 0; konv++)
		io_printf(io, "\t%#s\t%#s\n", konv->name, konv->value);

	io_printf(io, "}\n");
}
