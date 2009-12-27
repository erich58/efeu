/*
:en:floating point exception values
:de:Gleitkommaausnahmewerte

$Copyright (C) 2002 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/math.h>

#if	_AIX

#include <fp.h>

#endif

#if	EFEU_NEED_ISNAN
extern int isnan (double value)
{
#if	_AIX
	return IS_NAN(value);
#else
	return (value != value) ? 1 : 0;
#endif
}
#endif

#if	EFEU_NEED_ISINF
extern int isinf (double value)
{
#if	_AIX
	return IS_INF(value);
#else
	if	(value < 0.)	return (0.5 * value == value) ? -1 : 0;
	if	(value > 0.)	return (0.5 * value == value) ? 1 : 0;

	return 0;
#endif
}
#endif

/*
Die Funktion |$1| dividiert das Argument durch 0 und liefert
damit einen der Gleitkommaausnahmewerte -Inf, NaN oder Inf.
*/

double ExceptionValue (double x)
{
	static double zero = 0.;
	return x / zero;
}
