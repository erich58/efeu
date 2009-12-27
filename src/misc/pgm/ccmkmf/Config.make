include efeu.smh
mf_dir BIN=$TOP/bin
mf_dir PPINC=$TOP/ppinclude

foreach -m BIN -s c 'mf_cc -o $tg $src'
foreach -m BIN -s sh 'mf_script $tg $src'

foreach -m PPINC -s pphcfg 'mf_config $tg.pph $src'
foreach -m PPINC -S cfg 'mf_file $tg $src'
foreach -m PPINC -S pph 'mf_file $tg $src'
