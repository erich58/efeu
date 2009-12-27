include efeu.smh
mf_dir -p EXDIR=$TOP/share/examples/etk
foreach -m EXDIR -s etk 'mf_script -e -c etk $tg $src'
