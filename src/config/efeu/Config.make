# :*: EFEU specific tools
# :de: EFEU spezifische Werkzeuge
##Depends: tools

mf_srclist

mf_dir BIN=$TOP/bin
mf_dir SMH=$TOP/lib/shmkmf
mf_dir CFG=$TOP/etc/shmkmf

foreach -m BIN -s sh 'mf_file -x -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -m SMH -S smh 'mf_file $tg $src'
foreach -m BIN -s gencmd "mf_file -x \$tg \$src 'VN=\"$EFEU_VERSION\" sh' $TOP"
foreach -m CFG -s gencfg "mf_update \$tg.cfg 'VN=\"$EFEU_VERSION\" sh' \$src $TOP"

include efeudoc.smh
