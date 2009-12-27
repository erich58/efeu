/*	Textteile eines Aggregat-Datenfiles ausgeben
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include "oestz.h"
#include <math.h>


static void printgrp (io_t *io, const char *fmt, int flag);

#define	FORMAT		": $(#1:%3s) - $2\n"
#define	mkformat(x)	mstrcat(FORMAT, x, "", NULL)


void oestz_doc(io_t *io, OESTZ_HDR *hdr, int flag)
{
	size_t i;	/* Hilfszaehler */
	char *p;	/* Hilfspointer */
	char *fmt;	/* Hilfsformat */

/*	Test auf Textteile
*/
	if	((p = oestz_get('T', 0)) == NULL)
	{
		return;
	}

/*	Titel des Aggregatfiles
*/
	reg_cpy(0, hdr->kennung);
	reg_set(1, oestz_TeXstr(hdr->erhebung));
	reg_set(2, oestz_TeXstr(hdr->objekt));

	if	(flag)
	{
		oestz_msg(io, 11);
		fmt = mkformat(OestzHflag);
		io_psub(iostd, fmt);
		FREE(fmt);
	}
	else	oestz_msg(io, 111);

/*	Selektordefinitionen
*/
	if	(flag && hdr->sdim)	oestz_msg(io, 12);

	fmt = mkformat(OestzSflag);

	for (i = 0; i < hdr->sdim; i++)
	{
		printgrp(flag ? io : NULL, fmt, flag);
	}

	FREE(fmt);

/*	Gegenstandsbeschreibungen lesen
*/
	if	(flag)	oestz_msg(io, 13);

	fmt = mkformat(OestzGflag);

	for (i = 0; i < hdr->gdim; i++)
	{
		p = oestz_get('1', 1);

		reg_set(1, msprintf("%d", atoi(oestz_parse(p, 2, 6))));
		reg_set(2, oestz_TeXstr(oestz_parse(p, 7, 36)));
		reg_set(3, msprintf("%d", 16));

		if	(flag)
		{
			oestz_msg(io, 21);
			io_psub(iostd, fmt);
		}
		else	oestz_msg(io, 121);
	}

	FREE(fmt);

/*	Kriterien - Definitionen
*/
	if	(flag)	oestz_msg(io, 14);

	fmt = mkformat(OestzKflag);

	for (i = 0; i < hdr->kdim; i++)
	{
		printgrp(io, fmt, flag);
	}

	FREE(fmt);

	if	(flag)	oestz_msg(io, 15);
}


static void printgrp(io_t *io, const char *fmt, int flag)
{
	int i, j, n;
	size_t dim;
	char *p;
	int mgrp;

	p = oestz_get('2', 1);
	reg_cpy(1, oestz_parse(p, 2, 4));
	reg_set(2, oestz_TeXstr(oestz_parse(p, 5, 69)));
	dim = atoi(oestz_parse(p, 70, 74));

	frexp(dim + 1., &n);
	reg_set(3, msprintf("%d", n));
	reg_set(4, msprintf("%d", dim + 1));
	n = atoi(oestz_parse(p, 76, 80));

	oestz_msg(io, flag ? 21 : 122);
	io_psub(iostd, fmt);

	if	(!flag)	io = NULL;

/*	Kommentare
*/
	if	(n > 0)
	{
		/*
		oestz_msg(io, 22);
		*/

		for (i = 0; i < n; i++)
		{
			p = oestz_get('4', 1);
			oestz_TeXput(io, oestz_parse(p, 11, 82));
			/*
			io_puts("~\\\\\n", io);
			*/
			io_putc('\n', io);
		}

		/*
		oestz_msg(io, 23);
		*/
		io_putc('\n', io);
	}

/*	Gruppennamen
*/
	/*
	mgrp = dim < 150 ? 30 : 40;
	*/
	mgrp = 30;
	reg_set(1, msprintf("%d", dim));
	oestz_msg(io, mgrp + 1);

	for (i = 1; (p = oestz_get('3', 0)) != NULL; i++)
	{
		reg_set(1, msprintf("%d", i));
		reg_set(2, oestz_TeXstr(oestz_parse(p, 10, 54)));
		oestz_msg(io, mgrp + 2);

		n = atoi(oestz_parse(p, 71, 75));

		if	(n == 0)
		{
			oestz_msg(io, mgrp + 3);
			continue;
		}

		oestz_msg(io, mgrp + 4);

		for (j = 0; j < n; j++)
		{
			p = oestz_get('4', 1);

			if	(mgrp == 30)
			{
				oestz_TeXput(io, oestz_parse(p, 11, 82));
				io_putc('\n', io);
			}
		}

		oestz_msg(io, mgrp + 5);
	}

	oestz_msg(io, mgrp + 6);
}


void oestz_msg(io_t *io, int num)
{
	if	(io != NULL)
	{
		io_psub(io, getmsg(OESTZ, num, NULL));
	}
}
