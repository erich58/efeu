/*
error messages for database files

$Copyright (C) 1995 Erich Frühstück
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

pos	$!: DBData: field position $1, width $2 out of range (recl = $3).\n
idx	$!: DBData: field index $1 out of range (dim = $2).\n
recl	$!: DBData: invalid record length.\n

11	$!: column $1: invalid digit $2 in value.\n
12	$!: column $1: error in BCD-value $2 (HIGH)\n
13	$!: column $1: error in BCD-value $2 (LOW)\n
14	$!: column $1: error in BCD-value $2 (SIGNUM).\n
15	$!: column $1: character $2 not alphanumeric.\n
