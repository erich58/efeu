# :*: readline interface
# :de: Readline-Schnittstelle
##Depends: efm

NAME=iorl

xinclude readline.smh
include efeu.smh

mf_var CFLAGS '$(RL_INC)'
efeu_library -u -h EFEU $NAME -d readline.stat -lefm -ml RL_LIB

# check the status of readline availability

HDR=$TOP/include/EFEU/rl_config.h

add_depend $HDR
efeu_note -d $NAME

test -f $HDR || return

if
	grep "HAS_READLINE.*0" $HDR
then
	efeu_note $NAME <<!
Line editing and history for interactive commands are not available.
Maybe readline-development is not installed or on an unexpected place.
Correct this and rebuild this module. If shared libraries are not
supported, you need to rebuild the involved commands too.
!
elif
	grep "HAS_HISTORY.*0" $HDR
then
	efeu_note $NAME <<!
Some History-functions for interactive commands are not
available. Install a newer version of readline-development
and rebuild the module. If shared libraries are not supported,
you need to rebuild the involved commands too.
!
fi
