% Template file for the startup script.
% The script would be created by running "make starter".
% Lines beginning with % are removed.
% The key %T is replaced by the top directory of efeu.
#!/bin/sh
# :*:Startup script for efeu commandos
# :de:Startskript für efeu-Programme

# $Note
# :*:This command was created by |make starter|
# on installing EFEU.
# :de:Dieses Kommando wurde mit |make starter|
# bei der Installation von EFEU generiert.

PATH=%T:$PATH
export PATH

# $pconfig
# Version="$Id: efeu.tpl,v 1.2 2008-04-17 20:48:46 ef Exp $"
# ::cmd |
#	:*:command to start
#	:de:Kommando
# *args |
#	:*:arguments for <cmd>
#	:de:Argumente für <cmd>

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 [cmd args(s)]"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

# $Description
# :*:The command |$1| expands |PATH| with dthe bin directoty of EFEU
# and calls the given command <cmd> with arguments <args>. Without
# arguments, |$1| runs a shell in the new environment.
# :de:Das Kommando |$!| erweitert |PATH| mit dem bin-Verzeichnis von
# EFEU und ruft das gegebene Kommando <cmd> mit den
# zugehörigen Argumenten auf. Ohne Argumente wird eine Shell in der
# veränderten Umgebung gestartet.

test $# -ge 1 && exec "$@"
exec $SHELL
