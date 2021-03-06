# :*: man pages for installed files
# :de: Handbucheinträge aus installierten Dateien generieren
##Depends: .

include efeu.smh

mf_srclist ScriptList $TOP/lib/shmkmf
foreach -p $TOP/man/man7 -S smh 'efeu_man -l en_US -s 7 $tg $src'
foreach -p $TOP/man/de/man7 -S smh 'efeu_man -l de_AT -s 7 $tg $src'

mf_srclist MSList $TOP/lib/mksource
foreach -p $TOP/man/de/man7 -s ms 'efeu_man -l de_AT -s 7 $tg $src'
