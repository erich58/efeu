#!/bin/sh
#	Hauptmakefile für EFEU-Projekte generieren
#	(c) 2000 Erich Frühstück
#	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

#	Version 0.9

top=${EFEUTOP:-~}
cfg=etc/efeu.conf
prj=etc/efeu.prj

if [ $# -eq 1 ]; then
	top=$1
fi

cd $top || exit 1

#	Sourcebibliothek prüfen

if	[ ! -d $top/src ]; then
	echo "Bibliothek $top/src existiert nicht!"
	exit 1
fi

top=`pwd`
echo "Generierungsregeln für $top einrichten"

if [ -f $top/$cfg ]; then
	echo "Konfigurationsdatei $top/$cfg laden"
	. $top/$cfg
fi

#	Basisbibliotheken einrichten

for x in bin lib build
do
	if [ ! -d $top/$x ]; then
		echo "Bibliothek $top/$x einrichten"
		mkdir $top/$x || exit 1
	fi
done

#	Hilfsfunktionen

mf_echo ()	# Aufruf: mf_echo [target] < desc
{
	echo

	if [ $# -ge 1 ]; then
		echo "$1::"
	fi

	expand -12,30 | sed -e '
s/"/\\\\&/g
s/^/	@echo \"/
s/$/\"/
'
}

mf_target ()	# Aufruf: mf_target name deps
{
	cat >> Makefile << !

$1:: $2
!
}

mf_rule ()	# Aufruf: mf_rule name deps rule
{
	cat >> Makefile << !

$1: $2
	$3
!
}

#	Projekteintrag generieren

make_projekt ()	# make_projekt src name deplist desc
{
	if [ $3 != "." ]; then
		deplist=`echo $3 | tr ',' ' '`
	else
		deplist=""
	fi

	echo "$2	$4" | mf_echo usage >> Makefile
	mf_target init $2
	mf_rule $2.d "" "mkdir $2.d"
	mf_rule $2 "$deplist $2.d/Makefile" "( cd $2.d; make )"

	for x in all clean update
	do
		mf_target $x $2.$x
		mf_rule $2.$x $2.d/Makefile "( cd $2.d; make $x )"
	done

	if [ -f $1/Configure ]; then
		mf_rule $2.d/Makefile "$2.d $1/Configure" \
			"( cd $1; Configure \$(TOP) ) > \$@"
	elif [ -f $1/Imakefile ]; then
		cat >> Makefile <<!

$2.d/Imakefile: $2.d $1/Imakefile
	echo "SRCTOP=	$1" > \$@
	echo "#include \"$1/Imakefile\"" >> \$@

$2.d/Makefile: $2.d $2.d/Imakefile
	( cd $2.d; mkmf )
!
	else
		mf_rule $2.d/Makefile $2.d "( cd $2.d; dir2make $1 )"
	fi
}

#	Projektliste verarbeiten

eval_plist ()
{
	plist=""

	while
		read name dep desc
	do
		if [ "A$dep" = "A*" ]; then
			dep="$plist"
		fi

		make_projekt "$top/src/$name" "$name" "$dep" "$desc"
		plist="$plist,$name"
	done
}

#	Makefile in der Konfgurationsbibliothek generieren

gen=$top/build
cd $gen

cat > Makefile << !
#	Nicht editierten, Datei wurde automatisch generiert

TOP=	$top
!

mf_echo usage >> Makefile <<!
Aufruf:	make <name> | all | clean | update
	make <name>[.all|.clean|.update] | <name>.<sub>

init	Unterbibliotheken initialisieren (alle Ziele ohne Zusatz)
all	Alles generieren (alle Ziele mit Zusatz .all)
clean	Alles Aufräumen (alle Ziele mit Zusatz .clean)
update	Alles auf den letzten Stand bringen (alle Ziele mit Zusatz .update)
config	Generierungsregeln neu Konfigurieren
!

mf_rule config "" "efeubuild \$(TOP)"
mf_target init config
mf_target all config
mf_target update config

#	Projektdatei abarbeiten

if	[ -f $top/$prj ]; then
	echo "Projektdatei $top/$prj laden"
	grep '^[^#]' $top/$prj | eval_plist
	mf_rule Makefile "$top/$prj" "efeubuild \$(TOP)"
else
	makedep=""
	make_projekt "$top/src" "std" "." "Standardprojekt"
	mf_rule Makefile "" "efeubuild \$(TOP)"
fi

echo "Makefile in $gen wurde generiert."

