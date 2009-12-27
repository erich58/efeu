/*
:*:time period data
:de:Zeitramdaten

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

TypeDef def = TypeDef(BaseName, "time period");

def.addentry(string !
Date	beg	Beginndatum
Date	end	Endedatum
!);

def.read_body = string !
data->beg = io_ullget(io, 3);
data->end = io_ullget(io, 3);
size = 6;
!;

def.write_body = string !
io_ullput(data->beg, io, 3);
io_ullput(data->end, io, 3);
size = 6;
!;

def.copy_body = "*tg = *src;";
def.clean_body = "data->end = data->beg = 0;";
def.print_body = string !
io_putc('"', io);
$1_put(data, io, 0);
io_putc('"', io);
!;

def.func("from_str", "$1 $1 (str def)", string !
$1_set(rval, Val_str(arg[0]));
!);

def.func("from_date", "$1 $1 (Date dat)", string !
$1 *data = rval;
data->beg = data->end = CalBaseStd(Val_int(arg[0]));
!);

def.func("from_date2", "$1 $1 (Date beg, Date end)", string !
$1 *data = rval;
data->beg = CalBaseStd(Val_int(arg[0]));
data->end = CalBaseStd(Val_int(arg[1]));
!);

def.func("to_str", "str $1 ()", string !
StrBuf *buf = sb_acquire();
IO *io = io_strbuf(buf);
$1_put(arg[0], io, 0);
io_close(io);
Val_str(rval) = sb_cpyrelease(buf);
!);

def.func("tidx", "virtual Object operator[] ($1[], restricted Date t)", string !
EfiVec *vec = Val_ptr(arg[0]);
int t = Val_Date(arg[1]);
int n;

for (n = 0; n < vec->buf.used; n++)
{
	$1 *ptr = (void *) ((char *) vec->buf.data + n * vec->buf.elsize);

	if	(ptr->beg <= t && t <= ptr->end)
	{
		Val_obj(rval) = ConstObj(vec->type, ptr);
		return;
	}
}

Val_obj(rval) = ConstObj(vec->type, vec->type->defval);
!);

def.create(true);

#include "stock.def"
#include "duration.def"

def.setup(true);

GlobalFunction("void $1_set", "$1 *par, const char *def", string !
par->beg = par->end = 0;

if	(def == NULL)	return;

if	(*def != ':')
{
	char *p = NULL;
	par->beg = str2Calendar(def, &p, 0);

	if	(p && *p == ':')
		def = p + 1;
}
else	def++;

par->end = str2Calendar(def, NULL, 1);
!, BaseName);

GlobalFunction("int $1_put", "const $1 *par, IO *out, int fix", string !
int n = 0;

if	(par->beg)
	n += PrintCalendar(out, "%Y-%m-%d", par->beg);
else if (fix)
	n += io_nputc(' ', out, 10);

n += io_puts(":", out);

if	(par->end)
	n += PrintCalendar(out, "%Y-%m-%d", par->end);
else if (fix)
	n += io_nputc(' ', out, 10);

return n;
!, BaseName);
