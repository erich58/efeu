# :*: makefile generator based on cpp
# :de: Makefile Generator, basierend auf cpp
##Depends: .

include efeu.smh
mf_dir BIN=$TOP/bin
mf_dir PPINC=$TOP/ppinclude

foreach -m BIN -s c 'mf_cc -o $tg $src'
foreach -m BIN -s sh 'mf_script -s "/:VN:/$EFEU_VERSION/g" $tg $src'

foreach -m PPINC -s pphcfg 'mf_config $tg.pph $src'
foreach -m PPINC -S cfg 'mf_file -s "/:VN:/$EFEU_VERSION/g" $tg $src'
foreach -m PPINC -S pph 'mf_file $tg $src'
