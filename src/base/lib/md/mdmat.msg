/*
error messages for mdmat library

$Copyright (C) 1994 Erich Frühstück
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

1	#name	count	description\n

10	$!: $0: ATTENTION: old data format.\n
11	$!: bad magic in data file.\n
12	$!: wrong number of elments in data file.\n
13	$!: missing EOF mark.\n
14	$!: worng size of type $1.\n
15	$!: type $1 has no component $2.\n
16	$!: can not add values of type $1 (operator += not defined).\n

18	$!: binary operator $1 ($2, $3) is not defined.\n
19	$!: assignment operator $2::$1 & ($3) is not defined.\n

21	$!: object type $1 is not defined.\n
22	$!: can not read objects of type $1.\n
23	$!: can not read a component of type "$1".\n
24	$!: can not write objects of type $1.\n
25	$!: can not write a component of type "$1".\n

31	$!: wrong key in data file.\n
32	$!: error in dimension.\n
33	$!: missing data sector.\n

41	$!: counter is not defined.\n
42	$!: accumulation function is not defined.\n
43	$!: missing grouping information.\n
44	$!: undefined group $1.\n
45	$!: undefined counter $1.\n
46	$!: no matching label for partial classification.\n

51	$!: "$1" is not of type double.\n
52	$!: filed of type double "$1" - wrong object size.\n
53	$!: multiplication of "$1" with "$1":\n\
dimension not compatible.\n
54	$!: multiplication of "$1" with "$1":\n\
index values not compatible.\n

61	$!: mdmul: empty data cube(s).\n
62	$!: mdmul: axis "$1" - "$2" are not compatible (flags=$3).\n
63	$!: mdmul: incompatible number of axis.\n
64	$!: mdmul: can not multiply $1 with $2.\n
65	$!: mdmul: cann not add values of type $1.\n

71	$!: $0: missing column label.\n
72	$!: $0: incomplete line.\n
73	$!: $0: extra character: "$1".\n

81	$!: Character $1 in list where '=' is expected.\n

91	$!: mdterm: function not defined.\n
92	$!: incompatible axis $1 - $1 (differnd sizes).\n
93	$!: mdterm: function "$1" has no return value.\n
94	$!: mdterm: function "$1" is not defined.\n
95	$!: function $1 is not appropirate for queryng data of type $2.\n

101	$!: data cube has no component "$1".\n
102	$!: can not convert type "$1" into "$1".\n
103	$!: can not round data cube of type "$1".\n

191	$!: $0: incomplete line.\n
192	$!: $0: unexpected end of file, no data.\n
193	$!: $0: missing column seperator.\n
194	$!: $0: different labels ("$1" - "$1")\nby equal values.\n
195	$!: $0: incomplete axis label.\n
196	$!: $0: extra values in data file.\n
197	$!: $0: illegal character sequence "$1" in values.\n

201	$!: empty data cube by inversion.\n
202	$!: could not convert "$1" into double.\n
203	$!: inversion of data cube: not a square field.\n
204	$!: inversion of data cube: singulary data cube (dim=$1, range=$2).\n

301	$!: count object table is not initialized.\n
302	$!: classification table is not initialized.\n
303	$!: data cube nor prepared for counting.\n
304	$!: differing data type for additional classifications.\n
305	$!: time range $0: unexpected character $1.\n
