# :de: Shell-Scripts
##Depends: .

include efeu.smh
mf_dir BIN=$TOP/bin
foreach -m BIN -s sh 'mf_script -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -m BIN -s esh 'mf_script -s "/:VN:/$EFEU_VERSION/" -e -c esh $tg $src'
