# :*:create Imakefile from source tree
# :de:Imakefile aus Sourcebibliothek generieren
#
# Copyright (C) 2000 Erich Fr�hst�ck
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

: ${LANG:=en}

fmt_usage="
usage: $name [-r] [-hfxn] [-H dir] [-A dir] [-B dir] [-L dir]
	[-l name] [-i name] [-m name] [-s name] [-D dir] [-d name]
	[-p flag] dir

-r	call make, if Makefile has changed, must be the first Option!
-h	Show this text
-f	force creation of files
-x	create source with mksource
-n	do not use standard rules
-H dir	directory to install header files, default $HDR
-A dir	directory to install application files, default $APP
-B dir	directory to install executables, default $BIN
-L dir	directory to install Libraries, default $LDIR
-D dir	directory to install documents, default $DDIR
-d name	name of documentation
-l name	name of library
-i name	name of Imakefile, default $imakefile
-m name	name of Makefiles, default $makefile
-s name	name of source list, default $srclist
-p flag	flags for creating documents
dir	top directory of source files
"
fmt_new="%s is newer than %s\n"
fmt_create="%s was created\n"
fmt_head="@!\tDo not edit, file created by $0\n"
fmt_mobj="The following oject files are multiple definied:\n\n"
fmt_rmobj="The following object files are removed:\n"
fmt_rmlib="Library %s was removed\n"
fmt_mcreate="\n*** create %s ***\n\n"
fmt_mkeep="%s is up to date\n"
fmt_mupdate="\n*** %s has changed ***\n\n"

case $LANG in
de*)
	fmt_usage="
Aufruf: $name [-r] [-hfxn] [-H dir] [-A dir] [-B dir] [-L dir]
	[-l name] [-i name] [-m name] [-s name] [-D dir] [-d name]
	[-p flag] dir

-r	Aufruf von make, wenn sich Makefile ge�ndert hat.
	Mu� als erste Option angegeben werden!
-h	Hilfetext ausgeben
-f	Erneuerung aller Dateien forcieren
-x	Sourcen mit mksource generieren
-n	Keine Standardregeln verwenden
-H dir	Installationsverzeichnis f�r Include-Dateien, Vorgabe $HDR
-A dir	Installationsverzeichnis f�r Applikationsfiles, Vorgabe $APP
-B dir	Installationsverzeichnis f�r Kommandos, Vorgabe $BIN
-L dir	Installationsverzeichnis f�r Programmbibliothek, Vorgabe $LDIR
-D dir	Installationsverzeichnis f�r Dokumente, Vorgabe $DDIR
-d name	Name der Dokumentationsbibliothek
-l name	Name der Programmbibliothek
-i name	Name des Imakefiles, Vorgabe $imakefile
-m name	Name des Makefiles, Vorgabe $makefile
-s name	Name der Sourceliste, Vorgabe $srclist
-p flag	Generierungsflags f�r Dokumente
dir	Bibliothek mit Sourcefiles
"
	fmt_new="%s ist neuer als %s\n"
	fmt_create="%s wird generiert\n"
	fmt_head="@!\tNicht editieren, Datei wurde mit $0 generiert\n"
	fmt_mobj="Die folgenden Objektfiles sind mehrfach definiert:\n\n"
	fmt_rmobj="Die folgenden Objektfiles wurden entfernt:\n"
	fmt_rmlib="Programmbibliothek %s wurde entfernt\n"
	fmt_mcreate="\n*** %s wird generiert ***\n\n"
	fmt_mkeep="%s ist unver�ndert\n"
	fmt_mupdate="\n*** %s wurde ver�ndert ***\n\n"
	;;
esac

tmp=/usr/tmp/s2i$$
trap "rm -f $tmp*" 0
trap "exit 1" 1 2 3

# restart option

if [ A$1 = A-r ]; then
	restart=1;
	shift;
else
	restart=0;
fi

# command parameter

bootstrap="$*"
APP=.
BIN=.
HDR=.
LDIR=.
LIB=
DDIR=.
DOC=
srclist=SourceList
imakefile=Imakefile
makefile=Makefile
slflag=
mkflag=0
norule=0
pflag=NullArg

# parse command args

while getopts "hfnxH:A:B:I:L:D:l:d:i:m:s:p:" opt
do
	case $opt in
	h)	echo "$fmt_usage"; exit 0;;
	f)	slflag="-f";;
	n)	norule=1;;
	x)	mkflag=1;;
	A)	APP=$OPTARG;;
	B)	BIN=$OPTARG;;
	H)	HDR=$OPTARG;;
	L)	LDIR=$OPTARG;;
	D)	DDIR=$OPTARG;;
	l)	LIB=$OPTARG;;
	d)	DOC=$OPTARG;;
	i)	imakefile=$OPTARG;;
	m)	makefile=$OPTARG;;
	s)	srclist=$OPTARG;;
	p)	pflag=$OPTARG;;
	\?)	echo "$fmt_usage" | sed -e '/^$/q'; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

