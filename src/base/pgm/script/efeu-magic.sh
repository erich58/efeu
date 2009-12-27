#!/bin/sh
# :*: update magic file for efile
# :de: Magic-File für efile erneuern
#
# $Copyright (C) 2000 Erich Frühstück
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
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

# $pconfig
# Version="$Id: efeu-magic.sh,v 1.2 2007-08-19 04:22:10 ef Exp $"

case "$1" in
-\?|--help*)	efeuman -- $0 $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

# $SeeAlso
# efile(1), file(1), magic(5).

top=`efeutop`
cd $top/etc || exit 1

cat > magic <<EOF
# magic data for file command.
# Format is described in magic(5).
# This file was collected by $0.
EOF

if [ ! -w magic ]; then
	exit 1
fi

add_magic ()
{
	while
		read name
	do
		cat - $name <<EOF

# ---------------------------------------------------------------------
# $name
#
EOF
	done
}

if [ -d magic.d ]; then
	find magic.d -type f -print | add_magic >> magic
fi

for x in /etc/magic /usr/share/misc/magic
do
	if [ -f $x ]; then
		echo $x | add_magic >> magic
	fi
done
