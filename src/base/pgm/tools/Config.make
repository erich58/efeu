include efeu.smh

mf_dir -p BIN=$TOP/bin
foreach -m BIN -s sh 'mf_script $tg $src'
foreach -m BIN -s c 'mf_cc -o $tg $src -lefm'
foreach -e Rules.make 'include $src'
