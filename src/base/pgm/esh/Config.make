include efeu.smh
include readline.smh
mf_dir BIN=$TOP/bin
#efeu_libvar -lefence iorl -ml RL_LIB md efmath -ml EFX_LIB
efeu_libvar iorl -ml RL_LIB md efmath -ml EFX_LIB
foreach -m BIN -s c 'mf_cc -md DEP -o $tg $src -ml LIB'
foreach -m BIN -s esh 'mf_script -e -c esh $tg $src'
foreach -p $TOP/lib/esh -S hdr 'mf_file $tg $src'
foreach -p $TOP/lib/mksource -S ms 'mf_file $tg $src'
