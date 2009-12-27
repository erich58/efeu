# :*:awk script for system specific parameters in headerfiles
# :de:AWK-Skript zum Einbau systemspezifischer Parameter in Headerdateien
#
# Copyright (C) 1999 Erich Fr�hst�ck
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# EFEU is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with EFEU; see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

/^@eval/ {
	gsub("@eval", eval)
	system($0)
	next
}

/@SRC@/  {
	gsub("@SRC@", FILENAME);
}

{
	print
}
