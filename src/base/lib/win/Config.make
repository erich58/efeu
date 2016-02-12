# :*: curses tools
# :de: Fensterhilfsprogramme
##Depends: efm efx

NAME=efwin
STAT=win.stat

xinclude $SRC/win.smh $STAT
include efeu.smh
efeu_note -d $NAME

if
	grep -q SUCCESS $STAT
then
	efeu_library -h EFEU $NAME -d $STAT -ml EFX_LIB -ml LD_CURSES
else
	mf_rule -d all "" "rm -f $TOP/lib/lib$NAME.*"
	efeu_note $NAME << !
The building of the library $NAME were rejected due to problems
with curses. The most probable reason for this is the missing of
the curses development files. Install the corresponding
package(s) and rebuild the library.
!
fi
