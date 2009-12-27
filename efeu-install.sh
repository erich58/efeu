#!/bin/sh
# :*:Install EFEU in a standard place
# :de:EFEU an einem Standardort installieren.
#
# $Copyright (C) 2008 Erich Frühstück
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

UINST=efeu-uninstall
readonly UINST
id=$0

note ()
{
	fmt="$1"
	shift 1
	printf "$id: $fmt\n" "$@" >&2
}

usage ()
{
	cat << !
Usage: $id [options]

--help	This output
--local	Install files under /usr/local
--arch	Create archiv instead of installing files.
!
}

query ()
{
	key="[Y/n]: y\b"
	def=0

	case "$1" in
	-n)	def=1; key="[y/N]: n\b"; shift 1;;
	-y)	shift 1;;
	*)	;;
	esac

	while true
	do
		printf "$1 $key"
		read query_data

		case "$query_data" in
		[Yy]*)	return 0;;
		[Nn]*)	return 1;;
		?*)	echo "invalid input";;
		*)	return $def;
		esac
	done
}

# Parse command args

data=usr/
config=
arch=

while [ -n "$1" ]
do
	case "$1" in
	--help)
		usage
		exit 0
		;;
	--local)
		data=usr/local/
		config=usr/local/
		shift
		;;
	--arch)
		if [ -f CHANGELOG ]; then
			arch=`sed -e '2,$d' -e 's/^[^0-9]*//' CHANGELOG`
		else
			arch=x.x
		fi
		arch="efeu-$arch-`uname -m`.tgz"
		shift
		;;
	*)
		printf "$id: Unknown option %s\n" "$1"
		usage
		exit 1
		;;
	esac
done

ulist=

for x in /usr/bin /usr/local/bin
do
	if [ -f $x/$UINST ]; then
		ulist="$ulist $x"
	fi
done

if [ -n "$ulist" ]
then
	printf "Found previous installed EFEU-version in$ulist.\n"
	printf "This would be removed before installing the new one.\n"
fi

#query -y "Continue" || exit 0

if [ `id -u` != 0 ]; then
	note "Only root can do this."
	exit 1
fi

TMP=${TMPDIR:-/tmp}/efeu$$
mkdir $TMP || exit 1
chmod go-rwx $TMP
readonly TMP
trap "cd; rm -rf $TMP;" 0

mkdir -p $TMP/$data
mkdir -p $TMP/$config

doc=$TMP/$data/share/doc/efeu
mkdir -p $doc

for x in CHANGELOG README README.de
do
	test -f $x && cp $x $doc
done

if [ -d doc ]; then
	echo "Preparing files in doc"
	( cd doc; find . -depth | \
		cpio -padm -R root:root $doc )
fi

if [ -d etc ]; then
	echo "Preparing files in etc"
	find etc -depth | cpio -padm -R root:root $TMP/$config
fi

for x in bin lib man include
do
	test -d $x || continue
	echo "Preparing files in $x"
	find $x -depth | cpio -padm -R root:root $TMP/$data
done

chmod -R go-w $TMP

bin=$TMP/$data/bin
mkdir -p $bin

cat > $bin/$UINST << !
#!/bin/sh
# Uninstall efeu

if [ \`id -u\` != 0 ]; then
	echo "\$0: Only root can do this."
	exit 1;
fi

!

find $TMP -type d -exec chmod a+rx '{}' \;

(
	cd $TMP;
	find * -depth -type f -printf "rm -f /%p\n"
	find * -depth -type d -printf "rmdir -p /%p 2>/dev/null\n"
) >> $bin/$UINST

chmod a+x $bin/$UINST

if [ -n "$arch" ]; then
	echo $arch
	(cd $TMP; tar cz * ) > $arch
	exit 0
fi

x=/${data}bin/$UINST

if [ -f $x ]; then
	printf "Found previous installed EFEU-version in $x.\n"
	printf "This would be removed before installing the new one.\n"
	query -y "Continue" || exit 0
	$x
fi

echo "Installing data"
query -y "Continue" || exit 0
(cd $TMP; find * -depth | cpio -padm /)
