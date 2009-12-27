/*	Zeichen aus Eingabestruktur lesen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/EisDoc.h>
#include <ctype.h>

#define	TEST	0

#define	BSIZE	31	/* Buffergröße ohne Abschlußnull */

#define	KVAL(konv,input)	((input)->nokonv ? (konv).name : (konv).value)

static uchar_t buf[BSIZE + 1];

static int compose_key (const uchar_t *def, int c)
{
	if	(def)
	{
		for (; *def != 0; def++)
			if (*def == c)	return 1;
	}

	return 0;
}

static char *c2str (int c)
{
	buf[0] = c;
	buf[1] = 0;
	return buf;
}

static char *get_char (EisDocInput_t *input, int c)
{
	EisDocKonv_t *konv;
	int n, n0, i, i0, i1, i2;

	if	(c == EOF)	return NULL;

	if	(input->enc == NULL)	return c2str(c);

	if	(compose_key(input->enc->compose, c))
	{
		n = 0;
		buf[n++] = c;
		c = io_getc(input->io);

		if	(isalpha(c))	buf[n++] = c;
		else			io_ungetc(c, input->io);

		buf[n++] = 0;
		return buf;
	}

	if	(input->enc->dim == 0)	return c2str(c);

	konv = input->enc->konv;
	n = 0;
	i1 = 0;
	i2 = (int) input->enc->dim - 1;
	i0 = EOF;
	n0 = 1;

	do
	{
	/*	Bereich bestimmen
	*/
		while (i1 < i2)
		{
			i = (i1 + i2) / 2;

			if	(konv[i].name[n] < c)	i1 = i + 1;
			else if	(konv[i].name[n] > c)	i2 = i - 1;
			else				break;
		}

		while (i2 >= i1 && konv[i2].name[n] > c)
			i2--;

		while (i1 <= i2 && konv[i1].name[n] < c)
			i1++;

	/*	Zeichen buffern
	*/
		buf[n++] = c;

		if	(i1 > i2)	break;

	/*	Übereinstimmung speichern
	*/
		if	(konv[i1].name[n] == 0)
		{
			if	(i1 < i2)
			{
				n0 = n;
				i0 = i1;
				i1++;
			}
			else if	(input->nokonv)
			{
				buf[n++] = 0;
				return buf;
			}
			else	return konv[i1].value;
		}
#if	TEST
		for (i = 0; i < n; i++)
			io_putc(buf[i], ioerr);

		if	(i0 != EOF)
			io_printf(ioerr, " [%#s]", konv[i0].name);

		for (i = i1; i <= i2; i++)
			io_printf(ioerr, " %#s", konv[i].name);

		io_putc('\n', ioerr);
#endif
	}
	while (n < BSIZE && (c = io_getc(input->io)) != EOF);

	if	(n == 0)	return NULL;

/*	Reduktion auf letzte Übereinstimmung
*/
	while (n > n0)
	{
		n--;
		io_ungetc(buf[n], input->io);
	}

	if	(input->nokonv || i0 == EOF)
	{
		buf[n] = 0;
		return buf;
	}
	else	return konv[i0].value;
}

void EisDocMerge_char (EisDocMerge_t *eval, int c)
{
	EisDocOutput_char(eval->output, get_char(eval->input, c));
}
