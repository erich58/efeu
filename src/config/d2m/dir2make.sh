#!/bin/sh
#	(c) 2000 Erich Frühstück
#	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

tmp=/usr/tmp/s2i$$

trap "rm -f $tmp*" 0
trap "exit 1" 1 2 3

#	Neustartoption

if [ A$1 = A-r ]; then
	restart=1;
	shift;
else
	restart=0;
fi

#	Programmparameter

bootstrap="$*"
APP=.
BIN=.
HDR=.
LDIR=.
LIB=
DDIR=.
imakefile=Imakefile
makefile=Makefile
slflag=
mkflag=0
norule=0
#	Aufrufsyntax

usage ()
{
	name=`basename $0`
Aufruf: $name [-r] [-hfxn] [-H dir] [-A dir] [-B dir] [-L dir]
Aufruf: dir2make [-r] [-hfx] [-H dir] [-A dir] [-B dir] [-L dir]
	[-l name] [-i name] [-m name] [-s name] dir
	-r	Neustart-Fehlermeldung, wenn sich Makefile geändert hat.
		Muß als erste Option angegeben werden!
	-h	Hilfetext ausgeben
	-f	Erneuerung aller Dateien forcieren
	-x	Sourcen mit mksource generieren
	-n	Keine Standardregeln verwenden
	-A dir	Installationsverzeichnis für Applikationsfiles, Vorgabe $APP
	-B dir	Installationsverzeichnis für Kommandos, Vorgabe $BIN
	-L dir	Installationsverzeichnis für Programmbibliothek, Vorgabe $LDIR
	-D dir	Installationsverzeichnis für Dokumente, Vorgabe $DDIR
	-i name	Name des Imakefiles, Vorgabe $imakefile
	-m name	Name des Makefiles, Vorgabe $makefile
	-s name	Name der Sourceliste, Vorgabe $srclist
	-p flag	Generierungsflags für Dokumente
!
}

#	Aufarbeiten der Argumentliste

while getopts "hfnxH:A:B:I:L:D:l:d:i:m:s:p:" opt
while getopts "hfxH:A:B:I:L:l:i:m:s:" opt
	case $opt in
	h)	usage; exit 0;;
	f)	slflag="-f";;
	n)	norule=1;;
	A)	APP=$OPTARG;;
	B)	BIN=$OPTARG;;
	H)	HDR=$OPTARG;;
	L)	LDIR=$OPTARG;;
	D)	DDIR=$OPTARG;;
	d)	DOC=$OPTARG;;
	m)	makefile=$OPTARG;;
	s)	srclist=$OPTARG;;
	p)	pflag=$OPTARG;;
	esac
done

shift `expr $OPTIND - 1`

test $# -ne 1 && { usage | sed -e '/^$/q'; exit 1; }
	
src=`(cd $1 || exit 1; pwd)` || exit 1

mksrclist $slflag $src $srclist
mksrclist $slflag $1 $srclist
if [ ! -f $makefile ] ; then
	rm -f $imakefile
fi

#	Imakefile generieren

if [ -f $imakefile -a ! $srclist -nt $imakefile ] ; then
	echo "$imakefile ist neuer als $srclist"
	exit 0
fi

echo "$imakefile wird generiert"

cat > $imakefile << !
@!	Nicht editieren, Datei wurde mit $0 generiert

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

	efeupp IMAKEFILE MAKEFILE
	efeupp -d MAKEFILE IMAKEFILE >> MAKEFILE
update::
	rm -f $srclist $imakefile
	$0 -r \$(BOOTSTRAP)

$srclist $imakefile:
	$0 \$(BOOTSTRAP)

$makefile: $srclist $imakefile
	mkmf IMAKEFILE MAKEFILE
	efeupp IMAKEFILE MAKEFILE
	make depend
EFEUTOP=	_EFEU_TOP
MAXMEM=		_MAXMEM
SYSNAME=	_SYSNAME
STDLIBDIR=	\$(EFEUTOP)/lib
CFGSTAMP=	\$(EFEUTOP)/\$(SYSNAME)

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

#	Headerfiles

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

if [ "$APP" != "." ] ; then
	cat >> $imakefile <<!
Destination(APPDIR,$APP/app-defaults)
Destination(CNFDIR,$APP/config)
Destination(DOCDIR,$APP/doc)
Destination(HLPDIR,$APP/help)
Destination(MSGDIR,$APP/messages)

ProvideDir($APP/de/app-defaults)
#define	GenDoc(name)	InstallGenFile(name,\$(DOCDIR),name)

