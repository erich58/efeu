# :en: esh, mksource and scripts
# :de: esh, mksource und Scripts
##Depends: efm efx md math iorl

include efeu.smh
include readline.smh
mf_dir BIN=$TOP/bin
#efeu_libvar -p ESH -lefence iorl -ml RL_LIB md efmath -ml EFX_LIB
efeu_libvar -p ESH iorl -ml RL_LIB md efmath -ml EFX_LIB
foreach -m BIN -s c 'mf_cc -md DEP_ESH -o $tg $src -ml LIB_ESH'
foreach -m BIN -s esh 'mf_script -e -c esh $tg $src'
foreach -p $TOP/lib/esh -S hdr 'mf_file $tg $src'
foreach -p $TOP/lib/mksource -S ms 'mf_file $tg $src'
foreach -p $TOP/lib/mksource -s msx 'compose-file -r $src $tg.ms'
