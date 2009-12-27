#!/bin/sh
#	lpr Filter für BJC620 Tintenstrahldrucker
#	(c) 1997 Erich Frühstück
#	A-1090 Wien, Währinger Straße 64/6
#

tmp=/usr/tmp/lbp$$

#	GhostScript

#GS=/local/bin/gs261
GS=/usr/local/bin/gs353
#GS=/usr/bin/gs
#GDEV="-sDEVICE=eps9high"
GDEV="-sDEVICE=bjc600"
GSFLAGS="-q -r360 -dBitsPerPixel=32 -sPrintQuality=High -sOutputFile=- -sPAPERSIZE=a4 -dNOPAUSE"
#GSFLAGS="-q -r360 -sPrintQuality=High -sOutputFile=/tmp/X -sPAPERSIZE=a4 -dNOPAUSE"
#DVI=dvi560C
#DVIFLAGS=""

#	Hier kommt die eigentliche Druckersteuerung

#	Initialisierung:

#	^[c	Hartes Rueckstellen
#	^[[?5h	Auto CR bei LF freigeben
#	^[('$2	IBML Zeichensatz fuer Code 0 - 127 laden
#	^[)' 1	IBMR1 Zeichensatz fuer Code 128 - 255 laden

#	Die Ausgabe erfolgt mit dem Filter expand, damit Tabulatoren
#	in Leerzeichen umgewandelt werden.

#	Ein PostScript - File (durch %! gekennzeichnet) wird mit
#	GhostScript ausgegeben. Der Test erfolgt mit dem Programm file

cat > $tmp

case "`head -c2 $tmp`" in
"%!")
#	if
#		head $tmp | grep -q "Creator: dvips"
#	then
#		$GS $GDEVDVI $GSFLAGS -- $tmp
#	else
#		$GS $GDEV $GSFLAGS -- $tmp 2>/tmp/BJC_Error
		$GS $GDEV $GSFLAGS -- $tmp
#	fi
	;;
"÷")
#	$DVI $DVIFLAGS < $tmp 2>/dev/null
	mv $tmp $tmp.dvi
	dvips $tmp >/dev/null
	rm $tmp.dvi
	mv $tmp.ps $tmp
	$GS $GDEV $GSFLAGS -- $tmp
	;;
"@")
	cat $tmp
	;;
"[")
	cat $tmp
	;;
*)
#	echo "E(0N\c"
	expand $tmp 2>&1 | sed -e 's/$//g'
#	echo "E\c"
	/bin/echo "\f\c"
	;;
esac

rm -f $tmp
exit 0
