# :*: basic libraries
# :de: Basisbibliothek
##Depends: script

include efeu.smh
include dl.smh
mf_var -x CFLAGS "-DSB_REALLOC=lrealloc"
efeu_library -c -h EFEU efm -ml LD_DL
