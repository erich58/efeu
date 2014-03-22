/*
error messages for edb functions

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

cmd	$!: unknown command $1.\n
cmp	$!: data file $1 unsorted.\n
conv	$!: don't know how to convert $1 in $2.\n
aggr	$!: aggregate function $1 is not definied for data type $2.\n
dbi	$!: invalid data file identification $1.\n
inc	$!: key $1 not compatible.\n
key	$!: join key not defined.\n
mem	$!: type $1 has no member $2.\n
mhdr	$0: wrong magic number.\n
noc	$!: use of C-expression not available.\n
ord	$!: invalid order of parameters.\n
type	$0: data type not defined.\n
typ	$!: unknown datatype $1.\n
uchar	$0: unexpected character $1.\n
use	$!: name $1 arready used.\n
var	$!: invalid declaration $1.\n
vec	$!: vector could only used on the whole.\n
xchar	$0: extra character in header ignored.\n
print	$!: type $1: undefinied print method $2.\n
input	$!: type $1: undefined input method $2.\n

nread	$0: $1 records read.\n
nwrite	$0: $1 records written.\n
