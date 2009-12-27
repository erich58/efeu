#!/bin/sh
#	Magic-File für EFEU updaten
#	(c) 2000 Erich Frühstück
#	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

cd @TOP@/etc || exit 1

cat > magic <<!
# Magic data für file Kommando.
# Formatbeschreibung ist in magic(5).
# Diese Datei wurde mit update-magic zusammengestellt.
!

add_magic ()
{
	while
		read name
	do
		cat - $name <<!

# ---------------------------------------------------------------------
# $name
#
!
	done
}

if [ -d magic.d ]; then
	find magic.d -type f -print | add_magic >> magic
fi

find /etc/magic /usr/share/misc/magic -print 2>/dev/null | add_magic >> magic
