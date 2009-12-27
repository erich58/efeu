# :en: simple tools
# :de: Einfache Hilfsprogramme
##Depends: .

include efeu.smh

mf_var CFLAGS -DEFEUCFG
mf_dir -p BIN=$TOP/bin
foreach -m BIN -s sh 'mf_script -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -m BIN -s c 'mf_cc -o $tg $src -lefm'
foreach -e Rules.make 'include $src'
