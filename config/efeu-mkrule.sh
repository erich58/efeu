#!/bin/sh
# :*:create make rules for configuration files
# :de:Make-Regeln für Konfigurationsdateien
#
# $Copyright (C) 2019 Erich Frühstück
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

#	message formats

msg1="usage: $0 [--help] [opt] src tg\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] [opt] src tg\n"
	;;
esac

# $pconfig
# Version=":VN:"
# a:name |
#	:*:name of install target
# c:name |
#	:*:name of clean target
# l:lang|
#	:*:list of languages
#	:de:Liste der Sprachkennungen
# :src|
#	:*:file with source file names
#	:de:Datei mit Eingabedateien
# :dir|
#	:*:template for target directory
#	:de:Template für Zielverzeichnis

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

#	Set convention

case "$0" in
*.sh)	cmd="sh $0";;
*)	cmd="$0";;
esac

#	Parse command args

all=all
clean=clean
lang=

while getopts "a:c:l:" opt
do
	case $opt in
	a)	all="$OPTARG";;
	c)	clean="$OPTARG";;
	l)	lang="$OPTARG";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if	[ $# -ne 2 ]
then	usage -?; exit 1
fi

src=$1
tpl=$2
shift 2

awk -v all="$all" -v clean="$clean" -v lang="$lang" -v tpl="$tpl" '
BEGIN {
	gsub(",","|",lang);
	pattern="/(" lang ")$"
}
{
	n = match($0, "/[^/]*$");

	if	(n == 0)
		next

	f = substr($0, n + 1);
	d = substr($0, 1, n - 1);

	n = match(f, "[.][^.]*$");

	if	(n)
	{
		b = substr(f, 1, n - 1);
	}

	n = match(d, pattern);

	if	(n)
	{
		l = substr(d, n);
		d = substr(d, 1, n - 1)
	}
	else
	{
		l = ""
	}

	t = tpl;
	gsub("/%L", l, t);
	gsub("%B", b, t);
	gsub("%F", f, t);
	gsub("%D", d, t);
	printf("%s:: %s\n\n", all, t);
	printf("%s::\n\trm -f %s\n\n", clean, t);
	printf("%s: %s\n\t@mkdir -p $(@D)\n\tcp $< $@\n\n", t, $0);
}
' $src
