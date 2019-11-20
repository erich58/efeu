/*
:*:Age in the range of 0 to 99
:de:Alter im Bereich 0 bis 99

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

config	hdr src

#include <macro.ms>
#include <TypeDef.ms>

StdInclude(src, paste("/", "EfiType", BaseName));

TypeDef def = TypeDef(BaseName, "Age in the range from 0 to 99");
def.t_cname = "uint8_t";
def.t_recl = 1;
def.print_body = "io_xprintf(io, \"%02d\", *data);";
def.create(true);

#include "age_class.def"

StdFunc("void $1_set", "$1 *ptr, int age", string !
if	(age < 0)	*ptr = 0;
else if	(age < 100)	*ptr = age;
else	 		*ptr = 99;
!, BaseName);

def.func("from_int", "$1 $1 (int)", string !
$1_set(rval, Val_int(arg[0]));
!);

def.func("to_int", "int $1 ()", string !
Val_int(rval) = (($1 *) arg[0])[0];
!);

def.func("from_str", "$1 $1 (str)", string !
char *def = Val_str(arg[0]);
int age;

if	(!def)		age = 0;
else if	(def[0] == 'A')	age = strtol(def + 1, NULL, 10);
else			age = strtol(def, NULL, 10);

$1_set(rval, age);
!);

def.func("to_str", "str $1 ()", string !
Val_str(rval) = msprintf("A%02d", (($1 *) arg[0])[0]);
!);

def.setup(true);
