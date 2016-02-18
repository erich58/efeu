# :de: TCL/TK-interface
##Depends: config

xinclude tklib.smh
include efeu.smh

mf_var CFLAGS '$(TK_INC)'
efeu_library -u -h GUI ETK -d tklib.stat -lmd -ml EFX_LIB -ml TK_LIB
foreach -p $TOP/lib/esh -S hdr 'mf_file $tg $src'

# check the status of tk availability

HDR=$TOP/include/GUI/tk_config.h

add_depend $HDR
efeu_note -d ETK

test -f $HDR || exit 0

if
	grep "HAS_TCL.*0" $HDR
then
	efeu_note ETK <<!
Tcl/Tk interface for EFEU ist not available because Tcl/Tk development
files are not installed or on an unexpected place.
Correct this and rebuild the module.
!
fi

mf_rule -d doc