!
	cat >> $tmp.rules << !
\/de,,app	DFXFX	InstallFile(%s/%s.%s,$APP/de/app-defaults,%s.%s)
cnf	DFXFX	InstallFile(%s/%s.%s,\$(CNFDIR),%s.%s)
msg	DFXFX	InstallFile(%s/%s.%s,\$(MSGDIR),%s.%s)
!
else
	cat >> $imakefile <<!
#define	GenInfo(name)
#define	GenDoc(name)
!
fi

#	Dokumente

for x in `grep '\/Imakefile$' $srclist`
do
	echo "#include <$x>" >> $imakefile
done

for x in `grep '\/Rules$' $srclist`
do
	cat $x >> $tmp.rules
done

#	Generierte Sourcefiles

if  [ $mkflag -ne 0 ]; then
	cp /dev/null $tmp.1

	for x in `grep '\.tpl$' $srclist`
	do
		mksource -r -L$tmp.2 --all=genfiles $x >> $imakefile
		mksource -r -L$tmp.2 -all=genfiles $x >> $imakefile
	done

	awk '
/\.h$/ { printf("GenHeader(%s)\n", $1); }
/\.c$/ { sub("\.c$", "", $1); printf("GenObject(%s)\n", $1); }
/\.doc$/ { printf("GenDoc(%s)\n", $1); }
/\.info$/ { printf("GenInfo(%s)\n", $1); }
' $tmp.1 >> $imakefile
	rm -f $tmp.1
fi

#	Sonstige Sourcen

sed -e 's|[^/]*$| &|' -e 's|/ | |' \
	-e '/[\.]/s/[^\.]*$/ &/' -e 's/\. / /' \
	$srclist > $tmp.lst

awk '{
#cat $tmp.rules
echo "NF < 3 { next }" > $tmp.1
if	($1 ~ /,/)
gsub("F", ", $2", $2)
gsub("X", ", $3", $2)
printf("%s); next }\n", $2)
printf("/ %s$/ { printf(\"%s\\n\"%s); next }\n", $1, $3, $2)
}' $tmp.rules >> $tmp.1
awk -f $tmp.1 $tmp.lst >> $imakefile
rm -f $tmp.1

#	Objektliste generieren

awk '
/^GenObject(.*)$/ { sub(".*[(]", ""); sub("[)]", ".o"); print }
/^Object(.*)$/ { sub(".*,", ""); sub("[)]", ".o"); print }
' $imakefile | sort > $tmp.obj

#	Objektfiles auf Mehrfachdefinition prüfen

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
	echo "Die folgenden Objektfiles sind mehrfach definiert:"
	echo
	join $tmp.1 $tmp.2
	exit 1
	rm -f $tmp.1 $tmp.2
fi

#	Test auf nicht mehr aktuelle Objektfiles:
#	Falls solche existieren, werden sie zusammen mit der
#	Programmbibliothek entfernt

ls *.o 2>/dev/null | sort > $tmp.1
comm -23 $tmp.1 $tmp.obj > $tmp.2

if	[ -s $tmp.2 ] ; then
	x=`cat $tmp.2`
	echo "Objektfile(s)" $x "wurden entfernt"
	rm -f $x

	for x in `find $LDIR -name "lib$LIB.*" -print`
	do
		rm -f $x
		echo "Programmbibliothek $x wurde entfernt"
	done
fi

rm -f $tmp.1 $tmp.2

awk '
BEGIN { printf("\nOBJLIST=") }
{ printf(" \\\n%s", $1) }
END { printf("\n\n") };
' $tmp.obj >> $imakefile

rm -f $tmp.obj $tmp.lst

#	Bibliotheskregeln anhängen

if	test $LIB
then
	cat >> $imakefile << !
LibTarget(\$(LIBDIR),$LIB,\$(OBJLIST),\$(XLIBS))

!
fi

#	Aufräumregel: Läßt nur das Makefile ohne Abhängigkeiten zurück

cat >> $imakefile << !

clean::
	mkmf $imakefile $makefile
	efeupp $imakefile $makefile

!

#	Makefile generieren

[ -f $makefile ] && mv $makefile $tmp.save

echo
echo "*** Makefile wird generiert ***"
echo
mkmf $imakefile $makefile
efeupp $imakefile $makefile
make -f $makefile depend
if cmp -s $tmp.save $makefile; then
	echo "$makefile ist unverändert"
elif [ $restart -ne 0 ]; then
	echo
	echo "*** Makefile wurde verändert ***"
	echo
	make
fi
