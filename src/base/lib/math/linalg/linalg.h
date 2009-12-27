/*
Hilfsprogramnme zur Lineare Algebra

$Header <Math/$1>

$Copyright (C) 1997 Erich Frühstück
This file is part of Math.

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

#ifndef	MATH_LINALG_H
#define	MATH_LINALG_H	1

#include <EFEU/config.h>
#include <EFEU/types.h>

size_t GaussJordan (double *data, size_t dim);

#endif	/* MATH_LINALG_H */
