/*
:*:statistic data types
:de:Statistische Datentypen

$Header <EFEU/$1>

$Copyright (C) 2006 Erich Frühstück
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

#ifndef	EFEU_StatData_h
#define	EFEU_StatData_h	1

#include <EFEU/config.h>
#include <EFEU/object.h>
#include <EFEU/math.h>

typedef struct {
	double n;
	double x;
} FrequencyData;

typedef struct {
	double n;
	double x;
	double xx;
} VarianceData;

typedef struct {
	double n;
	double x;
	double xx;
	double y;
	double yy;
	double xy;
} CovarianceData;

extern EfiType Type_FrequencyData;
extern EfiType Type_VarianceData;
extern EfiType Type_CovarianceData;

#define	Val_FrequencyData(x)	((FrequencyData *) x)[0]
#define	Val_VarianceData(x)	((VarianceData *) x)[0]
#define	Val_CovarianceData(x)	((CovarianceData *) x)[0]

void CmdSetup_FrequencyData (void);
void CmdSetup_VarianceData (void);
void CmdSetup_CovarianceData (void);
void SetupStatData (void);

#endif	/* EFEU/StatData.h */