test $# -ne 1 && { echo "$fmt_usage" | sed -e '/^$/q'; exit 1; }
	
src=`(cd $1 || exit 1; pwd)` || exit 1

# create source list

mksrclist $slflag $src $srclist

if [ ! -f $makefile ] ; then
	rm -f $imakefile
fi

# create Imakefile

if [ -f $imakefile -a ! $srclist -nt $imakefile ] ; then
	printf "$fmt_new" $imakefile $srclist
	exit 0
fi

printf "$fmt_create" $imakefile

cat > $imakefile << !
$fmt_head

BOOTSTRAP= $bootstrap

#include <d2m_rules.pph>
#include <stdlib.pph>

#define	IMAKEFILE	$imakefile
#define	MAKEFILE	$makefile

all:: check genfiles

genfiles:: files

files::

check::
	$0 -r \$(BOOTSTRAP)

depend:: genfiles

update::
	rm -f $srclist $imakefile
	$0 -r \$(BOOTSTRAP)

$srclist $imakefile:
	$0 \$(BOOTSTRAP)

$makefile: $srclist $imakefile
	mkmf IMAKEFILE MAKEFILE
	
EFEUTOP=	_EFEU_TOP
MAXMEM=		_MAXMEM
STDLIBDIR=	\$(EFEUTOP)/lib

Destination(EFEULIB,\$(EFEUTOP)/lib/efeu)
Destination(ESHLIB,\$(EFEUTOP)/lib/esh)

#ifdef	AddDebugInfo
STDCFLAGS=	-g
#else
STDCFLAGS=
#endif

CC=		efeucc
CPP=		\$(CC) -E
CPP2DEP=	pp2dep -l
RM=		rm -f
CP=		cp
LN=		ln -s

INCFLG=	-I$HDR
BINDIR=	$BIN
LIBDIR=	$LDIR

XLIBS=
STDDEFINES=	
SYSCFLAGS=	

#ifdef	DoSharedLib
SYSLFLAGS=	-Wl,-rpath,\$(EFEUTOP)/lib
#else
SYSLFLAGS=
#endif
SYSLIBS=

ESH=	\$(EFEUTOP)/bin/esh

!

cat > $tmp.rules <<!
c	DF	Object(%s,%s)
sh	DFXF	ShellScript(%s,%s.%s,%s)
esh	DFXF	EshScript(%s,%s.%s,%s)
!

# header files

if [ "$HDR" != "." ] ; then
	cat >> $imakefile <<!
Destination(INCDIR,$HDR)

#define	GenHeader(name)	InstallGenFile(name,\$(INCDIR),name)

!
	cat >> $tmp.rules << !
h	DFXFX	InstallFile(%s/%s.%s,\$(INCDIR),%s.%s)
!
else
	cat >> $imakefile <<!
#define	GenHeader(name)

!
	cat >> $tmp.rules << !
h	DFXFX	InstallFile(%s/%s.%s,.,%s.%s)
!
fi

if [ $norule -ne 0 ]; then
	cp /dev/null $tmp.rules
fi

# application files

if [ "$APP" != "." ] ; then
	cat >> $imakefile <<!
Destination(APPDIR,$APP/app-defaults)
Destination(CNFDIR,$APP/config)
Destination(INFODIR,$APP/info)
Destination(HLPDIR,$APP/help)
Destination(MSGDIR,$APP/messages)

ProvideDir($APP/de/app-defaults)
ProvideDir($APP/de/config)
ProvideDir($APP/de/info)
ProvideDir($APP/de/help)
ProvideDir($APP/de/messages)

#define	GenInfo(name)	InstallGenFile(name,\$(INFODIR),name)

!
	cat >> $tmp.rules << !
\/de,,app	DFXFX	InstallFile(%s/%s.%s,$APP/de/app-defaults,%s.%s)
\/de,,cnf	DFXFX	InstallFile(%s/%s.%s,$APP/de/config,%s.%s)
\/de,,hlp	DFXFX	InstallFile(%s/%s.%s,$APP/de/help,%s.%s)
\/de,,msg	DFXFX	InstallFile(%s/%s.%s,$APP/de/messages,%s.%s)
app	DFXFX	InstallFile(%s/%s.%s,\$(APPDIR),%s.%s)
cnf	DFXFX	InstallFile(%s/%s.%s,\$(CNFDIR),%s.%s)
hlp	DFXFX	InstallFile(%s/%s.%s,\$(HLPDIR),%s.%s)
msg	DFXFX	InstallFile(%s/%s.%s,\$(MSGDIR),%s.%s)
!
else
	cat >> $imakefile <<!
#define	GenInfo(name)

!
fi

# documentations

if [ "$DDIR" != "." ] ; then
	echo "Destination(DOCDIR,$DDIR)" >> $imakefile
