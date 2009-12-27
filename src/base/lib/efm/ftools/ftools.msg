/*
error messages for file tools

$Copyright (C) 1997 Erich Frühstück
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

1	$!: file "$1": undefined access mode "$2".\n
2	$!: $0("$1", "$2"): execution failed.\n
3	$!: $0("$1", "$2"): could not open file.\n
5	$!: could not create temporary file.\n
6	$!: file $1 not found.\n

11	$!: closing a file not registered by filenotice().\n

21	$!: file $0: unexpected EOF.\n
22	$!: file $0: could not write data.\n

31	$!: $0: could not create tempoaray file $1.\n
32	$!: $0: could not truncate file $1.\n
33	$!: $0: output error in file $1.\n
34	$!: $0: input error in file $1.\n
35	$!: $0: seeking in file $1 failed.\n

41	$!: file $1.$2 not found.\n
