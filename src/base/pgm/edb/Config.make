# :en: edb commands
# :de: edb  Kommandos
##Depends: efm efx md math iorl esh

include efeu.smh
include readline.smh
mf_dir BIN=$TOP/bin
#efeu_libvar -p EDB -lefence iorl -ml RL_LIB md efmath -ml EFX_LIB
efeu_libvar -p EDB iorl -ml RL_LIB md efmath -ml EFX_LIB
foreach -m BIN -s c 'mf_cc -md DEP_EDB -o $tg $src -ml LIB_EDB'
foreach -m BIN -s esh 'mf_script -e -c esh $tg $src'
foreach -p $TOP/lib/esh -S hdr 'mf_file $tg $src'
