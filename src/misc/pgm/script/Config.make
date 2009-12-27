include efeu.smh
mf_dir BIN=$TOP/bin
foreach -m BIN -s sh 'mf_script $tg $src'
foreach -m BIN -s esh 'mf_script -e -c esh $tg $src'
