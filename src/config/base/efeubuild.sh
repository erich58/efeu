#!/bin/sh
# :*:create main Makefile for EFEU-projects
# :de:Hauptmakefile für EFEU-Projekte generieren
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

# message formats

msg1="usage: $0 [top]\n"
msg2="directory %s does not exist!\n"
msg3="create rules for %s\n"
msg4="create directory %s\n"
msg5="*** setting up %s ***\n"
msg6="Makefile in %s successfull created.\n"
msg7="Setup failed for project(s):%s.\n"

lbl_usage="usage:"
lbl_init="init Makefiles"
lbl_all="build all targets"
lbl_clean="clean all targets"
lbl_failed="setup failed:"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [top]\n"
	msg2="Bibliothek %s existiert nicht!\n"
	msg3="Generierungsregeln für %s einrichten\n"
	msg4="Bibliothek %s einrichten\n"
	msg5="*** Initialisierung von %s ***\n"
	msg6="Makefile in %s wurde generiert.\n"
	msg7="Initialisierung für Projekt(e)%s fehlgeschlagen.\n"

	lbl_usage="Aufruf:"
	lbl_init="Neugenerierung der Makefiles"
	lbl_all="Alle Ziele generieren"
	lbl_clean="Alle Ziele löschen"
	lbl_failed="Fehlgeschlagene Initialisierungen:"
	;;
esac

# $pconfig
# :top|
#	:*:TOP-directory for EFEU-projects
#	:de:Hauptverzeichnis der EFEU-Projekte

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
esac

top=${EFEUTOP:=~}

# parse command args

if [ $# -gt 1 ]; then
	usage; exit 1
elif [ $# -eq 1 ]; then
	top=$1
fi

# go to top, get absolut path

cd $top || exit 1
top=`pwd`

# test for source directory

if	[ ! -d $top/src ]; then
	printf "$msg2" $top/src
	exit 1
fi

printf "$msg3" $top

# create basic directories if neccessary

for x in bin lib build
do
	if [ ! -d $top/$x ]; then
		printf "$msg4" $top/$x
		mkdir $top/$x || exit 1
	fi
done

# tools for Makefile entries

mf_echo ()	# usage: mf_echo name desc
{
	printf "\t@echo '%-11s %s'\n" "$1" "$2" >> Makefile
}

mf_target ()	# usage: mf_target name deps
{
	printf "\n%s:: %s\n" "$1" "$2" >> Makefile
}

mf_rule ()	# usage: mf_rule name deps rule
{
	printf "\n%s: %s\n\t%s\n" "$1" "$2" "$3" >> Makefile
}

# create make rules for project

make_project ()	# usage: make_project src name deplist
{
	mf_target all $2
	mf_target clean $2.clean
	mf_rule $2 "$3 $2.d/Makefile" "( cd $2.d; make all )"
	mf_rule $2.clean $2.d/Makefile "( cd $2.d; make clean )"
	mf_rule $2.d "$1" "if [ ! -d \$@ ]; then mkdir -p \$@; fi; touch \$@"

	if [ -f $1/Configure ]; then
		mf_rule $2.d/Makefile "$2.d $1/Configure" \
			"( cd $1; ./Configure \$(TOP) ) > \$@"
	elif [ -f $1/Makefile ]; then
		cat >> Makefile <<EOF

$2.d/Makefile: $2.d
	@printf "all::\n\t(cd $1; make all)\n\n" > \$@
	@printf "clean::\n\t(cd $1; make clean)\n\n" >> \$@
EOF
	elif [ -f $1/Imakefile ]; then
		cat >> Makefile <<EOF

$2.d/Imakefile: $2.d $1/Imakefile
	echo "TOP=	$top" > \$@
	echo "SRC=	$1" >> \$@
	echo "#include \"$1/Imakefile\"" >> \$@

$2.d/Makefile: $2.d $2.d/Imakefile
	( cd $2.d; mkmf )
EOF
	else
		mf_rule $2.d/Makefile $2.d "( cd $2.d; dir2make $1 )"
	fi
}

#	Makefile in der Konfgurationsbibliothek generieren

gen=$top/build
cd $gen

cat > Makefile << !
#	Nicht editierten, Datei wurde automatisch generiert

TOP=	$top
!

mf_target usage ""
mf_echo "$lbl_usage" "make init | all | clean"
mf_echo " " "make <name> | <name>.clean"
mf_echo
mf_echo init	"$lbl_init"
mf_echo all	"$lbl_all"
mf_echo clean	"$lbl_clean"
mf_echo

mf_rule init: " " "efeubuild \$(TOP)"
mf_target all init

#	Projektliste aufarbeiten

failed=""

efeuprj -d $top/src/*/DESC | (while
	read name dep
do
	full_name="$top/src/$name"

	mf_rule usage: " " "@efeuprj $full_name/DESC"

	if
		[ -x $full_name/setup ]
	then
		printf "$msg5" $name >&2

		if
			(cd $full_name; ./setup)
		then
			:
		else
			failed="$failed $name"
			mf_rule $name " " "-(cd $full_name; ./setup)"
			continue
		fi
	fi

	make_project "$full_name" "$name" "$dep"
done)

printf "\n" >&2
printf "$msg6" $gen

if [ "$failed" != "" ]; then
	printf "$msg7" "$failed"
	mf_target usage ""
	mf_echo
	printf "\t@echo '%s'\n" "$lbl_failed$failed" >> Makefile
fi

