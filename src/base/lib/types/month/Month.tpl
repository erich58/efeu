/*
:*:month data
:de:Monatsdaten

$Header <EfiType/$1>

$Copyright (C) 2007 Erich Frühstück
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

config hdr src

#include <TypeDef.ms>

StdInclude(hdr, "EFEU/calendar");
StdInclude(hdr, "EFEU/EfiView");
StdInclude(src, paste("/", "EfiType", BaseName));

TypeDef def = TypeDef(BaseName, "month");

def.addentry(string !
int	year:12	Year
int	month:4	Month
!);

def.copy_body = "*tg = *src;";
def.clean_body = "data->year = data->month = 0;";
def.print_body = string !
if	(data->year)
	io_xprintf(io, "\"%4d-%02d\"", data->year, data->month);
else	io_puts("NULL", io);
!;

def.func("from_str", "$1 $1 (str def)", string !
$1_set(rval, str2Calendar(Val_str(arg[0]), NULL, 0));
!);

def.func("from_date", "$1 $1 (Date dat)", string !
$1_set(rval, Val_int(arg[0]));
!);

def.func("to_str", "str $1 ()", string !
$1 *mon = arg[0];
Val_str(rval) = mon->year ? msprintf("%4d-%02d", mon->year, mon->month) : NULL;
!);

def.func("tidx", "virtual Object operator[] ($1[], restricted Date t)", string !
EfiVec *vec = Val_ptr(arg[0]);
CalInfo *cal = Calendar(Val_Date(arg[1]), NULL);
int n;

for (n = 0; n < vec->buf.used; n++)
{
	$1 *ptr = (void *) ((char *) vec->buf.data + n * vec->buf.elsize);

	if	(ptr->year == cal->year && ptr->month == cal->month)
	{
		Val_obj(rval) = ConstObj(vec->type, ptr);
		return;
	}
}

Val_obj(rval) = ConstObj(vec->type, vec->type->defval);
!);

def.create(true);

def.setup(true);

GlobalFunction("void $1_set", "$1 *par, int idx", string !
CalInfo *cal = Calendar(idx, NULL);
par->year = cal->year;
par->month = cal->month;
!, BaseName);
