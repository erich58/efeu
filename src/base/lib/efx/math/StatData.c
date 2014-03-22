/*
:*:statistic data types
:de:Statistische Datentypen

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

#include <EFEU/StatData.h>
#include <EFEU/stdtype.h>

static int cov_print (const EfiType *st, const void *data, IO *io)
{
	const CovarianceData *cov = data;
	return io_xprintf(io, "{ %.16g, %.16g, %.16g, %.16g, %.16g, %.16g }",
		cov->n, cov->x, cov->xx, cov->y, cov->yy, cov->xy);
}

EfiType Type_CovarianceData = SIMPLE_TYPE("CovarianceData", CovarianceData,
	&Type_VarianceData, cov_print);

void SetupStatData (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;

	CmdSetup_FrequencyData();
	CmdSetup_VarianceData();
	AddType(&Type_CovarianceData);
	/*
	CmdSetup_CovarianceData();
	*/
}
