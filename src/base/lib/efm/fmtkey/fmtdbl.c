/*
Gleitkommawerte formatieren

$Copyright (C) 1994 Erich Fr�hst�ck
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/locale.h>
#include <EFEU/fmtkey.h>
#include <EFEU/math.h>
#include <float.h>
#include <ctype.h>

int float_align = 1;

static char *digit = "0123456789";

#define	LOCALE(name)	(Locale.print ? Locale.print->name : NULL)
#define	DP_KEY		(Locale.print ? Locale.print->decimal_point : ".")

static void f_fmt(StrBuf *sb, const FmtKey *key, int *sig, double val);
static void x_fmt(StrBuf *sb, const FmtKey *key, int *sig, double val);

static void put_exponent(StrBuf *sb, int exponent, int mode);
static void put_mantisse(StrBuf *sb, double val, int prec, int flags);


int fmt_double(IO *io, const FmtKey *key, double val)
{
	StrBuf *sb;
	int sig;

	if	(isnan(val))
		return io_printf(io, "%*s", key->width, "NaN");

	if	((sig = isinf(val)) != 0)
		return io_printf(io, "%*s", key->width - 1, sig < 0 ? 
			"-Inf" : "Inf");

	if	(val < 0)
	{
		sig = -1;
		val = -val;
	}
	else	sig = 1;

	sb = sb_create(32);

	switch (key->mode)
	{
	case 'f':
	case 'F':	f_fmt(sb, key, &sig, val); break;
	default:	x_fmt(sb, key, &sig, val); break;
	}

	ftool_addsig(sb, sig, key->flags);
	return ftool_ioalign(io, sb, key);
}

static int g_adjust(int mode, int exponent, int prec)
{
	switch (mode)
	{
	case 'g':
	case 'G':	break;
	default:	return 0;
	}

	if	(exponent < -3)	return 0;

	if	(float_align > 1)
		prec = float_align * ((prec + float_align - 1) / float_align);

	return exponent < prec + 3;
}

static void x_fmt(StrBuf *sb, const FmtKey *key, int *sig, double val)
{
	int exponent;
	int prec;
	double base;

/*	Pr�zession und Exponent bestimmen
*/
	prec = (key->flags & FMT_NOPREC) ? 6 : key->prec;
	base = 1.;

	for (exponent = 0; exponent < prec; exponent++)
		base *= 10;

	if	(val)
	{
		while (val < base)
		{
			val *= 10.;
			exponent--;
		}
	}
	else	*sig = 0;

	base *= 10.;

	while (val >= base)
	{
		val /= 10.;
		exponent++;
	}

/*	Mantisse runden
*/
	modf(val + 0.5, &val);

	if	(val >= base)	
	{
		val /= 10.;
		exponent++;
		modf(val + 0.5, &val);
	}

/*	Nachjustieren
*/
	if	(g_adjust(key->mode, exponent, prec))
	{
		prec -= exponent;
		exponent = 0;

		if	(float_align > 1)
		{
			while (prec % float_align != 0)
			{
				val *= 10.;
				prec++;
			}
		}
	}
	else
	{
		if	(float_align > 1)
		{
			while (prec % float_align != 0)
			{
				exponent--;
				prec++;
			}

			while (exponent % float_align != 0)
			{
				val *= 10.;
				exponent--;
			}
		}

		put_exponent(sb, exponent, key->mode);
	}

/*	Mantisse formatieren
*/
	put_mantisse(sb, val, prec, key->flags);
}


static void f_fmt(StrBuf *sb, const FmtKey *key, int *sig, double val)
{
	char *dp;
	int prec;
	int n;

	if	(key->flags & FMT_NOPREC)	prec = 2;
	else if	(key->flags & FMT_NEGPREC)	prec = -key->prec;
	else					prec = key->prec;

/*	Wert auf vorgegebene Genauigkeit standardisieren
*/
	n = prec;
	dp = (prec > 0 || (key->flags & FMT_ALTMODE)) ? DP_KEY : NULL;

	for (; n < 0; n++) val /= 10.;
	for (; n > 0; n--) val *= 10.;

	modf(val + 0.5, &val);

	if	(val == 0.)		*sig = 0;
	if	(val > DBL_MAX)		val = DBL_MAX;	/* Test auf INFINITY !!! */

	put_mantisse(sb, val, prec, key->flags);
}


static void put_exponent(StrBuf *sb, int exponent, int mode)
{
	char *p;
	int n;

	if	(exponent < 0)
	{
		p = LOCALE(negative_sign);
		exponent = -exponent;
	}
	else	p = LOCALE(positive_sign);

	for (n = 0; exponent > 0; n++)
	{
		sb_putc(digit[exponent % 10], sb);
		exponent /= 10;
	}

	if	(n == 0)
		sb_putc(digit[0], sb);

	sb_rputs(p, sb);
	sb_putc(isupper(mode) ? 'E' : 'e', sb);
}

static void put_mantisse(StrBuf *sb, double val, int prec, int flags)
{
	char *ts;
	double tmp;
	int n;

	ts = LOCALE(thousands_sep);

/*	Ziffern (und Trennzeichen) zwischenspeichern
*/
	if	(prec < 0)
	{
		if	(flags & FMT_ALTMODE)
			sb_rputs(DP_KEY, sb);

		for (n = 0; n < -prec; n++)
		{
			if	(ts && n && n % 3 == 0)
				sb_rputs(ts, sb);

			sb_putc(digit[0], sb);

			if	(val == 0.)	break;
		}
	}

	while (val || prec >= 0)
	{
		if	(prec == 0)
		{
			if	(sb->pos || (flags & FMT_ALTMODE))
				sb_rputs(DP_KEY, sb);
		}
		else if	(ts && prec % 3 == 0 && sb->pos && prec < 0)
		{
			sb_rputs(ts, sb);
		}

		if	(val > 9.5)
		{
			tmp = modf(val / 10., &val);
			sb_putc(digit[(int) (10. * tmp + 0.1)], sb);
		}
		else
		{
			sb_putc(digit[(int) (val + 0.1)], sb);
			val = 0.;
		}

		prec--;
	}
}
