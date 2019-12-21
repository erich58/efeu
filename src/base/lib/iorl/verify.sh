#!/bin/sh
# verify readline configuration

if
	grep "HAS_READLINE.*0" $2
then
	tee $1 <<!
Line editing and history for interactive commands are not available.
Maybe readline-development is not installed or on an unexpected place.
Correct this and rebuild this module. If shared libraries are not
supported, you need to rebuild the involved commands too.
!
elif
	grep "HAS_HISTORY.*0" $2
then
	tee $1 <<!
Some History-functions for interactive commands are not
available. Install a newer version of readline-development
and rebuild the module. If shared libraries are not supported,
you need to rebuild the involved commands too.
!
else
	rm -vf $1
fi
