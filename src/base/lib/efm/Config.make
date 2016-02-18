# :*: basic libraries
# :de: Basisbibliothek
##Depends: script

include efeu.smh
include dl.smh
mf_var -x CFLAGS "-DSB_REALLOC=lrealloc"
efeu_library -c -h EFEU efm -ml LD_DL

foreach -p $TOP/man/de/man3 -s c 'efeu_man -l de_AT -s 3 $tg $src'
foreach -p $TOP/man/de/man7 -s h 'efeu_man -l de_AT -s 7 $tg $src'