else
	echo "DOCDIR=	." >> $imakefile
fi

echo >> $imakefile

# local Imakefiles and rules

for x in `grep '\/Imakefile$' $srclist`
do
	echo "#include <$x>" >> $imakefile
done

for x in `grep '\/Rules$' $srclist`
do
	cat $x >> $tmp.rules
done

# createt sources

if  [ $mkflag -ne 0 ]; then
	cp /dev/null $tmp.1

	for x in `grep '\.tpl$' $srclist`
	do
		mksource -r -L$tmp.2 --all=genfiles $x >> $imakefile
		cat $tmp.2 >> $tmp.1
	done

	awk '
/\.h$/ { printf("GenHeader(%s)\n", $1); }
/\.c$/ { sub("\.c$", "", $1); printf("GenObject(%s)\n", $1); }
/\.doc$/ { printf("GenDoc(%s)\n", $1); }
/\.info$/ { printf("GenInfo(%s)\n", $1); }
' $tmp.1 >> $imakefile
	rm -f $tmp.1
fi

# other sources

sed -e 's|[^/]*$| &|' -e 's|/ | |' \
	-e '/[\.]/s/[^\.]*$/ &/' -e 's/\. / /' \
	$srclist > $tmp.lst

awk '{
if	($1 ~ /,/)
{
	gsub(",,", " .* ", $1)
	gsub(",", " ", $1)
	printf("/%s$/", $1)
}
else	printf("$3 ~ /^%s$/", $1)

delim = " { printf(\"";

for (i = 3; i <= NF; i++)
{
	gsub("\"", "\\\"", $i)
	printf("%s%s", delim, $i)
	delim = " ";
}

printf("\\n\"")
gsub("D", ", $1", $2)
gsub("F", ", $2", $2)
gsub("X", ", $3", $2)
printf("%s); next }\n", $2)
}' $tmp.rules > $tmp.1

awk -f $tmp.1 $tmp.lst >> $imakefile
rm -f $tmp.1

# create list of object files

awk '
/^GenObject(.*)$/ { sub(".*[(]", ""); sub("[)]", ".o"); print }
/^Object(.*)$/ { sub(".*,", ""); sub("[)]", ".o"); print }
' $imakefile | sort > $tmp.obj

# check name conflicts

if
	sort -u $tmp.obj | cmp -s - $tmp.obj
	test $? -ne 0
then
	sort -u $tmp.obj | comm -13 - $tmp.obj > $tmp.1
	awk '
/ c$/ || / tpl$/ {
	printf("%s.o durch %s/%s.%s\n", $2, $1, $2, $3)
}
' $tmp.lst | sort > $tmp.2
	printf "$fmt_mobj"
	join $tmp.1 $tmp.2
	exit 1
	rm -f $tmp.1 $tmp.2
fi

# :de:
# Test auf nicht mehr aktuelle Objektfiles:
# Falls solche existieren, werden sie zusammen mit der
# Programmbibliothek entfernt

ls *.o 2>/dev/null | sort > $tmp.1
comm -23 $tmp.1 $tmp.obj > $tmp.2

if	[ -s $tmp.2 ] ; then
	printf "$fmt_rmobj"
	cat $tmp.2
	rm -f `cat $tmp.2`

	for x in `find $LDIR -name "lib$LIB.*" -print`
	do
		rm -f $x
		printf "$fmt_rmlib" $x
	done
fi

rm -f $tmp.1 $tmp.2

awk '
BEGIN { printf("\nOBJLIST=") }
{ printf(" \\\n%s", $1) }
END { printf("\n\n") };
' $tmp.obj >> $imakefile

rm -f $tmp.obj $tmp.lst

# :de:Bibliotheskregeln anh�ngen

if	test $LIB
then
	cat >> $imakefile << !
LibTarget(\$(LIBDIR),$LIB,\$(OBJLIST),\$(XLIBS))

!
fi

# :de:Dokumentregel anh�ngen

if	[ "$DOC" -a -f $src/main.doc ]
then
	cat >> $imakefile << !
Destination(DOCSRC,\$(DOCDIR)/src)
SymLink(\$(DOCSRC)/$DOC,$src)
DocTarget(\$(DOCDIR),$DOC,$src,$pflag)

!
fi

# :de:Aufr�umregel: L��t nur das Makefile ohne Abh�ngigkeiten zur�ck

cat >> $imakefile << !

clean::
	mkmf $imakefile $makefile
	rm -f $imakefile $srclist

!

# :*:create Makefile
# :de:Makefile generieren

[ -f $makefile ] && mv $makefile $tmp.save

printf "$fmt_mcreate" $makefile
mkmf $imakefile $makefile

if cmp -s $tmp.save $makefile; then
	printf "$fmt_mkeep" $makefile
elif [ $restart -ne 0 ]; then
	printf "$fmt_mupdate" $makefile
	make
fi
